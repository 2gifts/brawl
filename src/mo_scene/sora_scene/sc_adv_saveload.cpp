#include <ad/ad_autosave.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <gm/gm_global.h>
#include <mu/adv/mu_adv_saveload.h>
#include <sc/sc_adv_saveload.h>
#include <sr/sr_common.h>
#include <types.h>

scAdvSave* scAdvSave::create() {
    return new (Heaps::GlobalMode) scAdvSave("scAdvSave");
}

void scAdvSave::setModule() { }

typedef gmAdvData::PadInfo UnkParam;

void scAdvSave::start() {
    UnkParam stack20;
    UnkParam stack18;
    UnkParam stack10;
    if (g_GameGlobal->m_advData) {
        stack18 = g_GameGlobal->m_advData->m_padInfo[0];
    } else {
        stack20.m_flag.m_isSet = false;
        stack20.m_padNo = -1;
        stack18 = stack20;
    }
    stack10 = stack18;
    muAdvSaveTaskParam param;
    param.unk0 = stack10.m_padNo;
    s32 layout = gfSceneManager::getInstance()->m_memoryLayout;
    if (layout == 0) {
        muAdvSaveTask* task = muAdvSaveTask::create(&param, Heaps::MenuInstance, 43);
        if (m_task) {
            m_task->exit();
        }
        m_task = task;
    } else if (layout == 1) {
        m_autosave = adAutoSave::create(false);
    } else {
        m_autosave = adAutoSave::create(true);
    }
}

s32 scAdvSave::process() {
    if (gfSceneManager::getInstance()->m_memoryLayout == 0) {
        if (m_task->unk1B8 == 1) {
            gfSceneManager* mgr = gfSceneManager::getInstance();
            mgr->unk1 = 0;
            mgr->processStep = 2;
        }
    } else if (!adAutoSave::isAlive()) {
        gfSceneManager* mgr = gfSceneManager::getInstance();
        mgr->unk1 = 0;
        mgr->processStep = 2;
    }
    return 0;
}

s32 scAdvSave::exit() {
    if (m_task) {
        m_task->exit();
    }
    m_task = nullptr;
    return 0;
}

scAdvLoad* scAdvLoad::create() {
    return new (Heaps::GlobalMode) scAdvLoad("scAdvLoad");
}

void scAdvLoad::setModule() { }

void scAdvLoad::start() {
    UnkParam stack20;
    UnkParam stack18;
    UnkParam stack10;
    if (g_GameGlobal->m_advData) {
        stack18 = g_GameGlobal->m_advData->m_padInfo[0];
    } else {
        stack20.m_flag.m_isSet = false;
        stack20.m_padNo = -1;
        stack18 = stack20;
    }
    stack10 = stack18;
    muAdvLoadTaskParam param;
    param.unk0 = stack10.m_padNo;
    muAdvLoadTask* task = muAdvLoadTask::create(&param);
    if (m_task) {
        m_task->exit();
    }
    m_task = task;
}

s32 scAdvLoad::process() {
    if (m_task->unk1B8 == 1) {
        bool isLoaded = m_task->isLoaded();
        gfSceneManager* mgr = gfSceneManager::getInstance();
        mgr->unk1 = isLoaded;
        mgr->processStep = 2;
    }
    return 0;
}

s32 scAdvLoad::exit() {
    if (m_task) {
        m_task->exit();
    }
    m_task = nullptr;
    return 0;
}
