#include <gf/gf_3d_scene.h>
#include <gf/gf_application.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <gm/gm_global.h>
#include <mu/mu_adv_difficulty.h>
#include <mu/mu_adv_name.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <revolution/OS/OSError.h>
#include <sc/sc_adv_name.h>
#include <snd/snd_system.h>
#include <sr/sr_common.h>
#include <types.h>

// The pad of a menu controller (main module).
extern "C" int fn_800BA3D8(muMenuController* controller);

scAdvName* scAdvName::create() {
    return new (Heaps::GlobalMode) scAdvName("scAdvName");
}

void scAdvName::setModule() {
    setLoadModule("sora_adv_menu_name.rel", false);
    setLoadModule("sora_adv_menu_difficulty.rel", false);
}

void scAdvName::start() {
    if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
        m_handle.readRequest("/menu/defaultcamera/CharacterSelect.brres", Heaps::MenuResource, 0, 0);
    } else {
        m_handle.readRequest("/menu/defaultcamera/CharacterSelectWide.brres", Heaps::MenuResource, 0, 0);
    }
    muAdvNameTaskParam param = { 0, 0, 1, -1 };
    if (m_data != NULL) {
        param.m_pad1 = m_data->m_pad1;
        param.m_pad2 = m_data->m_pad2;
    }
    m_task = muAdvNameTask::create(&param);
    m_step = 0;
}

s32 scAdvName::process() {
    bool isEnd = false;
    int result;
    switch (m_step) {
        case 0:
            m_step = 1;
            return 0;
        case 1:
            if (m_handle.isReady() == true) {
                m_step = 2;
            }
            return 0;
        case 2: {
            g_sndSystem->stopBGM(0x1E);
            m_resFile = m_handle.getBuffer();
            nw4r::g3d::ResFile::Init(&m_resFile);
            nw4r::g3d::ResAnmScn anmScn(NULL);
            anmScn = m_resFile.GetResAnmScn(0);
            anmScn.Bind(anmScn);
            g_gfSceneRoot->setInfoCamResAnm(&anmScn);
            m_step = 3;
            return 0;
        }
        case 4:
            return 0;
    }
    if (m_task == NULL) {
        return 0;
    }
    switch (m_task->m_result[0]) {
        case 2:
            isEnd = true;
            if (m_task->m_stCont == 1) {
                result = 2;
                m_data->m_name1 = 0;
                m_data->m_is2P = 0;
            } else {
                if (m_data != NULL) {
                    if (m_task->m_scroll[0] != 0 || m_task->m_cursor[0] != 0) {
                        m_data->m_name1 = m_task->m_nameSel1 + 1;
                    } else {
                        m_data->m_name1 = 0;
                    }
                    m_data->m_is2P = m_task->m_is2P;
                    if (m_data->m_is2P) {
                        if (m_task->m_scroll[1] != 0 || m_task->m_cursor[1] != 0) {
                            m_data->m_name2 = m_task->m_nameSel2 + 1;
                        } else {
                            m_data->m_name2 = 0;
                        }
                        m_data->m_pad2 = fn_800BA3D8(&m_task->m_players[1].m_controller);
                    } else {
                        m_data->m_name2 = -1;
                        m_data->m_pad2 = -1;
                    }
                }
                result = 1;
            }
            break;
        case 3:
            isEnd = true;
            result = 0;
            if (m_data != NULL) {
                m_data->m_name1 = 0;
                m_data->m_is2P = 0;
            }
            break;
        case 0:
            // TODO: clean up - the empty case keeps the compare against 0 of the original dispatch.
            break;
        case 1: {
            bool selected = selectDifficulty();
            if (m_diffTask == NULL) {
                // TODO: clean up - a local per branch, the direct member accesses reload m_task after each store.
                if (selected) {
                    muAdvNameTask* task = m_task;
                    task->m_flag352 = true;
                    task->m_flag353 = true;
                    OSReport("DIFF SELECTED\n");
                } else {
                    muAdvNameTask* task = m_task;
                    task->m_flag352 = true;
                    task->m_flag353 = false;
                    OSReport("DIFF CANCELED\n");
                }
            }
            break;
        }
    }
    if (isEnd) {
        gfSceneManager* mgr = gfSceneManager::getInstance();
        mgr->unk1 = result;
        mgr->processStep = 2;
    }
    return 0;
}

// Runs the difficulty task (made the first time); true when a difficulty was taken (it is written to the data).
bool scAdvName::selectDifficulty() {
    bool isEnd = false;
    bool selected = false;
    if (m_diffTask == NULL) {
        muAdvDifficultyTaskParam param = { 1, 0, 1 };
        if (m_data != NULL) {
            param.m_pad0 = m_data->m_pad1;
            param.m_pad1 = m_data->m_pad2;
        }
        m_diffTask = muAdvDifficultyTask::create(&param);
    }
    switch (m_diffTask->m_result) {
        case 1:
            isEnd = true;
            selected = true;
            m_data->m_difficulty = m_diffTask->m_difficulty;
            break;
        case 2:
            isEnd = true;
            selected = false;
            break;
    }
    if (isEnd) {
        m_diffTask->exit();
        m_diffTask = NULL;
    }
    return selected;
}

s32 scAdvName::exit() {
    if (m_task != NULL) {
        m_task->exit();
        m_task = NULL;
    }
    if (m_diffTask != NULL) {
        m_diffTask->exit();
        m_diffTask = NULL;
    }
    g_gfSceneRoot->removeInfoCamResAnm();
    m_resFile.Release();
    gfHeapManager::free(m_handle.getBuffer());
    m_handle.release();
    g_gfApplication->unkD0.startKeepScreen();
    return 0;
}
