#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_msg.h>
#include <mu/mu_notice_wnd.h>
#include <mu/mu_object.h>
#include <mu/wifi/mu_wifi_data_load_task.h>
#include <nt/nt_etc_nwc24.h>
#include <nt/nt_etc_xchg.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The window of the collection viewer that sends a collection through WiiConnect24 to a friend (module
// sora_menu_collect_viewer). The class is only described by the members the decompiled functions touch.
/// Function names come from the sora_menu_collect_viewer map.
class muCollectXchgWndTask : public gfTask {
public:
    MuObject* m_obj40;                      // 0x40
    u8 m_unk44[0x68 - 0x44];
    nw4r::g3d::ScnObj* m_unk68;             // 0x68
    u8 m_unk6C[0x70 - 0x6C];
    nw4r::g3d::ScnGroup* m_scnGroup;        // 0x70
    u8 m_unk74_[0x74 - 0x74];
    MuMsg* m_msg;                           // 0x74
    void* m_msgData;                        // 0x78
    u8 m_unk7C[0x124 - 0x7C];
    int m_stepResult;                       // 0x124 HYPOTHESIS: what the main steps return while they wait
    u8 m_unk128[0x72C - 0x128];
    int m_numFriend;                        // 0x72C
    nteExchange* m_exchange;                // 0x730
    u8 m_unk734[0x748 - 0x734];
    int m_unk748;                           // 0x748
    int m_unk74C;                           // 0x74C
    muNoticeWndTask* m_wnd;                 // 0x750
    nw4r::g3d::ResFile* m_resFile;          // 0x754
    int m_priority;                         // 0x758
    u8 m_unk75C[0x770 - 0x75C];
    muWifiDataLoadTask* m_dataLoadTask;     // 0x770
    u8 m_unk774;                            // 0x774
    u8 m_unk775[0x778 - 0x775];
    int m_msgId;                            // 0x778
    int m_unk77C;                           // 0x77C
    int m_unk780;                           // 0x780
    int m_libState;                         // 0x784
    int m_statusWait;                       // 0x788

    void mainStepLibOpenInit();
    int mainStepLibOpenMain();
    void mainStepCheckStatusInit();
    int mainStepCheckStatusMain();
    void mainStepWrongDataInit();
    int mainStepWrongDataMain();
    void mainStepLoadWifiDataInit();
    void mainStepMakeFriendListInit();
    int mainStepAppearMain();
    int mainStepNoFriendMain();
    void mainStepSelectingInit();
    void mainStepCancelConfirmInit();
    int mainStepCancelConfirmMain();
    void mainStepNoFriendInit();
    void mainStepSendOverInit();
    int mainStepSendOverMain();
    int mainStepSendConfirmMain();
    int mainStepSendMain();
    int mainStepSendCompleteMain();
    int mainStepErrorMain();
    void mainStepLeaveInit();
    int mainStepLeaveMain();
};

// The callback that keeps the error of WiiConnect24 for the window.
class muCollectXchgWndCB : public IWiiConnect24_Error {
public:
    int m_kind; // 0x04
    int m_code; // 0x08

    virtual void notify_Error(int kind, int unk, int code);
    ~muCollectXchgWndCB();
};

void muCollectXchgWndTask::mainStepLibOpenInit() {
    if (m_exchange->_requestJob(4, 0)) {
        m_libState = 1;
    } else {
        m_libState = 2;
    }
}

int muCollectXchgWndTask::mainStepLibOpenMain() {
    if (m_libState == 2) {
        m_unk77C = 3;
        m_unk780 = 0;
        m_unk774 = 1;
        return 0xD;
    }
    nteExchange* exchange = m_exchange;
    if (!exchange->m_jobDone) {
        return m_stepResult;
    }
    if (exchange->m_result != 0) {
        return m_stepResult;
    }
    return 1;
}

void muCollectXchgWndTask::mainStepCheckStatusInit() {
    m_exchange->check_getStatus(1);
    m_statusWait = 2;
}

int muCollectXchgWndTask::mainStepCheckStatusMain() {
    m_statusWait--;
    if (m_statusWait >= 0) {
        return m_stepResult;
    }
    return 3;
}

void muCollectXchgWndTask::mainStepWrongDataInit() {
    m_wnd = fn_800BEEB8(0, m_msg->getBinary(), m_msgId, m_resFile, m_scnGroup, m_priority + 3, 0, 0x2A, 0x2B, 0, 0, 0xF0);
}

int muCollectXchgWndTask::mainStepWrongDataMain() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        wnd->exit();
        m_wnd = NULL;
        return 0xE;
    }
    return m_stepResult;
}

void muCollectXchgWndTask::mainStepLoadWifiDataInit() {
    m_dataLoadTask = muWifiDataLoadTask::create(m_resFile, m_scnGroup, m_msgData, m_priority + 3);
}

void muCollectXchgWndTask::mainStepMakeFriendListInit() {
    m_exchange->_requestJob(1, 0);
}

int muCollectXchgWndTask::mainStepAppearMain() {
    if (m_obj40->isNodeAnimFinished()) {
        int step = 7;
        if (m_numFriend <= 0) {
            step = 6;
        }
        return step;
    }
    return m_stepResult;
}

int muCollectXchgWndTask::mainStepNoFriendMain() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        wnd->exit();
        m_wnd = NULL;
        return 7;
    }
    return m_stepResult;
}

void muCollectXchgWndTask::mainStepSelectingInit() {
    if (m_numFriend > 0) {
        return;
    }
    m_unk748 = 1;
    m_unk74C = 1;
}

void muCollectXchgWndTask::mainStepCancelConfirmInit() {
    m_wnd = fn_800BEEB8(1, m_msg->getBinary(), 6, m_resFile, m_scnGroup, m_priority + 3, 0, 0x2A, 0x2B, 0, 0, 0xF0);
}

int muCollectXchgWndTask::mainStepCancelConfirmMain() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        u8 selected = wnd->m_selected;
        wnd->exit();
        m_wnd = NULL;
        int step = 7;
        if (selected != 0) {
            step = 0xE;
        }
        return step;
    }
    return m_stepResult;
}

void muCollectXchgWndTask::mainStepNoFriendInit() {
    m_wnd = fn_800BEEB8(0, m_msg->getBinary(), 0xB, m_resFile, m_scnGroup, m_priority + 3, 0, 0x2A, 0x2B, 0, 0, 0xF0);
}

void muCollectXchgWndTask::mainStepSendOverInit() {
    m_wnd = fn_800BEEB8(0, m_msg->getBinary(), 8, m_resFile, m_scnGroup, m_priority + 3, 0, 0x2A, 0x2B, 0, 0, 0xF0);
}

int muCollectXchgWndTask::mainStepSendOverMain() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        wnd->exit();
        m_wnd = NULL;
        return 7;
    }
    return m_stepResult;
}

int muCollectXchgWndTask::mainStepSendConfirmMain() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        u8 selected = wnd->m_selected;
        wnd->exit();
        m_wnd = NULL;
        int step = 7;
        if (selected != 0) {
            step = 0xB;
        }
        return step;
    }
    return m_stepResult;
}

int muCollectXchgWndTask::mainStepSendMain() {
    nteExchange* exchange = m_exchange;
    if (!exchange->m_jobDone) {
        return m_stepResult;
    }
    if (exchange->m_result == 0) {
        return 0xC;
    }
    m_msgId = 9;
    return 2;
}

int muCollectXchgWndTask::mainStepSendCompleteMain() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        wnd->exit();
        m_wnd = NULL;
        return 0xE;
    }
    return m_stepResult;
}

int muCollectXchgWndTask::mainStepErrorMain() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        wnd->exit();
        m_wnd = NULL;
        return 0xE;
    }
    return m_stepResult;
}

void muCollectXchgWndTask::mainStepLeaveInit() {
    m_scnGroup->Remove(m_unk68);
}

int muCollectXchgWndTask::mainStepLeaveMain() {
    return 0xF;
}

muCollectXchgWndCB::~muCollectXchgWndCB() { }

void muCollectXchgWndCB::notify_Error(int kind, int unk, int code) {
    m_kind = kind;
    m_code = code;
}
