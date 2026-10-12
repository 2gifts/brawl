#include <gap_int.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/gap/gap_utils.c
 */

/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
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

#include "revolution/types.h" // NULL
#include <cstring>

#include <bt_trace.h>

#include <btm_api.h>
#include <btm_int.h>
#include <gap_api.h>

// not in btm_int.h yet. names follow the usual bte naming, addresses 0x8023C8BC / 0x8023C974

void gap_find_addr_name_cb(tBTM_REMOTE_DEV_NAME *p);

/*******************************************************************************
 * functions
 */

// release a gap control block (inlined everywhere it is used in this unit)
void gap_free_cb(tGAP_INFO *p_cb)
{
	if (p_cb)
	{
		p_cb->gap_cback = NULL;
		p_cb->in_use = FALSE;
	}
}

// shared completion handler for the inquiry/discovery/remote-name requests that go through
// a gap control block. index is which of the two blocks was used
static void gap_btm_cback(UINT16 index, void *p_data)
{
	tGAP_INFO *p_cb;
	tGAP_CALLBACK *p_cback;
	tGAP_INQ_CMPL inq_cmpl;
	tBTM_INQUIRY_CMPL *p_inq = (tBTM_INQUIRY_CMPL *)p_data;
	tBTM_REMOTE_DEV_NAME *p_name = (tBTM_REMOTE_DEV_NAME *)p_data;
	void *p_out = p_data;

	if (index >= GAP_MAX_BLOCKS)
		return;

	p_cb = &gap_cb.blk[index];

	if (!p_cb->in_use)
		return;

	switch (p_cb->event)
	{
	case GAP_EVT_INQUIRY_COMPLETE:
		inq_cmpl.num_results = p_inq->num_resp;
		inq_cmpl.status = (p_inq->status == BTM_SUCCESS) ? BT_PASS : GAP_ERR_PROCESSING;
		p_out = &inq_cmpl;

		GAP_TRACE(EVENT, "   GAP Inquiry Complete Event (Status 0x%04x, Result(s) %d)", inq_cmpl.status, inq_cmpl.num_results);
		break;

	case GAP_EVT_DISCOVERY_COMPLETE:
		if (*(UINT16 *)p_data != 0)
			GAP_TRACE(EVENT, "   GAP Discovery Complete Event(SDP Result: 0x%04x)", *(UINT16 *)p_data);
		else
			GAP_TRACE(EVENT, "   GAP Discovery Successfully Completed");
		break;

	case GAP_EVT_REM_NAME_COMPLETE:
		p_name->status = gap_convert_btm_status(p_name->status);

		GAP_TRACE(EVENT, "   GAP Remote Name Complete Event (status 0x%04x)", p_name->status);
		break;
	}

	p_cback = p_cb->gap_cback;
	if (p_cback)
		(*p_cback)(p_cb->event, p_out);

	gap_free_cb(p_cb);
}

void gap_btm_cback0(void *p1)
{
	gap_btm_cback(0, p1);
}

void gap_btm_cback1(void *p1)
{
	gap_btm_cback(1, p1);
}

// not present in the dol (the linker dropped it) but its strings are still in .data between
// the functions above and below
void gap_inq_results_cb(tBTM_INQ_RESULTS *p_results)
{
	tGAP_INFO *p_cb;
	UINT8 index;

	GAP_TRACE(EVENT, "GAP Inquiry Results Callback (bdaddr [%02x%02x%02x%02x%02x%02x])",
	          p_results->remote_bd_addr[0], p_results->remote_bd_addr[1], p_results->remote_bd_addr[2],
	          p_results->remote_bd_addr[3], p_results->remote_bd_addr[4], p_results->remote_bd_addr[5]);
	GAP_TRACE(EVENT, "                             (COD [%02x%02x%02x], clkoff 0x%04x)",
	          p_results->dev_class[0], p_results->dev_class[1], p_results->dev_class[2],
	          p_results->clock_offset);

	for (index = 0, p_cb = &gap_cb.blk[0]; index < GAP_MAX_BLOCKS; index++, p_cb++)
	{
		if (p_cb->in_use && p_cb->event == GAP_EVT_INQUIRY_RESULTS)
		{
			if (p_cb->gap_inq_rslt_cback)
				(*p_cb->gap_inq_rslt_cback)(GAP_EVT_INQUIRY_RESULTS, p_results);
		}
	}
}

// remote name read finished while searching the inquiry db for a device by name
void gap_find_addr_name_cb(tBTM_REMOTE_DEV_NAME *p)
{
	tGAP_FINDADDR_CB *p_cb = &gap_cb.findaddr_cb;
	tGAP_FINDADDR_RESULTS *p_result = &p_cb->results;

	if (!p_cb->in_use)
		return;

	if (p->status == BTM_SUCCESS)
	{
		GAP_TRACE(EVENT, "   GAP: FindAddrByName Rem Name Cmpl Evt (Status 0x%04x, Name [%s])", p->status, p->remote_bd_name);

		if (strncmp((char *)p_result->devname, (char *)p->remote_bd_name, strlen((char *)p_result->devname)) == 0)
		{
			memcpy(p_result->bd_addr, p_cb->p_cur_inq->results.remote_bd_addr, BD_ADDR_LEN);
			p_result->status = BT_PASS;
		}
		else
		{
			p_cb->p_cur_inq = (tBTM_INQ_INFO *)btm_inq_db_next(p_cb->p_cur_inq);

			if (p_cb->p_cur_inq != NULL)
			{
				if (BTM_ReadRemoteDeviceName(p_cb->p_cur_inq->results.remote_bd_addr, (tBTM_CMPL_CB *)gap_find_addr_name_cb) == BTM_CMD_STARTED)
					return;

				p_result->status = gap_convert_btm_status(p->status);
			}
			else
			{
				p_result->status = GAP_EOINQDB;
			}
		}
	}
	else
	{
		GAP_TRACE(EVENT, "   GAP: FindAddrByName Rem Name Cmpl Evt (Status 0x%04x)", p->status);

		p_result->status = gap_convert_btm_status(p->status);
	}

	if (p_cb->p_cback)
		(*p_cb->p_cback)(GAP_EVT_FIND_ADDR_COMPLETE, p_result);

	p_cb->in_use = FALSE;
	p_cb->p_cback = NULL;
}

// inquiry finished while searching for a device by name: start reading names of what it found
void gap_find_addr_inq_cb(tBTM_INQUIRY_CMPL *p)
{
	tGAP_FINDADDR_CB *p_cb = &gap_cb.findaddr_cb;
	tGAP_FINDADDR_RESULTS *p_result = &p_cb->results;

	if (!p_cb->in_use)
		return;

	GAP_TRACE(EVENT, "   GAP: FindAddrByName Inq Cmpl Evt (Status 0x%04x, Result(s) %d)", p->status, p->num_resp);

	if (p->status == BTM_SUCCESS)
	{
		p_result->status = GAP_NO_DATA_AVAIL;

		p_cb->p_cur_inq = (tBTM_INQ_INFO *)btm_inq_db_first();

		if (p_cb->p_cur_inq != NULL)
		{
			if (BTM_ReadRemoteDeviceName(p_cb->p_cur_inq->results.remote_bd_addr, (tBTM_CMPL_CB *)gap_find_addr_name_cb) == BTM_CMD_STARTED)
				return;

			p_result->status = gap_convert_btm_status(p->status);
		}
		else
		{
			p_result->status = GAP_EOINQDB;
		}
	}
	else
	{
		p_result->status = gap_convert_btm_status(p->status);
	}

	if (p_cb->p_cback)
		(*p_cb->p_cback)(GAP_EVT_FIND_ADDR_COMPLETE, p_result);

	p_cb->in_use = FALSE;
	p_cb->p_cback = NULL;
}

// status -> gap error code. the compiler inlines this into every caller below as well
UINT16 gap_convert_btm_status(tBTM_STATUS btm_status)
{
	switch (btm_status)
	{
	case BTM_SUCCESS:
		return BT_PASS;

	case BTM_CMD_STARTED:
		return GAP_CMD_INITIATED;

	case BTM_BUSY:
		return GAP_ERR_BUSY;

	case BTM_MODE_UNSUPPORTED:
	case BTM_ILLEGAL_VALUE:
		return GAP_ERR_ILL_PARM;

	case BTM_WRONG_MODE:
		return GAP_DEVICE_NOT_UP;

	case BTM_UNKNOWN_ADDR:
		return GAP_BAD_BD_ADDR;

	case BTM_DEVICE_TIMEOUT:
		return GAP_ERR_TIMEOUT;

	default:
		return GAP_ERR_PROCESSING;
	}
}
