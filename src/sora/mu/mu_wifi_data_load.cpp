#include <gf/gf_heap_manager.h>
#include <gf/gf_sys_recorder2.h>
#include <mu/mu_notice_wnd.h>
#include <mu/mu_wifi_data_load.h>
#include <nt/nt_offline.h>
#include <types.h>

extern const char lbl_80459C28[];
muWifiDataLoadTask* muWifiDataLoadTask::create(nw4r::g3d::ResFile* resFile, void* group, void* msgData, int arg) {
    muWifiDataLoadTask* task = new (Heaps::MenuInstance) muWifiDataLoadTask;
    task->unk40 = resFile;
    task->unk44 = group;
    task->unk4C = msgData;
    task->unk58 = arg;
    if (!gfSysRecorder2::isEnable(2)) {
        task->unk48 = 1;
        task->unk50 = 4;
    } else {
        task->unk60 = gfSysRecorder2::createInstance(Heaps::MenuResource);
        if (task->unk60 == NULL) {
            task->unk50 = 5;
        } else {
            task->unk50 = 0;
        }
    }
    return task;
}

muWifiDataLoadTask::muWifiDataLoadTask()
    : gfTask(lbl_80459C28, Category_Menu, 15, 8, true), unk40(NULL), unk44(NULL), unk48(0), unk4C(NULL),
      unk50(0), unk54(6), unk58(1), unk5C(NULL), unk60(NULL), unk64(0), unk68(0), unk6C(false) {
    unk2C_b1 = false;
}

void muWifiDataLoadTask::exit() {
    if (unk60 != NULL) {
        gfSysRecorder2::dropInstance();
    }
    gfTask::exit();
}

muWifiDataLoadTask::~muWifiDataLoadTask() {}

int muWifiDataLoadTask::mainStepLoadMain(muWifiDataLoadTask* task) {
    gfSysRecorder2* recorder = task->unk60;
    if (recorder->m_busy == 0) {
        if (recorder->m_result == 0) {
            ntFriendInfo* info = ntFriendInfo::getInstance();
            info->setSaveData(recorder->m_handle.getBuffer(), recorder->m_handle.getSize());
            gfHeapManager::free(recorder->m_handle.getBuffer());
            if (task->unk6C) {
                task->unk48 = 0;
                return 4;
            }
            return 5;
        }
        return 1;
    }
    return task->unk50;
}

void muWifiDataLoadTask::mainStepRemakeConfirmInit(muWifiDataLoadTask* task) {
    if (task->unk5C == NULL) {
        task->unk5C = fn_800BEEB8(1, task->unk4C, 0x44, task->unk40, task->unk44, task->unk58,
                                             0, 0x2A, 0x2B, 0, 0, 0xF0);
    }
}

int muWifiDataLoadTask::mainStepRemakeConfirmMain(muWifiDataLoadTask* task) {
    muNoticeWndTask* wnd = task->unk5C;
    int result;
    if (wnd == NULL) {
        result = 0;
    } else if (wnd->m_step != 3) {
        result = 1;
    } else {
        result = 3;
        if (wnd->m_selected != 0) {
            result = 2;
        }
    }
    int step;
    switch (result) {
    case 1:
        step = task->unk50;
        break;
    case 2:
        step = 2;
        break;
    case 3:
        task->unk48 = 1;
        step = 4;
        break;
    }
    if (step != task->unk50) {
        if (task->unk5C != NULL) {
            task->unk5C->exit();
            task->unk5C = NULL;
        }
    }
    return step;
}
