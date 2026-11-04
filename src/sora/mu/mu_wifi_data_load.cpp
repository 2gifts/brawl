#include <gf/gf_heap_manager.h>
#include <gf/gf_sys_recorder2.h>
#include <mu/wifi/mu_wifi_data_load_task.h>
#include <nt/nt_offline.h>
#include <types.h>

extern const char lbl_80459C28[13];
muWifiDataLoadTask* muWifiDataLoadTask::create(nw4r::g3d::ResFile* resFile, nw4r::g3d::ScnGroup* group, void* msgData, int arg) {
    muWifiDataLoadTask* task = new (Heaps::MenuInstance) muWifiDataLoadTask;
    task->m_resFile = resFile;
    task->m_scnGroup = group;
    task->m_msgData = msgData;
    task->m_priority = arg;
    if (!gfSysRecorder2::isEnable(2)) {
        task->m_result = 1;
        task->m_step = 4;
    } else {
        task->m_recorder = gfSysRecorder2::createInstance(Heaps::MenuResource);
        if (task->m_recorder == NULL) {
            task->m_step = 5;
        } else {
            task->m_step = 0;
        }
    }
    return task;
}

muWifiDataLoadTask::muWifiDataLoadTask()
    : gfTask(lbl_80459C28, Category_Menu, 15, 8, true), m_resFile(NULL), m_scnGroup(NULL), m_result(0), m_msgData(NULL),
      m_step(0), m_prevStep(6), m_priority(1), m_unk5C(0), m_recorder(NULL), m_unk64(0), m_unk68(0), m_unk6C(false) {
    unk2C_b1 = false;
}

void muWifiDataLoadTask::exit() {
    if (m_recorder != NULL) {
        gfSysRecorder2::dropInstance();
    }
    gfTask::exit();
}

muWifiDataLoadTask::~muWifiDataLoadTask() {}

int muWifiDataLoadTask::mainStepLoadMain(muWifiDataLoadTask* task) {
    gfSysRecorder2* recorder = task->m_recorder;
    if (recorder->m_busy == 0) {
        if (recorder->m_result == 0) {
            ntFriendInfo* info = ntFriendInfo::getInstance();
            info->setSaveData(recorder->m_handle.getBuffer(), recorder->m_handle.getSize());
            gfHeapManager::free(recorder->m_handle.getBuffer());
            if (task->m_unk6C) {
                task->m_result = 0;
                return 4;
            }
            return 5;
        }
        return 1;
    }
    return task->m_step;
}
