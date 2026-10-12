#include <bte.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * main/bte_main.c
 */

/******************************************************************************
 *
 *  Copyright (C) 2001-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/*******************************************************************************
 * headers
 */

#include "revolution/types.h"

#include <revolution/os.h>

#include <btu.h>
#include <gki.h>
#include <hci.h>

/*******************************************************************************
 * types
 */

// HYPOTHESIS: signature of the little callback the stack reports its state to
typedef void tBTE_STATE_CBACK(UINT8 state);

/*******************************************************************************
 * external functions
 */

// runs func on a separate stack whose top is stack_top, asm glue that lives elsewhere
extern void fn_801D8194(void (*func)(void), void *stack_top);

// bta_dm_api.c
extern BOOLEAN BTA_DmIsDeviceUp(void);

/*******************************************************************************
 * variables
 */

// set by hcisu_h2 (wud.c owns it), nonzero while the hci transport is still busy
extern int bte_hcisu_wait;

// HYPOTHESIS: usb vendor/product ids of the bcm2045 in the wii, terminated by a zero entry
tHCI_CFG bte_hcisu_cfg[2] = {{0x0a5c, 0x2101}, {0, 0}};

static UINT8 bte_stack[0x1000];
static OSAlarm bte_alarm;

tBTE_TARGET_MODE volatile bte_target_mode = BTE_MODE_SERIAL_APP;
static tBTE_STATE_CBACK *bte_state_cback;

/*******************************************************************************
 * functions
 */

static void bte_alarm_handler(OSAlarm *alarm, OSContext *context)
{
	fn_801D8194(btu_task_msg_handler, bte_stack + sizeof bte_stack);
}

int BTE_LoadStack(void)
{
	bte_state_cback = NULL;

	GKI_init();
	GKI_enable();

	btu_init_cb();

	p_hcisu_cfg = bte_hcisu_cfg;
	p_hcisu_if = (tHCI_IF *)&hcisu_h2;

	bte_hcisu_task(0);

	while (bte_hcisu_wait != 0)
		GKI_delay(100);

	btu_task_init();

	OSCreateAlarm(&bte_alarm);
	OSSetPeriodicAlarm(&bte_alarm, OSGetTime(), OSMillisecondsToTicks(2),
	                   bte_alarm_handler);

	GKI_run(0);

	while (!BTA_DmIsDeviceUp())
		GKI_delay(2000);

	return 0;
}

void BTE_UnloadStack(tBTE_STATE_CBACK *p_cback)
{
	bte_state_cback = p_cback;

	OSCancelAlarm(&bte_alarm);
	bte_hcisu_close();
	GKI_shutdown();
}

void bte_report_state(INT8 state)
{
	if (bte_state_cback)
	{
		if (state >= 0)
			(*bte_state_cback)(0);
		else
			(*bte_state_cback)(1);
	}
}
