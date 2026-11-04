#include <StaticAssert.h>
#include <gf/gf_3d_scene.h>
#include <if/if_wifi_intr_menu.h>
#include <if/if_wifipr_center.h>
#include <if/if_wifipr_player.h>
#include <if/if_wifipr_task.h>
#include <mu/mu_notice_wnd.h>
#include <mu/mu_wifi_cnt_wnd.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <sr/sr_common.h>
#include <types.h>

class IfWifiprMngrUnk38 {
public:
    char unk0[0x1C];
    u8 unk1C;
};

class IfWifiprMngr {
public:
    void main();
    IfWifiprMngr();
    static IfWifiprMngr* create();
    void createModel();
    void destroyModel();
    void startMelee();
    void setPlayerWiFiStateNone(int playerNo);
    void setPlayerWiFiStateAccept(int playerNo);
    void setPlayerWiFiStateSelChar(int playerNo);
    void setPlayerWiFiStateSelStage(int playerNo);
    void setPlayerWiFiStatePractice(int playerNo);
    void setPlayerWiFiStateEscape(int playerNo);
    void setPlayerWiFiStateMelee(int playerNo);
    void setCntWndTime(int frames);
    void setCntWndStyle(int style);
    void openDlg(int useSecondMsg, int msgId, int kind);
    void closeDlg();
    int getDlgState();

    u8 unk0[0x8];
    nw4r::g3d::ResFile unk8;
    nw4r::g3d::ResFile unkC;
    nw4r::g3d::ResFile unk10;
    nw4r::g3d::ScnGroup* unk14;
    union {
        s32 unk18;
        IfWifiprCenter* m_center;
    };
    union {
        s32 unk1C[7];
        IfWifiprPlayer* m_players[7];
    };
    union {
        IfWifiprMngrUnk38* unk38;
        IfWifiIntrMenu* m_intrMenu;
    };
    union {
        char unk3C[0x14];
        struct {
            ifWifiprTask* m_task;
            muWifiCntWndTask* m_cntWnd;
            muNoticeWndTask* m_notice;
            void* unk48;
            void* unk4C;
        };
    };
    u8 unk50;
    u8 unk51[0x17];
};
static_assert(sizeof(IfWifiprMngr) == 0x68, "Class is the wrong size!");

// muWifiCntWndTask::create (the map name); if_vs_result_task.cpp calls it by this stand-in name.
extern "C" muWifiCntWndTask* fn_800DA15C(int kind, nw4r::g3d::ResFile* resFile, nw4r::g3d::ScnMdlExpand* expand, const char* node,
                                         int priority, int heap, int heap2);
// muWifiCntWndTask::setCounter (the map name); if_vs_result_task.cpp calls it by this stand-in name.
extern "C" void fn_800DA6EC(muWifiCntWndTask* task, int seconds, int unk);
extern "C" void fn_800FD448(s32 unk);
extern "C" void fn_800FD8E0(s32 unk);
extern "C" s32 fn_800FBFDC();
extern "C" void fn_8000DB40(gfSceneRoot* root, s32 layer);

extern gfSceneRoot* g_gfSceneRoot;

namespace nw4r { namespace g3d { class ScnMdlExpand; } }
extern nw4r::g3d::ScnMdlExpand* lbl_805A030C;

void IfWifiprMngr::main() {
    fn_800FD448(unk18);
    for (s32 i = 0; i < 7; i++) {
        fn_800FD8E0(unk1C[i]);
    }
    if (unk38->unk1C != 0 && fn_800FBFDC() == 4) {
        unk50 = 1;
    }
    fn_8000DB40(g_gfSceneRoot, 8);
}

IfWifiprMngr* IfWifiprMngr::create() {
    return new (Heaps::InfoInstance) IfWifiprMngr();
}

void IfWifiprMngr::createModel() {
    nw4r::g3d::ResAnmScn anmScn;
    m_center = new (Heaps::InfoInstance) IfWifiprCenter();
    m_center->createModel(&unk8, unk48);
    IfWifiprPlayer::createPlayerLocator(&unk8);
    for (s32 i = 0; i < 7; i++) {
        m_players[i] = new (Heaps::InfoInstance) IfWifiprPlayer(i, 0);
        m_players[i]->createModel(&unk8, 1);
    }
    m_intrMenu = new (Heaps::InfoInstance) IfWifiIntrMenu();
    m_intrMenu->createModel(&unk8, unk4C);
    m_cntWnd = fn_800DA15C(3, &unkC, lbl_805A030C, "pos00", 0x40, Heaps::InfoInstance, Heaps::MenuInstance);
    anmScn = unk10.GetResAnmScn(0);
    anmScn.Bind(anmScn);
    g_gfSceneRoot->setInfoCamResAnm(&anmScn);
}

void IfWifiprMngr::destroyModel() {
    if (m_center != NULL) {
        delete m_center;
        m_center = NULL;
    }
    for (s32 i = 0; i < 7; i++) {
        if (m_players[i] != NULL) {
            delete m_players[i];
            m_players[i] = NULL;
        }
    }
    IfWifiprPlayer::destroyPlayerLocator();
    if (m_intrMenu != NULL) {
        delete m_intrMenu;
    }
    g_gfSceneRoot->removeInfoCamResAnm();
    if (m_cntWnd != NULL) {
        m_cntWnd->exit();
        m_cntWnd = NULL;
    }
    if (m_task != NULL) {
        m_task->exit();
        m_task = NULL;
    }
    if (m_notice != NULL) {
        fn_800BF830(m_notice);
        m_notice->exit();
        m_notice = NULL;
    }
}

void IfWifiprMngr::startMelee() {
    m_center->startMelee(unk14);
    IfWifiprPlayer::startMelee(unk14);
    m_intrMenu->startMelee(unk14);
    g_gfSceneRoot->add(gfSceneRoot::Layer_Info, (nw4r::g3d::ScnMdl*)unk14);
    m_task = ifWifiprTask::create();
    unk50 = 0;
}

void IfWifiprMngr::setPlayerWiFiStateNone(int playerNo) {
    m_players[playerNo]->setWifiDispState(0);
}

void IfWifiprMngr::setPlayerWiFiStateAccept(int playerNo) {
    m_players[playerNo]->setWifiDispState(1);
}

void IfWifiprMngr::setPlayerWiFiStateSelChar(int playerNo) {
    m_players[playerNo]->setWifiDispState(2);
}

void IfWifiprMngr::setPlayerWiFiStateSelStage(int playerNo) {
    m_players[playerNo]->setWifiDispState(3);
}

void IfWifiprMngr::setPlayerWiFiStatePractice(int playerNo) {
    m_players[playerNo]->setWifiDispState(4);
}

void IfWifiprMngr::setPlayerWiFiStateEscape(int playerNo) {
    m_players[playerNo]->setWifiDispState(5);
}

void IfWifiprMngr::setPlayerWiFiStateMelee(int playerNo) {
    m_players[playerNo]->setWifiDispState(6);
}

void IfWifiprMngr::setCntWndTime(int frames) {
    int seconds = (frames + 59) / 60;
    fn_800DA6EC(m_cntWnd, seconds, 0);
    if (m_cntWnd->unk64 == 1) {
        fn_800DA6EC(m_cntWnd, seconds / 60, seconds % 60);
    } else {
        fn_800DA6EC(m_cntWnd, seconds, 0);
    }
}

void IfWifiprMngr::setCntWndStyle(int style) {
    int wndStyle;
    int digits;
    if (style == 0) {
        wndStyle = 0;
        digits = 3;
    } else {
        wndStyle = 1;
        digits = 2;
    }
    m_cntWnd->setWndStyle(wndStyle, digits);
}

void IfWifiprMngr::openDlg(int useSecondMsg, int msgId, int kind) {
    bool isQuestion = kind != 0;
    void* msgData;
    if (useSecondMsg == 0) {
        msgData = unk48;
    } else {
        msgData = unk4C;
    }
    m_notice = fn_800BEEB8(isQuestion, msgData, msgId, &unkC, unk14, 0x40, 0x1E, 0x28, 0x2A, 1, 1, 0xF0);
}

void IfWifiprMngr::closeDlg() {
    fn_800BF830(m_notice);
    m_notice->exit();
    m_notice = NULL;
}

int IfWifiprMngr::getDlgState() {
    if (m_notice == NULL) {
        return 0;
    }
    if (m_notice->m_step != 3) {
        return 1;
    }
    int state = 3;
    if (m_notice->m_selected != 0) {
        state = 2;
    }
    return state;
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A02F8[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char g_IfWifiprMngr[0x8] __attribute__((aligned(4))) = { 0 };
