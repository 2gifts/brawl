#include <sdp_int.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/sdp/sdp_db.c
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
#include <sdpdefs.h>

/*******************************************************************************
 * local function declarations
 */

static BOOLEAN find_uuid_in_seq(UINT8 *p, UINT32 seq_len, UINT8 *p_uuid,
                                UINT16 uuid_len, int nest_level);

/*******************************************************************************
 * functions
 */

tSDP_RECORD *sdp_db_service_search(tSDP_RECORD *p_rec, tSDP_UUID_SEQ *p_seq)
{
	UINT16 xx, yy;
	tSDP_ATTRIBUTE *p_attr;
	tSDP_RECORD *p_end = &sdp_cb.server_db.record[sdp_cb.server_db.num_records];

	if (!p_rec)
		p_rec = &sdp_cb.server_db.record[0];
	else
		p_rec++;

	// a record matches when every uuid in the request shows up somewhere in it
	for (; p_rec < p_end; p_rec++)
	{
		for (yy = 0; yy < p_seq->num_uids; yy++)
		{
			p_attr = &p_rec->attribute[0];

			for (xx = 0; xx < p_rec->num_attributes; xx++, p_attr++)
			{
				if (p_attr->type == UUID_DESC_TYPE)
				{
					if (sdpu_compare_uuid_arrays(p_attr->value_ptr, p_attr->len,
					                             &p_seq->uuid_entry[yy].value[0],
					                             p_seq->uuid_entry[yy].len))
					{
						break;
					}
				}
				else if (p_attr->type == DATA_ELE_SEQ_DESC_TYPE)
				{
					if (find_uuid_in_seq(p_attr->value_ptr, p_attr->len,
					                     &p_seq->uuid_entry[yy].value[0],
					                     p_seq->uuid_entry[yy].len, 0))
					{
						break;
					}
				}
			}

			if (xx == p_rec->num_attributes)
				break;
		}

		if (yy == p_seq->num_uids)
			return p_rec;
	}

	return NULL;
}

static BOOLEAN find_uuid_in_seq(UINT8 *p, UINT32 seq_len, UINT8 *p_uuid,
                                UINT16 uuid_len, int nest_level)
{
	UINT8 *p_end = p + seq_len;
	UINT8 type;
	UINT32 len;

	// don't recurse forever
	if (nest_level > 3)
		return FALSE;

	while (p < p_end)
	{
		type = *p++;
		p = sdpu_get_len_from_type(p, type, &len);
		type = type >> 3;

		if (type == UUID_DESC_TYPE)
		{
			if (sdpu_compare_uuid_arrays(p, len, p_uuid, uuid_len))
				return TRUE;
		}
		else if (type == DATA_ELE_SEQ_DESC_TYPE)
		{
			if (find_uuid_in_seq(p, len, p_uuid, uuid_len, nest_level + 1))
				return TRUE;
		}

		p = p + len;
	}

	return FALSE;
}

tSDP_RECORD *sdp_db_find_record(UINT32 handle)
{
	tSDP_RECORD *p_rec;
	tSDP_RECORD *p_end = &sdp_cb.server_db.record[sdp_cb.server_db.num_records];

	for (p_rec = &sdp_cb.server_db.record[0]; p_rec < p_end; p_rec++)
	{
		if (p_rec->record_handle == handle)
			return p_rec;
	}

	return NULL;
}

tSDP_ATTRIBUTE *sdp_db_find_attr_in_rec(tSDP_RECORD *p_rec, UINT16 start_attr,
                                        UINT16 end_attr)
{
	tSDP_ATTRIBUTE *p_at;
	UINT16 xx;

	// attributes in a record are kept sorted by id
	for (xx = 0, p_at = &p_rec->attribute[0]; xx < p_rec->num_attributes;
	     xx++, p_at++)
	{
		if (p_at->id >= start_attr && p_at->id <= end_attr)
			return p_at;
	}

	return NULL;
}

UINT32 SDP_CreateRecord(void)
{
	UINT32 handle;
	UINT8 buf[4];
	tSDP_DB *p_db = &sdp_cb.server_db;

	if (p_db->num_records < SDP_MAX_RECORDS)
	{
		memset(&p_db->record[p_db->num_records], 0, sizeof(tSDP_RECORD));

		// handle is the one after the last record's, or the first handle if empty
		if (p_db->num_records)
			handle = p_db->record[p_db->num_records - 1].record_handle + 1;
		else
			handle = 0x10000;

		p_db->record[p_db->num_records].record_handle = handle;
		p_db->num_records++;

		// the handle itself is always the first attribute
		UINT32_TO_BE_FIELD(buf, handle);
		SDP_AddAttribute(handle, ATTR_ID_SERVICE_RECORD_HDL, UINT_DESC_TYPE, 4, buf);

		return p_db->record[p_db->num_records - 1].record_handle;
	}

	return 0;
}

BOOLEAN SDP_DeleteRecord(UINT32 handle)
{
	UINT16 xx, yy, zz;
	tSDP_RECORD *p_rec = &sdp_cb.server_db.record[0];

	if (handle == 0 || sdp_cb.server_db.num_records == 0)
	{
		// wipe the whole database
		sdp_cb.server_db.num_records = 0;
		sdp_cb.server_db.di_primary_handle = 0;
		sdp_cb.server_db.brcm_di_registered = FALSE;

		return TRUE;
	}
	else
	{
		for (xx = 0; xx < sdp_cb.server_db.num_records; xx++, p_rec++)
		{
			if (p_rec->record_handle == handle)
			{
				// shift everything after it up by one
				for (yy = xx; yy < sdp_cb.server_db.num_records; yy++, p_rec++)
				{
					*p_rec = *(p_rec + 1);

					for (zz = 0; zz < p_rec->num_attributes; zz++)
						p_rec->attribute[zz].value_ptr -= sizeof(tSDP_RECORD);
				}

				sdp_cb.server_db.num_records--;

				if (sdp_cb.server_db.di_primary_handle == handle)
				{
					sdp_cb.server_db.di_primary_handle = 0;
					sdp_cb.server_db.brcm_di_registered = FALSE;
				}

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOLEAN SDP_AddAttribute(UINT32 handle, UINT16 attr_id, UINT8 attr_type,
                         UINT32 attr_len, UINT8 *p_val)
{
	UINT16 xx, yy, zz;
	tSDP_RECORD *p_rec = &sdp_cb.server_db.record[0];

	for (zz = 0; zz < sdp_cb.server_db.num_records; zz++, p_rec++)
	{
		if (p_rec->record_handle == handle)
		{
			tSDP_ATTRIBUTE *p_attr = &p_rec->attribute[0];

			// see if the attribute is already there
			for (xx = 0; xx < p_rec->num_attributes; xx++, p_attr++)
			{
				if (p_attr->id == attr_id)
				{
					SDP_DeleteAttribute(handle, attr_id);
					break;
				}

				if (p_attr->id > attr_id)
					break;
			}

			if (p_rec->num_attributes == SDP_MAX_REC_ATTR)
				return FALSE;

			if (xx == p_rec->num_attributes)
			{
				p_attr = &p_rec->attribute[p_rec->num_attributes];
			}
			else
			{
				// keep the list sorted, make room
				for (yy = p_rec->num_attributes; yy > xx; yy--)
					p_rec->attribute[yy] = p_rec->attribute[yy - 1];
			}

			p_rec->num_attributes++;

			p_attr->id = attr_id;
			p_attr->len = attr_len;
			p_attr->type = attr_type;

			if (p_rec->free_pad_ptr + attr_len >= SDP_MAX_PAD_LEN)
			{
				SDP_TRACE(WARNING, "SDP_AddAttribute: attr_len:%d too long. truncate to (%d)",
				          attr_len, SDP_MAX_PAD_LEN - p_rec->free_pad_ptr);

				attr_len = SDP_MAX_PAD_LEN - p_rec->free_pad_ptr;
				p_val[SDP_MAX_PAD_LEN - p_rec->free_pad_ptr] = '\0';
				p_val[SDP_MAX_PAD_LEN - p_rec->free_pad_ptr + 1] = '\0';
			}

			if (attr_len > 0 && p_val != NULL)
			{
				memcpy(&p_rec->attr_pad[p_rec->free_pad_ptr], p_val, (size_t)attr_len);
				p_attr->value_ptr = &p_rec->attr_pad[p_rec->free_pad_ptr];
				p_rec->free_pad_ptr += attr_len;
			}

			// HYPOTHESIS: 0x8001 is the vendor attribute that marks the broadcom di record
			if (handle == sdp_cb.server_db.di_primary_handle && attr_id == 0x8001)
				sdp_cb.server_db.brcm_di_registered = TRUE;

			return TRUE;
		}
	}

	return FALSE;
}

BOOLEAN SDP_AddUuidSequence(UINT32 handle, UINT16 attr_id, UINT16 num_uuids,
                            UINT16 *p_uuids)
{
	UINT16 xx;
	UINT8 buff[SDP_MAX_ATTR_LEN * 2];
	UINT8 *p = buff;

	for (xx = 0; xx < num_uuids; xx++, p_uuids++)
	{
		UINT8_TO_BE_STREAM(p, (UUID_DESC_TYPE << 3) | SIZE_TWO_BYTES);
		UINT16_TO_BE_STREAM(p, *p_uuids);

		if ((p - buff) > (SDP_MAX_ATTR_LEN - 3))
		{
			SDP_TRACE(WARNING, "SDP_AddUuidSequence - too long, add %d uuids of %d", xx, num_uuids);
			break;
		}
	}

	return SDP_AddAttribute(handle, attr_id, DATA_ELE_SEQ_DESC_TYPE,
	                        (UINT32)(p - buff), buff);
}

BOOLEAN SDP_AddServiceClassIdList(UINT32 handle, UINT16 num_services,
                                  UINT16 *p_service_uuids)
{
	UINT16 xx;
	UINT8 buff[SDP_MAX_ATTR_LEN * 2];
	UINT8 *p = buff;

	for (xx = 0; xx < num_services; xx++, p_service_uuids++)
	{
		UINT8_TO_BE_STREAM(p, (UUID_DESC_TYPE << 3) | SIZE_TWO_BYTES);
		UINT16_TO_BE_STREAM(p, *p_service_uuids);
	}

	return SDP_AddAttribute(handle, ATTR_ID_SERVICE_CLASS_ID_LIST,
	                        DATA_ELE_SEQ_DESC_TYPE, (UINT32)(p - buff), buff);
}

BOOLEAN SDP_DeleteAttribute(UINT32 handle, UINT16 attr_id)
{
	UINT16 xx, yy;
	tSDP_RECORD *p_rec = &sdp_cb.server_db.record[0];
	UINT8 *pad_ptr;
	UINT32 len;

	for (xx = 0; xx < sdp_cb.server_db.num_records; xx++, p_rec++)
	{
		if (p_rec->record_handle == handle)
		{
			tSDP_ATTRIBUTE *p_attr = &p_rec->attribute[0];

			SDP_TRACE(API, "Deleting attr_id 0x%04x for handle 0x%x", attr_id, handle);

			for (xx = 0; xx < p_rec->num_attributes; xx++, p_attr++)
			{
				if (p_attr->id == attr_id)
				{
					pad_ptr = p_attr->value_ptr;
					len = p_attr->len;

					if (len)
					{
						for (yy = 0; yy < p_rec->num_attributes; yy++)
						{
							if (p_rec->attribute[yy].value_ptr > pad_ptr)
								p_rec->attribute[yy].value_ptr -= len;
						}
					}

					p_rec->num_attributes--;

					for (yy = xx; yy < p_rec->num_attributes; yy++, p_attr++)
						*p_attr = *(p_attr + 1);

					// close the gap in the attribute pad
					if (len)
					{
						xx = (p_rec->free_pad_ptr - ((pad_ptr + len) - &p_rec->attr_pad[0]));

						for (yy = 0; yy < xx; yy++, pad_ptr++)
							*pad_ptr = *(pad_ptr + len);

						p_rec->free_pad_ptr -= len;
					}

					return TRUE;
				}
			}
		}
	}

	return FALSE;
}
