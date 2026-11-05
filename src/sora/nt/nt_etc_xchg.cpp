#include <types.h>
#include <nt/nt_etc_xchg.h>
#include <gf/gf_heap_manager.h>
#include <string.h>

// Function bodies below are taken from https://github.com/jj-link/brawl.

extern "C" unsigned int fn_8014E708(unsigned int x) {
    return ((x + 0x1D45U) / 7474U) & 0xFFFFU;
}

void nteExchange::mail_AddSendTo(u64 recipient) {
    if (m_unk70 < 8) {
        m_unk30[m_unk70++] = recipient;
    }
}

void nteExchange::mail_ClearSendTo() {
    memset(m_unk30, 0, sizeof(m_unk30));
    m_unk70 = 0;
}

u16 nteExchange::getRemainSendPoint() {
    u32 size = _getMailSentSizeToday();
    if (size >= 0x32000) {
        return 0;
    }
    return (0x32000 - size) / 10240;
}

void nteExchange::run() {
    switch (m_job) {
    case 0:
        break;
    case 1:
        if (friend_makeList()) m_result = 0;
        else m_result = -1;
        break;
    case 2: {
        int result = _mail_SendMail();
        if (result == 0) m_result = 0;
        else if (result == -1) m_result = -1;
        else m_result = -4;
        break;
    }
    case 3:
        m_unk88 = gfHeapManager::alloc(m_heap, 0x23000, 0x20);
        if (m_unk88) {
            m_result = _mail_ReceiveMail(m_jobArg, m_unk80);
            gfHeapManager::free(m_unk88);
            m_unk88 = 0;
        } else {
            m_result = -1;
        }
        break;
    case 4:
        if (_openLibrary(1)) m_result = 0;
        else m_result = -1;
        break;
    }
    m_jobDone = true;
}
