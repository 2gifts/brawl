#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_msg.h>
#include <mu/mu_notice_wnd.h>
#include <mu/mu_object.h>
#include <mu/selchar/mu_select_character_name_entry.h>
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

// NtMatching::thSaveUserDataNow(): saves the user data of the Wi-Fi in a thread
extern "C" void thSaveUserDataNow__10NtMatchingFv();

// A report of the Wi-Fi code that the release does not print.
void netReport(const char* format, ...) { }

class muWifiFriendListTask : public gfTask {
public:
    u8 m_flag40;                            // 0x40
    u8 m_unk41[0x8C - 0x41];
    MuObject* m_obj8C;                      // 0x8C
    u8 m_unk90[0x9C - 0x90];
    MuObject* m_obj9C;                      // 0x9C
    MuObject* m_objA0;                      // 0xA0
    u8 m_unkA4[0xAC - 0xA4];
    MuObject* m_objAC;                      // 0xAC
    MuObject* m_objB0;                      // 0xB0
    MuObject* m_objB4;                      // 0xB4
    u8 m_unkB8[0xC0 - 0xB8];
    MuObject* m_objC0;                      // 0xC0
    MuObject* m_objC4;                      // 0xC4
    MuObject* m_objC8;                      // 0xC8
    u8 m_unkCC[0x1E0 - 0xCC];
    nw4r::g3d::ScnObj* m_unk1E0;            // 0x1E0
    u8 m_unk1E4[0x1E8 - 0x1E4];
    nw4r::g3d::ScnObj* m_unk1E8;            // 0x1E8
    nw4r::g3d::ScnObj* m_unk1EC;            // 0x1EC
    u8 m_unk1F0[0x204 - 0x1F0];
    nw4r::g3d::ScnMdlExpand* m_group204;    // 0x204
    u8 m_unk208[0x2B0 - 0x208];
    int m_baseState;                        // 0x2B0 HYPOTHESIS: the state that setBaseState sets
    u8 m_unk2B4[0x2310 - 0x2B4];
    char m_myName[0x15];                    // 0x2310
    char m_myApeal[4][0x51];                // 0x2325
    char m_myComment[0x57];                 // 0x2469
    int m_unk24C0;                          // 0x24C0
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
    u8 m_unk24F8[0x2570 - 0x24F8];
    MuSelctChrNameEntry m_nameEntry;        // 0x2570
    u8 m_unk2604[0x2A1C - 0x2604];
    nw4r::g3d::ScnGroup* m_scnRoot;         // 0x2A1C
    u8 m_unk2A20[0x2A2C - 0x2A20];
    int m_miiFlag;                          // 0x2A2C
    u8 m_unk2A30[0x2A34 - 0x2A30];
    int m_msgId;                            // 0x2A34
    u8 m_unk2A38[0x2AC4 - 0x2A38];
    int m_codeError;                        // 0x2AC4

    muWifiFriendListTask();
    static muWifiFriendListTask* create(void* param);
    void initProc(void* param);
    void destroy();
    static muWifiInterfaceTask* getWifiIf();
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
    void initFromFriendMenu();
    void initFromFriendCode();
    void initFriendCodeToFriendName();
    void initFriendNameToFriendCode();
    void dispNumFriend();
    void setGlobalCursorPos(int pos);
    void prepareFriendCodeTextBox(int mode);
    void initNameEntry(int mode);
    void setFriendCodeTextColor(int mode);
    void dispCode();
    void initJoinGameClosed();
    int mainJoinGameClosed();
    void initDeleteFriendDone();
    void initFriendCodeError();
    void initFriendName();
    int mainLeaveIconWnd();
    void initFromProfile();
    void flushMyProfile();
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

#pragma dont_inline on
muWifiInterfaceTask* muWifiFriendListTask::getWifiIf() {
    return g_muWifiInterfaceTask;
}
#pragma dont_inline reset

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

void muWifiFriendListTask::initFromFriendMenu() {
    int state = 0x10;
    if (m_unk24C0 <= 3) {
        state = 0xA;
    }
    setBaseState(state);
    m_obj9C->changeAnimN(1, 1, true, false);
    m_objA0->changeAnimN(1, 0x10, true, false);
}

void muWifiFriendListTask::initJoinGameClosed() {
    m_wnd = fn_800BEEB8(0, m_msg->getBinary(), 0x1A, m_resFile, m_scnRoot, 0x1F, 0, 0x2A, 0x2B, 0, 0, 0xF0);
}

int muWifiFriendListTask::mainJoinGameClosed() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        wnd->exit();
        m_wnd = NULL;
        m_scnRoot->Remove(m_unk1E0);
        return 7;
    }
    return m_stepResult;
}

void muWifiFriendListTask::initDeleteFriendDone() {
    m_wnd = fn_800BEEB8(0, m_msg->getBinary(), 0x1D, m_resFile, m_scnRoot, 0x1F, 0, 0x2A, 0x2B, 0, 0, 0xF0);
}

void muWifiFriendListTask::initFriendCodeError() {
    static const int msgIds[3] = {0x1E, 0x1F, 0x20};
    m_wnd = fn_800BEEB8(0, m_msg->getBinary(), msgIds[m_codeError], m_resFile, m_scnRoot, 0x1F, 0, 0x2A, 0x2B, 0, 0, 0xF0);
}

void muWifiFriendListTask::initFriendName() {
    MuObject* obj = m_objB4;
    obj->setFrameVisible(1.0f);
    obj->setFrameNode((float)m_nameEntry.m_caret);
}

int muWifiFriendListTask::mainLeaveIconWnd() {
    if (m_objC8->isNodeAnimFinished()) {
        m_group204->Remove(m_unk1EC);
        m_objC0->changeAnimN(8, 0x10, true, false);
        return 0x19;
    }
    return m_stepResult;
}

void muWifiFriendListTask::initFromFriendCode() {
    setBaseState(7);
    m_objB0->changeAnimN(3, 1, true, false);
    dispNumFriend();
    if (m_unk24C0 >= 0x40) {
        setGlobalCursorPos(0);
    }
}

void muWifiFriendListTask::initFriendCodeToFriendName() {
    prepareFriendCodeTextBox(1);
    m_objB0->changeAnimN(1, 1, true, false);
    initNameEntry(0);
    m_nameEntry.open(NULL, NULL, 5);
    setFriendCodeTextColor(0);
    dispCode();
    m_msg->printIndex(0x17, 0x14, NULL);
}

void muWifiFriendListTask::initFriendNameToFriendCode() {
    m_objB0->changeAnimN(2, 1, true, false);
    setFriendCodeTextColor(1);
    dispCode();
}

#pragma dont_inline on
void muWifiFriendListTask::setFriendCodeTextColor(int mode) {
    int color = 0xC6;
    if (mode != 0) {
        color = 0xFF;
    }
    for (int i = 0; i < 3; i++) {
        m_msg->setFontColor(0x18 + i, color, color, color, 0xFF);
    }
}
#pragma dont_inline reset

void muWifiFriendListTask::initFromProfile() {
    setBaseState(4);
    m_obj8C->changeAnimN(1, 3, true, false);
    m_objC4->changeAnimN(1, 3, true, false);
    m_group204->Remove(m_unk1E8);
    if (m_flag40 == 0) {
        flushMyProfile();
        u8* wifi = (u8*)getWifiIf();
        wifi[0xFCA] &= ~0x10;
        thSaveUserDataNow__10NtMatchingFv();
    }
}

#include <ut/ut_relocate.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <gf/gf_pad_system.h>
#include <gf/gf_pad_status.h>
#include <snd/snd_system.h>
#include <mu/wifi/mu_wifi_interface.h>
// TODO: clean up, give variables real names
namespace jj_60 {

class gfTask {
public:
    enum Category {};
};

struct FriendListArr {
    char pad[0x48];
    s32 arr[93];
    s32 arr2[18];
};

extern "C" {
extern const char lbl_14_data_0[];
extern u8 lbl_14_data_4C4[];
extern const float lbl_14_rodata_0;
gfTask* __ct__6gfTaskFPCcQ26gfTask8Categoryiib(
    gfTask*, const char*, gfTask::Category, int, int, bool);
void fn_800B96E8(void*, int);
void __ct__19MuSelctChrNameEntryFv(void*);
}

extern "C" gfTask* __ct__20muWifiFriendListTaskFv(gfTask* obj) {
    __ct__6gfTaskFPCcQ26gfTask8Categoryiib(
        obj, lbl_14_data_0, static_cast<gfTask::Category>(8), 0xF, 8, true);
    char* self = reinterpret_cast<char*>(obj);
    *(s8*)(self + 0x40) = 0;
    *(void**)(self + 0x3C) = lbl_14_data_4C4;
    *(s32*)(self + 0x44) = 0;
    *(s32*)(self + 0x204) = 0;
    fn_800B96E8(self + 0x208, 0xFF);
    *(s32*)(self + 0x2B0) = 0;
    *(s32*)(self + 0x24C0) = 0;
    *(s32*)(self + 0x24C4) = 0;
    *(f32*)(self + 0x24C8) = lbl_14_rodata_0;
    *(s32*)(self + 0x24CC) = 0;
    *(s32*)(self + 0x24D0) = 0;
    *(s32*)(self + 0x24D4) = 0;
    *(s32*)(self + 0x24D8) = 0;
    *(s32*)(self + 0x24DC) = 0;
    *(s32*)(self + 0x24E0) = 0;
    *(s32*)(self + 0x24E4) = 0;
    *(s8*)(self + 0x24E8) = 0;
    *(s32*)(self + 0x24EC) = 0;
    *(s32*)(self + 0x24F0) = 0;
    *(s32*)(self + 0x24F4) = 0;
    *(s32*)(self + 0x24F8) = 0;
    *(s32*)(self + 0x24FC) = 0;
    *(s32*)(self + 0x2510) = 0;
    *(s32*)(self + 0x2514) = 0;
    *(s32*)(self + 0x2518) = 0;
    __ct__19MuSelctChrNameEntryFv(self + 0x2570);
    *(s32*)(self + 0x2A04) = 0;
    *(f32*)(self + 0x2A08) = lbl_14_rodata_0;
    *(s32*)(self + 0x2A0C) = 0;
    *(s32*)(self + 0x2A10) = 0;
    *(s32*)(self + 0x2A14) = 0;
    *(s8*)(self + 0x2A18) = 0;
    *(s32*)(self + 0x2A1C) = 0;
    *(s32*)(self + 0x2A20) = 0;
    *(s8*)(self + 0x2A24) = 0;
    *(s32*)(self + 0x2A28) = 0;
    *(s32*)(self + 0x2A2C) = 0;
    *(s32*)(self + 0x2A34) = 0;
    *(s8*)(self + 0x2A38) = 0;
    *(s32*)(self + 0x2ABC) = 0;
    *(s32*)(self + 0x2AC0) = 0;
    *(s32*)(self + 0x2AC4) = 0;
    *(u8*)(self + 0x2C) &= ~2;
    FriendListArr* s = reinterpret_cast<FriendListArr*>(obj);
    for (int i = 0; i < 93; i++) {
        s->arr[i] = 0;
    }
    for (int i = 0; i < 18; i++) {
        s->arr2[i] = 0;
    }
    memset(self + 0x2B8, 0, 0x2000);
    memset(self + 0x22B8, 0, 0x40);
    memset(self + 0x2500, 0, 0xD);
    memset(self + 0x22F8, 0, 0x1C8);
    memset(self + 0x2604, 0, 0x400);
    memset(self + 0x2A39, 0, 0x80);
    return obj;
}
}

// TODO: clean up, give variables real names
namespace jj_2E8 {
typedef unsigned char u8;

class MenuObject {
    u8 padding[0x5C];

public:
    virtual ~MenuObject();
};

extern "C" {
extern u8 lbl_14_data_4C4[];

void Destroy__Q34nw4r3g3d6G3dObjFv(void*);
void __dt__5MuMsgFv(void*, int);
void __dt__6gfTaskFv(void*, int);
void __dl__FPv(void*);

void* __dt__20muWifiFriendListTaskFv(u8* object, int deleting)
{
    if (object) {
        *reinterpret_cast<void**>(object + 0x3C) = lbl_14_data_4C4;

        void* entry = *reinterpret_cast<void**>(object + 0x2A1C);
        if (entry) {
            Destroy__Q34nw4r3g3d6G3dObjFv(entry);
        }

        u8* cursor = object;
        for (int i = 0; i < 0x12; ++i, cursor += 4) {
            entry = *reinterpret_cast<void**>(cursor + 0x1BC);
            if (entry) {
                Destroy__Q34nw4r3g3d6G3dObjFv(entry);
            }
        }

        cursor = object;
        for (int i = 0; i < 0x5D; ++i, cursor += 4) {
            MenuObject* menuEntry =
                *reinterpret_cast<MenuObject**>(cursor + 0x48);
            if (menuEntry) {
                delete menuEntry;
            }
        }

        entry = *reinterpret_cast<void**>(object + 0x24CC);
        if (entry) {
            __dt__5MuMsgFv(entry, 1);
        }

        __dt__6gfTaskFv(object, 0);

        if (deleting > 0) {
            __dl__FPv(object);
        }
    }

    return object;
}
}
}

// TODO: clean up, give variables real names
namespace jj_964 {
extern "C" {
void createMyMiiCandidate__5gfMiiFv(void*);
void setFriendMiiData__5gfMiiFUsPC12RFLStoreData(void*, unsigned char, void*);
void createFriendMii__5gfMiiFv(void*);
void fn_800D2088(int, unsigned char, void*);
void* memset(void*, int, unsigned long);
extern int g_muWifiInterfaceTask;
}

extern "C" void requestCreateMiiTex__20muWifiFriendListTaskFv(unsigned char* self)
{
    unsigned char buf[12];
    unsigned char out[0x4C];
    unsigned char id;
    unsigned char* p;
    int i;
    unsigned char* ptr;

    createMyMiiCandidate__5gfMiiFv(*(void**)(self + 0x2A2C));
    memset(buf, 0, 12);
    p = self;
    i = 0;
    while (i < *(int*)(self + 0x24C0)) {
        if (p[0x2B9] == 0xFF) {
            id = p[0x2B8];
            if (self[0x40] == 0) {
                fn_800D2088(g_muWifiInterfaceTask, id, out);
                ptr = out;
            }
            setFriendMiiData__5gfMiiFUsPC12RFLStoreData(*(void**)(self + 0x2A2C), id, ptr);
        }
        p += 0x80;
        i++;
    }
    createFriendMii__5gfMiiFv(*(void**)(self + 0x2A2C));
}
}

// TODO: clean up, give variables real names
namespace jj_A18 {
typedef unsigned char u8;
typedef wchar_t u16;
typedef unsigned long long u64;

class Message {
public:
    static void utf8to16(u16*, const char*);
};

extern "C" {
struct muWifiInterfaceTask;
extern muWifiInterfaceTask* g_muWifiInterfaceTask;

void putMyFriendStatusName__19muWifiInterfaceTaskFPCw(muWifiInterfaceTask*, const u16*);
void fn_800D24CC(muWifiInterfaceTask*, unsigned int);
void setMyRFLID__19muWifiInterfaceTaskFUx(muWifiInterfaceTask*, u64);
void getMyMiiStoreDataByCreateID__5gfMiiFUxP12RFLStoreData(void*, u64, void*);
void fn_800D2520(muWifiInterfaceTask*, const void*);
void fn_800D2AD0(muWifiInterfaceTask*, const u16*, int);
void fn_800D2428(muWifiInterfaceTask*, const u16*);

void flushMyProfile__20muWifiFriendListTaskFv(u8* self) {
    u8 info[84];
    u16 finalText[22];
    u16 entries[22];
    u16 name[6];
    unsigned int selection;
    u64 id;
    const char* entry;
    muWifiInterfaceTask* task = g_muWifiInterfaceTask;

    Message::utf8to16(name, (const char*)self + 0x2310);
    putMyFriendStatusName__19muWifiInterfaceTaskFPCw(task, name);

    selection = self[0x2300];
    fn_800D24CC(task, selection);
    if (selection == 0xFF) {
        id = *(u64*)(self + 0x2308);
        setMyRFLID__19muWifiInterfaceTaskFUx(task, id);
        getMyMiiStoreDataByCreateID__5gfMiiFUxP12RFLStoreData(*(void**)(self + 0x2A2C), id, info);
        fn_800D2520(task, info);
    }

    entry = (const char*)self + 0x2325;
    for (selection = 0; (int)selection < 4; ++selection, entry += 0x51) {
        Message::utf8to16(entries, entry);
        fn_800D2AD0(task, entries, selection);
    }

    Message::utf8to16(finalText, (const char*)self + 0x2469);
    fn_800D2428(task, finalText);
}
}
}

// TODO: clean up, give variables real names
namespace jj_B14 {
extern "C" {
extern void* g_GameGlobal;
extern char lbl_14_data_50[];
void* getGlobalRecordGameDatap__10GameGlobalFv(void*);
int exchangeMuCharKindToMuStockchkind__6muMenuFiii(int);
int fn_800AF6F0(int);
int isCharUsable__6muMenuFiii(int);
u16 getMyMiiCandidateSize__5gfMiiFv(void*);
}
static inline bool usable(int want) {
    for (int k = 0; k < 0x23; ++k) {
        if (fn_800AF6F0(exchangeMuCharKindToMuStockchkind__6muMenuFiii(k)) == want && isCharUsable__6muMenuFiii(k))
            return true;
    }
    return false;
}
extern "C" int makeIconList__20muWifiFriendListTaskFv(char* base, u8* out, int max, u8* fileBuf, u32 fileSz) {
    int count = 0;
    char* mgr = (char*)getGlobalRecordGameDatap__10GameGlobalFv(g_GameGlobal);
    utRelocate rel(fileBuf, fileSz);
    u8* tbl = (u8*)rel.getPublicAddress(lbl_14_data_50);
    int i = 0;
    u8* p = tbl;
    for (; *p != 0xff && i < 0x80; p += 3)
        base[0x2a39 + i++] = *p;
    for (; i < 0x80; ++i)
        base[0x2a39 + i] = 0;
    int idx = 0;
    int zero = 0;
    for (p = tbl; *p != 0xff && count < max; p += 3, ++idx) {
        int ok = 0;
        switch (p[1]) {
        case 1:
            if (usable(p[2])) ok = 1;
            break;
        case 2:
            if (isCharUsable__6muMenuFiii(p[2])) ok = 1;
            break;
        case 3:
            if (*(u32*)(mgr + 0x804) >> 31) ok = 1;
            break;
        case 0:
        default:
            ok = 1;
            break;
        }
        if (ok) {
            out[0] = idx;
            ++count;
            *(u16*)(out + 2) = zero;
            out += 4;
        }
    }
    u16 extra;
    if (*(void**)(base + 0x2a2c) == 0)
        extra = 0;
    else
        extra = getMyMiiCandidateSize__5gfMiiFv(*(void**)(base + 0x2a2c));
    for (u16 j = 0; j < extra && count < max; ++j) {
        out[0] = (u8)0xff;
        ++count;
        *(u16*)(out + 2) = j;
        out += 4;
    }
    return count;
}
}

// TODO: clean up, give variables real names
namespace jj_ED0 {
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long long u64;

struct Input {
    u8 kind;
    u8 pad;
    u16 index;
};

struct Object {
    u8 pad0[0x2300];
    u8 kind;
    u8 pad1[7];
    u64 value;
    u8 pad2[0x71C];
    void* context;
};

extern "C" u64 getMyMiiID__5gfMiiFUs(void*, u16);

extern "C" void setMyIconList__20muWifiFriendListTaskFv(Object* self, const Input* input) {
    if (input->kind != 0xFF) {
        self->kind = input->kind;
    } else {
        self->kind = 0xFF;
        self->value = getMyMiiID__5gfMiiFUs(self->context, input->index);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_13CC {
extern "C" {
void* memmove(void* dst, const void* src, unsigned int n);
int deleteFriend__10NtMatchingFi(void* arg0, unsigned char arg1);
void fn_800D353C(int task);
}

extern int g_muWifiInterfaceTask;
extern int lbl_805A0434;

extern "C" void deleteFriend__20muWifiFriendListTaskFv(unsigned char* self, unsigned int idx) {
    unsigned char* p;
    unsigned char* slot = self + ((idx << 7) & 0x7F80);
    unsigned char* dst = slot + 0x2B8;
    unsigned char r31 = slot[0x2B8];
    int r6 = *(int*)(self + 0x24C0) - (int)idx - 1;
    int count;
    int j;
    int k;
    int task;

    if (r6 > 0) {
        memmove(dst, self + ((idx + 1) << 7) + 0x2B8, r6 << 7);
    }

    count = *(int*)(self + 0x24C0);
    for (j = 0; j < count; j++) {
        if (idx == self[0x22B8 + j]) {
            break;
        }
    }

    if (count - j - 1 > 0) {
        memmove(self + 0x22B8 + j, self + 0x22B9 + j, count - j - 1);
    }

    *(int*)(self + 0x24C0) = *(int*)(self + 0x24C0) - 1;

    for (k = 0; k < *(int*)(self + 0x24C0); k++) {
        p = self + k;
        if (p[0x22B8] >= idx) {
            p[0x22B8] = (unsigned char)(p[0x22B8] - 1);
        }
    }

    if (self[0x40] == 0) {
        task = g_muWifiInterfaceTask;
        deleteFriend__10NtMatchingFi((void*)lbl_805A0434, r31);
        fn_800D353C(task);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_14F0 {
typedef unsigned char u8;

struct Entry {
    const char* name;
    u8 begin;
    u8 end;
    u8 kind;
};

struct Owner {
    char pad[0x48];
    void* objs[1];
};

extern "C" {
extern char lbl_14_data_3D0[];
extern const float lbl_14_rodata_0;

void* create__8MuObjectFPQ34nw4r3g3d7ResFilePCciPQ34nw4r3g3d7ResFileQ25Heaps8HeapType(void*, const char*, int, void*, int);
void changeAnimN__8MuObjectFPCc(void*, const char*);
void setUpdateRate__16gfModelAnimationFf(void*, float);
char* strcpy(char*, const char*);
char* strcat(char*, const char*);
}

extern "C" void createObjResFile__20muWifiFriendListTaskFv(void* self, Entry* entries, int count, void* resFile, int argB) {
    char buf[0x48];
    int n;
    int i;
    int j;
    Entry* entry = entries;
    void* obj;

    for (i = 0; i < count; i++, entry++) {
        n = 1;
        if (entry->begin < entry->end) {
            n = entry->end - entry->begin;
        }
        for (j = 0; j < n; j++) {
            obj = create__8MuObjectFPQ34nw4r3g3d7ResFilePCciPQ34nw4r3g3d7ResFileQ25Heaps8HeapType(
                resFile, entry->name, entry->kind + argB, 0, 0x2a);
            const char* name = entry->name;
            ((Owner*)self)->objs[entry->begin + j] = obj;
            strcpy(buf, name);
            strcat(buf, lbl_14_data_3D0);
            changeAnimN__8MuObjectFPCc(obj, buf);
            if (*(void**)((char*)obj + 0x14) != 0) {
                setUpdateRate__16gfModelAnimationFf(*(void**)((char*)obj + 0x14), lbl_14_rodata_0);
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_24CC {

extern "C" void Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(void*, void*, char*);
extern "C" void setFrameNode__8MuObjectFf(void*, float);
extern "C" int changeAnimN__8MuObjectFiUlbb(void*, int, int, int, int);

extern float lbl_14_rodata_200;
extern const double lbl_14_rodata_210;
extern char lbl_14_data_3DC[];

struct FriendListVirtual {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8(float);
};

struct FriendListInner {
    char pad[0x18];
    FriendListVirtual* object;
};

struct FriendListMiddle {
    char pad[0x14];
    FriendListInner* inner;
};

extern "C" void initIconListWnd__20muWifiFriendListTaskFv(void* self) {
    char* base = (char*)self;
    int frame = (*(int*)(base + 0x2A04) + 5) / 6;
    void* model = *(void**)(base + 0x1F0);

    if (frame > 6) {
        void* scene = *(void**)(base + 0xD4);
        void* extra = *(void**)(base + 0x1F8);
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(
            extra, *(void**)((char*)scene + 0x10), lbl_14_data_3DC);
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(
            model, extra, lbl_14_data_3DC);
        frame = 6;
    }

    setFrameNode__8MuObjectFf(*(void**)(base + 0xCC),
                              (float)frame - lbl_14_rodata_200);

    void* sceneModel = *(void**)(base + 0x1EC);
    Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(
        sceneModel, model, lbl_14_data_3DC);
    Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(
        sceneModel, *(void**)((char*)*(void**)(base + 0x18C) + 0x10),
        lbl_14_data_3DC);
    Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(
        *(void**)(base + 0x1EC), *(void**)(base + 0x1F4), lbl_14_data_3DC);

    FriendListMiddle* first = (FriendListMiddle*)*(void**)(base + 0x180);
    first->inner->object->v8(lbl_14_rodata_200);

    changeAnimN__8MuObjectFiUlbb(*(void**)(base + 0xD4), 1, 2, 0, 0);

    FriendListMiddle* second = (FriendListMiddle*)*(void**)(base + 0xD4);
    second->inner->object->v8(lbl_14_rodata_200);
}
}

// TODO: clean up, give variables real names
namespace jj_2D04 {
class Sub {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void call(void*);
};

struct Self {
    char pad0[0xAC];
    void* unkAC;
    char pad1[0x1BC - 0xB0];
    Sub* unk1BC;
    char pad2[0x1D4 - 0x1C0];
    void* unk1D4;
    char pad3[0x24C0 - 0x1D8];
    s32 unk24C0;
    char pad4[0x24CC - 0x24C4];
    void* unk24CC;
};

extern "C" {
void printf__5MuMsgFUlPCce(void* msg, u32 index, const char* fmt, ...);
void setFrameMatCol__8MuObjectFf(void* obj, float f);
void Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(void*, void*, char*);

extern char lbl_14_data_3FC[];
extern char lbl_14_data_404[];
extern float lbl_14_rodata_0;
extern float lbl_14_rodata_200;

void dispNumFriend__20muWifiFriendListTaskFv(Self* self) {
    printf__5MuMsgFUlPCce(self->unk24CC, 0x13, lbl_14_data_3FC, self->unk24C0, 0x40);
    float f;
    if ((0x40 - self->unk24C0) <= 0) {
        f = lbl_14_rodata_200;
    } else {
        f = lbl_14_rodata_0;
    }
    setFrameMatCol__8MuObjectFf(self->unkAC, f);
    Sub* obj = self->unk1BC;
    void* arg = self->unk1D4;
    obj->call(arg);
    if (self->unk24C0 > 3) {
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(obj, arg, lbl_14_data_404);
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_3508 {
struct Entry { unsigned char a, b, c, enabled; };
extern Entry lbl_14_rodata_290[];
extern "C" int isNodeAnimFinished__8MuObjectFv(void*);
extern "C" int setBaseState__20muWifiFriendListTaskFv(void*, int);
extern "C" void baseMain__20muWifiFriendListTaskFv(void* self) {
    unsigned int state = *(unsigned int*)((char*)self + 0x2b0);
    if (lbl_14_rodata_290[state].enabled && isNodeAnimFinished__8MuObjectFv(*(void**)((char*)self + 0x4c))) {
        switch (state) {
        case 1: case 4: case 7: case 10: case 13:
            setBaseState__20muWifiFriendListTaskFv(self, 0); break;
        case 2: setBaseState__20muWifiFriendListTaskFv(self, 3); break;
        case 5: setBaseState__20muWifiFriendListTaskFv(self, 6); break;
        case 8: setBaseState__20muWifiFriendListTaskFv(self, 9); break;
        case 11: setBaseState__20muWifiFriendListTaskFv(self, 12); break;
        case 14: setBaseState__20muWifiFriendListTaskFv(self, 15); break;
        case 16: setBaseState__20muWifiFriendListTaskFv(self, 12); break;
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_39BC {
extern "C" {
extern char lbl_14_data_3DC[];
extern const float lbl_14_rodata_0;
extern const float lbl_14_rodata_200;

void Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(void*, void*, char*);
void setFrameVisible__8MuObjectFf(void*, float);
}

struct Scene {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void remove(int);

    char pad[0xE0];
    int count;
};

struct Object {
    char pad[0x10];
    void* model;
};

struct Menu {
    char pad0[0xB0];
    void* object;
    char pad1[4];
    Object* first;
    Object* second;
    char pad2[0x124];
    Scene* scene;
};

extern "C" void prepareFriendCodeTextBox__20muWifiFriendListTaskFv(Menu* self, int mode) {
    Scene* scene = self->scene;
    while (scene->count != 0) {
        if (scene->count != 0) {
            scene->remove(scene->count - 1);
        }
    }

    float frame;
    if (mode == 0) {
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(
            scene, self->first->model, lbl_14_data_3DC);
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(
            scene, self->second->model, lbl_14_data_3DC);
        frame = lbl_14_rodata_0;
    } else {
        frame = lbl_14_rodata_200;
    }

    setFrameVisible__8MuObjectFf(self->object, frame);
}
}

// TODO: clean up, give variables real names
namespace jj_3A7C {
extern "C" {
extern const float lbl_14_rodata_0;
extern const float lbl_14_rodata_200;
void setFrameNode__8MuObjectFf(void*, float);
void setFrameVisible__8MuObjectFf(void*, float);
unsigned long strlen(const char*);
}

struct Menu {
    char pad0[0xB8];
    void* object;
    void* other;
    char pad1[0x2438];
    int state;
    int mode;
    char text[1];
};

extern "C" void setFriendCodeNumCursorPos__20muWifiFriendListTaskFv(Menu* self, int mode) {
    float frame = mode;
    void* object = self->object;
    setFrameNode__8MuObjectFf(object, frame);
    setFrameVisible__8MuObjectFf(object, frame);
    float visible;
    if (strlen(self->text) >= 12) {
        visible = lbl_14_rodata_200;
    } else {
        visible = lbl_14_rodata_0;
    }
    setFrameVisible__8MuObjectFf(self->other, visible);
    self->mode = mode;
    self->state = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_3C50 {

extern "C" void printf__5MuMsgFUlPCce(void*, unsigned long, const char*, ...);
extern const char lbl_14_data_410[];

extern "C" void dispCode__20muWifiFriendListTaskFv(void* self) {
    char* base = (char*)self;
    int i;
    unsigned offset = 0;
    unsigned len = strlen(base + 0x2500);
    i = 0;

    for (; i < 3 && offset < len; i++) {
        char buf[4];
        int j = 0;
        while (j < 4 && offset < len) {
            buf[j] = (base + offset)[0x2500];
            j++;
            offset++;
        }
        buf[j] = 0;
        printf__5MuMsgFUlPCce(*(void**)(base + 0x24CC), i + 0x18, buf);
    }

    for (; i < 3; i++) {
        printf__5MuMsgFUlPCce(*(void**)(base + 0x24CC), i + 0x18, lbl_14_data_410);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_3EFC {
typedef unsigned char u8;
typedef unsigned long long u64;

extern "C" {
void flushMyProfile__20muWifiFriendListTaskFv(void* self);
int getMyMiiStoreDataByCreateID__5gfMiiFUxP12RFLStoreData(void* p, u64 id, void* info);

int mainStepAppear__20muWifiFriendListTaskFv(void* self) {
    u8* s = (u8*)self;
    int state = *(int*)(s + 0x24c4);
    if (*(int*)(s + 0x2b0) != 1) {
        void* p = *(void**)(s + 0x2a2c);
        state = 0;
        if (p != 0) {
            if (s[0x2300] == 0xff) {
                u8 info[80];
                if (getMyMiiStoreDataByCreateID__5gfMiiFUxP12RFLStoreData(p, *(u64*)(s + 0x2308), info) == 0) {
                    state = 2;
                }
            }
        } else {
            state = 1;
        }
        if (state == 0) {
            state = 2;
        } else {
            if (state == 1) {
                *(int*)(s + 0x2a34) = 0x15;
            } else {
                *(int*)(s + 0x2a34) = 0x16;
            }
            state = 1;
            s[0x2300] = 0;
        }
        if (s[0x40] == 0) {
            flushMyProfile__20muWifiFriendListTaskFv(self);
        }
    }
    return state;
}
}
}

// TODO: clean up, give variables real names
namespace jj_4238 {


struct RawPadStatus {
    u32 words[16];
};

struct Vec2 {
    float x;
    float y;
    Vec2() {}
    Vec2(const Vec2& other) {
        x = other.x;
        y = other.y;
    }
};

struct PadStatus {
    u32 words[6];
    Vec2 vectors[3];
    unsigned char bytes[9];
    int last;
};

struct Scene {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void remove(void*);
};

struct Object {
    char pad[0x10];
    void* model;
};

struct Entry {
    unsigned char scene;
    char pad[3];
    char* node;
    unsigned char frame;
    char tail[3];
};

extern "C" {
extern gfPadSystem* g_gfPadSystem;
extern const Entry lbl_14_rodata_3E8[];

void getSysPadStatus__11gfPadSystemFiP11gfPadStatus(void*, int, RawPadStatus*);
void Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(void*, void*, char*);
void setFrameNode__8MuObjectFf(void*, float);
void playSE__9sndSystemF5SndIDiiii(void*, int, int, int, int, int);
}

inline RawPadStatus getRawPadStatus() {
    RawPadStatus status;
    getSysPadStatus__11gfPadSystemFiP11gfPadStatus(g_gfPadSystem, 0xF0, &status);
    return status;
}

struct Input {
    PadStatus status;

    Input(const PadStatus& source) : status(source) {}
};

inline void updateSelection(char* self, int choice) {
    Object* object;
    int selected;

    switch (choice) {
    case 0:
        selected = 0;
        break;
    case 1:
        selected = 1;
        break;
    case 2:
        selected = 6;
        break;
    }

    object = *(Object**)(self + 0x94);
    for (int i = 0; i < 8; i++) {
        Scene* scene =
            *(Scene**)(self + 0x1BC + lbl_14_rodata_3E8[i].scene * 4);
        scene->remove(object->model);
    }

    if (selected != 7) {
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(
            *(void**)(self + 0x1BC +
                      lbl_14_rodata_3E8[selected].scene * 4),
            object->model,
            lbl_14_rodata_3E8[selected].node);

        setFrameNode__8MuObjectFf(
            object, (float)lbl_14_rodata_3E8[selected].frame);
    }
}

extern "C" int mainStepTopMenu__20muWifiFriendListTaskFv(char* self) {
    int choice;
    int state = *(int*)(self + 0x24C4);

    choice = *(int*)(self + 0x24D4);

    Input input(*(PadStatus*)&getRawPadStatus());
    u32 buttons = *(u32*)(self + 0x218);

    if (buttons & 8) {
        if (--choice < 0)
            choice = 0;

        if (choice == 1) {
            if (*(int*)(self + 0x2A28) != 0 ||
                64 - *(int*)(self + 0x24C0) <= 0)
                --choice;
        }
    }

    if (buttons & 4) {
        if (++choice >= 3)
            choice = 2;

        if (choice == 1) {
            if (*(int*)(self + 0x2A28) != 0 ||
                64 - *(int*)(self + 0x24C0) <= 0) {
                choice = *(int*)(self + 0x24C0) <= 0 ? 0 : choice + 1;
            }
        }

        if (*(int*)(self + 0x24C0) <= 0 && choice == 2)
            choice = 1;
    }

    if (choice != *(int*)(self + 0x24D4)) {
        updateSelection(self, choice);
        *(int*)(self + 0x24D4) = choice;
        playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0, -1, 0, 0, -1);
    }

    if (input.status.words[3] & 0x200) {
        playSE__9sndSystemF5SndIDiiii(g_sndSystem, 2, -1, 0, 0, -1);
        *(int*)(self + 0x44) = 7;
        state = 0x21;
    }

    if ((input.status.words[3] & 0x100) ||
        (input.status.words[3] & 0x1000)) {
        playSE__9sndSystemF5SndIDiiii(g_sndSystem, 1, -1, 0, 0, -1);

        switch (*(int*)(self + 0x24D4)) {
        case 0:
            if (*(int*)(self + 0x2A28) == 0)
                state = 0x18;
            else
                state = 0x20;
            break;
        case 1:
            state = 0x11;
            break;
        case 2:
            state = 3;
            break;
        }
    }

    if (choice == 2 && *(int*)(self + 0x24C0) <= 3) {
        *(int*)(self + 0x24E0) = 1;
        state = 4;
        *(int*)(self + 0x24D8) = 0;
    }

    return state;
}
}

// TODO: clean up, give variables real names
namespace jj_6538 {
extern "C" unsigned char lbl_8059E742;

extern "C" unsigned char fn_14_6538() {
    return lbl_8059E742;
}
}

// TODO: clean up, give variables real names
namespace jj_65A0 {

extern "C" {
int isPermittedEntry__19muWifiInterfaceTaskFi(void*, int);
void fn_800CCEC4(void*, int);
void deleteWnd__15muNoticeWndTaskFv(void*);
extern void* g_muWifiInterfaceTask;
extern unsigned char lbl_14_rodata_21C[0x70];
extern unsigned char lbl_14_rodata_480[0x38];
}

struct MenuObject {
    virtual void v0() = 0;
    virtual void v1() = 0;
    virtual void v2() = 0;
    virtual void v3() = 0;
    virtual void v4() = 0;
    virtual void v5() = 0;
    virtual void v6() = 0;
    virtual void v7() = 0;
    virtual void v8() = 0;
    virtual void v9() = 0;
    virtual void v10() = 0;
    virtual void v11() = 0;
    virtual void v12() = 0;
    virtual void v13(void*) = 0;
};

struct Entry {
    unsigned char a;
    unsigned char b;
    unsigned char c;
    unsigned char pad;
    unsigned short d;
    unsigned char e;
    unsigned char pad2;
};

struct Self {
    char pad0[0x40];
    unsigned char f40;
    char pad41[3];
    int f44;
    char pad48[0x190 - 0x48];
    struct F190 {
        char pad[0x10];
        void* f10;
    } *f190;
    char pad194[0x1e0 - 0x194];
    MenuObject* f1e0;
    char pad1e4[0x24c4 - 0x1e4];
    int f24c4;
    char pad24c8[0x24ec - 0x24c8];
    struct F24EC {
        char pad[0x104];
        int status;
    } *f24ec;
    char pad24f0[0x2a1c - 0x24f0];
    MenuObject* f2a1c;
    int f2a20;
    char pad2a24[0x2a30 - 0x2a24];
    unsigned char f2a30;
    char pad2a31[3];
    int f2a34;
    unsigned char f2a38;
};

extern "C" int mainWaitJoinGamePermitted__20muWifiFriendListTaskFv(Self* self) {
    int status;

    if (self->f40 == 0) {
        int result = isPermittedEntry__19muWifiInterfaceTaskFi(g_muWifiInterfaceTask, 0);
        switch (result) {
        case 0:
            status = 0;
            break;
        case 1:
            status = 1;
            break;
        case 2:
            status = 2;
            break;
        }
    }

    if (self->f24ec->status == 3) {
        if (self->f40 == 0) {
            fn_800CCEC4(g_muWifiInterfaceTask, 0);
        }
        status = 4;
    }

    if (status == 1) {
        int index = self->f2a20;
        Entry entry;

        entry.a = lbl_14_rodata_21C[index * 3];
        entry.b = *(&lbl_14_rodata_21C[index * 3] + 1);
        entry.c = *(&lbl_14_rodata_21C[index * 3] + 2);

        if (self->f2a30 == 0 &&
            (index == 6 || index == 0x12 || index == 0x16 || index == 0x1c)) {
            entry.a = 0x11;
            entry.c = 6;
        }

        entry.d = *(unsigned short*)&entry;
        entry.e = entry.c;
        self->f44 = lbl_14_rodata_480[entry.c];
        return 0x21;
    }

    if (status == 2 || status == 4) {
        deleteWnd__15muNoticeWndTaskFv(self->f24ec);
        ((gfTask*)self->f24ec)->exit();
        self->f24ec = 0;
        self->f1e0->v13(self->f190->f10);
        self->f2a1c->v13(self->f1e0);

        if (status == 2) {
            self->f2a34 = 0x1b;
            self->f2a38 = 1;
        } else {
            self->f2a38 = 0;
            self->f2a34 = 0x19;
        }
        return 0xd;
    }

    return self->f24c4;
}
}

// TODO: clean up, give variables real names
namespace jj_6788 {

extern "C" void fn_800CCEC4(muWifiInterfaceTask* t, int a);
extern "C" int fn_800BEEB8(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12);

extern "C" void fn_14_6788(void* self) {
    char* base = (char*)self;
    int val;
    if (*(unsigned char*)(base + 0x2a38) != 0) {
        val = *(int*)(base + 0x24d0);
    } else {
        int* p = *(int**)(base + 0x24cc);
        val = p[1];
    }
    fn_800CCEC4(g_muWifiInterfaceTask, 0x64);
    int a5 = *(int*)(base + 0x2a34);
    int a6 = *(int*)(base + 0x24f0);
    int a7 = *(int*)(base + 0x2a1c);
    int res = fn_800BEEB8(0, val, a5, a6, a7, 0x1f, 0, 0x2a, 0x2b, 0, 0, 0xf0);
    *(int*)(base + 0x24ec) = res;
}
}

// TODO: clean up, give variables real names
namespace jj_72DC {
extern "C" int fn_800BEEB8(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12);

extern "C" void fn_14_72DC(void* p, int a2, int value) {
    char* self = (char*)p;

    switch (*(int*)(self + 0x2a28)) {
    case 0:
        break;
    case 1:
        value = 0x21;
        break;
    case 2:
        value = 0x22;
        break;
    case 3:
        value = 0x23;
        break;
    case 4:
        value = 0x24;
        break;
    }

    *(int*)(self + 0x24ec) = fn_800BEEB8(
        1, *(int*)(*(int*)(self + 0x24cc) + 4), value,
        *(int*)(self + 0x24f0), *(int*)(self + 0x2a1c),
        0x1f, 0, 0x2a, 0x2b, 0, 0, 0xf0);
}
}

// TODO: clean up, give variables real names
namespace jj_7394 {
extern "C" {
void startMatchingDaiRantou__19muWifiInterfaceTaskFi(void*, int);
void startMatchingBattle100__19muWifiInterfaceTaskFi(void*, int);
void startMatchingHomerun__19muWifiInterfaceTaskFi(void*, int);
void startMatchingBossBattle__19muWifiInterfaceTaskFi(void*, int);
int mainCreateGame__20muWifiFriendListTaskFv(unsigned char* self) {
    unsigned char* task = *(unsigned char**)(self + 0x24ec);
    if (task) {
        if (*(int*)(task + 0x104) == 3) {
            unsigned char flag = task[0x100];
            *(int*)(self + 0x2abc) = *(int*)(task + 0x124);
            ((gfTask*)task)->exit();
            *(unsigned char**)(self + 0x24ec) = 0;
            if (!flag) {
                g_sndSystem->playSE((SndID)6, -1, 0, 0, -1);
                return 2;
            }
        }
        return *(int*)(self + 0x24c4);
    }
    int result;
    if (!self[0x40]) {
        muWifiInterfaceTask* wifi = g_muWifiInterfaceTask;
        switch (*(int*)(self + 0x2a28)) {
        case 0: break;
        case 1: startMatchingDaiRantou__19muWifiInterfaceTaskFi(wifi, 4); break;
        case 2: startMatchingBattle100__19muWifiInterfaceTaskFi(wifi, 2); break;
        case 3: startMatchingHomerun__19muWifiInterfaceTaskFi(wifi, 2); break;
        case 4: startMatchingBossBattle__19muWifiInterfaceTaskFi(wifi, 2); break;
        }
    }
    switch (*(int*)(self + 0x2a28)) {
    case 0: break;
    case 1: result = 1; break;
    case 2: result = 2; break;
    case 3: result = 3; break;
    case 4: result = 4; break;
    }
    *(int*)(self + 0x44) = result;
    return 0x21;
}
}
}

// TODO: clean up, give variables real names
namespace jj_74F4 {
extern "C" {
int setBaseState__20muWifiFriendListTaskFv(void* self, int v);
void prepareFriendCodeTextBox__20muWifiFriendListTaskFv(void* self, int mode);
int changeAnimN__8MuObjectFiUlbb(void* p, int a, int b, int c, int d);
void dispCode__20muWifiFriendListTaskFv(void* self);
void setFriendCodeTextColor__20muWifiFriendListTaskFi(void* self, int v);
void setFriendCodeNumCursorPos__20muWifiFriendListTaskFv(void* self, int v);
void printIndex__5MuMsgFUlUlPv(void* msg, unsigned long a, unsigned long b, void* c);
void printf__5MuMsgFUlPCce(void* msg, unsigned long idx, const char* fmt, ...);
}

extern char lbl_14_data_568[];

class VObj {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void target(void* a, void* b);
};

extern "C" void initToFriendCode__20muWifiFriendListTaskFv(void* self) {
    char* s = (char*)self;
    setBaseState__20muWifiFriendListTaskFv(self, 5);
    prepareFriendCodeTextBox__20muWifiFriendListTaskFv(self, 0);
    changeAnimN__8MuObjectFiUlbb(*(void**)(s + 0xb0), 0, 1, 1, 0);
    {
        VObj* o = *(VObj**)(s + 0x204);
        o->target(*(void**)((char*)o + 0xe4), *(void**)(s + 0x1e4));
    }
    printIndex__5MuMsgFUlUlPv(*(void**)(s + 0x24cc), 0x17, 0x13, 0);
    *(unsigned char*)(s + 0x2500) = 0;
    dispCode__20muWifiFriendListTaskFv(self);
    setFriendCodeTextColor__20muWifiFriendListTaskFi(self, 1);
    printf__5MuMsgFUlPCce(*(void**)(s + 0x24cc), 0x1b, lbl_14_data_568);
    setFriendCodeNumCursorPos__20muWifiFriendListTaskFv(self, 0);
}
}

// TODO: clean up, give variables real names
namespace jj_75D8 {

extern "C" {
void initRepeatCounter__16muMenuControllerFi(void*, u32);
void setFriendCodeNumCursorPos__20muWifiFriendListTaskFv(void*, int);
void setFriendCodeCursorCate__20muWifiFriendListTaskFv(void*, int);
void dispCode__20muWifiFriendListTaskFv(void*);
void* getWifiIf__20muWifiFriendListTaskFv();
int fn_800D1F04(void*, unsigned long long, unsigned char*, unsigned char*);
}

struct Vec2 {
    float x, y;
    Vec2() {}
    Vec2(const Vec2& v) { x = v.x; y = v.y; }
};
struct PadStatus {
    u32 words[6];
    Vec2 vectors[3];
    unsigned char bytes[9];
    int last;
};
struct Input {
    PadStatus status;
    Input(const PadStatus& v) : status(v) {}
};
static inline gfPadStatus getSysPad() {
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(0xF0, &status);
    return status;
}
extern "C" int mainFriendCode__20muWifiFriendListTaskFv(char* self) {
    int flags = *(int*)(self + 0x218);
    Input input(*(PadStatus*)&getSysPad());
    int digit = *(int*)(self + 0x24FC);
    int state = *(int*)(self + 0x24F8);
    switch (state) {
    case 0:
        if (flags & 2) {
            if (++digit >= 10) digit = 0;
            if (digit == 9) initRepeatCounter__16muMenuControllerFi(self + 0x208, 2);
            g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
            setFriendCodeNumCursorPos__20muWifiFriendListTaskFv(self, digit);
        }
        if (flags & 1) {
            if (--digit < 0) digit = 9;
            if (digit == 0) initRepeatCounter__16muMenuControllerFi(self + 0x208, 1);
            g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
            setFriendCodeNumCursorPos__20muWifiFriendListTaskFv(self, digit);
        }
        if ((flags & 4) && state != 1) {
            g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
            state = 1;
            setFriendCodeCursorCate__20muWifiFriendListTaskFv(self, 1);
        }
        if ((input.status.words[3] & 0x100) || (input.status.words[3] & 0x1000)) {
            g_sndSystem->playSE((SndID)1, -1, 0, 0, -1);
            u32 pos = strlen(self + 0x2500);
            pos = pos < 11 ? pos : 11;
            self[0x2500 + pos] = digit + '0';
            self[0x2501 + pos] = 0;
            dispCode__20muWifiFriendListTaskFv(self);
            if (pos >= 11) {
                state = 2;
                setFriendCodeCursorCate__20muWifiFriendListTaskFv(self, 2);
            }
        }
        break;
    case 1:
        if ((flags & 8) && state != 0) {
            g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
            state = 0;
            setFriendCodeCursorCate__20muWifiFriendListTaskFv(self, 0);
        }
        if ((flags & 4) && strlen(self + 0x2500) >= 12) {
            g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
            state = 2;
            setFriendCodeCursorCate__20muWifiFriendListTaskFv(self, 2);
        }
        break;
    case 2:
        if (flags & 8) {
            g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
            state = 1;
            setFriendCodeCursorCate__20muWifiFriendListTaskFv(self, 1);
        }
        if ((input.status.words[3] & 0x100) || (input.status.words[3] & 0x1000)) {
            unsigned char a, b;
            int result;
            if (*(unsigned char*)(self + 0x40) == 0) {
                unsigned long long number = 0;
                for (int i = 0; i < 12 && (signed char)self[0x2500 + i] != 0; i++)
                    number = number * 10 + ((signed char)self[0x2500 + i] - '0');
                result = fn_800D1F04(getWifiIf__20muWifiFriendListTaskFv(), number, &a, &b);
            } else {
                result = 1;
                a = 0;
                b = 0;
            }
            if (result == 0 || a != 0 || b != 0) {
                if (result == 0) *(int*)(self + 0x2AC4) = 0;
                else if (a != 0) *(int*)(self + 0x2AC4) = 1;
                else *(int*)(self + 0x2AC4) = 2;
                g_sndSystem->playSE((SndID)3, -1, 0, 0, -1);
                return 0x13;
            }
            g_sndSystem->playSE((SndID)1, -1, 0, 0, -1);
            return 0x15;
        }
        break;
    }
    int erase = 0;
    if (((input.status.words[5] & 0x100) || (input.status.words[5] & 0x1000)) && state == 1)
        erase = 1;
    if (input.status.words[5] & 0x200) erase = 1;
    if (erase) {
        g_sndSystem->playSE((SndID)2, -1, 0, 0, -1);
        int len = strlen(self + 0x2500);
        if (len == 0) return 0x14;
        (self + len)[0x24FF] = 0;
        dispCode__20muWifiFriendListTaskFv(self);
        if (state == 2) state = 1;
        setFriendCodeCursorCate__20muWifiFriendListTaskFv(self, state);
    }
    return *(int*)(self + 0x24C4);
}
}

// TODO: clean up, give variables real names
namespace jj_7CB4 {
extern "C" void* getWifiIf__20muWifiFriendListTaskFv();
extern "C" void close__19MuSelctChrNameEntryFv(void*);
extern "C" int isSaveFriendInfoDone__10NtMatchingFv(int);
extern int lbl_805A0434;

class Obj {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v14(int);
    virtual void v15(int);
};

extern "C" int mainFromFriendCode__20muWifiFriendListTaskFv(void* self) {
    unsigned char* p = (unsigned char*)self;
    int r;
    if (p[0x40] == 0) {
        getWifiIf__20muWifiFriendListTaskFv();
        r = isSaveFriendInfoDone__10NtMatchingFv(lbl_805A0434);
    } else {
        r = 1;
    }
    if (r != 0 && *(int*)(p + 0x2b0) != 7) {
        Obj* o = *(Obj**)(p + 0x204);
        o->v15(*(int*)(p + 0x1e4));
        if (*(int*)(p + 0x2570) != 0) {
            close__19MuSelctChrNameEntryFv(p + 0x2570);
        }
        Obj* n = *(Obj**)(p + 0x1fc);
        int c;
        while ((c = *(int*)((char*)n + 0xe4)) != 0) {
            if (c != 0) {
                n->v14(c - 1);
            }
        }
        return 2;
    }
    return *(int*)(p + 0x24c4);
}
}

// TODO: clean up, give variables real names
namespace jj_7EC4 {

extern f32 lbl_14_rodata_4D0;
extern f64 lbl_14_rodata_4C8;

extern "C" {
s32 main__19MuSelctChrNameEntryFP11gfPadStatusPcbb(void* self, gfPadStatus* pad, char* text, s32 a, s32 b);
void addFriendList__20muWifiFriendListTaskFv(void* self, s64 v, char* text, s32 a, s32 b, s32 c, char* d, s32 e);
void sortFriendList__20muWifiFriendListTaskFv(void* self);
void setFriendListScrollPosDirect__20muWifiFriendListTaskFv(void* self, f32 x);
void setFriendListCursorPos__20muWifiFriendListTaskFi(void* self, u32 v);
void redrawFriendList__20muWifiFriendListTaskFv(void* self);
}

struct Node {
    virtual void f00();
    virtual void f01();
    virtual void f02();
    virtual void f03();
    virtual void f04();
    virtual void f05();
    virtual void f06();
    virtual void f07();
    virtual void f08();
    virtual void f09();
    virtual void f10();
    virtual void f11();
    virtual void f12(s32 n);
    virtual void f13();
    virtual void f14(s32 n);
    char _04[0xE0];
    s32 m_count;
};

struct Self {
    char _000[0x0B4];
    MuObject* m_obj;
    char _0B8[0x1FC - 0x0B8];
    Node* m_node;
    char _200[0x210 - 0x200];
    u32 m_210;
    u32 m_214;
    u32 m_218;
    char _21C[0x24C0 - 0x21C];
    s32 m_count;
    s32 m_state;
    char _24C8[0x24E4 - 0x24C8];
    s32 m_saved;
    char _24E8[0x2500 - 0x24E8];
    char m_digits[0x70];
    char m_sub[0x84];
    s32 m_25F4;
};

inline s32 runInput(Self* self, gfPadStatus pad, char* text, gfPadStatus& tmp) {
    tmp.m_buttonsCurrentFrame = pad.m_buttonsCurrentFrame;
    tmp.m_buttonsCurrentFrame2 = pad.m_buttonsCurrentFrame2;
    tmp.m_buttonsHeld = pad.m_buttonsHeld;
    tmp.m_buttonsPressedThisFrame = pad.m_buttonsPressedThisFrame;
    tmp.m_buttonsReleasedThisFrame = pad.m_buttonsReleasedThisFrame;
    tmp.m_buttonsPressedThisFrame2 = pad.m_buttonsPressedThisFrame2;
    tmp._0x18 = pad._0x18;
    tmp._0x1c = pad._0x1c;
    tmp._0x20 = pad._0x20;
    tmp._0x24 = pad._0x24;
    tmp._0x28 = pad._0x28;
    tmp._0x2c = pad._0x2c;
    tmp.m_stickX = pad.m_stickX;
    tmp.m_stickY = pad.m_stickY;
    tmp.m_subStickX = pad.m_subStickX;
    tmp.m_subStickY = pad.m_subStickY;
    tmp.m_lTriggerAnalog = pad.m_lTriggerAnalog;
    tmp.m_rTriggerAnalog = pad.m_rTriggerAnalog;
    tmp._0x36 = pad._0x36;
    tmp._0x37 = pad._0x37;
    tmp.m_error = pad.m_error;
    tmp.m_controllerType = pad.m_controllerType;
    tmp.m_buttonsCurrentFrame2.bits = self->m_210;
    tmp.m_buttonsPressedThisFrame.bits = self->m_214;
    tmp.m_buttonsPressedThisFrame2.bits = self->m_218;
    return main__19MuSelctChrNameEntryFP11gfPadStatusPcbb(self->m_sub, &tmp, text, 1, 1);
}

inline s32 fetchInput(Self* self, char* text, gfPadStatus& tmp) {
    gfPadStatus pad;
    g_gfPadSystem->getSysPadStatus(0xF0, &pad);
    return runInput(self, pad, text, tmp);
}

extern "C" s32 mainFriendName__20muWifiFriendListTaskFv(Self* self) {
    s32 result = self->m_state;
    char flag = 0;
    char text[0x1C];
    gfPadStatus tmp;
    s32 r = fetchInput(self, text, tmp);
    switch (r) {
    case 0: {
        s64 v = 0;
        s32 i = 0;
        while (i < 0xC && self->m_digits[i] != 0) {
            v = v * 10 + (self->m_digits[i] - 0x30);
            i++;
        }
        addFriendList__20muWifiFriendListTaskFv(self, v, text, 0, 0, 0, &flag, 0);
        sortFriendList__20muWifiFriendListTaskFv(self);
        s32 n = self->m_count - 3;
        if (n < 0) {
            n = 0;
        }
        self->m_saved = n;
        setFriendListScrollPosDirect__20muWifiFriendListTaskFv(self, (f32)n);
        setFriendListCursorPos__20muWifiFriendListTaskFi(self, (u32)(self->m_count - n - 1));
        redrawFriendList__20muWifiFriendListTaskFv(self);
        result = 0x14;
        break;
    }
    case 1:
        result = 0x17;
        break;
    case 2:
        self->m_obj->setFrameNode((f32)self->m_25F4);
        break;
    case 0x7fffffff:
        break;
    default:
        break;
    }
    if (result != self->m_state) {
        self->m_obj->setFrameVisible(lbl_14_rodata_4D0);
        Node* node = self->m_node;
        while (node->m_count != 0) {
            if (node->m_count != 0) {
                node->f12(node->m_count - 1);
            }
        }
    }
    return result;
}
}

// TODO: clean up, give variables real names
namespace jj_8238 {
extern "C" {
int isNodeAnimFinished__8MuObjectFv(void*);
void close__19MuSelctChrNameEntryFv(void*);
void prepareFriendCodeTextBox__20muWifiFriendListTaskFv(void*, int);
void printIndex__5MuMsgFUlUlPv(void*, unsigned long, unsigned long, void*);
int changeAnimN__8MuObjectFiUlbb(void*, int, int, int, int);
}

class Scene {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void remove(int);

    char pad[0xE0];
    int count;
};

struct Menu {
    char pad0[0xB0];
    void* resource;
    char pad1[0x1FC - 0xB4];
    Scene* scene;
    char pad2[0x24C4 - 0x200];
    int ret;
    char pad3[4];
    void* msg;
    char pad4[0x2570 - 0x24D0];
    int state;
};

extern "C" int mainFriendNameToFriendCode__20muWifiFriendListTaskFv(Menu* self) {
    if (isNodeAnimFinished__8MuObjectFv(self->resource)) {
        if (self->state != 0) {
            close__19MuSelctChrNameEntryFv(&self->state);
        }
        Scene* scene = self->scene;
        int n;
        while (n = scene->count) {
            if (n == 0)
                continue;
            scene->remove(n - 1);
        }
        prepareFriendCodeTextBox__20muWifiFriendListTaskFv(self, 0);
        printIndex__5MuMsgFUlUlPv(self->msg, 0x17, 0x13, 0);
        changeAnimN__8MuObjectFiUlbb(self->resource, 1, 1, 0, 0);
        return 0x12;
    }
    return self->ret;
}
}

// TODO: clean up, give variables real names
namespace jj_8690 {
extern "C" {
extern char lbl_14_data_570[];
extern double fmod(float, double);
void Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(void*, void*, char*);
void setFrameVisible__8MuObjectFf(void*, float);
int sprintf(char*, const char*, ...);
}

struct Scene {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void pushback(void*);
};

struct Model {
    char pad[0x10];
    void* model;
};

struct RoData {
    float pad0[4];
    float f10;
    float pad1[3];
    double d20;
    float pad2[5];
    float f3c;
};

extern const RoData lbl_14_rodata_4D8;

struct Menu {
    char pad0[0xD4];
    void* objectD4;
    char pad1[0x180 - 0xD8];
    Model* model180;
    char pad2[0x1F4 - 0x184];
    Scene* scene;
    char pad3[0x2A08 - 0x1F8];
    float frame2A08;
    char pad4[0x2A18 - 0x2A0C];
    unsigned char flag2A18;
};

extern "C" void dispIconListCursor__20muWifiFriendListTaskFv(Menu* self, char* str, int param) {
    const RoData* rodata = &lbl_14_rodata_4D8;
    Scene* scene = self->scene;
    Model* model = self->model180;

    scene->pushback(model->model);

    if (self->flag2A18 == 0) {
        float x = self->frame2A08;
        int frame = (int)x;
        int digits = 6;
        if ((float)fmod(x, rodata->d20) != rodata->f10) {
            digits = 7;
        }
        if (param >= frame && param < frame + digits) {
            char buf[8];
            sprintf(buf, lbl_14_data_570, str + (param - frame) * 6);
            Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(scene, model->model, buf);
        }
        setFrameVisible__8MuObjectFf(self->objectD4, rodata->f10);
    } else {
        setFrameVisible__8MuObjectFf(self->objectD4, rodata->f3c);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_878C {
extern "C" {
void setBaseState__20muWifiFriendListTaskFv(void* self, int a);
void setVisibleApealPanelHelp__20muWifiFriendListTaskFi(void* self, int value);
void dispCursor__20muWifiFriendListTaskFv(void* self, int a);
int getProfileCursorPosByCate__20muWifiFriendListTaskFiii(int mode, int value, int index);
void setProfileCursorPos__20muWifiFriendListTaskFi(void* self, int a);
char* getMyApeal__20muWifiFriendListTaskFPci(char* base, char* dst, int idx);
void appendSubstr__7MessageFPcPCcii(char* dst, char* src, int a, int b);
int strlenUtf8__14msTagProcessorFPCci(char* s, int n);
int changeAnimN__8MuObjectFiUlbb(void* p, int a, int b, int c, int d);
void printf__5MuMsgFUlPCce(void* msg, unsigned long index, const char* fmt, ...);
char* strcpy(char* dst, const char* src);
int strlen(const char* s);
}

struct VFace {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v13(int a, int b);
};

extern "C" void fn_14_878C(char* self) {
    char name[0x54];
    char buf[0x54];

    setBaseState__20muWifiFriendListTaskFv(self, 2);
    VFace* o = *(VFace**)(self + 0x204);
    char* t30 = *(char**)(self + 0xC4);
    o->v13(*(int*)((char*)o + 0xE4), *(int*)(t30 + 0x10));
    changeAnimN__8MuObjectFiUlbb(t30, 0, 3, 1, 0);

    for (int i = 0; i < 4; i++) {
        getMyApeal__20muWifiFriendListTaskFPci(self, name, i);
        if (strlenUtf8__14msTagProcessorFPCci(name, 0x7FFF) > 0xA) {
            buf[0] = 0;
            appendSubstr__7MessageFPcPCcii(buf, name, 0, 0xA);
            int len = strlen(buf);
            char* p = buf;
            p += len;
            *p = 0xA;
            strcpy(p + 1, name + len);
        } else {
            strcpy(buf, name);
        }
        printf__5MuMsgFUlPCce(*(void**)(self + 0x24CC), i + 0x26, buf);
    }

    setVisibleApealPanelHelp__20muWifiFriendListTaskFi(self, 1);
    VFace* o2 = *(VFace**)(self + 0x204);
    o2->v13(*(int*)((char*)o2 + 0xE4), *(int*)(self + 0x1E8));
    changeAnimN__8MuObjectFiUlbb(*(void**)(self + 0xC0), 8, 0x10, 1, 0);
    changeAnimN__8MuObjectFiUlbb(*(void**)(self + 0x8C), 0, 3, 1, 0);
    *(int*)(self + 0x2510) = 0;
    *(int*)(self + 0x2514) = 0;
    *(int*)(self + 0x2518) = 0;
    setProfileCursorPos__20muWifiFriendListTaskFi(self, getProfileCursorPosByCate__20muWifiFriendListTaskFiii(0, 0, 0));
    dispCursor__20muWifiFriendListTaskFv(self, 7);
}
}

// TODO: clean up, give variables real names
namespace jj_8D34 {
extern "C" {
extern char lbl_14_data_584[];
extern float lbl_14_rodata_518;
extern int lbl_805A0434;

void removeObj__20muWifiFriendListTaskFP8MuObject(void* self, int v);
void setVisibleApealPanelHelp__20muWifiFriendListTaskFi(void* self, int v);
void dispCursor__20muWifiFriendListTaskFv(void* self, int v);
void getWifiIf__20muWifiFriendListTaskFv();
void appendSubstr__7MessageFPcPCcii(char* out, char* src, int a, int b);
int strlenUtf8__14msTagProcessorFPCci(char* p, int n);
int isSaveFriendInfoDone__10NtMatchingFv(int x);
void printf__5MuMsgFUlPCce(void* msg, unsigned long idx, const char* fmt, ...);
unsigned long strlen(const char* s);
char* strcpy(char* d, const char* s);
}

class MuObj {
public:
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void s5();
    virtual float s6();
};

extern "C" int mainFromProfile__20muWifiFriendListTaskFv(char* self) {
    char buf[0x60];
    char* d;
    int flag;

    if (*(unsigned char*)(self + 0x40) == 0) {
        getWifiIf__20muWifiFriendListTaskFv();
        flag = isSaveFriendInfoDone__10NtMatchingFv(lbl_805A0434);
    } else {
        flag = 1;
    }

    MuObj* obj = *(MuObj**)(*(char**)(*(char**)(self + 0x4c) + 0x14) + 0xc);
    float f = obj->s6();

    if (f >= lbl_14_rodata_518) {
        d = lbl_14_data_584;
        int i = 0;
        do {
            if (strlenUtf8__14msTagProcessorFPCci(d, 0x7fff) > 10) {
                buf[0] = 0;
                appendSubstr__7MessageFPcPCcii(buf, d, 0, 10);
                int len = (int)strlen(buf);
                buf[len] = 10;
                strcpy(buf + len + 1, d + len);
            } else {
                strcpy(buf, d);
            }
            printf__5MuMsgFUlPCce(*(void**)(self + 0x24cc), i + 0x26, buf);
            i++;
        } while (i < 4);
        setVisibleApealPanelHelp__20muWifiFriendListTaskFi(self, 0);
    }

    if (flag != 0 && *(int*)(self + 0x2b0) != 4) {
        removeObj__20muWifiFriendListTaskFP8MuObject(self, *(int*)(self + 0xc4));
        dispCursor__20muWifiFriendListTaskFv(self, 0);
        return 2;
    }
    return *(int*)(self + 0x24c4);
}
}

// TODO: clean up, give variables real names
namespace jj_9484 {
extern "C" void dispIconList__20muWifiFriendListTaskFv(void*, unsigned int, float, float);
extern "C" void dispIconListCursor__20muWifiFriendListTaskFv(void*, int, int);
extern "C" int changeAnimN__8MuObjectFiUlbb(void*, int, int, int, int);

struct Object {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void call(void*, int);
    char pad[0xE4 - 4];
    void* arg;
};

extern "C" void initAppearIconWnd__20muWifiFriendListTaskFv(char* self) {
    Object* obj = *(Object**)(self + 0x204);
    obj->call(obj->arg, *(int*)(self + 0x1EC));
    int v = *(int*)(self + 0x2A0C);
    float f = (float)v;
    *(int*)(self + 0x2A0C) = v;
    *(float*)(self + 0x2A08) = f;
    dispIconList__20muWifiFriendListTaskFv(self, (unsigned int)v ^ 0x80000000u, (float)v, f);
    dispIconListCursor__20muWifiFriendListTaskFv(self, *(int*)(self + 0x2A10), *(int*)(self + 0x2A14));
    changeAnimN__8MuObjectFiUlbb(*(void**)(self + 0xC8), 0, 1, 1, 0);
    changeAnimN__8MuObjectFiUlbb(*(void**)(self + 0xC0), 7, 0x10, 1, 0);
}
}

