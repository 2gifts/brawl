#include <l2c_int.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/l2cap/l2c_link.c
 */

/* Includes changes by muff1n1634 */

/*******************************************************************************
 * headers
 */

#include <stddef.h>
#include <cstring>

#include <bt_target.h>
#include <bt_trace.h>
#include <bt_types.h>
#include <data_types.h>

#include <btm_api.h>
#include <btm_int.h>
#include <btu.h>
#include <gki.h>
#include <hcidefs.h>
#include <hcimsgs.h>
#include <l2c_api.h>
#include <l2cdefs.h>

/*******************************************************************************
 * functions
 */

void l2c_link_hci_conn_req(BD_ADDR bd_addr)
{
	tL2C_LCB *p_lcb;
	void *p_buf;

	if ((p_buf = GKI_getpoolbuf(HCI_CMD_POOL_ID)) == NULL)
		return;

	if ((p_lcb = l2cu_find_lcb_by_bd_addr(bd_addr)) == NULL)
	{
		if ((p_lcb = l2cu_allocate_lcb(bd_addr)) == NULL)
		{
			btsnd_hcic_reject_conn(p_buf, bd_addr, HCI_ERR_PEER_LOW_RESOURCES);

			L2CAP_TRACE(ERROR, "L2CAP failed to allocate LCB");
		}
		else
		{
			UINT8 role = l2cu_get_conn_role(p_lcb);

			p_lcb->link_role = role;

			btsnd_hcic_accept_conn(p_buf, bd_addr, role);

			p_lcb->link_state = LST_CONNECTING;
			btu_start_timer(&p_lcb->timer_entry, BTU_TTYPE_L2CAP_LINK,
			                L2CAP_LINK_CONNECT_TOUT);
		}
	}
	else if (p_lcb->link_state == LST_CONNECTING
	         || p_lcb->link_state == LST_CONNECT_HOLDING)
	{
		btsnd_hcic_accept_conn(p_buf, bd_addr, p_lcb->link_role);

		p_lcb->link_state = LST_CONNECTING;
	}
	else if (p_lcb->link_state == LST_DISCONNECTING)
	{
		btsnd_hcic_reject_conn(p_buf, bd_addr, HCI_ERR_PEER_POWER_OFF);
	}
	else
	{
		L2CAP_TRACE(ERROR, "L2CAP got conn_req while connected");

		GKI_freebuf(p_buf);
	}
}

BOOLEAN l2c_link_hci_conn_comp(UINT8 status, UINT16 handle, BD_ADDR p_bda)
{
	tL2C_CONN_INFO ci;
	tL2C_LCB *p_lcb;
	tL2C_CCB *p_ccb;
	tBTM_SEC_DEV_REC *p_dev_info;

	ci.status = status;
	memcpy(ci.bd_addr, p_bda, BD_ADDR_LEN);

	if ((p_lcb = l2cu_find_lcb_by_bd_addr(ci.bd_addr)) == NULL)
	{
		// HYPOTHESIS: this is whatever fn_80240F8C really is; it reads like
		// "is this handle one we already know about"
		if (!btm_sec_is_bonding(handle))
			L2CAP_TRACE(WARNING, "L2CAP got conn_comp for unknown BD_ADDR");

		return FALSE;
	}

	if (p_lcb->link_state != LST_CONNECTING)
	{
		L2CAP_TRACE(ERROR, "L2CAP got conn_comp in bad state: %d  status: 0x%d",
		            p_lcb->link_state, status);

		if (status != HCI_SUCCESS)
			l2c_link_hci_disc_comp(p_lcb->handle, status);

		return FALSE;
	}

	p_lcb->handle = handle;

	if (ci.status == HCI_SUCCESS)
	{
		p_lcb->link_state = LST_CONNECTED;

		if ((p_dev_info = btm_find_dev(p_bda)) != NULL)
		{
			btm_acl_created(ci.bd_addr, p_dev_info->dev_class,
			                p_dev_info->sec_bd_name, handle, p_lcb->link_role);
		}
		else
		{
			btm_acl_created(ci.bd_addr, NULL, NULL, handle, p_lcb->link_role);
		}

		l2c_process_held_packets(FALSE);

		btu_stop_timer(&p_lcb->timer_entry);

		for (p_ccb = p_lcb->p_first_ccb; p_ccb; p_ccb = p_ccb->p_next_ccb)
			l2c_csm_execute(p_ccb, L2CEVT_LP_CONNECT_CFM, &ci);

		if (p_lcb->p_echo_rsp_cb)
		{
			l2cu_send_peer_echo_req(p_lcb, NULL, 0);
			btu_start_timer(&p_lcb->timer_entry, BTU_TTYPE_L2CAP_LINK,
			                L2CAP_ECHO_RSP_TOUT);
		}
		else if (!p_lcb->p_first_ccb)
		{
			btu_start_timer(&p_lcb->timer_entry, BTU_TTYPE_L2CAP_LINK,
			                L2CAP_LINK_CONNECT_TOUT);
		}
	}
	else if (ci.status == HCI_ERR_MAX_NUM_OF_CONNECTIONS
	         && l2cu_lcb_disconnecting())
	{
		p_lcb->link_state = LST_CONNECT_HOLDING;
		p_lcb->handle = 0xffff;
	}
	else
	{
		p_ccb = p_lcb->p_first_ccb;
		while (p_ccb)
		{
			tL2C_CCB *pn = p_ccb->p_next_ccb;

			l2c_csm_execute(p_ccb, L2CEVT_LP_CONNECT_CFM_NEG, &ci);

			p_ccb = pn;
		}

		l2cu_release_lcb(p_lcb);
	}

	return TRUE;
}

void l2c_link_sec_comp(BD_ADDR p_bda, void *p_ref_data, UINT8 status)
{
	tL2C_CONN_INFO ci;
	tL2C_LCB *p_lcb;
	tL2C_CCB *p_ccb;
	tL2C_CCB *p_next_ccb;
	UINT8 event;

	ci.status = status;
	memcpy(ci.bd_addr, p_bda, BD_ADDR_LEN);

	if ((p_lcb = l2cu_find_lcb_by_bd_addr(p_bda)) == NULL)
	{
		L2CAP_TRACE(WARNING, "L2CAP got sec_comp for unknown BD_ADDR");
		return;
	}

	for (p_ccb = p_lcb->p_first_ccb; p_ccb; p_ccb = p_next_ccb)
	{
		p_next_ccb = p_ccb->p_next_ccb;

		event = (status == BTM_SUCCESS) ? L2CEVT_SEC_COMP : L2CEVT_SEC_COMP_NEG;
		l2c_csm_execute(p_ccb, event, &ci);
	}
}

BOOLEAN l2c_link_hci_disc_comp(UINT16 handle, UINT8 reason)
{
	UINT8 ci_status;
	tL2C_LCB *p_lcb;
	tL2C_CCB *p_ccb;
	BOOLEAN status = TRUE;

	ci_status = reason;

	if ((p_lcb = l2cu_find_lcb_by_handle(handle)) == NULL)
	{
		status = FALSE;
	}
	else
	{
		if (btm_cb.acl_disc_reason != HCI_ERR_HOST_REJECT_SECURITY)
			btm_cb.acl_disc_reason = ci_status;

		p_ccb = p_lcb->p_first_ccb;
		while (p_ccb)
		{
			tL2C_CCB *pn = p_ccb->p_next_ccb;

			l2c_csm_execute(p_ccb, L2CEVT_LP_DISCONNECT_IND, &ci_status);

			p_ccb = pn;
		}

		btm_sco_acl_removed(p_lcb->remote_bd_addr);

		l2cu_release_lcb(p_lcb);
	}

	if ((p_lcb = l2cu_find_lcb_by_state(LST_CONNECT_HOLDING)) != NULL)
		l2cu_create_conn(p_lcb);

	return status;
}

BOOLEAN l2c_link_hci_qos_violation(UINT16 handle)
{
	tL2C_LCB *p_lcb;
	tL2C_CCB *p_ccb;

	if ((p_lcb = l2cu_find_lcb_by_handle(handle)) == NULL)
		return FALSE;

	for (p_ccb = p_lcb->p_first_ccb; p_ccb; p_ccb = p_ccb->p_next_ccb)
	{
		if (p_ccb->p_rcb->api.pL2CA_QoSViolationInd_Cb)
			l2c_csm_execute(p_ccb, L2CEVT_LP_QOS_VIOLATION_IND, NULL);
	}

	return TRUE;
}

void l2c_link_timeout(tL2C_LCB *p_lcb)
{
	tL2C_CCB *p_ccb;
	UINT16 timeout;
	tBTM_STATUS rc;

	if (p_lcb->link_state == LST_CONNECTING_WAIT_SWITCH
	    || p_lcb->link_state == LST_CONNECTING
	    || p_lcb->link_state == LST_CONNECT_HOLDING
	    || p_lcb->link_state == LST_DISCONNECTING)
	{
		p_ccb = p_lcb->p_first_ccb;
		while (p_ccb)
		{
			tL2C_CCB *pn = p_ccb->p_next_ccb;

			l2c_csm_execute(p_ccb, L2CEVT_LP_DISCONNECT_IND, NULL);

			p_ccb = pn;
		}

		l2cu_release_lcb(p_lcb);
	}

	if (p_lcb->link_state == LST_CONNECTED)
	{
		if (p_lcb->p_echo_rsp_cb)
		{
			tL2CA_ECHO_RSP_CB *p_cb = p_lcb->p_echo_rsp_cb;

			p_lcb->p_echo_rsp_cb = NULL;

			(*p_cb)(L2CAP_PING_RESULT_NO_RESP);

			L2CAP_TRACE(WARNING, "L2CAP - ping timeout");

			p_ccb = p_lcb->p_first_ccb;
			while (p_ccb)
			{
				tL2C_CCB *pn = p_ccb->p_next_ccb;

				l2c_csm_execute(p_ccb, L2CEVT_LP_DISCONNECT_IND, NULL);

				p_ccb = pn;
			}
		}

		if (p_lcb->p_first_ccb == NULL)
		{
			rc = btm_sec_disconnect(p_lcb->handle, HCI_ERR_PEER_USER);

			if (rc == BTM_CMD_STARTED)
			{
				p_lcb->link_state = LST_DISCONNECTING;
				timeout = L2CAP_LINK_DISCONNECT_TOUT;
			}
			else if (rc == BTM_SUCCESS)
			{
				p_lcb->link_state = LST_DISCONNECTING;
				timeout = 0xffff;
			}
			else
			{
				timeout = BT_1SEC_TIMEOUT;
			}

			if (timeout != 0xffff)
			{
				btu_start_timer(&p_lcb->timer_entry, BTU_TTYPE_L2CAP_LINK,
				                timeout);
			}
		}
		else
		{
			l2c_link_check_send_pkts(p_lcb, NULL, NULL);
		}
	}
}

BOOLEAN l2c_link_send_to_lower(tL2C_LCB *p_lcb, BT_HDR *p_buf)
{
	UINT16 xmit_window;
	UINT16 acl_data_size;
	UINT16 num_segs;

	if (p_buf->len <= btu_cb.hcit_acl_pkt_size)
	{
		p_lcb->link_xmit_window--;
		p_lcb->sent_not_acked++;
		l2cb.controller_xmit_window--;

		bte_hcisu_send(p_buf, BT_EVT_TO_LM_HCI_ACL);
	}
	else
	{
		acl_data_size = btu_cb.hcit_acl_data_size;

		num_segs = (p_buf->len - 4 + acl_data_size - 1) / acl_data_size;

		xmit_window = l2cb.controller_xmit_window;

		if (num_segs > xmit_window)
		{
			p_buf->layer_specific = xmit_window;
			num_segs = xmit_window;
			p_lcb->partial_segment_being_sent = TRUE;
		}

		if (num_segs > p_lcb->link_xmit_window)
		{
			p_buf->layer_specific = num_segs = p_lcb->link_xmit_window;
			p_lcb->partial_segment_being_sent = TRUE;
		}

		l2cb.controller_xmit_window -= num_segs;
		p_lcb->link_xmit_window -= num_segs;
		p_lcb->sent_not_acked += num_segs;

		bte_hcisu_send(p_buf, BT_EVT_TO_LM_HCI_ACL);
	}

	return TRUE;
}

void l2c_link_check_send_pkts(tL2C_LCB *p_lcb, tL2C_CCB *p_ccb, BT_HDR *p_buf)
{
	tBTM_PM_MODE mode;
	tBTM_PM_PWR_MD pm;
	tL2C_CCB *p_cur_ccb;

	if (p_buf)
	{
		if (p_lcb->link_xmit_data_q.count >= p_lcb->link_cong_start
		    && !p_lcb->cong_sent)
		{
			for (p_cur_ccb = p_lcb->p_first_ccb; p_cur_ccb;
			     p_cur_ccb = p_cur_ccb->p_next_ccb)
			{
				L2CAP_TRACE(WARNING, "L2CAP - Congested(), CID: 0x%04x, Congested",
				            p_cur_ccb->local_cid);

				if (p_cur_ccb->p_rcb->api.pL2CA_CongestionStatus_Cb)
				{
					L2CAP_TRACE(API, "L2CAP - Calling CongestionStatus_Cb(), CID: 0x%04x, Congested",
					            p_cur_ccb->local_cid);

					(*p_cur_ccb->p_rcb->api.pL2CA_CongestionStatus_Cb)(
						p_cur_ccb->local_cid, TRUE);
				}
			}

			p_lcb->cong_sent = TRUE;
		}

		if (p_lcb->link_xmit_data_q.count <= p_lcb->link_cong_discard)
		{
			p_buf->layer_specific = 0;
			GKI_enqueue(&p_lcb->link_xmit_data_q, p_buf);
		}
		else
		{
			GKI_freebuf(p_buf);

			L2CAP_TRACE(WARNING, "!!! L2CAP - buffer dropped");
		}
	}

	if (p_lcb->partial_segment_being_sent)
		return;

	if (!BTM_ReadPowerMode(p_lcb->remote_bd_addr, &mode)
	    && mode == BTM_PM_STS_PARK && p_lcb->link_xmit_data_q.count)
	{
		pm.mode = BTM_PM_MD_ACTIVE;

		BTM_SetPowerMode(BTM_PM_SET_ONLY_ID, p_lcb->remote_bd_addr, &pm);

		btu_start_timer(&p_lcb->timer_entry, BTU_TTYPE_L2CAP_LINK,
		                BT_1SEC_TIMEOUT);

		return;
	}

	while (l2cb.controller_xmit_window != 0 && p_lcb->link_xmit_window != 0)
	{
		if (p_lcb->link_xmit_data_q.p_first == NULL
		    || ((BT_HDR *)p_lcb->link_xmit_data_q.p_first)->layer_specific != 0)
		{
			break;
		}

		if (p_lcb->partial_segment_being_sent)
			return;

		if ((p_buf = (BT_HDR *)GKI_dequeue(&p_lcb->link_xmit_data_q)) == NULL)
			break;

		if (!l2c_link_send_to_lower(p_lcb, p_buf))
			break;
	}

	if (p_lcb->partial_segment_being_sent)
		return;

	if (p_lcb->link_xmit_data_q.count && p_lcb->link_xmit_window)
	{
		btu_start_timer(&p_lcb->timer_entry, BTU_TTYPE_L2CAP_LINK,
		                L2CAP_LINK_FLOW_CONTROL_TOUT);
	}

	if (p_lcb->cong_sent)
	{
		if (p_lcb->link_xmit_data_q.count <= p_lcb->link_cong_end)
		{
			p_lcb->cong_sent = FALSE;

			for (p_cur_ccb = p_lcb->p_first_ccb; p_cur_ccb;
			     p_cur_ccb = p_cur_ccb->p_next_ccb)
			{
				if (p_cur_ccb->p_rcb->api.pL2CA_CongestionStatus_Cb)
				{
					L2CAP_TRACE(API, "L2CAP - Calling CongestionStatus_Cb(), CID: 0x%04x, Uncongested",
					            p_cur_ccb->local_cid);

					(*p_cur_ccb->p_rcb->api.pL2CA_CongestionStatus_Cb)(
						p_cur_ccb->local_cid, FALSE);

					if (p_lcb->cong_sent)
						break;
				}
			}
		}
	}
}

void l2c_link_adjust_allocation(void)
{
	UINT16 qq, yy;
	tL2C_LCB *p_lcb;
	int high_pri_link_count = 0;

	if (l2cb.num_links_active == 0)
		return;

	for (yy = 0, p_lcb = &l2cb.lcb_pool[0]; yy < MAX_L2CAP_LINKS;
	     yy++, p_lcb++)
	{
		if (p_lcb->in_use && p_lcb->acl_priority == L2CAP_PRIORITY_HIGH)
			high_pri_link_count++;
	}

	qq = l2cb.num_lm_acl_bufs / l2cb.num_links_active + 1;

	for (yy = 0, p_lcb = &l2cb.lcb_pool[0]; yy < MAX_L2CAP_LINKS;
	     yy++, p_lcb++)
	{
		if (!p_lcb->in_use)
			continue;

		if (high_pri_link_count == 0
		    || p_lcb->acl_priority == L2CAP_PRIORITY_HIGH)
		{
			p_lcb->link_xmit_quota = qq;
			p_lcb->link_xmit_window = qq;
		}
		else
		{
			p_lcb->link_xmit_quota = 3;
			p_lcb->link_xmit_window = 3;
		}

		p_lcb->link_cong_start = (p_lcb->link_xmit_quota * 120) / 100;

		p_lcb->link_cong_end = p_lcb->link_cong_start > 3
		                           ? p_lcb->link_cong_start - 3
		                           : 0;

		p_lcb->link_cong_discard = p_lcb->link_cong_start + 2;

		L2CAP_TRACE(DEBUG, "LCB %d Priority:%d XmitWindow:%d Congestion Start:%d End:%d Discard:%d",
		            yy, p_lcb->acl_priority, p_lcb->link_xmit_window,
		            p_lcb->link_cong_start, p_lcb->link_cong_end,
		            p_lcb->link_cong_discard);
	}
}

void l2c_link_process_num_completed_pkts(UINT8 *p)
{
	UINT8 num_handles, xx;
	UINT16 handle;
	UINT16 num_sent;
	tL2C_LCB *p_lcb;

	STREAM_TO_UINT8(p, &num_handles);

	for (xx = 0; xx < num_handles; xx++)
	{
		STREAM_TO_UINT16(p, &handle);
		STREAM_TO_UINT16(p, &num_sent);

		if ((p_lcb = l2cu_find_lcb_by_handle(handle)) != NULL)
		{
			p_lcb->link_xmit_window += num_sent;

			if (p_lcb->link_xmit_window > p_lcb->link_xmit_quota)
				p_lcb->link_xmit_window = p_lcb->link_xmit_quota;

			if (p_lcb->sent_not_acked > num_sent)
				p_lcb->sent_not_acked -= num_sent;
			else
				p_lcb->sent_not_acked = 0;

			l2cb.controller_xmit_window += num_sent;

			l2c_link_check_send_pkts(p_lcb, NULL, NULL);
		}
	}
}

void l2c_link_processs_num_bufs(UINT16 num_lm_acl_bufs)
{
	l2cb.controller_xmit_window = l2cb.num_lm_acl_bufs = num_lm_acl_bufs;
}

BT_HDR *l2cap_link_chk_pkt_start(BT_HDR *p_cur_buf)
{
	UINT8 *p;
	UINT16 handle, hci_len;
	UINT8 pkt_type;
	tL2C_LCB *p_lcb;
	BT_HDR *p_return_buf;
	UINT8 *p_partial_hdr;
	UINT16 partial_hci_len;

	p = (UINT8 *)(p_cur_buf + 1) + p_cur_buf->offset;
	p_cur_buf->layer_specific = 0;

	STREAM_TO_UINT16(p, &handle);
	STREAM_TO_UINT16(p, &hci_len);

	l2cb.p_cur_hcit_lcb = NULL;

	pkt_type = HCID_GET_EVENT(handle);
	handle = HCID_GET_HANDLE(handle);

	if ((p_lcb = l2cu_find_lcb_by_handle(handle)) == NULL)
		return p_cur_buf;

	if (pkt_type == L2CAP_PKT_START)
	{
		if (p_lcb->p_hcit_rcv_acl)
		{
			L2CAP_TRACE(WARNING, "L2CAP - dropping incomplete pkt");

			GKI_freebuf(p_lcb->p_hcit_rcv_acl);
		}

		p_lcb->p_hcit_rcv_acl = p_cur_buf;
		p_return_buf = p_cur_buf;
		l2cb.p_cur_hcit_lcb = p_lcb;
	}
	else if ((p_return_buf = p_lcb->p_hcit_rcv_acl) != NULL)
	{
		p_partial_hdr = (UINT8 *)(p_return_buf + 1) + p_return_buf->offset;
		partial_hci_len = p_partial_hdr[2] + (p_partial_hdr[3] << 8);

		if (partial_hci_len + hci_len <= 1695)
		{
			l2cb.p_cur_hcit_lcb = p_lcb;

			if (p_cur_buf->len > 4)
			{
				memcpy((UINT8 *)(p_return_buf + 1) + p_return_buf->offset
				           + p_return_buf->len,
				       (UINT8 *)(p_cur_buf + 1) + p_cur_buf->offset + 4,
				       p_cur_buf->len - 4);

				p_return_buf->len += p_cur_buf->len - 4;
			}

			GKI_freebuf(p_cur_buf);

			partial_hci_len += hci_len;
			p_partial_hdr[2] = (UINT8)partial_hci_len;
			p_partial_hdr[3] = (UINT8)(partial_hci_len >> 8);
		}
		else
		{
			L2CAP_TRACE(WARNING, "L2CAP - dropping too long pkt");

			GKI_freebuf(p_lcb->p_hcit_rcv_acl);
			p_lcb->p_hcit_rcv_acl = NULL;
			p_return_buf = NULL;
		}
	}
	else
	{
		p_return_buf = NULL;
	}

	if (p_return_buf == NULL)
		GKI_freebuf(p_cur_buf);

	return p_return_buf;
}

BOOLEAN l2cap_link_chk_pkt_end(void)
{
	tL2C_LCB *p_lcb = l2cb.p_cur_hcit_lcb;
	BT_HDR *p_buf;
	UINT8 *p;
	UINT16 l2cap_len;

	if (p_lcb == NULL || (p_buf = p_lcb->p_hcit_rcv_acl) == NULL)
		return TRUE;

	p = (UINT8 *)(p_buf + 1) + p_buf->offset;
	l2cap_len = p[4] + (p[5] << 8);

	if (l2cap_len > p_buf->len - 8)
		return FALSE;

	p_lcb->p_hcit_rcv_acl = NULL;

	return TRUE;
}

void l2c_link_role_changed(BD_ADDR bd_addr, UINT8 new_role)
{
	tL2C_LCB *p_lcb;
	int xx;

	p_lcb = l2cu_find_lcb_by_bd_addr(bd_addr);

	if (p_lcb)
		p_lcb->link_role = new_role;

	for (xx = 0, p_lcb = &l2cb.lcb_pool[0]; xx < MAX_L2CAP_LINKS;
	     xx++, p_lcb++)
	{
		if (p_lcb->in_use && p_lcb->link_state == LST_CONNECTING_WAIT_SWITCH)
			l2cu_create_conn_after_switch(p_lcb);
	}
}

void l2c_link_role_change_failed(void)
{
	tL2C_LCB *p_lcb;
	int xx;

	for (xx = 0, p_lcb = &l2cb.lcb_pool[0]; xx < MAX_L2CAP_LINKS;
	     xx++, p_lcb++)
	{
		if (p_lcb->in_use && p_lcb->link_state == LST_CONNECTING_WAIT_SWITCH)
			l2cu_create_conn_after_switch(p_lcb);
	}
}

void l2c_link_segments_xmitted(BT_HDR *p_msg)
{
	UINT8 *p = (UINT8 *)(p_msg + 1) + p_msg->offset;
	UINT16 handle;
	tL2C_LCB *p_lcb;

	STREAM_TO_UINT16(p, &handle);
	handle = HCID_GET_HANDLE(handle);

	if ((p_lcb = l2cu_find_lcb_by_handle(handle)) == NULL)
	{
		L2CAP_TRACE(WARNING, "L2CAP - rcvd segment complete, unknown handle: %d",
		            handle);

		GKI_freebuf(p_msg);
		return;
	}

	if (p_lcb->link_state == LST_CONNECTED)
	{
		p_msg->layer_specific = 0;
		GKI_enqueue_head(&p_lcb->link_xmit_data_q, p_msg);

		p_lcb->partial_segment_being_sent = FALSE;

		l2c_link_check_send_pkts(p_lcb, NULL, NULL);
	}
	else
	{
		GKI_freebuf(p_msg);
	}
}

void l2c_pin_code_request(BD_ADDR bd_addr)
{
	tL2C_LCB *p_lcb = l2cu_find_lcb_by_bd_addr(bd_addr);

	if (p_lcb && !p_lcb->p_first_ccb)
	{
		btu_start_timer(&p_lcb->timer_entry, BTU_TTYPE_L2CAP_LINK,
		                L2CAP_LINK_CONNECT_TOUT_EXT);
	}
}
