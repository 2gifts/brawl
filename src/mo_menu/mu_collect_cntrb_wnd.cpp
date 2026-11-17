#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_msg.h>
#include <mu/mu_notice_wnd.h>
#include <mu/mu_object.h>
#include <mu/wifi/mu_wifi_data_load_task.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <types.h>

// The window of the collection viewer that sends a collection to the server that the other players download it from
// (module sora_menu_collect_viewer). The class is only described by the members the decompiled functions touch.
// Its functions have no names in the map of the module (the names here are made of the address of the function).
// something of the save data that the window asks about (HYPOTHESIS: if the contribution is already made)
extern "C" int fn_80154DF8();

class muCollectCntrbWndTask : public gfTask {
public:
    MuObject* m_obj40;                      // 0x40
    MuObject* m_obj44;                      // 0x44
    MuObject* m_obj48;                      // 0x48
    u8 m_unk4C[0x70 - 0x4C];
    nw4r::g3d::ScnMdlExpand* m_group70;     // 0x70
    nw4r::g3d::ScnGroup* m_scnGroup;        // 0x74
    MuMsg* m_msg;                           // 0x78
    void* m_msgData;                        // 0x7C
    u8 m_unk80[0x128 - 0x80];
    int m_stepResult;                       // 0x128 HYPOTHESIS: what the main steps return while they wait
    u8 m_unk12C[0x140 - 0x12C];
    muNoticeWndTask* m_wnd;                 // 0x140
    nw4r::g3d::ResFile* m_resFile;          // 0x144
    int m_priority;                         // 0x148
    u8 m_unk14C[0x150 - 0x14C];
    muWifiDataLoadTask* m_dataLoadTask;     // 0x150
    u8 m_unk154[0x261 - 0x154];
    u8 m_flag261;                           // 0x261

    int fn_B0B8();
    void fn_B0FC();
    int fn_B4A8();
    void fn_B4EC();
    void fn_B6E0();
    void fn_B910();
    int fn_BF0C();
    void fn_BF60();
    int fn_BF7C();
    void fn_B758();
    int fn_B7CC();
    void fn_B820();
    int fn_B8A4();
    int fn_BCA8();
    int fn_BD70();
};

int muCollectCntrbWndTask::fn_B0B8() {
    if (m_obj40->isNodeAnimFinished()) {
        return 1;
    }
    return m_stepResult;
}

void muCollectCntrbWndTask::fn_B0FC() { }

int muCollectCntrbWndTask::fn_B4A8() {
    if (m_obj40->isNodeAnimFinished()) {
        return 3;
    }
    return m_stepResult;
}

void muCollectCntrbWndTask::fn_B4EC() {
    if (m_flag261 == 0) {
        return;
    }
    m_group70->PushBack(m_obj48->m_sceneModel, "pos02");
}

void muCollectCntrbWndTask::fn_B6E0() {
    MuObject* obj = m_obj40;
    int anim = 2;
    if (m_flag261 != 0) {
        anim = 4;
    }
    obj->changeAnimN(anim, 1, true, false);
}

void muCollectCntrbWndTask::fn_B910() {
    m_dataLoadTask = muWifiDataLoadTask::create(m_resFile, m_scnGroup, m_msgData, m_priority + 7);
}

int muCollectCntrbWndTask::fn_BF0C() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        wnd->exit();
        m_wnd = NULL;
        return 0xC;
    }
    return m_stepResult;
}

void muCollectCntrbWndTask::fn_BF60() {
    m_scnGroup->Remove(m_group70);
}

int muCollectCntrbWndTask::fn_BF7C() {
    return 0xD;
}

void muCollectCntrbWndTask::fn_B758() {
    m_wnd = fn_800BEEB8(0, m_msg->getBinary(), 0x10, m_resFile, m_scnGroup, m_priority + 7, 0, 0x2A, 0x2B, 0, 0, 0xF0);
}

int muCollectCntrbWndTask::fn_B7CC() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        wnd->exit();
        m_wnd = NULL;
        return 1;
    }
    return m_stepResult;
}

void muCollectCntrbWndTask::fn_B820() {
    int msgId = 0x11 + (fn_80154DF8() != 0);
    m_wnd = fn_800BEEB8(1, m_msg->getBinary(), msgId, m_resFile, m_scnGroup, m_priority + 7, 0, 0x2A, 0x2B, 0, 0, 0xF0);
}

int muCollectCntrbWndTask::fn_B8A4() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        u8 selected = wnd->m_selected;
        wnd->exit();
        m_wnd = NULL;
        int step = 1;
        if (selected != 0) {
            step = 7;
        }
        return step;
    }
    return m_stepResult;
}

int muCollectCntrbWndTask::fn_BCA8() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        wnd->exit();
        m_wnd = NULL;
        return 0xC;
    }
    return m_stepResult;
}

int muCollectCntrbWndTask::fn_BD70() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        u8 selected = wnd->m_selected;
        wnd->exit();
        m_wnd = NULL;
        int step = 1;
        if (selected != 0) {
            step = 0xC;
        }
        return step;
    }
    return m_stepResult;
}
