#include <cm/cm_camera_controller.h>
#include <gf/gf_application.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <gm/gm_global.h>
#include <mu/mu_intro.h>
#include <mu/mu_menuroot.h>
#include <sc/sc_intro.h>
#include <sr/sr_common.h>
#include <types.h>

// A function of the scene module (called with 1 when the intro starts and with 0 once it runs).
// TODO: clean up - name it.
extern "C" void fn_1_40E0(int unk);

// TODO: clean up - the constructor is not inlined here in the original.
#pragma dont_inline on
scIntro* scIntro::create() {
    return new (Heaps::GlobalMode) scIntro();
}
#pragma dont_inline reset

scIntro::scIntro() : muMenu("scIntro"), m_rumbleController(0), m_work(NULL), m_isLoaded(0), m_rumbleSetting(0x78),
                     m_unk3A4(0) {
    m_param.reset();
}

void scIntro::setModule() {
    setLoadModule("sora_menu_intro.rel", false);
}

void scIntro::start() {
    m_unk3A8 = 0;
    m_work = static_cast<Work*>(gfHeapManager::alloc(Heaps::MenuInstance, sizeof(Work)));
    m_work->m_root = MenuRoot::create("IntroRoot", 0x40, NULL);
    m_isLoaded = 0;
    u32 wide = g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen;
    m_handle.readRequest(wide ? "/menu/defaultcamera/CharacterSelectWide.brres" : "/menu/defaultcamera/CharacterSelect.brres",
                         Heaps::MenuResource, 0, 0);
    m_work->m_task = NULL;
    m_work->m_unk8 = 0;
    CameraController::getInstance()->changeInput(CameraController::MenuFixed);
    fn_1_40E0(1);
}

s32 scIntro::process() {
    if (!m_isLoaded && m_handle.isReady()) {
        if (m_work->m_task == NULL) {
            m_work->m_task = muIntroTask::create();
        }
        if (m_work->m_task->isLoadFinished() == true) {
            m_work->m_root->attachScnAnm(&m_handle);
            m_handle.release();
            m_isLoaded = 1;
            fn_1_40E0(0);
        }
    }
    return 0;
}

s32 scIntro::exit() {
    g_gfApplication->unkD0.startKeepScreen();
    if (m_work == NULL) {
        return 0;
    }
    m_work->m_root->exit();
    m_work->m_task->exit();
    gfHeapManager::free(m_work);
    m_work = NULL;
    m_handle.cancelRequest();
    return 0;
}
