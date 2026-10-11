#include <bta_sys_int.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * bta/sys/bta_sys_main.c
 */

/******************************************************************************
 *
 *  Copyright (C) 2003-2012 Broadcom Corporation
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

#include <bt_trace.h>
#include <gki.h>
#include <ptim.h>

// bta_dm_api.c
extern void fn_80234A90(void);

/*******************************************************************************
 * variables
 */

tBTA_SYS_CB bta_sys_cb;

UINT8 appl_trace_level;

/*******************************************************************************
 * functions
 */

void bta_sys_rm_register(tBTA_SYS_CONN_CBACK *p_cback)
{
	bta_sys_cb.prm_cb = p_cback;
}

void bta_sys_compress_register(tBTA_SYS_CONN_CBACK *p_cback)
{
	bta_sys_cb.compress_cb = p_cback;
}

void bta_sys_pm_register(tBTA_SYS_CONN_CBACK *p_cback)
{
	bta_sys_cb.ppm_cb = p_cback;
}

void bta_sys_conn_open(UINT8 id, UINT8 app_id, BD_ADDR peer_addr)
{
	if (bta_sys_cb.prm_cb)
		(*bta_sys_cb.prm_cb)(BTA_SYS_CONN_OPEN, id, app_id, peer_addr);

	if (bta_sys_cb.ppm_cb)
		(*bta_sys_cb.ppm_cb)(BTA_SYS_CONN_OPEN, id, app_id, peer_addr);

	if (bta_sys_cb.compress_cb)
		(*bta_sys_cb.compress_cb)(BTA_SYS_CONN_OPEN, id, app_id, peer_addr);
}

void bta_sys_conn_close(UINT8 id, UINT8 app_id, BD_ADDR peer_addr)
{
	if (bta_sys_cb.prm_cb)
		(*bta_sys_cb.prm_cb)(BTA_SYS_CONN_CLOSE, id, app_id, peer_addr);

	if (bta_sys_cb.ppm_cb)
		(*bta_sys_cb.ppm_cb)(BTA_SYS_CONN_CLOSE, id, app_id, peer_addr);

	if (bta_sys_cb.compress_cb)
		(*bta_sys_cb.compress_cb)(BTA_SYS_CONN_CLOSE, id, app_id, peer_addr);
}

void bta_sys_sco_close(UINT8 id, UINT8 app_id, BD_ADDR peer_addr)
{
	if (bta_sys_cb.ppm_cb)
		(*bta_sys_cb.ppm_cb)(BTA_SYS_SCO_CLOSE, id, app_id, peer_addr);
}

void bta_sys_idle(UINT8 id, UINT8 app_id, BD_ADDR peer_addr)
{
	if (bta_sys_cb.ppm_cb)
		(*bta_sys_cb.ppm_cb)(BTA_SYS_CONN_IDLE, id, app_id, peer_addr);
}

void bta_sys_busy(UINT8 id, UINT8 app_id, BD_ADDR peer_addr)
{
	if (bta_sys_cb.ppm_cb)
		(*bta_sys_cb.ppm_cb)(BTA_SYS_CONN_BUSY, id, app_id, peer_addr);
}

void bta_sys_init(void)
{
	memset(&bta_sys_cb, 0, sizeof bta_sys_cb);

	ptim_init(&bta_sys_cb.ptim_cb, 1000, p_bta_sys_cfg->timer);

	bta_sys_cb.task_id = GKI_get_taskid();

	appl_trace_level = p_bta_sys_cfg->trace_level;
}

void bta_sys_event(BT_HDR *p_msg)
{
	UINT8 id;
	BOOLEAN freebuf = TRUE;

	APPL_TRACE(EVENT, "BTA got event 0x%x", p_msg->event);

	id = (UINT8)(p_msg->event >> 8);

	// HYPOTHESIS: once the stack is being shut down only the disable event gets any attention
	if (bta_sys_cb.events_disabled)
	{
		if (p_msg->event == 0x101)
			fn_80234A90();

		GKI_freebuf(p_msg);
		return;
	}

	if (id < BTA_ID_MAX && bta_sys_cb.reg[id] != NULL)
		freebuf = (*bta_sys_cb.reg[id]->evt_hdlr)(p_msg);
	else
		APPL_TRACE(WARNING, "BTA got unregistered event id %d", id);

	if (freebuf)
		GKI_freebuf(p_msg);
}

void bta_sys_timer_update(void)
{
	if (!bta_sys_cb.timers_disabled)
		ptim_timer_update(&bta_sys_cb.ptim_cb);
}

void bta_sys_register(UINT8 id, tBTA_SYS_REG const *p_reg)
{
	bta_sys_cb.reg[id] = (tBTA_SYS_REG *)p_reg;
}

void bta_sys_sendmsg(void *p_msg)
{
	GKI_send_msg(bta_sys_cb.task_id, p_bta_sys_cfg->mbox, p_msg);
}

void bta_sys_start_timer(TIMER_LIST_ENT *p_tle, UINT16 type, INT32 timeout)
{
	ptim_start_timer(&bta_sys_cb.ptim_cb, p_tle, type, timeout);
}

void bta_sys_stop_timer(TIMER_LIST_ENT *p_tle)
{
	ptim_stop_timer(&bta_sys_cb.ptim_cb, p_tle);
}

void bta_sys_disable(void)
{
	int i;

	for (i = 0; i < BTA_ID_MAX; i++)
	{
		if (bta_sys_cb.reg[i] != NULL && bta_sys_cb.reg[i]->disable != NULL)
			(*bta_sys_cb.reg[i]->disable)();
	}
}

void bta_sys_set_trace_level(UINT8 level)
{
	appl_trace_level = level;
}
