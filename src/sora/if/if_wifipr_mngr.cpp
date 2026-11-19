#include <gf/gf_file_io_handle.h>
#include <gm/gm_global.h>
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
#include <ms/ms_message.h>
#include <stdio.h>
#include <string.h>

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
    void setDlgDWCErrorCode(int code);
    void openJoinFriendDlg(const wchar_t* name);

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
extern "C" void fn_800FD448(s32 unk);
extern "C" void fn_800FD8E0(s32 unk);
extern "C" s32 fn_800FBFDC();
// muNoticeWndTask::create (the map name); mu_notice_wnd.h calls it by this stand-in name.
extern "C" muNoticeWndTask* fn_800BF048(int kind, const char* text, int size, nw4r::g3d::ResFile* resFile, void* scnRoot, int unk14,
                                       int defaultButton, int heap, int heap2, int unk0, int unk1, int unk2);

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
    g_gfSceneRoot->layerUpdateFrame(8);
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
        m_notice->deleteWnd();
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
    m_cntWnd->setCounter(seconds, 0);
    if (m_cntWnd->unk64 == 1) {
        m_cntWnd->setCounter(seconds / 60, seconds % 60);
    } else {
        m_cntWnd->setCounter(seconds, 0);
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
    m_notice->deleteWnd();
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

void IfWifiprMngr::setDlgDWCErrorCode(int code) {
    u32 size;
    char* text;
    char buffer[128];
    Message::getPrintIndexData(unk4C, 0x13, &text, &size);
    memcpy(buffer, text, size);
    size += sprintf(buffer + size, "%05d", code);
    m_notice->dispSubMsg(buffer);
}

void IfWifiprMngr::openJoinFriendDlg(const wchar_t* name) {
    char buffer[256];
    char utf8Name[24];
    char* text;
    u32 size;
    int length = Message::writeTagClear(buffer);
    Message::getPrintIndexData(unk4C, 0x1F, &text, &size);
    memcpy(buffer + length, text, size);
    length += size;
    Message::utf16to8(utf8Name, name);
    int nameLength = strlen(utf8Name);
    memcpy(buffer + length, utf8Name, nameLength);
    length += nameLength;
    Message::getPrintIndexData(unk4C, 0x20, &text, &size);
    memcpy(buffer + length, text, size);
    length += size;
    buffer[length] = '\0';
    m_notice = fn_800BF048(1, buffer, length + 1, &unkC, unk14, 0x40, 0x3C, 0x28, 0x2A, 1, 1, 0xF0);
    m_notice->m_unk120 = 1;
    m_notice->unk128 = 0x2055;
    m_notice->setYesButtonLabel(unk4C, 0x21);
    m_notice->setNoButtonLabel(unk4C, 0x22);
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A02F8[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char g_IfWifiprMngr[0x8] __attribute__((aligned(4))) = { 0 };

// TODO: clean up, give variables real names
extern "C" void fn_800FD470(void*);

extern "C" void fn_800FCC7C(void* self) {
    fn_800FD470(*reinterpret_cast<void**>(reinterpret_cast<u8*>(self) + 0x18));
}

extern "C" void fn_800DA9CC(void*);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_800FCDF8(void* self) {
    fn_800DA9CC(*reinterpret_cast<void**>(reinterpret_cast<u8*>(self) + 0x40));
}

extern "C" void fn_800FD4F0(void*);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_800FCE00(void* self) {
    fn_800FD4F0(*reinterpret_cast<void**>(reinterpret_cast<u8*>(self) + 0x18));
}

extern "C" void fn_800FD508(void*);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_800FCE08(void* self) {
    fn_800FD508(*reinterpret_cast<void**>(reinterpret_cast<u8*>(self) + 0x18));
}

extern "C" void fn_800FBF90(void*);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_800FD108(void* self) {
    fn_800FBF90(*reinterpret_cast<void**>(reinterpret_cast<u8*>(self) + 0x38));
}

// TODO: clean up, give variables real names
namespace jj_800FC3D8 {
extern "C" {
void fn_800FC3D8(int *p) {
    *p = 0;
}
}
}

namespace jj_800FCC84 {
struct Inner {
    int a;
    int b;
    int c;
    int d;
};

struct Outer {
    int a;
    int b;
    int c;
    int d;
    int e;
    int f;
    Inner* g;
};

// TODO: clean up, give variables real names
extern "C" int fn_800FCC84(Outer* o) {
    return o->g->d;
}
}

namespace jj_800FD0F0 {
extern "C" void fn_800FBDF4(void*, void*, int);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_800FD0F0(void* self, void* other) {
    fn_800FBDF4(*(void**)((char*)self + 0x38), other, 0);
}
}

namespace jj_800FD0FC {
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" unsigned char fn_800FD0FC(void *arg0) {
    return *(unsigned char *)((unsigned char *)*(void **)((unsigned char *)arg0 + 0x38) + 0x1c);
}
}

namespace jj_800FC3E4 {
extern "C" {
void __dt__14gfFileIOHandleFv(void* p, int flag);
void __dl__FPv(void* p);

// TODO: clean up, give variables real names
void* fn_800FC3E4(void* p, int n) {
    if (p) {
        __dt__14gfFileIOHandleFv(p, 0);
        if (n > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}
}

namespace jj_800FC290 {
extern "C" {

extern void* g_IfWifiprMngr;
void fn_800FC840(void*);
void Destroy__Q34nw4r3g3d6G3dObjFv(void*);
void fn_800FC3E4(void*);
void __destroy_arr(void*, void (*)(void*), unsigned long, unsigned long);
void __dl__FPv(void*);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void fn_800FC290(void) {
    void* p = g_IfWifiprMngr;
    if (p != 0) {
        fn_800FC840(p);
        Destroy__Q34nw4r3g3d6G3dObjFv(*(void**)((char*)p + 0x14));
        *(void**)((char*)p + 0x14) = 0;
        __destroy_arr((char*)p + 0x54, fn_800FC3E4, 4, 5);
        __dl__FPv(p);
    }
    g_IfWifiprMngr = 0;
}

}
}

// TODO: clean up, give variables real names
namespace jj_800FC43C {
struct FileHolder { u8 flags; u8 pad[7]; void* res8; void* resC; void* res10; u8 pad14[0x34]; void* p48; void* p4C; u8 pad50[4]; gfFileIOHandle h54,h58,h5c,h60,h64; };
extern "C" { void fn_800FC43C(FileHolder*, int); }
#pragma push
#pragma opt_propagation off
#pragma opt_lifetimes off
// TODO: clean up, give variables real names
void fn_800FC43C(FileHolder* self, int arg1) {
    u32 sp10, spC;
    self->h54.read("/info/info_wifi_practice.brres", (HeapType)9, arg1);
    self->h58.read("/info/cmnwindow/wifi_notice_wnd.brres", (HeapType)9, arg1);
    u8* rec = (u8*)g_GameGlobal->getGlobalRecordMenuDatap();
    gfFileIOHandle* h5c = &self->h5c;
    const char* name = ((rec[0x28] >> 7) & 1) ? "/info/defaultcamera/InfoWide.brres" : "/info/defaultcamera/Info.brres";
    h5c->read(name, (HeapType)9, arg1);
    self->h60.read("/menu/msg/wifipr_info.msbin", (HeapType)9, arg1);
    self->h64.read("/menu/msg/wifi_common.msbin", (HeapType)9, arg1);
    if (arg1 == 0) {
        u32 sp8 = 0, r31;
        if(self->h54.getReturnStatus()==0x15) r31=0;
        else {r31=(u32)self->h54.getBuffer();self->h54.release();}
        if(r31){sp8=r31;nw4r::g3d::ResFile::Init(&sp8);} self->res8=(void*)sp8;
        spC=0;
        if(self->h58.getReturnStatus()==0x15)r31=0;
        else{r31=(u32)self->h58.getBuffer();self->h58.release();}
        if(r31){spC=r31;nw4r::g3d::ResFile::Init(&spC);} self->resC=(void*)spC;
        sp10=0;
        if(self->h5c.getReturnStatus()==0x15)r31=0;
        else{r31=(u32)self->h5c.getBuffer();self->h5c.release();}
        if(r31){sp10=r31;nw4r::g3d::ResFile::Init(&sp10);} self->res10=(void*)sp10;
        if(self->h60.getReturnStatus()==0x15)r31=0;
        else{r31=(u32)self->h60.getBuffer();self->h60.release();} self->p48=(void*)r31;
        if(self->h64.getReturnStatus()==0x15)r31=0;
        else{r31=(u32)self->h64.getBuffer();self->h64.release();} self->p4C=(void*)r31;
        self->flags|=0x80;
    }
}
#pragma pop
}

// TODO: clean up, give variables real names
namespace jj_800FC840 {
typedef unsigned char u8;

struct Flags {
    unsigned char b0 : 1;
    unsigned char b1 : 1;
    unsigned char b2 : 1;
    unsigned char b3 : 1;
    unsigned char b4 : 1;
    unsigned char b5 : 1;
    unsigned char b6 : 1;
    unsigned char b7 : 1;
};

extern "C" {
void cancelRequest__14gfFileIOHandleFv(void*);
void free__13gfHeapManagerFPv(void*);

void fn_800FC840(void* self)
{
    u8* p = reinterpret_cast<u8*>(self);
    if (*reinterpret_cast<void**>(p + 0x8)) {
        free__13gfHeapManagerFPv(*reinterpret_cast<void**>(p + 0x8));
        *reinterpret_cast<void**>(p + 0x8) = 0;
    }
    if (*reinterpret_cast<void**>(p + 0xC)) {
        free__13gfHeapManagerFPv(*reinterpret_cast<void**>(p + 0xC));
        *reinterpret_cast<void**>(p + 0xC) = 0;
    }
    if (*reinterpret_cast<void**>(p + 0x10)) {
        free__13gfHeapManagerFPv(*reinterpret_cast<void**>(p + 0x10));
        *reinterpret_cast<void**>(p + 0x10) = 0;
    }
    if (*reinterpret_cast<void**>(p + 0x48)) {
        free__13gfHeapManagerFPv(*reinterpret_cast<void**>(p + 0x48));
        *reinterpret_cast<void**>(p + 0x48) = 0;
    }
    if (*reinterpret_cast<void**>(p + 0x4C)) {
        free__13gfHeapManagerFPv(*reinterpret_cast<void**>(p + 0x4C));
        *reinterpret_cast<void**>(p + 0x4C) = 0;
    }
    u8* cursor = p + 0x54;
    for (int i = 0; i < 5; ++i, cursor += 4) {
        cancelRequest__14gfFileIOHandleFv(cursor);
    }
    reinterpret_cast<Flags*>(p)->b0 = 0;
}
}
}

