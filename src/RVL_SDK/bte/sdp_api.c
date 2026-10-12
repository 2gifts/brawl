#include <sdp_api.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/sdp/sdp_api.c
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
#include <sdp_int.h>
#include <sdpdefs.h>

/*******************************************************************************
 * functions
 */

BOOLEAN SDP_InitDiscoveryDb(tSDP_DISCOVERY_DB *p_db, UINT32 len,
                            UINT16 num_uuid, tSDP_UUID *p_uuid_list,
                            UINT16 num_attr, UINT16 *p_attr_list)
{
	UINT16 xx;

	if (p_db == NULL || sizeof(tSDP_DISCOVERY_DB) > len
	    || num_attr > SDP_MAX_ATTR_FILTERS || num_uuid > SDP_MAX_UUID_FILTERS)
	{
		SDP_TRACE(ERROR, "SDP_InitDiscoveryDb Illegal param: p_db 0x%x, len %d, num_uuid %d, num_attr %d",
		          p_db, len, num_uuid, num_attr);

		return FALSE;
	}

	memset(p_db, 0, (size_t)len);

	p_db->mem_size = len - sizeof(tSDP_DISCOVERY_DB);
	p_db->mem_free = p_db->mem_size;
	p_db->p_first_rec = NULL;
	p_db->p_free_mem = (UINT8 *)(p_db + 1);

	for (xx = 0; xx < num_uuid; xx++)
		p_db->uuid_filters[xx] = *p_uuid_list++;

	p_db->num_uuid_filters = num_uuid;

	for (xx = 0; xx < num_attr; xx++)
		p_db->attr_filters[xx] = *p_attr_list++;

	sdpu_sort_attr_list(num_attr, p_db);

	p_db->num_attr_filters = num_attr;

	return TRUE;
}

BOOLEAN SDP_ServiceSearchRequest(UINT8 *p_bd_addr, tSDP_DISCOVERY_DB *p_db,
                                 tSDP_DISC_CMPL_CB *p_cb)
{
	tCONN_CB *p_ccb;

	p_ccb = sdp_conn_originate(p_bd_addr);

	if (!p_ccb)
		return FALSE;

	p_ccb->disc_state = SDP_DISC_WAIT_CONN;
	p_ccb->p_db = p_db;
	p_ccb->p_cb = p_cb;

	return TRUE;
}

BOOLEAN SDP_ServiceSearchAttributeRequest(UINT8 *p_bd_addr,
                                          tSDP_DISCOVERY_DB *p_db,
                                          tSDP_DISC_CMPL_CB *p_cb)
{
	tCONN_CB *p_ccb;

	p_ccb = sdp_conn_originate(p_bd_addr);

	if (!p_ccb)
		return FALSE;

	p_ccb->disc_state = SDP_DISC_WAIT_CONN;
	p_ccb->p_db = p_db;
	p_ccb->p_cb = p_cb;

	p_ccb->is_attr_search = TRUE;

	return TRUE;
}

tSDP_DISC_ATTR *SDP_FindAttributeInRec(tSDP_DISC_REC *p_rec, UINT16 attr_id)
{
	tSDP_DISC_ATTR *p_attr;

	p_attr = p_rec->p_first_attr;

	while (p_attr)
	{
		if (p_attr->attr_id == attr_id)
			return p_attr;

		p_attr = p_attr->p_next_attr;
	}

	return NULL;
}

tSDP_DISC_REC *SDP_FindServiceInDb(tSDP_DISCOVERY_DB *p_db,
                                   UINT16 service_uuid,
                                   tSDP_DISC_REC *p_start_rec)
{
	tSDP_DISC_REC *p_rec;
	tSDP_DISC_ATTR *p_attr, *p_sattr;

	if (!p_start_rec)
		p_rec = p_db->p_first_rec;
	else
		p_rec = p_start_rec->p_next_rec;

	while (p_rec)
	{
		p_attr = p_rec->p_first_attr;

		while (p_attr)
		{
			if (p_attr->attr_id == ATTR_ID_SERVICE_CLASS_ID_LIST
			    && SDP_DISC_ATTR_TYPE(p_attr->attr_len_type) == DATA_ELE_SEQ_DESC_TYPE)
			{
				for (p_sattr = p_attr->attr_value.v.p_sub_attr; p_sattr;
				     p_sattr = p_sattr->p_next_attr)
				{
					if (SDP_DISC_ATTR_TYPE(p_sattr->attr_len_type) == UUID_DESC_TYPE
					    && SDP_DISC_ATTR_LEN(p_sattr->attr_len_type) == 2)
					{
						if (p_sattr->attr_value.v.u16 == service_uuid)
							return p_rec;
					}
				}

				break;
			}
			else if (p_attr->attr_id == ATTR_ID_SERVICE_ID)
			{
				if (SDP_DISC_ATTR_TYPE(p_attr->attr_len_type) == UUID_DESC_TYPE
				    && SDP_DISC_ATTR_LEN(p_attr->attr_len_type) == 2)
				{
					if (p_attr->attr_value.v.u16 == service_uuid)
						return p_rec;
				}
			}

			p_attr = p_attr->p_next_attr;
		}

		p_rec = p_rec->p_next_rec;
	}

	return NULL;
}

tSDP_DISC_REC *SDP_FindServiceUUIDInDb(tSDP_DISCOVERY_DB *p_db,
                                       tBT_UUID *p_uuid,
                                       tSDP_DISC_REC *p_start_rec)
{
	tSDP_DISC_REC *p_rec;
	tSDP_DISC_ATTR *p_attr, *p_sattr;

	if (!p_start_rec)
		p_rec = p_db->p_first_rec;
	else
		p_rec = p_start_rec->p_next_rec;

	while (p_rec)
	{
		p_attr = p_rec->p_first_attr;

		while (p_attr)
		{
			if (p_attr->attr_id == ATTR_ID_SERVICE_CLASS_ID_LIST
			    && SDP_DISC_ATTR_TYPE(p_attr->attr_len_type) == DATA_ELE_SEQ_DESC_TYPE)
			{
				for (p_sattr = p_attr->attr_value.v.p_sub_attr; p_sattr;
				     p_sattr = p_sattr->p_next_attr)
				{
					if (SDP_DISC_ATTR_TYPE(p_sattr->attr_len_type) == UUID_DESC_TYPE)
					{
						if (sdpu_compare_uuid_with_attr(p_uuid, p_sattr))
							return p_rec;
					}
				}

				break;
			}
			else if (p_attr->attr_id == ATTR_ID_SERVICE_ID)
			{
				if (SDP_DISC_ATTR_TYPE(p_attr->attr_len_type) == UUID_DESC_TYPE)
				{
					if (sdpu_compare_uuid_with_attr(p_uuid, p_attr))
						return p_rec;
				}
			}

			p_attr = p_attr->p_next_attr;
		}

		p_rec = p_rec->p_next_rec;
	}

	return NULL;
}

UINT16 SDP_SetLocalDiRecord(tSDP_DI_RECORD *p_device_info, UINT32 *p_handle)
{
	UINT16 result = SDP_SUCCESS;
	UINT32 handle;
	UINT16 di_uuid = UUID_SERVCLASS_PNP_INFORMATION;
	UINT8 temp_u16[2];

	*p_handle = 0;

	if (p_device_info == NULL)
		return SDP_ILLEGAL_PARAMETER;

	// reuse the primary record if there already is one
	if (p_device_info->primary_record == TRUE
	    && sdp_cb.server_db.di_primary_handle)
	{
		handle = sdp_cb.server_db.di_primary_handle;
	}
	else
	{
		if ((handle = SDP_CreateRecord()) == 0)
			return SDP_NO_RESOURCES;
	}

	*p_handle = handle;

	if (SDP_AddServiceClassIdList(handle, 1, &di_uuid) == FALSE)
		result = SDP_DI_REG_FAILED;

	// HYPOTHESIS: specification id the asm stores is 0x0009, not 0x0103 like upstream
	if (result == SDP_SUCCESS)
	{
		temp_u16[0] = 0;
		temp_u16[1] = 9;

		if (!SDP_AddAttribute(handle, ATTR_ID_SPECIFICATION_ID, UINT_DESC_TYPE, 2, temp_u16))
			result = SDP_DI_REG_FAILED;
	}

	if (result == SDP_SUCCESS)
	{
		if (p_device_info->client_executable_url[0] != '\0')
		{
			if (strlen(p_device_info->client_executable_url) + 1 > SDP_MAX_ATTR_LEN
			    || !SDP_AddAttribute(handle, ATTR_ID_CLIENT_EXE_URL, URL_DESC_TYPE,
			                         (UINT32)(strlen(p_device_info->client_executable_url) + 1),
			                         (UINT8 *)p_device_info->client_executable_url))
			{
				result = SDP_DI_REG_FAILED;
			}
		}
	}

	if (result == SDP_SUCCESS)
	{
		if (p_device_info->service_description[0] != '\0')
		{
			if (strlen(p_device_info->service_description) + 1 > SDP_MAX_ATTR_LEN
			    || !SDP_AddAttribute(handle, ATTR_ID_SERVICE_DESCRIPTION, TEXT_STR_DESC_TYPE,
			                         (UINT32)(strlen(p_device_info->service_description) + 1),
			                         (UINT8 *)p_device_info->service_description))
			{
				result = SDP_DI_REG_FAILED;
			}
		}
	}

	if (result == SDP_SUCCESS)
	{
		if (p_device_info->documentation_url[0] != '\0')
		{
			if (strlen(p_device_info->documentation_url) + 1 > SDP_MAX_ATTR_LEN
			    || !SDP_AddAttribute(handle, ATTR_ID_DOCUMENTATION_URL, URL_DESC_TYPE,
			                         (UINT32)(strlen(p_device_info->documentation_url) + 1),
			                         (UINT8 *)p_device_info->documentation_url))
			{
				result = SDP_DI_REG_FAILED;
			}
		}
	}

	if (result == SDP_SUCCESS)
	{
		UINT16_TO_BE_FIELD(temp_u16, p_device_info->vendor);

		if (!SDP_AddAttribute(handle, ATTR_ID_VENDOR_ID, UINT_DESC_TYPE, 2, temp_u16))
			result = SDP_DI_REG_FAILED;
	}

	if (result == SDP_SUCCESS)
	{
		UINT16_TO_BE_FIELD(temp_u16, p_device_info->product);

		if (!SDP_AddAttribute(handle, ATTR_ID_PRODUCT_ID, UINT_DESC_TYPE, 2, temp_u16))
			result = SDP_DI_REG_FAILED;
	}

	if (result == SDP_SUCCESS)
	{
		UINT16_TO_BE_FIELD(temp_u16, p_device_info->version);

		if (!SDP_AddAttribute(handle, ATTR_ID_PRODUCT_VERSION, UINT_DESC_TYPE, 2, temp_u16))
			result = SDP_DI_REG_FAILED;
	}

	if (result == SDP_SUCCESS)
	{
		if (!SDP_AddAttribute(handle, ATTR_ID_PRIMARY_RECORD, BOOLEAN_DESC_TYPE, 1,
		                      (UINT8 *)&p_device_info->primary_record))
		{
			result = SDP_DI_REG_FAILED;
		}
	}

	if (result == SDP_SUCCESS)
	{
		UINT16_TO_BE_FIELD(temp_u16, p_device_info->vendor_id_source);

		if (!SDP_AddAttribute(handle, ATTR_ID_VENDOR_ID_SOURCE, UINT_DESC_TYPE, 2, temp_u16))
			result = SDP_DI_REG_FAILED;
	}

	if (result != SDP_SUCCESS)
		SDP_DeleteRecord(handle);
	else if (p_device_info->primary_record == TRUE)
		sdp_cb.server_db.di_primary_handle = handle;

	return result;
}

UINT16 SDP_GetLocalDiRecord(tSDP_DI_GET_RECORD *p_device_info, UINT32 *p_handle)
{
	UINT16 result = SDP_NO_DI_RECORD_FOUND;
	tSDP_RECORD *p_rec;
	tSDP_ATTRIBUTE *p_attr;
	UINT8 *p_src;
	int len;
	int i;

	if (*p_handle == 0)
		*p_handle = sdp_cb.server_db.di_primary_handle;

	if ((p_rec = sdp_db_find_record(*p_handle)) != NULL)
	{
		memset(p_device_info, 0, sizeof(tSDP_DI_RECORD));

		result = SDP_SUCCESS;

		if ((p_attr = sdp_db_find_attr_in_rec(p_rec, ATTR_ID_SPECIFICATION_ID, ATTR_ID_SPECIFICATION_ID)) != NULL)
			p_device_info->spec_id = (p_attr->value_ptr[0] << 8) + p_attr->value_ptr[1];

		if ((p_attr = sdp_db_find_attr_in_rec(p_rec, ATTR_ID_VENDOR_ID, ATTR_ID_VENDOR_ID)) != NULL)
			p_device_info->rec.vendor = (p_attr->value_ptr[0] << 8) + p_attr->value_ptr[1];

		if ((p_attr = sdp_db_find_attr_in_rec(p_rec, ATTR_ID_PRODUCT_ID, ATTR_ID_PRODUCT_ID)) != NULL)
			p_device_info->rec.product = (p_attr->value_ptr[0] << 8) + p_attr->value_ptr[1];

		if ((p_attr = sdp_db_find_attr_in_rec(p_rec, ATTR_ID_PRODUCT_VERSION, ATTR_ID_PRODUCT_VERSION)) != NULL)
			p_device_info->rec.version = (p_attr->value_ptr[0] << 8) + p_attr->value_ptr[1];

		if ((p_attr = sdp_db_find_attr_in_rec(p_rec, ATTR_ID_VENDOR_ID_SOURCE, ATTR_ID_VENDOR_ID_SOURCE)) != NULL)
			p_device_info->rec.vendor_id_source = (p_attr->value_ptr[0] << 8) + p_attr->value_ptr[1];

		if ((p_attr = sdp_db_find_attr_in_rec(p_rec, ATTR_ID_PRIMARY_RECORD, ATTR_ID_PRIMARY_RECORD)) != NULL)
			p_device_info->rec.primary_record = (BOOLEAN)p_attr->value_ptr[0];

		if ((p_attr = sdp_db_find_attr_in_rec(p_rec, ATTR_ID_CLIENT_EXE_URL, ATTR_ID_CLIENT_EXE_URL)) != NULL)
		{
			len = (p_attr->len < SDP_MAX_ATTR_LEN) ? p_attr->len : SDP_MAX_ATTR_LEN;

			p_src = p_attr->value_ptr;

			for (i = 0; i < len; i++)
				p_device_info->rec.client_executable_url[i] = *p_src++;
		}

		if ((p_attr = sdp_db_find_attr_in_rec(p_rec, ATTR_ID_SERVICE_DESCRIPTION, ATTR_ID_SERVICE_DESCRIPTION)) != NULL)
		{
			len = (p_attr->len < SDP_MAX_ATTR_LEN) ? p_attr->len : SDP_MAX_ATTR_LEN;

			p_src = p_attr->value_ptr;

			for (i = 0; i < len; i++)
				p_device_info->rec.service_description[i] = *p_src++;
		}

		if ((p_attr = sdp_db_find_attr_in_rec(p_rec, ATTR_ID_DOCUMENTATION_URL, ATTR_ID_DOCUMENTATION_URL)) != NULL)
		{
			len = (p_attr->len < SDP_MAX_ATTR_LEN) ? p_attr->len : SDP_MAX_ATTR_LEN;

			p_src = p_attr->value_ptr;

			for (i = 0; i < len; i++)
				p_device_info->rec.documentation_url[i] = *p_src++;
		}
	}
	else
	{
		*p_handle = 0;
	}

	return result;
}

UINT8 SDP_SetTraceLevel(UINT8 new_level)
{
	if (new_level != 0xff)
		sdp_cb.trace_level = new_level;

	return sdp_cb.trace_level;
}
