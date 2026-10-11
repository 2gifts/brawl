#include <sdp_int.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/sdp/sdp_utils.c
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

#include <bt_trace.h>
#include <bt_types.h>

#include <btu.h>
#include <gki.h>
#include <l2c_api.h>
#include <sdpdefs.h>

/*******************************************************************************
 * variables
 */

// .rodata
static UINT8 const sdp_base_uuid[MAX_UUID_SIZE] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb,
};

/*******************************************************************************
 * functions
 */

tCONN_CB *sdpu_find_ccb_by_cid(UINT16 cid)
{
	UINT16 xx;
	tCONN_CB *p_ccb;

	for (xx = 0, p_ccb = sdp_cb.ccb; xx < SDP_MAX_CONNECTIONS; xx++, p_ccb++)
	{
		if (p_ccb->con_state != SDP_STATE_IDLE && p_ccb->connection_id == cid)
			return p_ccb;
	}

	return NULL;
}

tCONN_CB *sdpu_allocate_ccb(void)
{
	UINT16 xx;
	tCONN_CB *p_ccb;

	for (xx = 0, p_ccb = sdp_cb.ccb; xx < SDP_MAX_CONNECTIONS; xx++, p_ccb++)
	{
		if (p_ccb->con_state == SDP_STATE_IDLE)
		{
			memset(p_ccb, 0, sizeof *p_ccb);

			p_ccb->timer_entry.param = (TIMER_PARAM_TYPE)p_ccb;

			return p_ccb;
		}
	}

	return NULL;
}

void sdpu_release_ccb(tCONN_CB *p_ccb)
{
	btu_stop_timer(&p_ccb->timer_entry);

	p_ccb->con_state = SDP_STATE_IDLE;
}

// writes a data element sequence of attribute ids (or the 0x0000-0xffff wildcard if none given)
UINT8 *sdpu_build_attrib_seq(UINT8 *p_out, UINT16 *p_attr, UINT16 num_attrs)
{
	UINT16 xx;

	if (p_attr)
		xx = num_attrs * 3;
	else
		xx = 5;

	if (xx > 255)
	{
		UINT8_TO_BE_STREAM(p_out, (DATA_ELE_SEQ_DESC_TYPE << 3) | SIZE_IN_NEXT_WORD);
		UINT16_TO_BE_STREAM(p_out, xx);
	}
	else
	{
		UINT8_TO_BE_STREAM(p_out, (DATA_ELE_SEQ_DESC_TYPE << 3) | SIZE_IN_NEXT_BYTE);
		UINT8_TO_BE_STREAM(p_out, xx);
	}

	if (!p_attr)
	{
		UINT8_TO_BE_STREAM(p_out, (UINT_DESC_TYPE << 3) | SIZE_FOUR_BYTES);
		UINT16_TO_BE_STREAM(p_out, 0);
		UINT16_TO_BE_STREAM(p_out, 0xffff);
	}
	else
	{
		for (xx = 0; xx < num_attrs; xx++, p_attr++)
		{
			UINT8_TO_BE_STREAM(p_out, (UINT_DESC_TYPE << 3) | SIZE_TWO_BYTES);
			UINT16_TO_BE_STREAM(p_out, *p_attr);
		}
	}

	return p_out;
}

// writes "attribute id, attribute value" for one record attribute
UINT8 *sdpu_build_attrib_entry(UINT8 *p_out, tSDP_ATTRIBUTE *p_attr)
{
	int xx;

	UINT8_TO_BE_STREAM(p_out, (UINT_DESC_TYPE << 3) | SIZE_TWO_BYTES);
	UINT16_TO_BE_STREAM(p_out, p_attr->id);

	switch (p_attr->type)
	{
	case TEXT_STR_DESC_TYPE:
	case DATA_ELE_SEQ_DESC_TYPE:
	case DATA_ELE_ALT_DESC_TYPE:
	case URL_DESC_TYPE:
		// variable length types always get a one byte length
		UINT8_TO_BE_STREAM(p_out, (p_attr->type << 3) | SIZE_IN_NEXT_BYTE);
		UINT8_TO_BE_STREAM(p_out, p_attr->len);

		for (xx = 0; xx < (INT32)p_attr->len; xx++)
			*p_out++ = p_attr->value_ptr[xx];

		return p_out;

	default:
		switch (p_attr->len)
		{
		case 1:
			UINT8_TO_BE_STREAM(p_out, (p_attr->type << 3) | SIZE_ONE_BYTE);
			break;

		case 2:
			UINT8_TO_BE_STREAM(p_out, (p_attr->type << 3) | SIZE_TWO_BYTES);
			break;

		case 4:
			UINT8_TO_BE_STREAM(p_out, (p_attr->type << 3) | SIZE_FOUR_BYTES);
			break;

		case 8:
			UINT8_TO_BE_STREAM(p_out, (p_attr->type << 3) | SIZE_EIGHT_BYTES);
			break;

		case 16:
			UINT8_TO_BE_STREAM(p_out, (p_attr->type << 3) | SIZE_SIXTEEN_BYTES);
			break;

		default:
			UINT8_TO_BE_STREAM(p_out, (p_attr->type << 3) | SIZE_IN_NEXT_BYTE);
			UINT8_TO_BE_STREAM(p_out, p_attr->len);
			break;
		}

		for (xx = 0; xx < (INT32)p_attr->len; xx++)
			*p_out++ = p_attr->value_ptr[xx];

		return p_out;
	}
}

void sdpu_build_n_send_error(tCONN_CB *p_ccb, UINT16 trans_num, UINT16 error_code, char *p_error_text)
{
	UINT8 *p_rsp, *p_rsp_start, *p_rsp_param_len;
	UINT16 rsp_param_len;
	BT_HDR *p_buf;

	SDP_TRACE(WARNING, "SDP - sdpu_build_n_send_error  code: 0x%x  CID: 0x%x", error_code, p_ccb->connection_id);

	if ((p_buf = (BT_HDR *)GKI_getpoolbuf(SDP_POOL_ID)) == NULL)
	{
		SDP_TRACE(ERROR, "SDP - no buf for err msg");
		return;
	}

	p_buf->offset = L2CAP_MIN_OFFSET;
	p_rsp = p_rsp_start = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

	UINT8_TO_BE_STREAM(p_rsp, SDP_PDU_ERROR_RESPONSE);
	UINT16_TO_BE_STREAM(p_rsp, trans_num);

	// parameter length gets filled in at the end
	p_rsp_param_len = p_rsp;
	p_rsp += 2;

	UINT16_TO_BE_STREAM(p_rsp, error_code);

	if (p_error_text)
		ARRAY_TO_BE_STREAM(p_rsp, p_error_text, (int)strlen(p_error_text));

	rsp_param_len = p_rsp - p_rsp_param_len - 2;
	UINT16_TO_BE_STREAM(p_rsp_param_len, rsp_param_len);

	p_buf->len = p_rsp - p_rsp_start;

	L2CA_DataWrite(p_ccb->connection_id, p_buf);
}

// reads a data element sequence of uuids out of a request
UINT8 *sdpu_extract_uid_seq(UINT8 *p, UINT16 param_len, tSDP_UUID_SEQ *p_seq)
{
	UINT8 *p_seq_end;
	UINT8 descr, type, size;
	UINT32 seq_len, uuid_len;
	UINT16 u16;
	UINT32 u32;
	int xx;

	p_seq->num_uids = 0;

	descr = *p++;
	type = descr >> 3;
	size = descr & 7;

	if (type != DATA_ELE_SEQ_DESC_TYPE)
		return NULL;

	switch (size)
	{
	case SIZE_TWO_BYTES:
		seq_len = 2;
		break;

	case SIZE_FOUR_BYTES:
		seq_len = 4;
		break;

	case SIZE_SIXTEEN_BYTES:
		seq_len = 16;
		break;

	case SIZE_IN_NEXT_BYTE:
		seq_len = *p++;
		break;

	case SIZE_IN_NEXT_WORD:
		BE_STREAM_TO_UINT16(p, &u16);
		seq_len = u16;
		break;

	case SIZE_IN_NEXT_LONG:
		BE_STREAM_TO_UINT32(p, &u32);
		seq_len = u32;
		break;

	default:
		return NULL;
	}

	if (seq_len >= param_len)
		return NULL;

	p_seq_end = p + seq_len;

	for (; p < p_seq_end;)
	{
		descr = *p++;
		type = descr >> 3;
		size = descr & 7;

		if (type != UUID_DESC_TYPE)
			return NULL;

		switch (size)
		{
		case SIZE_TWO_BYTES:
			uuid_len = 2;
			break;

		case SIZE_FOUR_BYTES:
			uuid_len = 4;
			break;

		case SIZE_SIXTEEN_BYTES:
			uuid_len = 16;
			break;

		case SIZE_IN_NEXT_BYTE:
			uuid_len = *p++;
			break;

		case SIZE_IN_NEXT_WORD:
			BE_STREAM_TO_UINT16(p, &u16);
			uuid_len = u16;
			break;

		case SIZE_IN_NEXT_LONG:
			BE_STREAM_TO_UINT32(p, &u32);
			uuid_len = u32;
			break;

		default:
			return NULL;
		}

		if (uuid_len == 2 || uuid_len == 4 || uuid_len == 16)
		{
			p_seq->uuid_entry[p_seq->num_uids].len = (UINT16)uuid_len;

			for (xx = 0; xx < (int)uuid_len; xx++)
				p_seq->uuid_entry[p_seq->num_uids].value[xx] = *p++;

			p_seq->num_uids++;
		}
		else
		{
			return NULL;
		}

		if (p_seq->num_uids >= MAX_UUIDS_PER_SEQ)
			return NULL;
	}

	if (p != p_seq_end)
		return NULL;

	return p;
}

// reads a data element sequence of attribute ids / id ranges out of a request
UINT8 *sdpu_extract_attr_seq(UINT8 *p, UINT16 param_len, tSDP_ATTR_SEQ *p_seq)
{
	UINT8 *p_end_list;
	UINT8 descr, type, size;
	UINT32 list_len, attr_len;
	UINT16 u16;
	UINT32 u32;

	p_seq->num_attr = 0;

	descr = *p++;
	type = descr >> 3;
	size = descr & 7;

	if (type != DATA_ELE_SEQ_DESC_TYPE)
		return p;

	switch (size)
	{
	case SIZE_IN_NEXT_BYTE:
		list_len = *p++;
		break;

	case SIZE_IN_NEXT_WORD:
		BE_STREAM_TO_UINT16(p, &u16);
		list_len = u16;
		break;

	case SIZE_IN_NEXT_LONG:
		BE_STREAM_TO_UINT32(p, &u32);
		list_len = u32;
		break;

	default:
		return p;
	}

	if (list_len > param_len)
		return p;

	p_end_list = p + list_len;

	for (; p < p_end_list;)
	{
		descr = *p++;
		type = descr >> 3;
		size = descr & 7;

		if (type != UINT_DESC_TYPE)
			return p;

		switch (size)
		{
		case SIZE_TWO_BYTES:
			attr_len = 2;
			break;

		case SIZE_FOUR_BYTES:
			attr_len = 4;
			break;

		case SIZE_IN_NEXT_BYTE:
			attr_len = *p++;
			break;

		case SIZE_IN_NEXT_WORD:
			BE_STREAM_TO_UINT16(p, &u16);
			attr_len = u16;
			break;

		case SIZE_IN_NEXT_LONG:
			BE_STREAM_TO_UINT32(p, &u32);
			attr_len = u32;
			break;

		default:
			return NULL;
		}

		if (attr_len == 2)
		{
			BE_STREAM_TO_UINT16(p, &p_seq->attr_entry[p_seq->num_attr].start);
			p_seq->attr_entry[p_seq->num_attr].end = p_seq->attr_entry[p_seq->num_attr].start;
		}
		else if (attr_len == 4)
		{
			BE_STREAM_TO_UINT16(p, &p_seq->attr_entry[p_seq->num_attr].start);
			BE_STREAM_TO_UINT16(p, &p_seq->attr_entry[p_seq->num_attr].end);
		}
		else
		{
			return NULL;
		}

		if (++p_seq->num_attr >= MAX_ATTR_PER_SEQ)
			return NULL;
	}

	return p;
}

// decodes the length that goes with a data element header's size bits
UINT8 *sdpu_get_len_from_type(UINT8 *p, UINT8 type, UINT32 *p_len)
{
	UINT8 u8;
	UINT16 u16;
	UINT32 u32;

	switch (type & 7)
	{
	case SIZE_ONE_BYTE:
		*p_len = 1;
		break;

	case SIZE_TWO_BYTES:
		*p_len = 2;
		break;

	case SIZE_FOUR_BYTES:
		*p_len = 4;
		break;

	case SIZE_EIGHT_BYTES:
		*p_len = 8;
		break;

	case SIZE_SIXTEEN_BYTES:
		*p_len = 16;
		break;

	case SIZE_IN_NEXT_BYTE:
		BE_STREAM_TO_UINT8(p, &u8);
		*p_len = u8;
		break;

	case SIZE_IN_NEXT_WORD:
		BE_STREAM_TO_UINT16(p, &u16);
		*p_len = u16;
		break;

	case SIZE_IN_NEXT_LONG:
		BE_STREAM_TO_UINT32(p, &u32);
		*p_len = (UINT16)u32;
		break;
	}

	return p;
}

// does this 128 bit uuid end with the bluetooth base uuid (so it's really a 16/32 bit one)
BOOLEAN sdpu_is_base_uuid(UINT8 *p_uuid)
{
	UINT16 xx;

	for (xx = 4; xx < MAX_UUID_SIZE; xx++)
	{
		if (p_uuid[xx] != sdp_base_uuid[xx])
			return FALSE;
	}

	return TRUE;
}

// compares uuids that may have different sizes by widening the shorter one to the base uuid form
BOOLEAN sdpu_compare_uuid_arrays(UINT8 *p_uuid1, UINT32 len1, UINT8 *p_uuid2, UINT16 len2)
{
	UINT8 nu1[MAX_UUID_SIZE];
	UINT8 nu2[MAX_UUID_SIZE];

	if (len1 == len2)
	{
		if (len1 == 2)
		{
			return p_uuid1[0] == p_uuid2[0] && p_uuid1[1] == p_uuid2[1];
		}
		else if (len1 == 4)
		{
			return p_uuid1[0] == p_uuid2[0] && p_uuid1[1] == p_uuid2[1] && p_uuid1[2] == p_uuid2[2] && p_uuid1[3] == p_uuid2[3];
		}
		else
		{
			return memcmp(p_uuid1, p_uuid2, (int)len1) == 0;
		}
	}
	else if (len1 > len2)
	{
		if (len1 == 4)
		{
			return p_uuid1[0] == 0 && p_uuid1[1] == 0 && p_uuid1[2] == p_uuid2[0] && p_uuid1[3] == p_uuid2[1];
		}
		else
		{
			memcpy(nu1, p_uuid1, MAX_UUID_SIZE);
			memcpy(nu2, sdp_base_uuid, MAX_UUID_SIZE);

			if (len2 == 4)
				memcpy(nu2, p_uuid2, len2);
			else
				memcpy(nu2 + 2, p_uuid2, len2);

			return memcmp(nu1, nu2, MAX_UUID_SIZE) == 0;
		}
	}
	else
	{
		if (len2 == 4)
		{
			return p_uuid2[0] == 0 && p_uuid2[1] == 0 && p_uuid2[2] == p_uuid1[0] && p_uuid2[3] == p_uuid1[1];
		}
		else
		{
			memcpy(nu2, p_uuid2, MAX_UUID_SIZE);
			memcpy(nu1, sdp_base_uuid, MAX_UUID_SIZE);

			if (len1 == 4)
				memcpy(nu1, p_uuid1, len1);
			else
				memcpy(nu1 + 2, p_uuid1, len1);

			return memcmp(nu1, nu2, MAX_UUID_SIZE) == 0;
		}
	}
}

BOOLEAN sdpu_compare_uuid_with_attr(tBT_UUID *p_btuuid, tSDP_DISC_ATTR *p_attr)
{
	UINT16 attr_len = SDP_DISC_ATTR_LEN(p_attr->attr_len_type);

	if (p_btuuid->len != attr_len)
		return FALSE;

	if (p_btuuid->len == 2)
		return (BOOLEAN)(p_btuuid->uu.uuid16 == p_attr->attr_value.v.u16);

	if (p_btuuid->len == 4)
		return (BOOLEAN)(p_btuuid->uu.uuid32 == p_attr->attr_value.v.u32);

	if (!memcmp(p_btuuid->uu.uuid128, p_attr->attr_value.v.array, MAX_UUID_SIZE))
		return TRUE;

	return FALSE;
}

// little bubble sort over the discovery db's attribute filter list
void sdpu_sort_attr_list(UINT16 num_attr, tSDP_DISCOVERY_DB *p_db)
{
	UINT16 i;
	UINT16 x;

	if (num_attr <= 1)
		return;

	for (x = 0; x < num_attr - 1; x++)
	{
		if (p_db->attr_filters[x] > p_db->attr_filters[x + 1])
		{
			i = p_db->attr_filters[x];
			p_db->attr_filters[x] = p_db->attr_filters[x + 1];
			p_db->attr_filters[x + 1] = i;

			x = -1;
		}
	}
}
