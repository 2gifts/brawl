#include <cm/cm_camera_controller.h>
#include <gf/gf_application.h>
#include <gf/gf_archive.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <mu/mu_challenger_approach.h>
#include <sr/sr_common.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

extern "C" void fn_800151FC(gfArchive* archive, const char* path, Heaps::HeapType heap, int offset, int size, u32 id);

class muMenu {
public:
    static void loadMenuSound();
    static bool isLoadFinishMenuSound();
};

// The "challenger approaches" scene shown before a challenger battle.
class scChallenger : public gfScene {
    char m_paths[15][0x30];
    gfArchive* m_archive;
    muChallengerApproachTask* m_task;
    s32 m_step;
    s32 m_index;

public:
    static scChallenger* create();
    scChallenger();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
};

scChallenger* scChallenger::create() {
    return new (Heaps::GlobalMode) scChallenger;
}

scChallenger::scChallenger() : gfScene("scChallenger") {
    memset(m_paths, 0, sizeof(m_paths));
    s32 i = 0;
    sprintf(m_paths[i++], "/menu/cmnwindow/challenger/Challenger01.pac");
    sprintf(m_paths[i++], "/menu/cmnwindow/challenger/Challenger02.pac");
    sprintf(m_paths[i++], "/menu/cmnwindow/challenger/Challenger03.pac");
    sprintf(m_paths[i++], "/menu/cmnwindow/challenger/Challenger04.pac");
    sprintf(m_paths[i++], "/menu/cmnwindow/challenger/Challenger05.pac");
    sprintf(m_paths[i++], "/menu/cmnwindow/challenger/Challenger06.pac");
    sprintf(m_paths[i++], "/menu/cmnwindow/challenger/Challenger07.pac");
    sprintf(m_paths[i++], "/menu/cmnwindow/challenger/Challenger08.pac");
    sprintf(m_paths[i++], "/menu/cmnwindow/challenger/Challenger09.pac");
    sprintf(m_paths[i++], "/menu/cmnwindow/challenger/Challenger10.pac");
    sprintf(m_paths[i++], "/menu/cmnwindow/challenger/Challenger11.pac");
    sprintf(m_paths[i++], "/menu/cmnwindow/challenger/Challenger12.pac");
    sprintf(m_paths[i++], "/menu/cmnwindow/challenger/Challenger13.pac");
    sprintf(m_paths[i++], "/menu/cmnwindow/challenger/Challenger14.pac");
    m_archive = nullptr;
    m_task = nullptr;
}

void scChallenger::start() {
    m_step = 0;
}

s32 scChallenger::process() {
    switch (m_step) {
    case 0: {
        CameraController::getInstance()->changeInput(CameraController::Kind(9));
        const s32 table[14] = { 2, 3, 0, 5, 1, 7, 12, 8, 6, 4, 13, 9, 10, 11 };
        s32 index = gfSceneManager::getInstance()->m_memoryLayout;
        if (index < 0) {
            index = 0;
        }
        m_index = table[index];
        m_archive = new (Heaps::MenuInstance) gfArchive;
        m_task = muChallengerApproachTask::create();
        m_task->initialize(m_index);
        fn_800151FC(m_archive, m_paths[m_index], Heaps::MenuResource, 0, 0, 0xFF);
        muMenu::loadMenuSound();
        m_step = 1;
        break;
    }
    case 1:
        {
            bool waiting = !m_archive->unk9 ? true : !muMenu::isLoadFinishMenuSound();
            if (!waiting) {
                m_step = 2;
            }
        }
        break;
    case 2:
        m_task->createData(m_archive);
        g_gfApplication->unkD0.endKeepScreen();
        m_step = 3;
        break;
    case 3:
        m_task->m_animState = 1;
        m_step = 4;
        break;
    case 4:
        if (m_task->m_animState == 11) {
            m_step = 5;
        }
        break;
    case 5:
        m_step = 6;
        break;
    case 6: {
        g_gfApplication->unkD0.startKeepScreen();
        delete m_archive;
        m_archive = nullptr;
        m_task->release();
        m_task->exit();
        m_task = nullptr;
        gfSceneManager* mgr = gfSceneManager::getInstance();
        mgr->unk1 = 0;
        mgr->processStep = 2;
        m_step++;
        break;
    }
    }
    return 0;
}

s32 scChallenger::exit() {
    return 0;
}

void scChallenger::setModule() {
    setLoadModule("sora_menu_challenger.rel", false);
}
