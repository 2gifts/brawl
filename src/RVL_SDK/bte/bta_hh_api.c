#include <bta_hh_int.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * bta/hh/bta_hh_api.c
 */

/******************************************************************************
 *
 *  Copyright (C) 2005-2012 Broadcom Corporation
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

/*******************************************************************************
 * variables
 */

static tBTA_SYS_REG const bta_hh_reg = {
	bta_hh_hdl_event,
	BTA_HhDisable,
};

/*******************************************************************************
 * functions
 */

void BTA_HhEnable(tBTA_SEC sec_mask, tBTA_HH_CBACK *p_cback)
{
	tBTA_HH_API_ENABLE *p_buf;

	GKI_sched_lock();
	bta_sys_register(BTA_ID_HH, &bta_hh_reg);
	GKI_sched_unlock();

	p_buf = (tBTA_HH_API_ENABLE *)GKI_getbuf(sizeof(tBTA_HH_API_ENABLE));

	if (p_buf != NULL)
	{
		memset(p_buf, 0, sizeof(tBTA_HH_API_ENABLE));

		p_buf->hdr.event = BTA_HH_API_ENABLE_EVT;
		p_buf->p_cback = p_cback;
		p_buf->sec_mask = sec_mask;

		bta_sys_sendmsg(p_buf);
	}
}

void BTA_HhDisable(void)
{
	BT_HDR *p_buf = (BT_HDR *)GKI_getbuf(sizeof(BT_HDR));

	if (p_buf != NULL)
	{
		p_buf->event = BTA_HH_API_DISABLE_EVT;
		bta_sys_sendmsg(p_buf);
	}
}

void BTA_HhClose(UINT8 dev_handle)
{
	BT_HDR *p_buf = (BT_HDR *)GKI_getbuf(sizeof(BT_HDR));

	if (p_buf != NULL)
	{
		memset(p_buf, 0, sizeof(BT_HDR));

		p_buf->event = BTA_HH_API_CLOSE_EVT;
		p_buf->layer_specific = (UINT16)dev_handle;

		bta_sys_sendmsg(p_buf);
	}
}

void BTA_HhOpen(BD_ADDR dev_bda, tBTA_HH_PROTO_MODE mode, tBTA_SEC sec_mask)
{
	tBTA_HH_API_CONN *p_buf =
		(tBTA_HH_API_CONN *)GKI_getbuf(sizeof(tBTA_HH_API_CONN));

	if (p_buf != NULL)
	{
		memset(p_buf, 0, sizeof(tBTA_HH_API_CONN));

		p_buf->hdr.event = BTA_HH_API_OPEN_EVT;
		p_buf->hdr.layer_specific = BTA_HH_INVALID_HANDLE;
		p_buf->sec_mask = sec_mask;
		p_buf->mode = mode;
		bdcpy(p_buf->bd_addr, dev_bda);

		bta_sys_sendmsg(p_buf);
	}
	else
	{
		APPL_TRACE(ERROR, "No resource to send HID host Connect request.");
	}
}

void BTA_HhSendData(UINT8 dev_handle, BT_HDR *p_data)
{
	tBTA_HH_CMD_DATA *p_buf =
		(tBTA_HH_CMD_DATA *)GKI_getbuf(sizeof(tBTA_HH_CMD_DATA));

	if (p_buf != NULL)
	{
		memset(p_buf, 0, sizeof(tBTA_HH_CMD_DATA));

		p_buf->hdr.event = BTA_HH_API_WRITE_DEV_EVT;
		p_buf->hdr.layer_specific = (UINT16)dev_handle;
		p_buf->t_type = HID_TRANS_DATA;
		p_buf->data = 0;
		p_buf->param = BTA_HH_RPTT_OUTPUT;
		p_buf->p_data = p_data;
		p_buf->rpt_id = 0;

		bta_sys_sendmsg(p_buf);
	}
}

void BTA_HhAddDev(BD_ADDR bda, tBTA_HH_ATTR_MASK attr_mask, UINT8 sub_class,
                  UINT8 app_id, tHID_DEV_DSCP_INFO dscp_info)
{
	tBTA_HH_MAINT_DEV *p_buf =
		(tBTA_HH_MAINT_DEV *)GKI_getbuf(sizeof(tBTA_HH_MAINT_DEV));

	if (p_buf != NULL)
	{
		memset(p_buf, 0, sizeof(tBTA_HH_MAINT_DEV));

		p_buf->hdr.event = BTA_HH_API_MAINT_DEV_EVT;
		p_buf->sub_event = BTA_HH_ADD_DEV_EVT;
		p_buf->hdr.layer_specific = BTA_HH_INVALID_HANDLE;
		p_buf->attr_mask = (UINT16)attr_mask;
		p_buf->sub_class = sub_class;
		p_buf->app_id = app_id;

		memcpy(&p_buf->dscp_info, &dscp_info, sizeof(tHID_DEV_DSCP_INFO));
		bdcpy(p_buf->bda, bda);

		bta_sys_sendmsg(p_buf);
	}
}

void BTA_HhRemoveDev(UINT8 dev_handle)
{
	tBTA_HH_MAINT_DEV *p_buf =
		(tBTA_HH_MAINT_DEV *)GKI_getbuf(sizeof(tBTA_HH_MAINT_DEV));

	if (p_buf != NULL)
	{
		memset(p_buf, 0, sizeof(tBTA_HH_MAINT_DEV));

		p_buf->hdr.event = BTA_HH_API_MAINT_DEV_EVT;
		p_buf->sub_event = BTA_HH_RMV_DEV_EVT;
		p_buf->hdr.layer_specific = (UINT16)dev_handle;

		bta_sys_sendmsg(p_buf);
	}
}

void BTA_HhGetAclQueueInfo(void)
{
	BT_HDR *p_buf = (BT_HDR *)GKI_getbuf(sizeof(BT_HDR));

	if (p_buf != NULL)
	{
		memset(p_buf, 0, sizeof(BT_HDR));

		p_buf->event = BTA_HH_API_GET_ACL_Q_EVT;

		bta_sys_sendmsg(p_buf);
	}
}
