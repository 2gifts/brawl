#include <bta_dm_int.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * bta/dm/bta_dm_api.c
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

#include <gki.h>

/*******************************************************************************
 * variables
 */

static tBTA_SYS_REG const bta_dm_reg = {
	bta_dm_sm_execute,
	NULL,
};

static tBTA_SYS_REG const bta_dm_search_reg = {
	bta_dm_search_sm_execute,
	NULL,
};

/*******************************************************************************
 * functions
 */

tBTA_STATUS BTA_EnableBluetooth(tBTA_DM_SEC_CBACK *p_cback)
{
	tBTA_DM_API_ENABLE *p_msg;

	memset(&bta_dm_cb, 0, sizeof bta_dm_cb);

	GKI_sched_lock();
	bta_sys_register(BTA_ID_DM, &bta_dm_reg);
	bta_sys_register(BTA_ID_DM_SEARCH, &bta_dm_search_reg);
	GKI_sched_unlock();

	if ((p_msg = (tBTA_DM_API_ENABLE *)GKI_getbuf(sizeof(tBTA_DM_API_ENABLE))) != NULL)
	{
		p_msg->hdr.event = BTA_DM_API_ENABLE_EVT;
		p_msg->p_sec_cback = p_cback;

		bta_sys_sendmsg(p_msg);

		return BTA_SUCCESS;
	}

	return BTA_FAILURE;
}

void BTA_DisableBluetooth(void)
{
	BT_HDR *p_msg;

	if ((p_msg = (BT_HDR *)GKI_getbuf(sizeof(BT_HDR))) != NULL)
	{
		p_msg->event = BTA_DM_API_DISABLE_EVT;
		bta_sys_sendmsg(p_msg);
	}
}

BOOLEAN BTA_DmIsDeviceUp(void)
{
	BOOLEAN is_up;

	GKI_sched_lock();
	is_up = BTM_IsDeviceUp();
	GKI_sched_unlock();

	return is_up;
}

void BTA_DmSetDeviceName(char *p_name)
{
	tBTA_DM_API_SET_NAME *p_msg;

	if ((p_msg = (tBTA_DM_API_SET_NAME *)GKI_getbuf(sizeof(tBTA_DM_API_SET_NAME))) != NULL)
	{
		p_msg->hdr.event = BTA_DM_API_SET_NAME_EVT;

		strncpy(p_msg->name, p_name, BTA_DM_REMOTE_DEVICE_NAME_LENGTH);

		bta_sys_sendmsg(p_msg);
	}
}

void BTA_DmSetVisibility(tBTA_DM_DISC disc_mode, tBTA_DM_CONN conn_mode)
{
	tBTA_DM_API_SET_VISIBILITY *p_msg;

	if ((p_msg = (tBTA_DM_API_SET_VISIBILITY *)GKI_getbuf(sizeof(tBTA_DM_MSG))) != NULL)
	{
		p_msg->hdr.event = BTA_DM_API_SET_VISIBILITY_EVT;
		p_msg->disc_mode = disc_mode;
		p_msg->conn_mode = conn_mode;

		bta_sys_sendmsg(p_msg);
	}
}

void BTA_DmSearch(tBTA_DM_INQ *p_dm_inq, tBTA_SERVICE_MASK services,
                  tBTA_DM_SEARCH_CBACK *p_cback)
{
	tBTA_DM_API_SEARCH *p_msg;

	if ((p_msg = (tBTA_DM_API_SEARCH *)GKI_getbuf(sizeof(tBTA_DM_API_SEARCH))) != NULL)
	{
		p_msg->hdr.event = BTA_DM_API_SEARCH_EVT;
		memcpy(&p_msg->inq_params, p_dm_inq, sizeof(tBTA_DM_INQ));
		p_msg->services = services;
		p_msg->p_cback = p_cback;

		bta_sys_sendmsg(p_msg);
	}
}

void BTA_DmSearchCancel(void)
{
	BT_HDR *p_msg;

	if ((p_msg = (BT_HDR *)GKI_getbuf(sizeof(BT_HDR))) != NULL)
	{
		p_msg->event = BTA_DM_API_SEARCH_CANCEL_EVT;
		bta_sys_sendmsg(p_msg);
	}
}

void BTA_DmPinReply(BD_ADDR bd_addr, BOOLEAN accept, UINT8 pin_len,
                    PIN_CODE p_pin)
{
	tBTA_DM_API_PIN_REPLY *p_msg;

	if ((p_msg = (tBTA_DM_API_PIN_REPLY *)GKI_getbuf(sizeof(tBTA_DM_API_PIN_REPLY))) != NULL)
	{
		p_msg->hdr.event = BTA_DM_API_PIN_REPLY_EVT;
		bdcpy(p_msg->bd_addr, bd_addr);
		p_msg->accept = accept;

		if (accept)
		{
			p_msg->pin_len = pin_len;
			memcpy(p_msg->p_pin, p_pin, pin_len);
		}

		bta_sys_sendmsg(p_msg);
	}
}

tBTA_STATUS BTA_DmAddDevice(BD_ADDR bd_addr, LINK_KEY link_key,
                            tBTA_SERVICE_MASK trusted_mask, BOOLEAN is_trusted)
{
	BOOLEAN added;
	UINT32 trusted_services_mask[BTM_SEC_SERVICE_ARRAY_SIZE];
	UINT8 index = 0;
	UINT8 btm_mask_index = 0;

	memset(trusted_services_mask, 0, sizeof trusted_services_mask);

	if (is_trusted)
	{
		// translate the bta service bits into btm service bits
		while (trusted_mask && index < BTA_MAX_SERVICE_ID)
		{
			if (trusted_mask & (UINT32)(1 << index))
			{
				btm_mask_index = bta_service_id_to_btm_srv_id_lkup_tbl[index] / BTM_SEC_ARRAY_BITS;

				trusted_services_mask[btm_mask_index] |=
					(UINT32)(1 << (bta_service_id_to_btm_srv_id_lkup_tbl[index]
					               - (UINT32)(btm_mask_index * 32)));

				trusted_mask &= ~(UINT32)(1 << index);
			}

			index++;
		}
	}

	GKI_sched_lock();
	added = BTM_SecAddDevice(bd_addr, NULL, NULL, NULL, trusted_services_mask, link_key);
	GKI_sched_unlock();

	return added ? BTA_SUCCESS : BTA_FAILURE;
}

tBTA_STATUS BTA_DmRemoveDevice(BD_ADDR bd_addr)
{
	BOOLEAN removed;

	GKI_sched_lock();
	removed = BTM_SecDeleteDevice(bd_addr);
	GKI_sched_unlock();

	return removed ? BTA_SUCCESS : BTA_FAILURE;
}

void BTA_DmSendHciReset(void)
{
	BT_HDR *p_msg;

	if ((p_msg = (BT_HDR *)GKI_getbuf(sizeof(BT_HDR))) != NULL)
	{
		p_msg->event = BTA_DM_API_RESET_HCI;
		bta_sys_sendmsg(p_msg);
	}
}
