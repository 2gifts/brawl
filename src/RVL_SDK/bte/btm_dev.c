#include <btm_int.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/btm/btm_dev.c
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

#include <btm_api.h>

/*******************************************************************************
 * local function declarations
 */

static tBTM_SEC_DEV_REC *btm_sec_find_oldest_dev(void);

/*******************************************************************************
 * functions
 */

BOOLEAN BTM_SecAddDevice(BD_ADDR bd_addr, DEV_CLASS dev_class, BD_NAME bd_name,
                         BD_FEATURES features, UINT32 *trusted_mask,
                         LINK_KEY link_key)
{
	tBTM_SEC_DEV_REC *p_dev_rec;

	p_dev_rec = btm_find_dev(bd_addr);

	if (!p_dev_rec)
	{
		// no record for this device yet, make one
		p_dev_rec = btm_sec_alloc_dev(bd_addr);

		if (!p_dev_rec)
			return FALSE;
	}
	else
	{
		p_dev_rec->timestamp = btm_cb.dev_rec_count++;
	}

	if (dev_class)
		memcpy(p_dev_rec->dev_class, dev_class, DEV_CLASS_LEN);

	memset(p_dev_rec->sec_bd_name, 0, sizeof(tBTM_BD_NAME));

	if (bd_name && bd_name[0])
	{
		p_dev_rec->sec_flags |= BTM_SEC_NAME_KNOWN;
		strncpy((char *)p_dev_rec->sec_bd_name, (char *)bd_name, BTM_MAX_REM_BD_NAME_LEN);
	}

	if (features)
		memcpy(p_dev_rec->features, features, sizeof(p_dev_rec->features));
	else
		memset(p_dev_rec->features, 0, sizeof(p_dev_rec->features));

	BTM_SEC_COPY_TRUSTED_DEVICE(trusted_mask, p_dev_rec->trusted_mask);

	if (link_key)
	{
		p_dev_rec->sec_flags |= BTM_SEC_LINK_KEY_KNOWN;
		memcpy(p_dev_rec->link_key, link_key, LINK_KEY_LEN);
	}

	return TRUE;
}

BOOLEAN BTM_SecDeleteDevice(BD_ADDR bd_addr)
{
	tBTM_SEC_DEV_REC *p_dev_rec;

	p_dev_rec = btm_find_dev(bd_addr);

	if (p_dev_rec == NULL)
		return FALSE;

	p_dev_rec->sec_flags = 0;

	BTM_DeleteStoredLinkKey(bd_addr, NULL);

	return TRUE;
}

char *BTM_SecReadDevName(BD_ADDR bd_addr)
{
	char *p_name = NULL;
	tBTM_SEC_DEV_REC *p_srec;

	p_srec = btm_find_dev(bd_addr);

	if (p_srec)
		p_name = (char *)p_srec->sec_bd_name;

	return p_name;
}

tBTM_SEC_DEV_REC *btm_sec_alloc_dev(BD_ADDR bd_addr)
{
	tBTM_SEC_DEV_REC *p_dev_rec = NULL;
	tBTM_INQ_INFO *p_inq_info;
	int i;

	// look for a record nobody is using first
	for (i = 0; i < BTM_SEC_MAX_DEVICE_RECORDS; i++)
	{
		if (!(btm_cb.sec_dev_rec[i].sec_flags & BTM_SEC_IN_USE))
		{
			p_dev_rec = &btm_cb.sec_dev_rec[i];
			break;
		}
	}

	if (p_dev_rec == NULL)
		p_dev_rec = btm_sec_find_oldest_dev();

	memset(p_dev_rec, 0, sizeof(tBTM_SEC_DEV_REC));

	p_dev_rec->sec_flags = BTM_SEC_IN_USE;
	p_dev_rec->sec_acl_connected = FALSE;

	// see if the inquiry db already knows what this thing is
	p_inq_info = BTM_InqDbRead(bd_addr);

	if (p_inq_info != NULL)
		memcpy(p_dev_rec->dev_class, p_inq_info->results.dev_class, DEV_CLASS_LEN);
	else
		memcpy(p_dev_rec->dev_class, btm_cb.connecting_dc, DEV_CLASS_LEN);

	memcpy(p_dev_rec->bd_addr, bd_addr, BD_ADDR_LEN);

	p_dev_rec->hci_handle = BTM_GetHCIConnHandle(bd_addr);

	p_dev_rec->timestamp = btm_cb.dev_rec_count++;

	return p_dev_rec;
}

tBTM_SEC_DEV_REC *btm_find_dev_by_handle(UINT16 handle)
{
	tBTM_SEC_DEV_REC *p_dev_rec = &btm_cb.sec_dev_rec[0];
	int i;

	for (i = 0; i < BTM_SEC_MAX_DEVICE_RECORDS; i++, p_dev_rec++)
	{
		if ((p_dev_rec->sec_flags & BTM_SEC_IN_USE) && p_dev_rec->hci_handle == handle)
			return p_dev_rec;
	}

	return NULL;
}

tBTM_SEC_DEV_REC *btm_find_dev(BD_ADDR bd_addr)
{
	tBTM_SEC_DEV_REC *p_dev_rec = &btm_cb.sec_dev_rec[0];
	int i;

	for (i = 0; i < BTM_SEC_MAX_DEVICE_RECORDS; i++, p_dev_rec++)
	{
		if ((p_dev_rec->sec_flags & BTM_SEC_IN_USE)
		    && memcmp(p_dev_rec->bd_addr, bd_addr, BD_ADDR_LEN) == 0)
		{
			return p_dev_rec;
		}
	}

	return NULL;
}

static tBTM_SEC_DEV_REC *btm_sec_find_oldest_dev(void)
{
	tBTM_SEC_DEV_REC *p_cur = &btm_cb.sec_dev_rec[0];
	tBTM_SEC_DEV_REC *p_dev_rec = p_cur;
	UINT32 ts = 0xffffffff;
	int i;

	// oldest record that never got a link key

	for (i = 0; i < BTM_SEC_MAX_DEVICE_RECORDS; i++, p_cur++)
	{
		if ((p_cur->sec_flags & BTM_SEC_IN_USE)
		    && !(p_cur->sec_flags & BTM_SEC_LINK_KEY_KNOWN)
		    && p_cur->timestamp < ts)
		{
			p_dev_rec = p_cur;
			ts = p_cur->timestamp;
		}
	}

	if (ts != 0xffffffff)
		return p_dev_rec;

	// everyone has a key, so just take the oldest of the lot
	p_cur = &btm_cb.sec_dev_rec[0];

	for (i = 0; i < BTM_SEC_MAX_DEVICE_RECORDS; i++, p_cur++)
	{
		if ((p_cur->sec_flags & BTM_SEC_IN_USE) && p_cur->timestamp < ts)
		{
			p_dev_rec = p_cur;
			ts = p_cur->timestamp;
		}
	}

	return p_dev_rec;
}
