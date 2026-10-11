#include <port_api.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/rfcomm/port_api.c
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

#include <cstring>

#include <bt_target.h>
#include <rfc_int.h>

/*******************************************************************************
 * variables
 */

tRFC_CB rfc_cb;

/*******************************************************************************
 * functions
 */

void RFCOMM_Init(void)
{
	memset(&rfc_cb, 0, sizeof rfc_cb);

	rfc_cb.rfc.last_mux = MAX_BD_CONNECTIONS;
	rfc_cb.trace_level = 5; // HYPOTHESIS: initial trace level, 5 is what the asm stores

	rfcomm_l2cap_if_init();
}
