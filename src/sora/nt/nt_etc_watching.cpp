#include <gf/gf_crypto_aes.h>
#include <nt/nt_etc_watching.h>
#include <nt/nt_net_report.h>
#include <revolution/OS/OSThread.h>
#include <revolution/OS/OSTime.h>
#include <types.h>

nteWatching::nteWatching(Heaps::HeapType heap) : gfThread(this, 0xF, 0x2000, Heaps::Thread) {
    m_unk348 = 0;
    m_unk34C = 0;
    m_unk358 = 0;
    m_unk350 = 0x64;
    m_sbs = new (heap) nteSBS(1);
    m_sbs->unk50 = this;
    m_sbs->unk4C = this;
    m_heap = heap;
    m_unk398 = 0;
    m_unk38C = 0;
    m_unk390 = 0;
    m_crypto = NULL;
}

nteWatching::~nteWatching() {
    bool terminated = OSIsThreadTerminated(&m_thread) == true;
    if (!terminated) {
        m_unk398 = 1;
        OSResumeThread(&m_thread);
        bool running;
        while (running = OSIsThreadTerminated(&m_thread) == true, !running) {
            OSSleepTicks(OS_MSEC_TO_TICKS(1));
        }
    }
    if (m_crypto != NULL) {
        delete m_crypto;
        m_crypto = NULL;
    }
    netReport("SBS\x82\xCC\x89\xF0\x95\xFA\n");
    m_sbs->exit();
}

bool nteWatching::download() {
    bool terminated = OSIsThreadTerminated(&m_thread) == true;
    if (terminated) {
        return false;
    }
    if (m_unk350 != 0x64) {
        return false;
    }
    m_unk350 = 1;
    OSResumeThread(&m_thread);
    return true;
}

bool nteWatching::cancel() {
    bool terminated = OSIsThreadTerminated(&m_thread) == true;
    if (terminated) {
        return false;
    }
    if (m_unk350 == 0x64) {
        return false;
    }
    m_sbs->cancel();
    m_unk398 = 1;
    netReport("\x1b[31m [cancel] \x1b[00m\n");
    return true;
}

void nteWatching::showError(s32 kind, s32 code) {
    netReport("\x1b[31m nteWatching : Error = %d \x1b[00m\n");
    if (code == 6) {
        m_unk398 = 1;
    } else if (code == 7) {
        m_unk398 = 1;
    }
    if (kind != 0) {
        m_unk388 = 1;
    }
}

void nteWatching::notifySBSEndProc(s32 arg, s32 code) {
    s32 err = m_sbs->getErrorCode(code);
    if (err != 0) {
        m_unk38C = err;
        m_unk390 = 1;
        m_unk398 = 1;
    }
}
