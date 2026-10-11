#include <port_int.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/rfcomm/port_utils.c
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

#include <bt_trace.h>
#include <rfc_int.h>

/*******************************************************************************
 * functions
 */

tPORT *port_find_mcb_dlci_port(tRFC_MCB *p_mcb, UINT8 dlci)
{
	UINT8 inx;

	if (!p_mcb)
		return NULL;

	if (dlci > RFCOMM_MAX_DLCI)
		return NULL;

	inx = p_mcb->port_inx[dlci];

	if (inx == 0)
		return NULL;
	else
		return &rfc_cb.port.port[inx - 1];
}

tPORT *port_find_dlci_port(UINT8 dlci)
{
	UINT16 i;
	tPORT *p_port;

	for (i = 0; i < MAX_RFC_PORTS; i++)
	{
		p_port = &rfc_cb.port.port[i];

		if (p_port->in_use && p_port->rfc.p_mcb == NULL)
		{
			if (p_port->dlci == dlci)
			{
				return p_port;
			}
			else if ((dlci & 0x01) && p_port->dlci == (dlci - 1))
			{
				p_port->dlci++;
				return p_port;
			}
		}
	}

	return NULL;
}

UINT32 port_flow_control_user(tPORT *p_port)
{
	BOOLEAN fc;

	// the user gets flow controlled when the peer says so, the link isn't ready,
	// or the tx queue backed up too far
	fc = p_port->tx.peer_fc
	     || !p_port->rfc.p_mcb
	     || !p_port->rfc.p_mcb->peer_ready
	     || p_port->tx.queue_size > PORT_TX_HIGH_WM
	     || p_port->tx.queue.count > PORT_TX_BUF_HIGH_WM;

	if (p_port->tx.user_fc == fc)
		return 0;

	p_port->tx.user_fc = fc;

	if (fc)
		return PORT_EV_FC;
	else
		return PORT_EV_FC | PORT_EV_FCS;
}

UINT32 port_get_signal_changes(tPORT *p_port, UINT8 old_signals, UINT8 signal)
{
	UINT8 changed_signals;
	UINT32 events = 0;

	changed_signals = (signal ^ old_signals);

	if (changed_signals & PORT_DTRDSR_ON)
	{
		events |= PORT_EV_DSR;

		if (signal & PORT_DTRDSR_ON)
			events |= PORT_EV_DSRS;
	}

	if (changed_signals & PORT_CTSRTS_ON)
	{
		events |= PORT_EV_CTS;

		if (signal & PORT_CTSRTS_ON)
			events |= PORT_EV_CTSS;
	}

	if (changed_signals & PORT_RING_ON)
		events |= PORT_EV_RING;

	if (changed_signals & PORT_DCD_ON)
	{
		events |= PORT_EV_RLSD;

		if (signal & PORT_DCD_ON)
			events |= PORT_EV_RLSDS;
	}

	return p_port->ev_mask & events;
}

void port_flow_control_peer(tPORT *p_port, BOOLEAN enable, UINT16 count)
{
	if (!p_port->rfc.p_mcb)
		return;

	if (p_port->rfc.p_mcb->flow == PORT_FC_CREDIT)
	{
		if (enable)
		{
			if (count > p_port->credit_rx)
				p_port->credit_rx = 0;
			else
				p_port->credit_rx -= count;

			if (p_port->credit_rx <= p_port->credit_rx_low
			    && !p_port->rx.user_fc
			    && p_port->credit_rx_max > p_port->credit_rx)
			{
				rfc_send_credit(p_port->rfc.p_mcb, p_port->dlci,
				                (UINT8)(p_port->credit_rx_max - p_port->credit_rx));

				p_port->credit_rx = p_port->credit_rx_max;
				p_port->rx.peer_fc = FALSE;
			}
		}
		else
		{
			if (p_port->p_data_callback)
				p_port->rx.peer_fc = TRUE;
			else if (p_port->rx.queue.count >= p_port->credit_rx_max)
				p_port->rx.peer_fc = TRUE;
		}
	}
	else
	{
		if (enable)
		{
			if (p_port->rx.peer_fc
			    && p_port->rx.queue_size < PORT_RX_LOW_WM
			    && p_port->rx.queue.count < PORT_RX_BUF_LOW_WM)
			{
				p_port->rx.peer_fc = FALSE;

				if (!p_port->rx.user_fc)
					RFCOMM_FlowReq(p_port->rfc.p_mcb, p_port->dlci, TRUE);
			}
		}
		else
		{
			if (p_port->p_data_callback)
			{
				p_port->rx.peer_fc = TRUE;
				RFCOMM_FlowReq(p_port->rfc.p_mcb, p_port->dlci, FALSE);
			}
			else if (p_port->rx.queue_size > PORT_RX_HIGH_WM
			         || p_port->rx.queue.count > PORT_RX_BUF_HIGH_WM)
			{
				if (!p_port->rx.peer_fc)
				{
					RFCOMM_TRACE(EVENT, "PORT_DataInd Data reached HW. Sending FC set.");

					p_port->rx.peer_fc = TRUE;
					RFCOMM_FlowReq(p_port->rfc.p_mcb, p_port->dlci, FALSE);
				}
			}
		}
	}
}
