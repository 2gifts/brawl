#include <btu.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * (btu startup glue, drafted from the dol asm)
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

#include <mem.h>

#include <btm_int.h>
#include <sdp_api.h>

// these three live in the not-yet-split part of the stack, names are guesses
extern void l2c_init(void);       // fn_8024D538
extern void sdp_init(void);       // fn_80259668
extern void btm_sdp_db_init(void); // fn_8023BC98 (clears a chunk of btm_cb and sets up an sdp-ish db)

void btu_init_core(void)
{
	btm_init();
	l2c_init();
	sdp_init();
	btm_sdp_db_init();
}

// HYPOTHESIS: name. resets the btu control block to its defaults
void btu_init_cb(void)
{
	memset(&btu_cb, 0, sizeof btu_cb);

	btu_cb.hcit_acl_pkt_size = BTU_DEFAULT_DATA_SIZE + 4;
	btu_cb.controller_cmd_window = 1;
}

// HYPOTHESIS: name. makes sure the local device-id sdp record exists (vendor 0xf, primary)
// and puts it in the public browse group
BOOLEAN btu_set_local_di_record(void)
{
	tSDP_DI_GET_RECORD di;
	UINT32 handle = 0;
	UINT8 val[2];
	UINT16 browse_group = 0x1002;
	tSDP_DI_RECORD *p_rec;

	if (SDP_GetLocalDiRecord(&di, &handle) != 0)
	{
		p_rec = &di.rec;
		memset(p_rec, 0, sizeof *p_rec);

		p_rec->vendor = 0xf;
		p_rec->vendor_id_source = 1;
		p_rec->primary_record = TRUE;

		if (SDP_SetLocalDiRecord(p_rec, &handle) != 0)
		{
			handle = 0;
			return FALSE;
		}
	}

	SDP_AddUuidSequence(handle, 5, 1, &browse_group);

	val[0] = 0;
	val[1] = 1;
	SDP_AddAttribute(handle, 0x8001, 1, 2, val);

	return TRUE;
}
