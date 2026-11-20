#include <cm/cm_camera_controller.h>
#include <gf/gf_application.h>
#include <gf/gf_archive.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <mu/mu_game_over.h>
#include <mu/mu_menuroot.h>
#include <mu/mu_resource_loader.h>
#include <sc/sc_gameover.h>
#include <sr/sr_common.h>
#include <types.h>

// Starts the read of a pack of the scene module into the archive (scResourceLoader of the scene module).
extern "C" void fn_1_3C68(scResourceLoader* loader, int heap, const char* path, gfArchive* archive, int size);
// Tells whether the pack is read.
extern "C" bool fn_1_3E50(scResourceLoader* loader);
// Makes the scene root of a pack of the main module.
extern "C" MenuRoot* fn_800AFBFC(const char* name, int heap, gfArchive* archive, int index);

scGameover* scGameover::create() {
    return new (Heaps::GlobalMode) scGameover("scGameover");
}

void scGameover::start() {
    m_work = static_cast<Work*>(gfHeapManager::alloc(Heaps::MenuInstance, sizeof(Work)));
    m_loader = new (Heaps::MenuResource) scResourceLoader();
    m_archive = new (Heaps::MenuResource) gfArchive();
    fn_1_3C68(m_loader, Heaps::MenuResource, "/menu2/sc_gameover.pac", m_archive, 0);
    m_isStarted = false;
    m_work->m_task = NULL;
}

nw4r::g3d::ScnGroup* scGameover::getScnRoot() {
    return m_work->m_root->scene;
}

s32 scGameover::process() {
    if (!m_isStarted) {
        if (!fn_1_3E50(m_loader)) {
            return 0;
        }
        m_work->m_root = fn_800AFBFC("GameoverTask", 0x100, m_archive, 10);
        CameraController::getInstance()->changeInput(CameraController::MenuFixed);
        m_work->m_task = muGameOverTask::create(&m_param, m_archive);
        g_gfApplication->unkD0.endKeepScreen();
        m_isStarted = true;
    }
    return 0;
}

s32 scGameover::exit() {
    g_gfApplication->unkD0.startKeepScreen();
    if (m_work == NULL) {
        return 0;
    }
    m_work->m_root->exit();
    if (m_work->m_task != NULL) {
        m_work->m_task->exit();
    }
    delete m_archive;
    m_archive = NULL;
    if (m_loader != NULL) {
        delete m_loader;
        m_loader = NULL;
    }
    gfHeapManager::free(m_work);
    m_work = NULL;
    return 0;
}

void scGameover::setModule() {
    setLoadModule("sora_menu_game_over.rel", false);
}
