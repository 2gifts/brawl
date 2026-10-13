#include <gki_int.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * gki/ulinux/gki_ulinux.c
 */

/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
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
#include <cstring>

#include <revolution/os.h>

#include <bt_target.h>
#include <gki.h>
#include <gki_target.h>

/*******************************************************************************
 * macros
 */

/*******************************************************************************
 * external functions
 */

// these two live in the wud code and bounce into its alloc/free callbacks, no names for them yet
extern void *WUDiMemAlloc(UINT32 size);
extern void WUDiMemFree(void *p_mem);

// bte_main.c
extern void bte_report_state(INT8 state);

/*******************************************************************************
 * variables
 */

tGKI_CB gki_cb;

/*******************************************************************************
 * functions
 */

void GKI_init(void)
{
	memset(&gki_cb, 0, sizeof gki_cb);

	gki_buffer_init();
	gki_timers_init();

	gki_cb.com.OSRdyTbl[BTU_TASK] = TASK_READY;
	gki_cb.com.OSTName[BTU_TASK] = NULL;
	gki_cb.com.OSWaitTmr[BTU_TASK] = 0;
	gki_cb.com.OSWaitEvt[BTU_TASK] = 0;

	gki_cb.os.int_index = 0;
	gki_cb.os.int_state[gki_cb.os.int_index] = OSEnableInterrupts();
	gki_cb.os.int_index++;
}

void GKI_shutdown(void)
{
	GKI_disable();

	gki_cb.com.OSRdyTbl[BTU_TASK] = TASK_DEAD;

	GKI_enable();

	// unwind anything that's still nested
	while (gki_cb.os.int_index != 0)
		GKI_enable();
}

void GKI_run(void *p_task_id)
{
	return;
}

void GKI_sched_lock(void)
{
	return;
}

void GKI_sched_unlock(void)
{
	return;
}

void GKI_delay(UINT32 timeout)
{
	return;
}

UINT8 GKI_send_event(UINT8 task_id, UINT16 event)
{
	if (task_id >= GKI_MAX_TASKS)
		return GKI_FAILURE;

	GKI_disable();

	gki_cb.com.OSWaitEvt[task_id] |= event;

	GKI_enable();

	return GKI_SUCCESS;
}

UINT8 GKI_get_taskid(void)
{
	return BTU_TASK;
}

void GKI_enable(void)
{
	gki_cb.os.int_index--;
	OSRestoreInterrupts(gki_cb.os.int_state[gki_cb.os.int_index]);
}

void GKI_disable(void)
{
	gki_cb.os.int_state[gki_cb.os.int_index] = OSDisableInterrupts();
	gki_cb.os.int_index++;
}

void GKI_exception(UINT16 code, char *msg)
{
	return;
}

void *GKI_os_malloc(UINT32 size)
{
	void *p_mem = WUDiMemAlloc(size);

	if (p_mem == NULL)
		return NULL;

	return p_mem;
}

void GKI_os_free(void *p_mem)
{
	WUDiMemFree(p_mem);
}

void bte_usb_state_cback(int type, INT8 arg)
{
	switch (type)
	{
	case 0:
		break;

	case 4:
		bte_report_state(arg);
		break;
	}
}
