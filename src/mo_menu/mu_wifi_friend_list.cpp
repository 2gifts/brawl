#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_msg.h>
#include <mu/mu_notice_wnd.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <string.h>
#include <types.h>

// The friend list of the Wi-Fi menu (module sora_menu_friend_list). The class is only described by the members the
// decompiled functions touch.
/// Function names come from the sora_menu_friend_list map.
class muWifiInterfaceTask;
extern muWifiInterfaceTask* g_muWifiInterfaceTask;

// gfMii::drop(), the Mii data of the friend list is dropped with it
extern "C" void drop__5gfMiiFv();

class muWifiFriendListTask : public gfTask {
public:
    u8 m_unk40[0x8C - 0x40];
    MuObject* m_obj8C;                      // 0x8C
    u8 m_unk90[0x9C - 0x90];
    MuObject* m_obj9C;                      // 0x9C
    u8 m_unkA0[0xAC - 0xA0];
    MuObject* m_objAC;                      // 0xAC
    MuObject* m_objB0;                      // 0xB0
    u8 m_unkB4[0xC0 - 0xB4];
    MuObject* m_objC0;                      // 0xC0
    u8 m_unkC4[0xC8 - 0xC4];
    MuObject* m_objC8;                      // 0xC8
    u8 m_unkCC[0x204 - 0xCC];
    nw4r::g3d::ScnMdlExpand* m_group204;    // 0x204
    u8 m_unk208[0x2B0 - 0x208];
    int m_baseState;                        // 0x2B0 HYPOTHESIS: the state that setBaseState sets
    u8 m_unk2B4[0x2310 - 0x2B4];
    char m_myName[0x15];                    // 0x2310
    char m_myApeal[4][0x51];                // 0x2325
    char m_myComment[0x5B];                 // 0x2469
    int m_stepResult;                       // 0x24C4 HYPOTHESIS: what the main steps return while they wait
    float m_scrollPos;                      // 0x24C8
    MuMsg* m_msg;                           // 0x24CC
    u8 m_unk24D0[0x24D8 - 0x24D0];
    int m_friendListCursor;                 // 0x24D8
    u8 m_unk24DC[0x24E0 - 0x24DC];
    int m_unk24E0;                          // 0x24E0
    u8 m_unk24E4[0x24EC - 0x24E4];
    muNoticeWndTask* m_wnd;                 // 0x24EC
    nw4r::g3d::ResFile* m_resFile;          // 0x24F0
    int m_unk24F4;                          // 0x24F4
    u8 m_unk24F8[0x2A1C - 0x24F8];
    void* m_scnRoot;                        // 0x2A1C
    u8 m_unk2A20[0x2A2C - 0x2A20];
    int m_miiFlag;                          // 0x2A2C
    u8 m_unk2A30[0x2A34 - 0x2A30];
    int m_msgId;                            // 0x2A34
    u8 m_unk2A38[0x2AC8 - 0x2A38];

    muWifiFriendListTask();
    static muWifiFriendListTask* create(void* param);
    void initProc(void* param);
    void destroy();
    muWifiInterfaceTask* getWifiIf();
    void getMyName(char* dst);
    void setMyName(const char* src);
    void getMyApeal(char* dst, int index);
    void setMyApeal(const char* src, int index);
    void getMyComment(char* dst);
    void setMyComment(const char* src);
    void removeObj(MuObject* obj);
    void setVisibleApealPanelHelp(int visible);
    void redrawFriendList();
    void setFriendListScrollPosDirect(float pos);
    void setFriendListCursorPos(int pos);
    void setProfileCursorPos(int index);
    void setBaseState(int state);
    void initStepAppear();
    void initStepInitialError();
    int mainStepInitialError();
    void initStepToFriendList();
    int mainStepToFriendList();
    void initFriendList();
    void initFromFriendList();
    int mainFromFriendList();
    void initWaitJoinGamePermitted();
    void initFriendCode();
    void initProfile();
    void initProfileName();
    void initSelectIcon();
    int mainToFriendMenu();
    int mainDeleteFriendDone();
    int mainToFriendCode();
    int mainFriendCodeError();
    int mainFriendCodeToFriendName();
    int mainToProfile();
    int mainToProfileName();
    int mainAppearIconWnd();
    void initLeaveIconWnd();
    static int getProfileCursorPosByCate(int cate, int b, int c);
};

muWifiFriendListTask* muWifiFriendListTask::create(void* param) {
    muWifiFriendListTask* task = new (Heaps::MenuInstance) muWifiFriendListTask();
    task->initProc(param);
    return task;
}

void muWifiFriendListTask::destroy() {
    if (m_wnd != NULL) {
        m_wnd->exit();
    }
    if (m_miiFlag != 0) {
        drop__5gfMiiFv();
    }
    exit();
}

muWifiInterfaceTask* muWifiFriendListTask::getWifiIf() {
    return g_muWifiInterfaceTask;
}

void muWifiFriendListTask::getMyName(char* dst) {
    strcpy(dst, m_myName);
}

void muWifiFriendListTask::setMyName(const char* src) {
    strcpy(m_myName, src);
}

void muWifiFriendListTask::getMyApeal(char* dst, int index) {
    strcpy(dst, m_myApeal[index]);
}

void muWifiFriendListTask::setMyApeal(const char* src, int index) {
    strcpy(m_myApeal[index], src);
}

void muWifiFriendListTask::getMyComment(char* dst) {
    strcpy(dst, m_myComment);
}

void muWifiFriendListTask::setMyComment(const char* src) {
    strcpy(m_myComment, src);
}

void muWifiFriendListTask::removeObj(MuObject* obj) {
    m_group204->Remove(obj->m_sceneModel);
}

void muWifiFriendListTask::setVisibleApealPanelHelp(int visible) {
    if (visible) {
        m_msg->printIndex(0x2A, 0x12, NULL);
    } else {
        m_msg->printf(0x2A, "");
    }
}

void muWifiFriendListTask::redrawFriendList() {
    setFriendListScrollPosDirect(m_scrollPos);
}

void muWifiFriendListTask::setFriendListCursorPos(int pos) {
    m_friendListCursor = pos;
}

void muWifiFriendListTask::setProfileCursorPos(int index) {
    m_objC0->changeAnimN(index, 1, false, false);
}

void muWifiFriendListTask::initStepAppear() {
    setBaseState(1);
}

void muWifiFriendListTask::initStepInitialError() {
    m_wnd = fn_800BEEB8(0, m_msg->getBinary(), m_msgId, m_resFile, m_scnRoot, 0x1F, 0x1E, 0x2A, 0x2B, 0, 0, 0xF0);
}

int muWifiFriendListTask::mainStepInitialError() {
    if (m_wnd->m_step == 3) {
        m_wnd->deleteWnd();
        m_wnd->exit();
        m_wnd = NULL;
        return 2;
    }
    return m_stepResult;
}

void muWifiFriendListTask::initStepToFriendList() {
    setBaseState(0xB);
    m_objAC->setFrameMatCol(2.0f);
    m_unk24E0 = 1;
}

int muWifiFriendListTask::mainStepToFriendList() {
    if (m_baseState != 0xB) {
        return 4;
    }
    return m_stepResult;
}

void muWifiFriendListTask::initFriendList() { }

void muWifiFriendListTask::initFromFriendList() {
    setBaseState(0xD);
    m_obj8C->setFrameMatCol(0.0f);
}

int muWifiFriendListTask::mainFromFriendList() {
    if (m_baseState != 0xD) {
        return 2;
    }
    return m_stepResult;
}

void muWifiFriendListTask::initWaitJoinGamePermitted() {
    m_unk24F4 = 0;
}

void muWifiFriendListTask::initFriendCode() { }

void muWifiFriendListTask::initProfile() { }

void muWifiFriendListTask::initProfileName() { }

void muWifiFriendListTask::initSelectIcon() { }

int muWifiFriendListTask::mainToFriendMenu() {
    if (m_obj9C->isNodeAnimFinished()) {
        return 7;
    }
    return m_stepResult;
}

int muWifiFriendListTask::mainDeleteFriendDone() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        wnd->exit();
        m_wnd = NULL;
        return 8;
    }
    return m_stepResult;
}

int muWifiFriendListTask::mainToFriendCode() {
    if (m_baseState != 5) {
        return 0x12;
    }
    return m_stepResult;
}

int muWifiFriendListTask::mainFriendCodeError() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        wnd->exit();
        m_wnd = NULL;
        return 0x12;
    }
    return m_stepResult;
}

int muWifiFriendListTask::mainFriendCodeToFriendName() {
    if (m_objB0->isNodeAnimFinished()) {
        return 0x16;
    }
    return m_stepResult;
}

int muWifiFriendListTask::mainToProfile() {
    if (m_baseState != 2) {
        return 0x19;
    }
    return m_stepResult;
}

int muWifiFriendListTask::mainToProfileName() {
    if (m_objC0->isNodeAnimFinished()) {
        return 0x1C;
    }
    return m_stepResult;
}

int muWifiFriendListTask::mainAppearIconWnd() {
    if (m_objC8->isNodeAnimFinished()) {
        return 0x1E;
    }
    return m_stepResult;
}

void muWifiFriendListTask::initLeaveIconWnd() {
    m_objC8->changeAnimN(1, 1, true, false);
}

int muWifiFriendListTask::getProfileCursorPosByCate(int cate, int b, int c) {
    switch (cate) {
    case 0:
        if (b == 0) {
            cate = 0;
        } else {
            cate = 1;
        }
        return cate;
    case 1:
        return c + 2;
    case 2:
        return 6;
    }
    return cate;
}
