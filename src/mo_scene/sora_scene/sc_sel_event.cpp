#include <gf/gf_application.h>
#include <gf/gf_archive.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_event.h>
#include <mu/mu_menuroot.h>
#include <mu/mu_resource_loader.h>
#include <cm/cm_camera_controller.h>
#include <sc/sc_sel_event.h>
#include <sr/sr_common.h>
#include <string.h>
#include <types.h>

// Starts the read of a pack of the scene module into the archive (scResourceLoader of the scene module).
extern "C" void fn_1_3C68(scResourceLoader* loader, int heap, const char* path, gfArchive* archive, int size);
// Tells whether the pack is read.
extern "C" bool fn_1_3E50(scResourceLoader* loader);
// Makes the scene root of a pack of the main module.
extern "C" MenuRoot* fn_800AFBFC(const char* name, int heap, gfArchive* archive, int index);

scSelEvent* scSelEvent::create() {
    return new (Heaps::GlobalMode) scSelEvent("scSelEvent");
}

// Clears the choice of the player.
#pragma dont_inline on
void scSelEvent::reset() {
    m_setting.m_cursor = 0;
    m_setting.m_level = 0;
    m_setting.m_playerCount = 1;
    m_setting.m_pad[0] = 0;
    m_setting.m_pad[1] = 0;
    m_setting.m_name[0] = -1;
    m_setting.m_name[1] = -1;
    m_setting.m_eventNo = 0;
    m_setting.m_unk20 = 0;
    memset(&m_fighters, 0, sizeof(m_fighters));
}
#pragma dont_inline reset

void scSelEvent::setModule() {
    setLoadModule("sora_menu_name.rel", false);
    setLoadModule("sora_menu_event.rel", false);
}

void scSelEvent::start() {
    if (gfSceneManager::getInstance()->m_memoryLayout == 0) {
        reset();
        m_setting.m_pad[0] = m_firstPad;
    } else {
        m_setting.m_unk20 = 1;
    }
    m_work = static_cast<Work*>(gfHeapManager::alloc(Heaps::MenuInstance, sizeof(Work)));
    m_work->m_root = NULL;
    m_work->m_task = NULL;
    m_loader = new (Heaps::MenuResource) scResourceLoader();
    m_archive = new (Heaps::MenuResource) gfArchive();
    fn_1_3C68(m_loader, Heaps::MenuResource, "/menu2/sc_sel_event.pac", m_archive, 0);
}

s32 scSelEvent::process() {
    if (m_work->m_task == NULL && fn_1_3E50(m_loader)) {
        m_work->m_root = fn_800AFBFC("EventTask", 0x100, m_archive, 10);
        CameraController::getInstance()->changeInput(CameraController::MenuFixed);
        m_work->m_task = muEventTask::create(m_work->m_root->scene, &m_setting, m_archive);
    }
    return 0;
}

s32 scSelEvent::exit() {
    g_gfApplication->unkD0.startKeepScreen();
    if (m_work == NULL) {
        return 0;
    }
    m_work->m_root->exit();
    m_work->m_task->exit();
    if (m_archive != NULL) {
        delete m_archive;
        m_archive = NULL;
    }
    if (m_loader != NULL) {
        delete m_loader;
        m_loader = NULL;
    }
    gfHeapManager::free(m_work);
    m_work = NULL;
    return 0;
}
