#include <cm/cm_camera_controller.h>
#include <gf/gf_application.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <mu/menu.h>
#include <mu/mu_menuroot.h>
#include <sr/sr_common.h>
#include <types.h>

// The friend list menu of sora_menu_friend_list.rel (declared in full in src/mo_menu/mu_wifi_friend_list.cpp).
// Only what the scene reads is declared.
class muWifiFriendListTask : public gfTask {
public:
    int m_unk40;
    int m_result;                       // 0x44 how the menu was left (0: not yet)
    u8 m_unk48[0x2ABC - 0x48];
    int m_unk2ABC;                      // 0x2ABC what the menu hands back to the scene

    void destroy();
};

// A file of the menu that is read when the scene starts (the handle is the only thing it holds).
struct scNetFileHandle : gfFileIOHandle {
    scNetFileHandle() { m_request = NULL; }
};

// The scene of the friend list of the online mode (module sora_scene): reads the files of the menu, makes the scene root and
// leaves the menu itself to the task of sora_menu_friend_list.rel.
class scNetFriendList : public muMenu {
public:
    u8 m_unk350[0x35C - 0x350];
    MenuRoot* m_root;                   // 0x35C
    int m_unk360;
    int m_unk364;
    int m_unk368;
    int m_unk36C;
    int m_unk370;
    int m_unk374;
    int m_unk378;
    int m_unk37C;
    muWifiFriendListTask* m_task;       // 0x380
    scNetFileHandle m_files[9];         // 0x384 the files of the menu (the last one is the camera)
    int m_unk3A8;

    scNetFriendList();
    static scNetFriendList* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
    muWifiFriendListTask* createTask();
    // Tells whether all the files are read.
    bool isAllReady() {
        for (int i = 0; i < 9; i++) {
            if (!m_files[i].isReady()) {
                return false;
            }
        }
        return true;
    }
};
static_assert(sizeof(scNetFriendList) == 0x3AC, "Class is wrong size!");

scNetFriendList* scNetFriendList::create() {
    return new (Heaps::GlobalMode) scNetFriendList();
}

scNetFriendList::scNetFriendList()
    : muMenu("scNetFriendList"), m_root(NULL), m_unk360(0), m_unk364(0), m_unk368(0), m_unk36C(0), m_unk370(0),
      m_unk374(0), m_unk37C(0), m_task(NULL) {
    m_unk3A8 = 0;
    m_unk360 = 0;
    m_unk364 = 0;
    m_unk368 = 0;
    m_unk36C = 0;
}

void scNetFriendList::setModule() {
    setLoadModule("sora_menu_friend_list.rel", false);
    setLoadModule("sora_menu_name.rel", false);
}

void scNetFriendList::start() {
    m_root = MenuRoot::create("NetFriendListMenuRootTask", 100, NULL);
    CameraController::getInstance()->changeInput(CameraController::MenuFixed);
    m_task = NULL;
    m_files[8].readRequest(g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen
                               ? "/menu/defaultcamera/CharacterSelectWide.brres"
                               : "/menu/defaultcamera/CharacterSelect.brres",
                           Heaps::MenuResource, 0, 0);
    m_files[0].readRequest("/menu/wifi/wifi_friend_list.brres", Heaps::MenuResource, 0, 0);
    m_files[1].readRequest("/menu/cmnwindow/wifi_notice_wnd.brres", Heaps::MenuResource, 0, 0);
    m_files[2].readRequest("/menu/common/Wait.brres", Heaps::MenuResource, 0, 0);
    m_files[3].readRequest("/menu/common/name_entry.brres", Heaps::MenuResource, 0, 0);
    m_files[4].readRequest("/menu/msg/wifi_friend_list.msbin", Heaps::MenuResource, 0, 0);
    m_files[5].readRequest("/menu/msg/wifi_common.msbin", Heaps::MenuResource, 0, 0);
    m_files[6].readRequest("/system/RFLRes01.arc", Heaps::MenuResource, 0, 0);
    m_files[7].readRequest("/menu/wifi/mu_friend_icon_tbl.dat", Heaps::MenuResource, 0, 0);
}

s32 scNetFriendList::process() {
    if (m_task == NULL) {
        if (!isAllReady()) {
            return 0;
        }
        g_gfApplication->unkD0.endKeepScreen();
        m_task = createTask();
    }
    int result = m_task->m_result;
    if (result != 0) {
        m_unk3A8 = m_task->m_unk2ABC;
        static const u8 nextLayout[16] = {0, 0, 1, 2, 3, 4, 5, 6, 7, 0, 0, 0, 0, 0, 0, 0};
        gfSceneManager* mgr = gfSceneManager::getInstance();
        mgr->unk1 = nextLayout[result];
        mgr->processStep = 2;
    }
    return 0;
}

s32 scNetFriendList::exit() {
    g_gfApplication->unkD0.startKeepScreen();
    if (m_task != NULL) {
        m_task->destroy();
        m_task = NULL;
    }
    if (m_root != NULL) {
        m_root->exit();
        m_root = NULL;
    }
    if (m_unk360 != 0) {
        gfHeapManager::free(reinterpret_cast<void*>(m_unk360));
        m_unk360 = 0;
    }
    if (m_unk364 != 0) {
        gfHeapManager::free(reinterpret_cast<void*>(m_unk364));
        m_unk364 = 0;
    }
    if (m_unk368 != 0) {
        gfHeapManager::free(reinterpret_cast<void*>(m_unk368));
        m_unk368 = 0;
    }
    if (m_unk36C != 0) {
        gfHeapManager::free(reinterpret_cast<void*>(m_unk36C));
        m_unk36C = 0;
    }
    if (m_unk370 != 0) {
        operator delete(reinterpret_cast<void*>(m_unk370));
        m_unk370 = 0;
    }
    if (m_unk374 != 0) {
        operator delete(reinterpret_cast<void*>(m_unk374));
        m_unk374 = 0;
    }
    if (m_unk378 != 0) {
        gfHeapManager::free(reinterpret_cast<void*>(m_unk378));
        m_unk378 = 0;
    }
    if (m_unk37C != 0) {
        gfHeapManager::free(reinterpret_cast<void*>(m_unk37C));
        m_unk37C = 0;
    }
    for (int i = 0; i < 9; i++) {
        m_files[i].cancelRequest();
    }
    return 0;
}
