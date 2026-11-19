#include <cm/cm_camera_controller.h>
#include <gf/gf_application.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <mu/menu.h>
#include <it/it_manager.h>
#include <mu/mu_menuroot.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <sr/sr_common.h>
#include <string.h>
#include <types.h>

// Makes the prize window of the main module (the arguments are the model file, the texts, the scene group, ...).
extern "C" gfTask* fn_800C001C(void* resFile, void* msgData, void* group, int unk, void* unk2, int unk3);

// A file of the menu that is read when the scene starts (the handle is the only thing it holds).
struct scNetFileHandle : gfFileIOHandle {
    scNetFileHandle() { m_request = NULL; }
};

// The scene of the prize information (module sora_scene): reads the files of the prize window, makes the scene root and leaves
// the window itself to a task that the scene makes later.
class scPrizeInfo : public muMenu {
public:
    u8 m_unk350[0x35C - 0x350];
    int m_step;                         // 0x35C 0: waits for the files, 2: makes the window, 3: waits until it is over
    MenuRoot* m_root;                   // 0x360
    void* m_buf364;                     // 0x364 the buffers of the menu
    void* m_buf368;
    gfTask* m_task;                     // 0x36C the prize window
    int m_unk370;
    u8 m_unk374[0xC0];                  // 0x374
    scNetFileHandle m_files[3];         // 0x434 the files of the menu (the last one is the camera)
    u8 m_unk440;
    int m_unk444;
    s16 m_unk448;
    s16 m_unk44A;
    int m_unk44C;
    int m_unk450;
    int m_unk454;

    scPrizeInfo();
    static scPrizeInfo* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    // Fills the window with the prizes.
    void setupPrizes();
    // Tells whether all the files are read.
    bool isAllReady() {
        for (int i = 0; i < 3; i++) {
            if (!m_files[i].isReady()) {
                return false;
            }
        }
        return true;
    }
};
static_assert(sizeof(scPrizeInfo) == 0x458, "Class is wrong size!");

scPrizeInfo* scPrizeInfo::create() {
    return new (Heaps::GlobalMode) scPrizeInfo();
}

scPrizeInfo::scPrizeInfo()
    : muMenu("scPrizeInfo"), m_step(0), m_root(NULL), m_buf364(NULL), m_buf368(NULL), m_task(NULL), m_unk370(0) {
    m_unk440 = 0;
    m_unk444 = 0;
    m_unk448 = 1;
    m_unk44A = 1;
    m_unk44C = 0x26F9;
    m_unk450 = 9;
    m_unk454 = 0;
    m_buf364 = NULL;
    memset(m_unk374, 0, sizeof(m_unk374));
}

void scPrizeInfo::start() {
    m_unk440 = 0;
    m_root = MenuRoot::create("prizeInfoMenuRoot", 0x100, NULL);
    u32 wide = g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen;
    m_files[2].readRequest(wide ? "/menu/defaultcamera/characterselectWide.brres" : "/menu/defaultcamera/characterselect.brres",
                           Heaps::MenuResource, 0, 0);
    m_files[0].readRequest("/menu/cmnwindow/prize_wnd.brres", Heaps::MenuResource, 0, 0);
    m_files[1].readRequest("/menu/msg/info.msbin", Heaps::MenuResource, 0, 0);
    m_step = 0;
    m_task = NULL;
    CameraController::getInstance()->changeInput(CameraController::MenuFixed);
}

s32 scPrizeInfo::exit() {
    g_gfApplication->unkD0.startKeepScreen();
    if (m_task != NULL) {
        m_task->exit();
        m_task = NULL;
    }
    if (m_root != NULL) {
        m_root->exit();
        m_root = NULL;
    }
    if (m_buf364 != NULL) {
        gfHeapManager::free(m_buf364);
        m_buf364 = NULL;
    }
    if (m_buf368 != NULL) {
        gfHeapManager::free(m_buf368);
        m_buf368 = NULL;
    }
    for (int i = 0; i < 3; i++) {
        m_files[i].cancelRequest();
    }
    return 0;
}

s32 scPrizeInfo::process() {
    switch (m_step) {
    case 0:
        if (isAllReady()) {
            m_step = 2;
        }
        break;
    case 1:
        m_step = 2;
        break;
    case 2: {
        m_root->attachScnAnm(&m_files[2]);
        m_files[2].release();
        nw4r::g3d::ResFile resFile(static_cast<void*>(NULL));
        void* buffer;
        if (m_files[0].getReturnStatus() == 0x15) {
            buffer = NULL;
        } else {
            buffer = m_files[0].getBuffer();
            m_files[0].release();
        }
        if (buffer != NULL) {
            resFile = nw4r::g3d::ResFile(buffer);
            nw4r::g3d::ResFile::Init(&resFile);
        }
        m_buf364 = resFile.ptr();
        if (m_files[1].getReturnStatus() == 0x15) {
            buffer = NULL;
        } else {
            buffer = m_files[1].getBuffer();
            m_files[1].release();
        }
        m_buf368 = buffer;
        void* firstArg = NULL;
        void** list = *reinterpret_cast<void***>(reinterpret_cast<u8*>(itManager::getInstance()) + 0x14B8);
        if (list != NULL) {
            firstArg = list[1];
        }
        m_task = fn_800C001C(&m_buf364, m_buf368, m_root->scene, 1, firstArg, 10);
        setupPrizes();
        g_gfApplication->unkD0.endKeepScreen();
        m_step = 3;
        break;
    }
    case 3:
        if (*reinterpret_cast<int*>(reinterpret_cast<u8*>(m_task) + 0x10C) == 5) {
            gfSceneManager* mgr = gfSceneManager::getInstance();
            mgr->unk1 = 0;
            mgr->processStep = 2;
        }
        break;
    }
    return 0;
}
