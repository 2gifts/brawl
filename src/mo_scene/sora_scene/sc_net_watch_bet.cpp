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
    bool withPrize = gfSceneManager::getInstance()->m_memoryLayout != 0;
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
