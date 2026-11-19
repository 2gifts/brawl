#include <cm/cm_camera_controller.h>
#include <gf/gf_application.h>
#include <gf/gf_archive.h>
#include <gf/gf_display.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <gm/gm_global.h>
#include <mu/menu.h>
#include <mu/mu_menuroot.h>
#include <mu/mu_resource_loader.h>
#include <mu/mu_select_stage_task.h>
#include <sr/sr_common.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

// The loader of the packs as the scenes of the character and stage selection see it (one bit flags).
class scResourceLoaderEx {
public:
    gfFileIOHandle m_handle;            // 0x00
    u32 m_unk04;
    u32 m_unk08;
    u32 m_unk0C;
    u32 m_unk10;
    u32 m_unk14;
    u32 m_unk18;
    u8 m_flagA : 1;                     // 0x1C
    u8 m_flagB : 1;
    u8 m_flagC : 1;
    u8 m_flagD : 5;
    u8 m_unk1D[3];
    scResourceLoaderEx() {
        m_unk04 = 0;
        m_unk08 = 0;
        m_unk0C = 0;
        m_unk10 = 0;
        m_flagA = 0;
        m_unk18 = 0;
        m_flagB = 1;
        m_unk14 = 0xFF;
        m_flagC = 0;
    }
    ~scResourceLoaderEx() { fn_1_406C(reinterpret_cast<scResourceLoader*>(this)); }
};

// Starts the read of a pack of the scene module into the archive (scResourceLoader of the scene module).
extern "C" void fn_1_3C68(scResourceLoaderEx* loader, int heap, const char* path, gfArchive* archive, int size);

// Makes the scene root of a pack of the main module.
extern "C" MenuRoot* fn_800AFBFC(const char* name, int heap, gfArchive* archive, int index);
// Gets the data of the file with that number from the archive (main module).
extern "C" void* fn_800B03F0(gfArchive* archive, int index);
// Tells whether the pack is read.
extern "C" bool fn_1_3E50(scResourceLoaderEx* loader);

// What the scene gives to the stage selection screen (the layout of muSelectStageTask::Param; this scene's constructor
// sets every member).
struct scSelStageParam {
    int m_mode;
    void* m_data1;
    void* m_data2;
    void* m_data4;
    void* m_data3;
    nw4r::g3d::ScnGroup* m_group;
    int m_unk18;
    void* m_msgData;
    int m_unk20;
    int m_padNo;
    int m_unk28;
    u8 m_unk2C;
    void* m_unk30;
    void* m_unk34;
    void* m_unk38;
    void* m_unk3C;
    int m_unk40;
    void* m_unk44;
    int m_unk48;
    u8 m_unk4C;

    scSelStageParam() {
        m_mode = 0;
        m_data1 = NULL;
        m_data2 = NULL;
        m_data4 = NULL;
        m_data3 = NULL;
        m_group = NULL;
        m_unk18 = 1;
        m_msgData = NULL;
        m_unk20 = -1;
        m_padNo = 0xF0;
        m_unk28 = 0;
        m_unk2C = 1;
        m_unk30 = NULL;
        m_unk34 = NULL;
        m_unk38 = NULL;
        m_unk3C = NULL;
        m_unk40 = 0;
        m_unk44 = NULL;
        m_unk48 = 0;
        m_unk4C = 0;
    }
};

// A file of the menu that is read when the scene starts (the handle is the only thing it holds).
struct scNetFileHandle : gfFileIOHandle {
    scNetFileHandle() { m_request = NULL; }
};

// The scene of the stage selection (module sora_scene): reads the pack of the screen and runs the stage selection task of
// sora_menu_sel_stage.rel. The scenes of the modes of the game (sq_*) hand their choice to it and read it back.
class scSelStage : public muMenu {
public:
    u8 m_unk350[0x35C - 0x350];
    int m_unk35C;
    int m_padNo;                        // 0x360 the pad that uses the screen (0xF0: the pad of the system menu)
    MenuRoot* m_unk364;
    CameraController* m_camera;         // 0x368
    scNetFileHandle m_files[8];         // 0x36C
    int m_unk38C;
    int m_unk390;
    int m_unk394;
    int m_unk398;
    int m_unk39C;
    int m_unk3A0;
    int m_unk3A4;
    int m_unk3A8;
    muSelectStageTask* m_task;          // 0x3AC
    int m_unk3B0;
    int m_unk3B4;
    u8 m_unk3B8[0x80];                  // 0x3B8
    int m_unk438;
    int m_unk43C;
    u8 m_unk440;
    int m_mode;                         // 0x444 0: the screen is entered anew
    u8 m_unk448;
    u8 m_clearColor[4];                 // 0x449 the color the screen is cleared with (restored when the scene ends)
    u8 m_unk44D[3];
    gfArchive* m_archive;               // 0x450 the pack of the screen
    scResourceLoaderEx* m_loader;         // 0x454 reads the pack

    scSelStage();
    static scSelStage* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
    muSelectStageTask* createTask(int mode);
};
static_assert(sizeof(scSelStage) == 0x458, "Class is wrong size!");

scSelStage* scSelStage::create() {
    return new (Heaps::GlobalMode) scSelStage();
}

scSelStage::scSelStage() : muMenu("scSelStage"), m_unk35C(-1), m_padNo(0xF0), m_unk364(NULL), m_camera(NULL) {
    m_unk3A4 = 0;
    m_unk3A8 = 0;
    m_task = NULL;
    m_unk3B4 = 0;
    m_unk3B0 = 0;
    m_unk43C = 0;
    m_unk438 = 0;
    m_unk440 = 0;
    m_mode = 0;
    m_unk448 = 1;
    m_archive = NULL;
    m_loader = NULL;
    m_unk38C = 0;
    m_unk390 = 0;
    m_unk394 = 0;
    m_unk398 = 0;
    m_unk39C = 0;
    m_unk3A0 = 0;
    memset(m_unk3B8, 0, sizeof(m_unk3B8));
    memset(m_clearColor, 0, sizeof(m_clearColor));
}

void scSelStage::setModule() {
    setLoadModule("sora_menu_sel_stage.rel", false);
}

void scSelStage::start() {
    if (m_mode == 0) {
        m_archive = new (Heaps::MenuResource) gfArchive();
        m_loader = new (Heaps::MenuResource) scResourceLoaderEx();
        m_camera = CameraController::getInstance();
        fn_1_3C68(m_loader, Heaps::MenuResource, "/menu2/sc_selmap.pac", m_archive, 0x100000);
        m_camera->changeInput(CameraController::MenuPad);
        m_task = NULL;
    } else {
        m_task = createTask(0);
    }
    memset(m_clearColor, 0, sizeof(m_clearColor));
}

s32 scSelStage::exit() {
    if (m_task != NULL) {
        m_task->exit();
        m_task = NULL;
    }
    if (m_unk364 != NULL) {
        m_unk364->exit();
        m_unk364 = NULL;
    }
    if (m_archive != NULL) {
        delete m_archive;
        m_archive = NULL;
    }
    scResourceLoaderEx* loader = m_loader;
    if (loader != NULL) {
        delete loader;
        m_loader = NULL;
    }
    g_gfApplication->unkD0.startKeepScreen();
    if (m_mode == 0) {
        u8 r = m_clearColor[0];
        u8 g = m_clearColor[1];
        u8 b = m_clearColor[2];
        u8 a = m_clearColor[3];
        gfFrameBuffer* frameBuffer = gfFrameBuffer::getInstance();
        frameBuffer->m_clearColor.r = r;
        frameBuffer->m_clearColor.g = g;
        frameBuffer->m_clearColor.b = b;
        frameBuffer->m_clearColor.a = a;
    }
    return 0;
}

// Runs the stage selection screen; when it decides, the choice is written to the data of the game and the scene ends.
s32 scSelStage::process() {
    if (m_task == NULL) {
        if (!fn_1_3E50(m_loader)) {
            return 0;
        }
        m_unk364 = fn_800AFBFC("SelectStageTask", 0x70, m_archive, 10);
        m_task = createTask(1);
        g_gfApplication->unkD0.endKeepScreen();
        if (m_mode == 0) {
            gfFrameBuffer* frameBuffer = gfFrameBuffer::getInstance();
            m_clearColor[0] = frameBuffer->m_clearColor.r;
            m_clearColor[1] = frameBuffer->m_clearColor.g;
            m_clearColor[2] = frameBuffer->m_clearColor.b;
            m_clearColor[3] = frameBuffer->m_clearColor.a;
            frameBuffer = gfFrameBuffer::getInstance();
            frameBuffer->m_clearColor.r = 0xFF;
            frameBuffer->m_clearColor.g = 0xFF;
            frameBuffer->m_clearColor.b = 0xFF;
            frameBuffer->m_clearColor.a = 0xFF;
        }
    }
    switch (m_task->getDecision()) {
    case 1: {
        OSReport("Edit stage selected [%s]\n", m_task->m_selKind == 3 ? "Yes" : "No");
        int stage;
        if (m_task->m_selKind == 0) {
            stage = 0x42;
        } else if (m_task->m_selKind == 1) {
            stage = m_task->m_selStage;
        } else {
            stage = 0;
        }
        OSReport("Selected stage = %d\n", stage);
        OSReport("Selected sub-stage = %d\n", reinterpret_cast<u8*>(m_task)[0x25C]);
        int next;
        if (m_task->m_selKind == 3) {
            next = 3;
        } else {
            u8* data = reinterpret_cast<u8*>(g_GameGlobal->m_selStageData);
            int id;
            if (m_task->m_selKind == 0) {
                id = 0x42;
            } else if (m_task->m_selKind == 1) {
                id = m_task->m_selStage;
            } else {
                id = 0;
            }
            *reinterpret_cast<u16*>(data + 0x22) = id;
            next = 1;
            data[0x24] = reinterpret_cast<u8*>(m_task)[0x25C];
        }
        gfSceneManager* mgr = gfSceneManager::getInstance();
        mgr->unk1 = next;
        mgr->processStep = 2;
        break;
    }
    case 2: {
        gfSceneManager* mgr = gfSceneManager::getInstance();
        mgr->unk1 = 0;
        mgr->processStep = 2;
        break;
    }
    case 3: {
        gfSceneManager* mgr = gfSceneManager::getInstance();
        mgr->unk1 = 2;
        mgr->processStep = 2;
        break;
    }
    }
    return 0;
}

// Hands the files of the pack to the stage selection screen and makes it.
muSelectStageTask* scSelStage::createTask(int load) {
    static const u8 mode[8] = {0, 0, 1, 2, 3, 0, 0, 0};
    scSelStageParam param;
    nw4r::g3d::ScnGroup* group = NULL;
    u8* rule = reinterpret_cast<u8*>(g_GameGlobal->m_selCharData);
    int kind = rule[4];
    int special;
    if (kind >= 12 && kind < 19) {
        special = 1;
    } else {
        special = 0;
    }
    if (load != 0) {
        m_unk38C = reinterpret_cast<int>(fn_800B03F0(m_archive, 0x14));
        m_unk390 = reinterpret_cast<int>(fn_800B03F0(m_archive, 0x50));
        m_unk394 = reinterpret_cast<int>(fn_800B03F0(m_archive, 0x28));
        m_unk398 = reinterpret_cast<int>(fn_800B03F0(m_archive, 0x1E));
        m_unk39C = reinterpret_cast<int>(fn_800B03F0(m_archive, 0x32));
        m_unk3A0 = reinterpret_cast<int>(fn_800B03F0(m_archive, 0x5A));
        m_unk3A4 = reinterpret_cast<int>(m_archive->getData(0, static_cast<ARCNodeType>(1), 0x46, 0xFFFE));
        m_unk3A8 = reinterpret_cast<int>(m_archive->getData(0, static_cast<ARCNodeType>(1), 0x3C, 0xFFFE));
        group = m_unk364->scene;
    }
    param.m_mode = mode[special + m_mode * 2];
    param.m_group = group;
    param.m_unk18 = 1;
    param.m_msgData = &m_unk390;
    param.m_unk30 = &m_unk38C;
    param.m_unk34 = &m_unk394;
    param.m_unk38 = &m_unk398;
    param.m_unk3C = &m_unk39C;
    param.m_unk28 = m_unk3A8;
    param.m_unk2C = m_unk448;
    if (special) {
        param.m_unk40 = m_unk3A4;
        param.m_unk44 = &m_unk3A0;
        param.m_unk48 = 0x4B0;
        param.m_unk4C = rule[6] != 0;
    }
    param.m_data1 = &m_unk3B0;
    param.m_data2 = m_unk3B8;
    param.m_data3 = &m_unk438;
    param.m_data4 = &m_unk440;
    param.m_unk20 = m_unk35C;
    param.m_padNo = m_padNo;
    return muSelectStageTask::create(reinterpret_cast<muSelectStageTask::Param*>(&param));
}
