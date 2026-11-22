#include <gf/gf_3d_scene.h>
#include <gf/gf_application.h>
#include <gf/gf_archive.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_task.h>
#include <mu/mu_object.h>
#include <gf/gf_scene.h>
#include <snd/snd_system.h>
#include <types.h>

// The movie scene of the adventure mode (module sora_scene): it plays the cut scenes (sora_adv_menu_visual.rel) and the
// character select steps between them (sora_adv_menu_telop.rel).
// HYPOTHESIS: the member names; only the constructor's values and start's writes are known.
class scAdvVisual : public gfScene {
public:
    // The movie parameters the constructor sets (0x8..0x20).
    struct Param {
        int m_unk0;  // 5
        int m_unk4;
        int m_unk8;
        int m_unkC;  // 1
        int m_unk10;
        int m_unk14; // 0x3F
        int m_unk18; // 1
    };

    Param m_param;              // 0x08
    u8 m_unk24[4];
    int m_step;                 // 0x28
    u8 m_unk2C[4];
    int m_unk30;                // 0x30
    gfTask* m_task34;           // 0x34
    gfTask* m_task38;           // 0x38
    gfTask* m_task3C;           // 0x3C
    gfFileIOHandle* m_handles;  // 0x40 three file handles
    int m_unk44;
    MuObject* m_object48;       // 0x48
    MuObject* m_object4C;       // 0x4C
    void* m_buffer;             // 0x50
    gfArchive* m_archive;       // 0x54
    u8 m_unk58[8];
    u8 m_isLoaded;              // 0x60
    u8 m_isEnd;                 // 0x61

    scAdvVisual();
    static scAdvVisual* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
    void load();
};
static_assert(sizeof(scAdvVisual) == 0x64, "Class is wrong size!");

scAdvVisual* scAdvVisual::create() {
    return new (Heaps::GlobalMode) scAdvVisual();
}

scAdvVisual::scAdvVisual()
    : gfScene("scAdvVisual"), m_step(0), m_unk30(0), m_task34(NULL), m_task38(NULL), m_task3C(NULL), m_handles(NULL),
      m_unk44(0), m_object48(NULL), m_object4C(NULL) {
    m_param.m_unk0 = 5;
    m_param.m_unk4 = 0;
    m_param.m_unk8 = 0;
    m_param.m_unkC = 1;
    m_param.m_unk10 = 0;
    m_param.m_unk14 = 0x3F;
    m_param.m_unk18 = 1;
}

void scAdvVisual::setModule() {
    setLoadModule("sora_adv_menu_visual.rel", false);
    setLoadModule("sora_adv_menu_telop.rel", false);
}

void scAdvVisual::start() {
    m_step = 0;
    m_isLoaded = 0;
    m_isEnd = 0;
    g_sndSystem->stopBGM(0);
    load();
}

// The movie task of the module (sora_scene bss 0x1790).
// TODO: clean up - name it once the unit that creates it is decompiled.
extern gfTask* lbl_1_bss_1790;

s32 scAdvVisual::exit() {
    if (lbl_1_bss_1790 != NULL) {
        lbl_1_bss_1790->exit();
        lbl_1_bss_1790 = NULL;
    }
    if (m_handles != NULL) {
        for (int i = 0; i < 3; i++) {
            gfHeapManager::free(m_handles[i].getBuffer());
            m_handles[i].release();
        }
        delete[] m_handles;
        m_handles = NULL;
    }
    if (m_object48 != NULL) {
        delete m_object48;
        m_object48 = NULL;
    }
    if (m_object4C != NULL) {
        delete m_object4C;
        m_object4C = NULL;
    }
    g_gfSceneRoot->removeInfoCamResAnm();
    if (m_task34 != NULL) {
        m_task34->exit();
        m_task34 = NULL;
    }
    if (m_task38 != NULL) {
        m_task38->exit();
        m_task38 = NULL;
    }
    if (m_task3C != NULL) {
        m_task3C->exit();
        m_task3C = NULL;
    }
    if (m_archive != NULL) {
        delete m_archive;
        m_archive = NULL;
    }
    if (m_buffer != NULL) {
        gfHeapManager::free(m_buffer);
        m_buffer = NULL;
    }
    g_gfApplication->unkD0.startKeepScreen();
    return 0;
}
