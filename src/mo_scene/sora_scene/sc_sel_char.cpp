#include <cm/cm_camera_controller.h>
#include <gf/gf_application.h>
#include <gf/gf_archive.h>
#include <gf/gf_display.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/menu.h>
#include <mu/mu_menuroot.h>
#include <mu/mu_resource_loader.h>
#include <sr/sr_common.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <string.h>
#include <types.h>

// Starts the read of a pack of the scene module into the archive (scResourceLoader of the scene module).
extern "C" void fn_1_3C68(scResourceLoader* loader, int heap, const char* path, gfArchive* archive, int size);

// Tells whether the pack is read.
extern "C" bool fn_1_3E50(scResourceLoader* loader);
// Hands the archives that the packs are read into to the loaders (scene module).
extern "C" void fn_1_3F04(scResourceLoader* loader, gfArchive* archive, gfArchive* main, int unk);
extern "C" void fn_1_3F84(scResourceLoader* loader, gfArchive* archive, int unk);
// Gets the data of the file with that number from the archive (main module).
extern "C" void* fn_800B03F0(gfArchive* archive, int index);
// Makes the scene root of a pack of the main module.
extern "C" MenuRoot* fn_800AFBFC(const char* name, int heap, gfArchive* archive, int index);

// The character selection task of sora_menu_sel_char.rel. Only what the scene reads is declared.
class muSelCharTask : public gfTask {
public:
    int m_result;                       // 0x40 how the screen was left (2: on, 4: back, 5: ...)

    static muSelCharTask* create(void* param);
};

// What the scene gives to the character selection task: the data of the pack and what the scene keeps.
// HYPOTHESIS: the names follow how the scene fills the members.
struct scSelCharParam {
    void* m_data0;
    void* m_data1;
    void* m_data2;
    void* m_archive2;                   // 0x0C
    void* m_data3;
    void* m_data4;
    void* m_data5;
    void* m_data6;
    void* m_data7;
    void* m_data8;
    nw4r::g3d::ScnGroup* m_group;
    void* m_data9;
    void* m_data10;
    int m_unk3C;
    int m_unk40;
    int m_unk44;
    u8 m_unk48;

    scSelCharParam() {
        m_data0 = NULL;
        m_data1 = NULL;
        m_data2 = NULL;
        m_data3 = NULL;
        m_data4 = NULL;
        m_data5 = NULL;
        m_data6 = NULL;
        m_data7 = NULL;
        m_data8 = NULL;
        m_group = NULL;
        m_data9 = NULL;
        m_data10 = NULL;
        m_unk3C = 0;
        m_unk40 = 0;
        m_unk44 = 0;
        m_unk48 = 0;
    }
};

// A file of the menu that is read when the scene starts (the handle is the only thing it holds).
struct scNetFileHandle : gfFileIOHandle {
    scNetFileHandle() { m_request = NULL; }
};

// The scene of the character selection (module sora_scene): reads the packs of the screen and runs the character selection
// of sora_menu_sel_char.rel. The scenes of the modes of the game hand their choice to it and read it back.
class scSelChar : public muMenu {
public:
    u8 m_unk350[0x35C - 0x350];
    MenuRoot* m_unk35C;                 // 0x35C
    CameraController* m_camera;         // 0x360
    scNetFileHandle m_files[10];        // 0x364
    int m_unk38C;
    int m_unk390;
    int m_unk394;
    int m_unk398;
    int m_unk39C;
    int m_unk3A0;
    int m_unk3A4;
    int m_unk3A8;
    gfArchive* m_unk3AC;                // 0x3AC
    gfArchive* m_unk3B0;                // 0x3B0
    int m_unk3B4;
    int m_unk3B8;
    int m_unk3BC;
    int m_unk3C0;
    int m_unk3C4;
    u8 m_pad3C8[0x4];
    int m_unk3CC;
    int m_unk3D0;
    int m_unk3D4;
    int m_unk3D8;
    int m_unk3DC;
    int m_unk3E0;
    int m_unk3E4;
    int m_unk3E8;
    int m_unk3EC;
    int m_unk3F0;
    int m_unk3F4;
    int m_unk3F8;
    u8 m_unk3FC;
    u8 m_pad3FD[0x3];
    muSelCharTask* m_task;              // 0x400
    int m_unk404;
    u8 m_unk408;
    u8 m_color[4];
    u8 m_pad40D[0x3];
    gfArchive* m_archive;               // 0x410
    scResourceLoader* m_loader;         // 0x414
    scResourceLoader* m_loader2;        // 0x418

    scSelChar();
    static scSelChar* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
    muSelCharTask* createTask();
};
static_assert(sizeof(scSelChar) == 0x41C, "Class is wrong size!");

scSelChar* scSelChar::create() {
    return new (Heaps::GlobalMode) scSelChar();
}

scSelChar::scSelChar() : muMenu("scSelctCharacter"), m_unk35C(NULL), m_camera(NULL) {
    m_unk3AC = 0;
    m_unk3B0 = 0;
    m_unk3B4 = 0;
    m_unk3B8 = 0;
    m_unk3BC = 0;
    m_unk3C0 = 0;
    m_unk3C4 = 0;
    m_unk3CC = 0;
    m_unk3D0 = 0;
    m_unk3D4 = 0;
    m_unk3D8 = 0;
    m_unk3DC = 0;
    m_unk3E0 = 0;
    m_unk3E4 = 0;
    m_unk3E8 = 0;
    m_unk3EC = 0;
    m_unk3F0 = 0;
    m_unk3F4 = 0;
    m_unk3F8 = 0;
    m_unk3FC = 0;
    m_task = NULL;
    m_unk404 = -1;
    m_unk408 = 0;
    m_archive = NULL;
    m_loader = NULL;
    m_loader2 = NULL;
    m_unk38C = 0;
    m_unk390 = 0;
    m_unk394 = 0;
    m_unk398 = 0;
    m_unk39C = 0;
    m_unk3A0 = 0;
    m_unk3A4 = 0;
    m_unk3A8 = 0;
    memset(m_color, 0, sizeof(m_color));
}

void scSelChar::setModule() {
    setLoadModule("sora_menu_sel_char.rel", false);
    setLoadModule("sora_menu_name.rel", false);
    setLoadModule("sora_menu_rule.rel", false);
    setLoadModule("sora_menu_sel_char_access.rel", false);
}

void scSelChar::start() {
    m_unk408 = 0;
    m_archive = new (Heaps::MenuResource) gfArchive();
    m_unk3B0 = new (Heaps::MenuResource) gfArchive();
    m_unk3AC = new (Heaps::InfoExtraResource) gfArchive();
    m_loader = new (Heaps::MenuResource) scResourceLoader();
    m_loader2 = new (Heaps::InfoExtraResource) scResourceLoader();
    m_camera = CameraController::getInstance();
    fn_1_3C68(m_loader, Heaps::MenuResource, "/menu2/sc_selcharacter.pac", m_archive, 0x100000);
    fn_1_3C68(m_loader2, Heaps::InfoExtraResource, "/menu2/sc_selcharacter2.pac", NULL, 0);
    m_camera->changeInput(CameraController::MenuPad);
    muMenu::loadMenuSound();
    m_task = NULL;
    memset(m_color, 0, sizeof(m_color));
}

s32 scSelChar::exit() {
    g_gfApplication->unkD0.startKeepScreen();
    u8 r, g, b, a;
    a = m_color[3];
    b = m_color[2];
    g = m_color[1];
    r = m_color[0];
    gfFrameBuffer* frameBuffer = gfFrameBuffer::getInstance();
    frameBuffer->m_clearColor.r = r;
    frameBuffer->m_clearColor.g = g;
    frameBuffer->m_clearColor.b = b;
    frameBuffer->m_clearColor.a = a;
    if (m_task != NULL) {
        m_task->exit();
        m_task = NULL;
    }
    if (m_unk35C != NULL) {
        m_unk35C->exit();
        m_unk35C = NULL;
    }
    if (m_unk3AC != NULL) {
        delete m_unk3AC;
        m_unk3AC = NULL;
    }
    if (m_unk3B0 != NULL) {
        delete m_unk3B0;
        m_unk3B0 = NULL;
    }
    if (m_archive != NULL) {
        delete m_archive;
        m_archive = NULL;
    }
    scResourceLoader* loader = m_loader;
    if (loader != NULL) {
        delete loader;
        m_loader = NULL;
    }
    scResourceLoader* loader2 = m_loader2;
    if (loader2 != NULL) {
        delete loader2;
        m_loader2 = NULL;
    }
    return 0;
}

s32 scSelChar::process() {
    if (m_task == NULL) {
        bool ready;
        if (fn_1_3E50(m_loader)) {
            ready = fn_1_3E50(m_loader2);
        } else {
            ready = false;
        }
        if (ready) {
            fn_1_3F04(m_loader, m_unk3B0, m_archive, 20);
            fn_1_3F84(m_loader2, m_unk3AC, 10);
            m_unk35C = fn_800AFBFC("SelCharMenuRootTask", 0x12C, m_archive, 10);
            m_task = createTask();
            g_gfApplication->unkD0.endKeepScreen();
            gfFrameBuffer* frameBuffer = gfFrameBuffer::getInstance();
            m_color[0] = frameBuffer->m_clearColor.r;
            m_color[1] = frameBuffer->m_clearColor.g;
            m_color[2] = frameBuffer->m_clearColor.b;
            m_color[3] = frameBuffer->m_clearColor.a;
            frameBuffer = gfFrameBuffer::getInstance();
            frameBuffer->m_clearColor.r = 0xFF;
            frameBuffer->m_clearColor.g = 0xFF;
            frameBuffer->m_clearColor.b = 0xFF;
            frameBuffer->m_clearColor.a = 0xFF;
        }
    } else {
        switch (m_task->m_result) {
        case 2: {
            gfSceneManager* mgr = gfSceneManager::getInstance();
            mgr->unk1 = 1;
            mgr->processStep = 2;
            break;
        }
        case 4: {
            gfSceneManager* mgr = gfSceneManager::getInstance();
            mgr->unk1 = 0;
            mgr->processStep = 2;
            break;
        }
        case 5: {
            gfSceneManager* mgr = gfSceneManager::getInstance();
            mgr->unk1 = 2;
            mgr->processStep = 2;
            break;
        }
        }
    }
    return 0;
}

// Hands the data of the pack to the character selection and makes it.
muSelCharTask* scSelChar::createTask() {
    scSelCharParam param;
    m_unk38C = reinterpret_cast<int>(fn_800B03F0(m_archive, 0x1E));
    m_unk390 = reinterpret_cast<int>(fn_800B03F0(m_archive, 0x46));
    m_unk394 = reinterpret_cast<int>(fn_800B03F0(m_archive, 0x50));
    m_unk398 = reinterpret_cast<int>(fn_800B03F0(m_archive, 0x28));
    m_unk39C = reinterpret_cast<int>(fn_800B03F0(m_archive, 0x5A));
    m_unk3A0 = reinterpret_cast<int>(fn_800B03F0(m_archive, 0x32));
    m_unk3A4 = reinterpret_cast<int>(fn_800B03F0(m_archive, 0x3C));
    m_unk3A8 = reinterpret_cast<int>(fn_800B03F0(m_archive, 0x82));
    m_unk3B4 = reinterpret_cast<int>(m_archive->getData(0, static_cast<ARCNodeType>(1), 0x6E, 0xFFFE));
    m_unk3B8 = reinterpret_cast<int>(m_archive->getData(0, static_cast<ARCNodeType>(1), 0x78, 0xFFFE));
    param.m_data0 = &m_unk38C;
    param.m_data1 = &m_unk390;
    param.m_data2 = &m_unk394;
    param.m_archive2 = m_unk3B0;
    param.m_data4 = &m_unk398;
    param.m_data5 = &m_unk39C;
    param.m_data6 = &m_unk3A0;
    param.m_data7 = &m_unk3A4;
    param.m_data8 = &m_unk3A8;
    param.m_data3 = m_unk3AC;
    param.m_group = m_unk35C->scene;
    param.m_data9 = reinterpret_cast<void*>(m_unk3B4);
    param.m_data10 = reinterpret_cast<void*>(m_unk3B8);
    param.m_unk44 = 0xA8C;
    param.m_unk3C = 0x27;
    param.m_unk40 = 0x27;
    param.m_unk48 = m_unk408;
    return muSelCharTask::create(&param);
}
