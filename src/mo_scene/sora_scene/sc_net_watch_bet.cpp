#include <cm/cm_camera_controller.h>
#include <gf/gf_application.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/menu.h>
#include <mu/mu_menuroot.h>
#include <sr/sr_common.h>
#include <string.h>
#include <types.h>

// The betting menu of the online watch mode of sora_menu_watch.rel. Only what the scene reads is declared.
class muWifiWatchBetTask : public gfTask {
public:
    u8 m_unk40[0x504 - 0x40];
    int m_state;                        // 0x504 (0xC: the menu is over)
};

// A file of the menu that is read when the scene starts (the handle is the only thing it holds).
struct scNetFileHandle : gfFileIOHandle {
    scNetFileHandle() { m_request = NULL; }
};

// The scene of the betting of the online watch mode (module sora_scene): reads the files of the menu, makes the scene root and
// leaves the menu itself to the task of sora_menu_watch.rel.
class scNetWatchBet : public muMenu {
public:
    u8 m_unk350[0x35C - 0x350];
    MenuRoot* m_root;                   // 0x35C
    void* m_buf360;                     // 0x360 the buffers of the menu
    void* m_buf364;
    void* m_buf368;
    void* m_buf36C;
    void* m_buf370;
    void* m_buf374;
    void* m_buf378;
    void* m_buf37C;
    void* m_buf380;
    void* m_buf384;
    int m_unk388;
    muWifiWatchBetTask* m_task;         // 0x38C
    scNetFileHandle m_files[10];        // 0x390 the files of the menu
    u8 m_unk3B8[0xD0];                  // 0x3B8
    u8 m_unk488[0xD0];                  // 0x488
    int m_unk558;
    u8 m_unk55C[0xCA];                  // 0x55C
    u8 m_unk626[2];
    u8 m_unk628[0x684 - 0x628];
    u8 m_unk684;
    u8 m_unk685[3];
    int m_unk688;
    int m_unk68C;

    scNetWatchBet();
    static scNetWatchBet* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
};
static_assert(sizeof(scNetWatchBet) == 0x690, "Class is wrong size!");

scNetWatchBet* scNetWatchBet::create() {
    return new (Heaps::GlobalMode) scNetWatchBet();
}

scNetWatchBet::scNetWatchBet()
    : muMenu("scNetWatchBet"), m_root(NULL), m_buf360(NULL), m_buf364(NULL), m_buf368(NULL), m_buf36C(NULL), m_buf370(NULL),
      m_buf374(NULL), m_buf378(NULL), m_buf37C(NULL), m_buf380(NULL), m_buf384(NULL), m_unk388(0), m_task(NULL) {
    m_unk558 = 0;
    m_unk626[0] = 0;
    m_unk684 = 0;
    m_unk688 = 0;
    m_unk68C = 0;
    m_buf360 = NULL;
    m_buf364 = NULL;
    m_buf368 = NULL;
    m_buf36C = NULL;
    m_buf370 = NULL;
    memset(m_unk3B8, 0, sizeof(m_unk3B8));
    memset(m_unk488, 0, sizeof(m_unk488));
    memset(m_unk55C, 0, sizeof(m_unk55C));
}

void scNetWatchBet::setModule() {
    setLoadModule("sora_menu_watch.rel", false);
}

void scNetWatchBet::start() {
    m_root = MenuRoot::create("NetWatchMenuRootTask", 0x96, "/menu/defaultcamera/CharacterSelect.brres");
    CameraController::getInstance()->changeInput(CameraController::MenuFixed);
    m_task = NULL;
    int withPrize = gfSceneManager::getInstance()->m_memoryLayout;
    m_files[0].readRequest("/menu/wifi/wifi_watch.brres", Heaps::MenuResource, 0, 0);
    m_files[1].readRequest("/menu/wifi/wifi_count.brres", Heaps::MenuResource, 0, 0);
    m_files[2].readRequest("/menu/cmnwindow/wifi_notice_wnd.brres", Heaps::MenuResource, 0, 0);
    m_files[3].readRequest("/menu/msg/wifi_watch.msbin", Heaps::MenuResource, 0, 0);
    m_files[4].readRequest("/menu/msg/stage_name.msbin", Heaps::MenuResource, 0, 0);
    m_files[5].readRequest("/menu/msg/wifi_common.msbin", Heaps::MenuResource, 0, 0);
    if (withPrize) {
        m_files[6].readRequest("/menu/cmnwindow/prize_wnd.brres", Heaps::MenuResource, 0, 0);
        m_files[7].readRequest("/menu/msg/info.msbin", Heaps::MenuResource, 0, 0);
        m_files[8].readRequest("/toy/seal/ToySealThumb.brres", Heaps::MenuResource, 0, 0);
        m_files[9].readRequest("/toy/seal/ty_seal_data.dat", Heaps::MenuResource, 0, 0);
    }
    muMenu::loadMenuSound();
}

s32 scNetWatchBet::exit() {
    g_gfApplication->unkD0.startKeepScreen();
    if (m_task != NULL) {
        m_task->exit();
        m_task = NULL;
    }
    if (m_root != NULL) {
        m_root->exit();
        m_root = NULL;
    }
    if (m_buf360 != NULL) {
        gfHeapManager::free(m_buf360);
        m_buf360 = NULL;
    }
    if (m_buf370 != NULL) {
        gfHeapManager::free(m_buf370);
        m_buf370 = NULL;
    }
    if (m_buf364 != NULL) {
        gfHeapManager::free(m_buf364);
        m_buf364 = NULL;
    }
    if (m_buf368 != NULL) {
        gfHeapManager::free(m_buf368);
        m_buf368 = NULL;
    }
    if (m_buf380 != NULL) {
        gfHeapManager::free(m_buf380);
        m_buf380 = NULL;
    }
    if (m_buf36C != NULL) {
        gfHeapManager::free(m_buf36C);
        m_buf36C = NULL;
    }
    if (m_buf374 != NULL) {
        operator delete(m_buf374);
        m_buf374 = NULL;
    }
    if (m_buf378 != NULL) {
        operator delete(m_buf378);
        m_buf378 = NULL;
    }
    if (m_buf384 != NULL) {
        operator delete(m_buf384);
        m_buf384 = NULL;
    }
    if (m_buf37C != NULL) {
        operator delete(m_buf37C);
        m_buf37C = NULL;
    }
    for (int i = 0; i < 10; i++) {
        m_files[i].cancelRequest();
    }
    return 0;
}

#include <nw4r/g3d/g3d_resfile.h>
// TODO: clean up, give variables real names
namespace jj_BF1C {

struct CopyD0 { u32 first; u32 words[51]; };
struct Copy5C { u32 a, b; u8 c; u32 words[20]; };
struct CopyCA { u16 words[100]; };
struct Args {
    u32 mode;
    void* res1;
    void* res2;
    void* res3;
    void* data1;
    void* data2;
    void* data3;
    void* data4;
    void* data5;
    CopyD0 a;
    Copy5C b;
    CopyD0 c;
    u8 flag1, flag2;
    u32 value1, value2;
    void* res4;
    void* data6;
    CopyCA d;
    u16 last;
    void* res5;
    void* data7;
    u32 size;
    u8 flag3;
};
extern "C" void create__15muWifiWatchTaskFPv(void*);
#define HANDLE(off) ((gfFileIOHandle*)(self + (off)))

extern "C" void createTask__13scNetWatchBetFv(u8* self) {
    Args args;
    u32 sp18, sp14, sp10, spC, sp8;
    void* buf;
    int special;
    int layout;

    args.mode = 0;
    args.res1 = 0;
    args.res2 = 0;
    args.res3 = 0;
    args.data1 = 0;
    args.data2 = 0;
    args.data3 = 0;
    args.data4 = 0;
    args.data5 = 0;
    args.flag1 = 0;
    args.flag2 = 0;
    args.value1 = 0;
    args.value2 = 0;
    args.res4 = 0;
    args.data6 = 0;
    args.res5 = 0;
    args.data7 = 0;
    args.size = 0;
    args.flag3 = 0;
    memset(&args.b, 0, sizeof(args.b));
    memset(&args.a, 0, sizeof(args.a));
    memset(&args.c, 0, sizeof(args.c));
    memset(&args.d, 0, 0xca);

    special = 0;
    layout = gfSceneManager::getInstance()->m_memoryLayout;
    if (layout < 0) {
        special = 1;
        layout = (int)((u32)((-1 - layout) | (layout + 1)) >> 31);
    }

    sp18 = 0;
    if (HANDLE(0x390)->getReturnStatus() == 0x15) buf = 0;
    else { buf = HANDLE(0x390)->getBuffer(); HANDLE(0x390)->release(); }
    if (buf) { sp18 = (u32)buf; nw4r::g3d::ResFile::Init(&sp18); }
    *(u32*)(self + 0x360) = sp18;
    sp14 = 0;
    if (HANDLE(0x394)->getReturnStatus() == 0x15) buf = 0;
    else { buf = HANDLE(0x394)->getBuffer(); HANDLE(0x394)->release(); }
    if (buf) { sp14 = (u32)buf; nw4r::g3d::ResFile::Init(&sp14); }
    *(u32*)(self + 0x364) = sp14;
    sp10 = 0;
    if (HANDLE(0x398)->getReturnStatus() == 0x15) buf = 0;
    else { buf = HANDLE(0x398)->getBuffer(); HANDLE(0x398)->release(); }
    if (buf) { sp10 = (u32)buf; nw4r::g3d::ResFile::Init(&sp10); }
    *(u32*)(self + 0x370) = sp10;
    if (HANDLE(0x39c)->getReturnStatus() == 0x15) buf = 0;
    else { buf = HANDLE(0x39c)->getBuffer(); HANDLE(0x39c)->release(); }
    *(void**)(self + 0x374) = buf;
    if (HANDLE(0x3a0)->getReturnStatus() == 0x15) buf = 0;
    else { buf = HANDLE(0x3a0)->getBuffer(); HANDLE(0x3a0)->release(); }
    *(void**)(self + 0x378) = buf;
    if (HANDLE(0x3a4)->getReturnStatus() == 0x15) buf = 0;
    else { buf = HANDLE(0x3a4)->getBuffer(); HANDLE(0x3a4)->release(); }
    *(void**)(self + 0x37c) = buf;
    args.res1 = self + 0x360;
    args.res2 = self + 0x364;
    args.res3 = self + 0x370;
    args.data1 = *(void**)(self + 0x374);
    args.data2 = *(void**)(self + 0x37c);
    args.data3 = *(void**)(self + 0x378);
    args.data4 = *(void**)(*(u8**)(self + 0x35c) + 0x48);
    args.data5 = *(void**)(self + 0x558);

    if (layout == 0) {
        args.mode = 0;
        args.a = *(CopyD0*)(self + 0x3b8);
    } else {
        args.mode = 1;
        args.b = *(Copy5C*)(self + 0x628);
        args.c = *(CopyD0*)(self + 0x3b8);
        args.a = *(CopyD0*)(self + 0x488);
        spC = 0;
        if (HANDLE(0x3a8)->getReturnStatus() == 0x15) buf = 0;
        else { buf = HANDLE(0x3a8)->getBuffer(); HANDLE(0x3a8)->release(); }
        if (buf) { spC = (u32)buf; nw4r::g3d::ResFile::Init(&spC); }
        *(u32*)(self + 0x368) = spC;
        args.res4 = self + 0x368;
        if (HANDLE(0x3ac)->getReturnStatus() == 0x15) buf = 0;
        else { buf = HANDLE(0x3ac)->getBuffer(); HANDLE(0x3ac)->release(); }
        *(void**)(self + 0x380) = buf;
        args.data6 = buf;
        args.d = *(CopyCA*)(self + 0x55c);
        args.last = *(u16*)(self + 0x624);
        sp8 = 0;
        if (HANDLE(0x3b0)->getReturnStatus() == 0x15) buf = 0;
        else { buf = HANDLE(0x3b0)->getBuffer(); HANDLE(0x3b0)->release(); }
        if (buf) { sp8 = (u32)buf; nw4r::g3d::ResFile::Init(&sp8); }
        *(u32*)(self + 0x36c) = sp8;
        args.res5 = self + 0x36c;
        int size = HANDLE(0x3b4)->getSize();
        if (HANDLE(0x3b4)->getReturnStatus() == 0x15) buf = 0;
        else { buf = HANDLE(0x3b4)->getBuffer(); HANDLE(0x3b4)->release(); }
        *(void**)(self + 0x384) = buf;
        args.data7 = buf;
        args.size = size;
        args.flag2 = *(u8*)(self + 0x684);
        args.value1 = *(u32*)(self + 0x688);
        args.value2 = *(u32*)(self + 0x68c);
        args.flag1 = *(u8*)(self + 0x626);
    }
    args.flag3 = special;
    create__15muWifiWatchTaskFPv(&args);
}
}

// TODO: clean up, give variables real names
namespace jj_C44C {


typedef struct {
    u32 w[20];
} Blob80;

typedef struct {
    u32 w[51];
} Blob204;

extern "C" char* createTask__13scNetWatchBetFv(char*);
extern "C" bool isReady__14gfFileIOHandleCFv(const void*);
extern "C" int isLoadFinishMenuSound__6muMenuFv(void*);
extern unsigned char lbl_1_rodata_6E8[];

extern "C" int process__13scNetWatchBetFv(char* self) {
    int ready;
    if (*(char**)(self + 0x38C) == 0) {
        int n = 0;
        char* cur = self + 0x390;
        for (; n < 10; n++, cur += 4) {
            if (!isReady__14gfFileIOHandleCFv(cur)) {
                ready = 0;
                goto done;
            }
        }
        {
            char* menu = *(char**)(self + 0x35C);
            if (*(unsigned char*)(menu + 0x1A4) == 0)
                ready = 0;
            else
                ready = isLoadFinishMenuSound__6muMenuFv(menu) != 0;
        }
    done:
        if (!ready)
            return 0;
        g_gfApplication->unkD0.endKeepScreen();
        *(char**)(self + 0x38C) = createTask__13scNetWatchBetFv(self);
    }

    char* object = *(char**)(self + 0x38C);
    int state;
    if (*(int*)(object + 0x504) != 12) {
        state = 0;
    } else {
        state = *(int*)(object + 0x48);
    }

    if (state != 0) {
        int pos = *(int*)(object + 0x510) + *(int*)(object + 0x50C) * 10;
        *(int*)(self + 0x558) = pos;
        if (pos > 0x1869F) {
            *(int*)(self + 0x558) = 0x1869F;
        }

        if (state == 1) {
            char* src = *(char**)(self + 0x38C);
            *(int*)(self + 0x628) = *(int*)(src + 0x434);
            *(int*)(self + 0x62C) = *(int*)(src + 0x438);
            *(unsigned char*)(self + 0x630) = *(unsigned char*)(src + 0x43C);
            *(Blob80*)(self + 0x634) = *(Blob80*)(src + 0x440);

            int c;
            char* o = *(char**)(self + 0x38C);
            if (*(int*)(o + 0x434) == 0) {
                c = *(int*)(o + 0x514);
                unsigned char v;
                if (c == 0)
                    v = 0;
                else
                    v = *(unsigned char*)(o + (c - 1) * 0x14 + 0x450);
                *(unsigned char*)(self + 0x684) = v;
            } else {
                c = *(int*)(o + 0x514);
                int v;
                if (c == 0)
                    v = 0;
                else
                    v = *(int*)(o + (((c - 5) << 1)) * 0x14 + 0x44C);
                *(int*)(self + 0x688) = v;
            }

            object = *(char**)(self + 0x38C);
            int m;
            if (*(int*)(object + 0x514) == 0)
                m = 0;
            else
                m = *(int*)(object + 0x520) * 10;
            *(int*)(self + 0x68C) = m;
            *(int*)(self + 0x558) -= m;
            if (*(int*)(self + 0x558) < 0) {
                *(int*)(self + 0x558) = 0;
            }

            *(int*)(self + 0x488) = *(int*)(self + 0x3B8);
            *(Blob204*)(self + 0x48C) = *(Blob204*)(self + 0x3BC);
        }

        gfSceneManager* manager = gfSceneManager::getInstance();
        manager->unk1 = lbl_1_rodata_6E8[state];
        manager->processStep = 2;
    }
    return 0;
}
}

