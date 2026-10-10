#include <gap_int.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/gap/gap_api.c
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

#include "revolution/types.h"
#include <cstring>

#include <bt_trace.h>

#include <gap_api.h>

/*******************************************************************************
 * variables
 */

// .bss
tGAP_CB gap_cb;

/*******************************************************************************
 * functions
 */

void GAP_Init(void)
{
	memset(&gap_cb, 0, sizeof gap_cb);

	gap_cb.btm_cback[0] = gap_btm_cback0;
	gap_cb.btm_cback[1] = gap_btm_cback1;

	gap_cb.trace_level = BT_TRACE_LEVEL_DEBUG;

	gap_conn_init();
}
