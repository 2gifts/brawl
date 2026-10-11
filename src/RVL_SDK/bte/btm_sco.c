#include <btm_int.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/btm/btm_sco.c
 */

/* Includes changes by muff1n1634 */

/*******************************************************************************
 * headers
 */

#include <cstring>
#include <stddef.h>

#include <bt_target.h>
#include <bt_trace.h>
#include <bt_types.h>
#include <data_types.h>

#include <btm_api.h>
#include <gki.h>
#include <hcidefs.h>
#include <hcimsgs.h>

/*******************************************************************************
 * local function declarations
 */

static void btm_esco_conn_rsp(UINT16 sco_inx, UINT8 hci_status, BD_ADDR bda,
                              tBTM_ESCO_PARAMS *p_parms);
static tBTM_STATUS btm_send_connect_request(UINT16 acl_handle,
                                            tBTM_ESCO_PARAMS *p_setup);

/*******************************************************************************
 * variables
 */

const tBTM_ESCO_PARAMS btm_esco_defaults =
{
	8000,	/* tx_bw */
	8000,	/* rx_bw */
	0x000a,	/* max_latency */
	0x0060,	/* voice_contfmt */
	0x003f,	/* packet_types */
	1		/* retrans_effort */
};

/*******************************************************************************
 * functions
 */

void btm_sco_init(void)
{
	btm_cb.sco_cb.sco_disc_reason = BTM_INVALID_SCO_DISC_REASON;
	btm_cb.sco_cb.def_esco_parms = btm_esco_defaults;
	btm_cb.sco_cb.desired_sco_mode = BTM_LINK_TYPE_ESCO;
}

static void btm_esco_conn_rsp(UINT16 sco_inx, UINT8 hci_status, BD_ADDR bda,
                              tBTM_ESCO_PARAMS *p_parms)
{
	tSCO_CONN *p_sco = NULL;
	UINT16 temp_pkt_types;
	void *p_buf;

	if ((p_buf = GKI_getpoolbuf(HCI_CMD_POOL_ID)) == NULL)
	{
		BTM_TRACE(ERROR, "btm_esco_conn_rsp -> No Resources");
		return;
	}

	if (sco_inx < BTM_MAX_SCO_LINKS)
		p_sco = &btm_cb.sco_cb.sco_db[sco_inx];

	if (hci_status != HCI_SUCCESS)
	{
		if (p_sco)
		{
			p_sco->state = (p_sco->state == SCO_ST_W4_CONN_RSP)
			                   ? SCO_ST_LISTENING
			                   : SCO_ST_UNUSED;
		}

		if (!btm_cb.sco_cb.esco_supported)
			btsnd_hcic_reject_conn(p_buf, bda, hci_status);
		else
			btsnd_hcic_reject_esco_conn(p_buf, bda, hci_status);
	}
	else
	{
		btm_chg_all_acl_pkt_types(TRUE);

		p_sco->state = SCO_ST_CONNECTING;

		if (btm_cb.sco_cb.esco_supported
		    && p_sco->esco.data.link_type == BTM_LINK_TYPE_ESCO)
		{
			tBTM_ESCO_PARAMS *p_setup = &p_sco->esco.setup;

			if (p_parms)
				*p_setup = *p_parms;

			p_setup->packet_types = btm_cb.sco_cb.def_esco_parms.packet_types;

			temp_pkt_types = p_setup->packet_types
			               & BTM_SCO_SUPPORTED_PKTS_MASK
			               & btm_cb.btm_sco_pkt_types_supported;

			if (!(temp_pkt_types & BTM_ESCO_LINK_ONLY_MASK))
				temp_pkt_types |= BTM_SCO_PKT_TYPES_MASK_EV3;

			if (btm_cb.devcb.local_version.hci_version >= HCI_PROTO_VERSION_2_0)
			{
				temp_pkt_types |= (p_setup->packet_types
				                   | btm_cb.btm_sco_pkt_types_supported)
				                & BTM_SCO_EXCEPTION_PKTS_MASK;
			}

			btsnd_hcic_accept_esco_conn(
				p_buf, bda, p_setup->tx_bw, p_setup->rx_bw,
				p_setup->max_latency, p_setup->voice_contfmt,
				p_setup->retrans_effort, temp_pkt_types);

			p_setup->packet_types = temp_pkt_types;
		}
		else
		{
			btsnd_hcic_accept_conn(p_buf, bda, HCI_ROLE_MASTER);
		}
	}
}

// HYPOTHESIS: BTM_CreateSco never made it into the final dol (dead stripped),
// so all that survives is its string pool entries.  The body here is a loose
// guess at what bluedroid did, written just to get the strings in the right
// order.
tBTM_STATUS BTM_CreateSco(BD_ADDR remote_bda, BOOLEAN is_orig,
                          UINT16 pkt_types, UINT16 *p_sco_inx,
                          tBTM_SCO_CB *p_conn_cb, tBTM_SCO_CB *p_disc_cb)
{
	tSCO_CONN *p = &btm_cb.sco_cb.sco_db[0];
	UINT16 xx;
	UINT16 acl_handle = 0;

	*p_sco_inx = BTM_INVALID_SCO_INDEX;

	if (is_orig)
	{
		if (!remote_bda)
			return BTM_ILLEGAL_VALUE;

		if ((acl_handle = BTM_GetHCIConnHandle(remote_bda)) == 0xffff)
			return BTM_UNKNOWN_ADDR;
	}
	else
	{
		BTM_TRACE(API, "TCS accept SCO: Packet Types 0x%04x", pkt_types);
	}

	for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++)
	{
		if (p->state == SCO_ST_UNUSED)
		{
			p->is_orig = is_orig;
			p->p_conn_cb = p_conn_cb;
			p->p_disc_cb = p_disc_cb;
			p->hci_handle = BTM_INVALID_HCI_HANDLE;
			p->rem_bd_known = remote_bda != NULL;

			if (is_orig)
			{
				BTM_TRACE(API, "BTM_CreateSco -> (e)SCO Link for ACL handle 0x%04x, Desired Type %d",
				          acl_handle, btm_cb.sco_cb.desired_sco_mode);

				if (btm_send_connect_request(acl_handle, &p->esco.setup)
				    != BTM_CMD_STARTED)
				{
					p->state = SCO_ST_UNUSED;
					return BTM_NO_RESOURCES;
				}
			}

			*p_sco_inx = xx;
			return BTM_CMD_STARTED;
		}
	}

	return BTM_NO_RESOURCES;
}

static tBTM_STATUS btm_send_connect_request(UINT16 acl_handle,
                                            tBTM_ESCO_PARAMS *p_setup)
{
	UINT16 temp_pkt_types;
	BOOLEAN ok;

	if (!btm_cb.sco_cb.esco_supported)
	{
		ok = btsnd_hcic_add_SCO_conn(acl_handle, BTM_ESCO_2_SCO(p_setup->packet_types));
		if (!ok)
			return BTM_NO_RESOURCES;
	}
	else
	{
		temp_pkt_types = p_setup->packet_types & BTM_SCO_SUPPORTED_PKTS_MASK
		               & btm_cb.btm_sco_pkt_types_supported;

		if (btm_cb.devcb.local_version.hci_version >= HCI_PROTO_VERSION_2_0)
		{
			temp_pkt_types |= (p_setup->packet_types
			                   | btm_cb.btm_sco_pkt_types_supported)
			                & BTM_SCO_EXCEPTION_PKTS_MASK;
		}

		BTM_TRACE(API, "      txbw 0x%x, rxbw 0x%x, lat 0x%x, voice 0x%x, retrans 0x%02x, pkt 0x%04x",
		          p_setup->tx_bw, p_setup->rx_bw, p_setup->max_latency,
		          p_setup->voice_contfmt, p_setup->retrans_effort,
		          temp_pkt_types);

		ok = btsnd_hcic_setup_esco_conn(
				acl_handle, p_setup->tx_bw, p_setup->rx_bw,
				p_setup->max_latency, p_setup->voice_contfmt,
				p_setup->retrans_effort, temp_pkt_types);
		if (!ok)
			return BTM_NO_RESOURCES;

		p_setup->packet_types = temp_pkt_types;
	}

	return BTM_CMD_STARTED;
}

void btm_sco_chk_pend_unpark(UINT8 hci_status, UINT16 hci_handle, UINT8 mode)
{
	UINT16 xx;
	tSCO_CONN *p = &btm_cb.sco_cb.sco_db[0];
	UINT16 acl_handle;

	for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++)
	{
		if (p->state == SCO_ST_PEND_UNPARK && mode == BTM_PM_STS_ACTIVE
		    && hci_status == HCI_SUCCESS
		    && (acl_handle = BTM_GetHCIConnHandle(p->esco.data.bd_addr))
		           == hci_handle)
		{
			BTM_TRACE(API, "btm_sco_chk_pend_unpark -> (e)SCO Link for ACL handle 0x%04x, Desired Type %d",
			          acl_handle, btm_cb.sco_cb.desired_sco_mode);

			btm_chg_all_acl_pkt_types(TRUE);

			if (btm_send_connect_request(acl_handle, &p->esco.setup)
			    == BTM_CMD_STARTED)
			{
				p->state = SCO_ST_CONNECTING;
			}
		}
	}
}

void btm_sco_conn_req(BD_ADDR bda, DEV_CLASS dev_class, UINT8 link_type)
{
	tSCO_CONN *p = &btm_cb.sco_cb.sco_db[0];
	UINT16 xx;
	tBTM_ESCO_CONN_REQ_EVT_DATA evt_data;

	for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++)
	{
		if (((p->state == SCO_ST_LISTENING && p->rem_bd_known)
		     || p->state == SCO_ST_CONNECTING)
		    && !memcmp(p->esco.data.bd_addr, bda, BD_ADDR_LEN))
		{
			p->rem_bd_known = TRUE;
			p->esco.data.link_type = link_type;
			memcpy(p->esco.data.bd_addr, bda, BD_ADDR_LEN);

			if (p->esco.p_esco_cback == NULL)
			{
				btm_esco_conn_rsp(xx, HCI_SUCCESS, bda, NULL);
			}
			else
			{
				memcpy(evt_data.bd_addr, bda, BD_ADDR_LEN);
				memcpy(evt_data.dev_class, dev_class, DEV_CLASS_LEN);
				evt_data.link_type = link_type;
				evt_data.sco_inx = xx;
				p->state = SCO_ST_W4_CONN_RSP;

				(*p->esco.p_esco_cback)(BTM_ESCO_CONN_REQ_EVT,
				                        (tBTM_ESCO_EVT_DATA *)&evt_data);
			}

			return;
		}
	}

	if (btm_cb.sco_cb.app_sco_ind_cb)
	{
		for (xx = 0, p = &btm_cb.sco_cb.sco_db[0]; xx < BTM_MAX_SCO_LINKS; xx++, p++)
		{
			if (p->state == SCO_ST_UNUSED)
			{
				p->is_orig = FALSE;
				p->state = SCO_ST_LISTENING;
				p->esco.data.link_type = link_type;
				memcpy(p->esco.data.bd_addr, bda, BD_ADDR_LEN);
				p->rem_bd_known = TRUE;
				break;
			}
		}

		if (xx < BTM_MAX_SCO_LINKS)
		{
			(*btm_cb.sco_cb.app_sco_ind_cb)(xx);
			return;
		}
	}

	BTM_TRACE(WARNING, "btm_sco_conn_req: No one wants this SCO connection; rejecting it");

	btm_esco_conn_rsp(BTM_MAX_SCO_LINKS, HCI_ERR_HOST_REJECT_RESOURCES, bda,
	                  NULL);
}

void btm_sco_connected(UINT8 hci_status, BD_ADDR bda, UINT16 hci_handle,
                       tBTM_ESCO_DATA *p_esco_data)
{
	tSCO_CONN *p = &btm_cb.sco_cb.sco_db[0];
	UINT16 xx;
	BOOLEAN spt = FALSE;
	tBTM_CHG_ESCO_PARAMS parms;

	btm_cb.sco_cb.sco_disc_reason = hci_status;

	for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++)
	{
		if ((p->state == SCO_ST_LISTENING || p->state == SCO_ST_W4_CONN_RSP
		     || p->state == SCO_ST_CONNECTING)
		    && p->rem_bd_known
		    && (!bda || !memcmp(p->esco.data.bd_addr, bda, BD_ADDR_LEN)))
		{
			if (hci_status != HCI_SUCCESS)
			{
				if (p->state == SCO_ST_CONNECTING)
				{
					if (hci_status != HCI_ERR_LMP_ERR_TRANS_COLLISION)
					{
						p->state = SCO_ST_UNUSED;
						(*p->p_disc_cb)(xx);
					}
				}
				else
				{
					p->state = SCO_ST_LISTENING;
				}

				return;
			}

			if (p->state == SCO_ST_LISTENING)
				spt = TRUE;

			p->state = SCO_ST_CONNECTED;
			p->hci_handle = hci_handle;

			if (!btm_cb.sco_cb.esco_supported)
			{
				p->esco.data.link_type = BTM_LINK_TYPE_SCO;

				if (spt)
				{
					parms.packet_types = p->esco.setup.packet_types;
					parms.max_latency = p->esco.setup.max_latency;
					parms.retrans_effort = p->esco.setup.retrans_effort;

					BTM_ChangeEScoLinkParms(xx, &parms);
				}
			}
			else
			{
				if (p_esco_data)
					p->esco.data = *p_esco_data;
			}

			(*p->p_conn_cb)(xx);
			return;
		}
	}
}

tBTM_STATUS BTM_RemoveSco(UINT16 sco_inx)
{
	tSCO_CONN *p = &btm_cb.sco_cb.sco_db[sco_inx];
	UINT16 tempstate;

	if (sco_inx >= BTM_MAX_SCO_LINKS || p->state == SCO_ST_UNUSED)
		return BTM_UNKNOWN_ADDR;

	if (p->hci_handle == BTM_INVALID_HCI_HANDLE
	    || p->state == SCO_ST_PEND_UNPARK)
	{
		p->hci_handle = BTM_INVALID_HCI_HANDLE;
		p->state = SCO_ST_UNUSED;
		p->esco.p_esco_cback = NULL;

		return BTM_SUCCESS;
	}

	tempstate = p->state;
	p->state = SCO_ST_DISCONNECTING;

	if (!btsnd_hcic_disconnect(p->hci_handle, HCI_ERR_PEER_USER))
	{
		p->state = tempstate;
		return BTM_NO_RESOURCES;
	}

	return BTM_CMD_STARTED;
}

void btm_remove_sco_links(BD_ADDR bda)
{
	tSCO_CONN *p = &btm_cb.sco_cb.sco_db[0];
	UINT16 xx;

	for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++)
	{
		if (p->rem_bd_known && !memcmp(p->esco.data.bd_addr, bda, BD_ADDR_LEN))
			BTM_RemoveSco(xx);
	}
}

void btm_sco_removed(UINT16 hci_handle, UINT8 reason)
{
	tSCO_CONN *p = &btm_cb.sco_cb.sco_db[0];
	UINT16 xx;

	btm_cb.sco_cb.sco_disc_reason = reason;

	if (BTM_NumScoLinks() <= 1)
		btm_chg_all_acl_pkt_types(FALSE);

	p = &btm_cb.sco_cb.sco_db[0];
	for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++)
	{
		if (p->state != SCO_ST_UNUSED && p->state != SCO_ST_LISTENING
		    && p->hci_handle == hci_handle)
		{
			p->state = SCO_ST_UNUSED;
			p->hci_handle = BTM_INVALID_HCI_HANDLE;
			p->rem_bd_known = FALSE;
			p->esco.p_esco_cback = NULL;

			(*p->p_disc_cb)(xx);
			return;
		}
	}
}

void btm_sco_acl_removed(BD_ADDR bda)
{
	tSCO_CONN *p = &btm_cb.sco_cb.sco_db[0];
	UINT16 xx;

	for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++)
	{
		if (p->state != SCO_ST_UNUSED)
		{
			if (!bda
			    || (!memcmp(p->esco.data.bd_addr, bda, BD_ADDR_LEN)
			        && p->rem_bd_known))
			{
				p->state = SCO_ST_UNUSED;
				p->esco.p_esco_cback = NULL;

				(*p->p_disc_cb)(xx);
			}
		}
	}
}

// HYPOTHESIS: the next two (and BTM_ReadEScoLinkParms) are stripped, only
// their strings are left, so the bodies are guesses.
tBTM_STATUS BTM_SetEScoMode(tBTM_SCO_TYPE sco_mode, tBTM_ESCO_PARAMS *p_parms)
{
	tBTM_ESCO_PARAMS *p_def = &btm_cb.sco_cb.def_esco_parms;

	BTM_TRACE(API, "BTM_SetEScoMode -> mode %d", sco_mode);

	if (!btm_cb.sco_cb.esco_supported)
	{
		BTM_TRACE(API, "BTM_SetEScoMode -> mode SCO (eSCO not supported)");
		sco_mode = BTM_LINK_TYPE_SCO;
	}
	else if (p_parms)
	{
		*p_def = *p_parms;
	}

	btm_cb.sco_cb.desired_sco_mode = sco_mode;

	BTM_TRACE(API, "    txbw 0x%08x, rxbw 0x%08x, max_lat 0x%04x, voice 0x%04x, pkt 0x%04x, rtx effort 0x%02x",
	          p_def->tx_bw, p_def->rx_bw, p_def->max_latency,
	          p_def->voice_contfmt, p_def->packet_types,
	          p_def->retrans_effort);

	return BTM_SUCCESS;
}

tBTM_STATUS BTM_RegForEScoEvts(UINT16 sco_inx, tBTM_ESCO_CBACK *p_esco_cback)
{
	if (sco_inx >= BTM_MAX_SCO_LINKS
	    || btm_cb.sco_cb.sco_db[sco_inx].state == SCO_ST_UNUSED)
	{
		return BTM_ILLEGAL_VALUE;
	}

	btm_cb.sco_cb.sco_db[sco_inx].esco.p_esco_cback = p_esco_cback;

	return BTM_SUCCESS;
}

tBTM_STATUS BTM_ReadEScoLinkParms(UINT16 sco_inx, tBTM_ESCO_DATA *p_parms)
{
	BTM_TRACE(API, "BTM_ReadEScoLinkParms -> sco_inx 0x%04x", sco_inx);

	if (sco_inx >= BTM_MAX_SCO_LINKS
	    || btm_cb.sco_cb.sco_db[sco_inx].state != SCO_ST_CONNECTED)
	{
		return BTM_WRONG_MODE;
	}

	*p_parms = btm_cb.sco_cb.sco_db[sco_inx].esco.data;

	return BTM_SUCCESS;
}

void btm_route_sco_data(BT_HDR *p_msg)
{
	GKI_freebuf(p_msg);
}

tBTM_STATUS BTM_ChangeEScoLinkParms(UINT16 sco_inx,
                                    tBTM_CHG_ESCO_PARAMS *p_parms)
{
	tBTM_ESCO_PARAMS *p_setup;
	tSCO_CONN *p_sco;
	UINT16 temp_pkt_types;

	if (sco_inx >= BTM_MAX_SCO_LINKS
	    || btm_cb.sco_cb.sco_db[sco_inx].state != SCO_ST_CONNECTED)
	{
		return BTM_WRONG_MODE;
	}

	p_sco = &btm_cb.sco_cb.sco_db[sco_inx];
	p_setup = &p_sco->esco.setup;

	if (p_sco->esco.data.link_type == BTM_LINK_TYPE_SCO
	    || !btm_cb.sco_cb.esco_supported)
	{
		p_setup->packet_types = p_parms->packet_types
		                      & (btm_cb.btm_sco_pkt_types_supported
		                         & BTM_SCO_LINK_ONLY_MASK);

		BTM_TRACE(API, "BTM_ChangeEScoLinkParms -> SCO Link for handle 0x%04x, pkt 0x%04x",
		          p_sco->hci_handle, p_setup->packet_types);

		if (!btsnd_hcic_change_conn_type(
				p_sco->hci_handle, BTM_ESCO_2_SCO(p_setup->packet_types)))
		{
			return BTM_NO_RESOURCES;
		}
	}
	else
	{
		temp_pkt_types = p_parms->packet_types & BTM_SCO_SUPPORTED_PKTS_MASK
		               & btm_cb.btm_sco_pkt_types_supported;

		if (btm_cb.devcb.local_version.hci_version >= HCI_PROTO_VERSION_2_0)
		{
			temp_pkt_types |= (p_parms->packet_types
			                   | btm_cb.btm_sco_pkt_types_supported)
			                & BTM_SCO_EXCEPTION_PKTS_MASK;
		}

		BTM_TRACE(API, "BTM_ChangeEScoLinkParms -> eSCO Link for handle 0x%04x",
		          p_sco->hci_handle);

		BTM_TRACE(API, "      txbw 0x%x, rxbw 0x%x, lat 0x%x, voice 0x%x, retrans 0x%02x, pkt 0x%04x",
		          p_setup->tx_bw, p_setup->rx_bw, p_parms->max_latency,
		          p_setup->voice_contfmt, p_parms->retrans_effort,
		          temp_pkt_types);

		if (!btsnd_hcic_setup_esco_conn(
				p_sco->hci_handle, p_setup->tx_bw, p_setup->rx_bw,
				p_parms->max_latency, p_setup->voice_contfmt,
				p_parms->retrans_effort, temp_pkt_types))
		{
			return BTM_NO_RESOURCES;
		}

		p_parms->packet_types = temp_pkt_types;
	}

	return BTM_CMD_STARTED;
}

void btm_esco_proc_conn_chg(UINT8 status, UINT16 handle, UINT8 tx_interval,
                            UINT8 retrans_window, UINT16 rx_pkt_len,
                            UINT16 tx_pkt_len)
{
	tSCO_CONN *p = &btm_cb.sco_cb.sco_db[0];
	tBTM_CHG_ESCO_EVT_DATA data;
	UINT16 xx;

	BTM_TRACE(EVENT, "btm_esco_proc_conn_chg -> handle 0x%04x, status 0x%02x",
	          handle, status);

	for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++)
	{
		if (p->state == SCO_ST_CONNECTED && handle == p->hci_handle)
		{
			if (p->esco.p_esco_cback)
			{
				memcpy(data.bd_addr, p->esco.data.bd_addr, BD_ADDR_LEN);
				data.hci_status = status;
				data.sco_inx = xx;
				data.rx_pkt_len = p->esco.data.rx_pkt_len = rx_pkt_len;
				data.tx_pkt_len = p->esco.data.tx_pkt_len = tx_pkt_len;
				data.tx_interval = p->esco.data.tx_interval = tx_interval;
				data.retrans_window = p->esco.data.retrans_window
				    = retrans_window;

				(*p->esco.p_esco_cback)(BTM_ESCO_CHG_EVT,
				                        (tBTM_ESCO_EVT_DATA *)&data);
			}

			return;
		}
	}
}

BOOLEAN btm_is_sco_active(UINT16 handle)
{
	tSCO_CONN *p = &btm_cb.sco_cb.sco_db[0];
	UINT16 xx;

	for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++)
	{
		if (handle == p->hci_handle && p->state == SCO_ST_CONNECTED)
			return TRUE;
	}

	return FALSE;
}

UINT8 BTM_NumScoLinks(void)
{
	tSCO_CONN *p = &btm_cb.sco_cb.sco_db[0];
	UINT16 xx;
	UINT8 num_scos = 0;

	for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++)
	{
		switch (p->state)
		{
		case SCO_ST_W4_CONN_RSP:
		case SCO_ST_CONNECTING:
		case SCO_ST_CONNECTED:
		case SCO_ST_DISCONNECTING:
		case SCO_ST_PEND_UNPARK:
			num_scos++;
			break;

		default:
			break;
		}
	}

	return num_scos;
}

BOOLEAN btm_is_sco_active_by_bdaddr(BD_ADDR remote_bda)
{
	UINT16 xx;
	tSCO_CONN *p;

	for (xx = 0, p = &btm_cb.sco_cb.sco_db[0]; xx < BTM_MAX_SCO_LINKS;
	     xx++, p++)
	{
		if (!memcmp(p->esco.data.bd_addr, remote_bda, BD_ADDR_LEN)
		    && p->state == SCO_ST_CONNECTED)
		{
			return TRUE;
		}
	}

	return FALSE;
}
