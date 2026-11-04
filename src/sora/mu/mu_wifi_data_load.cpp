#include <gf/gf_heap_manager.h>
#include <gf/gf_sys_recorder2.h>
#include <mu/mu_notice_wnd.h>
#include <mu/wifi/mu_wifi_data_load_task.h>
#include <nt/nt_offline.h>
#include <sora/ms_message.h>
#include <stdio.h>
#include <string.h>
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

void muWifiDataLoadTask::mainStepRemakeInit(muWifiDataLoadTask* task) {
    task->m_recorder->requestRenew(Heaps::MenuResource, 2);
    if (task->m_unk5C == NULL) {
        task->m_unk5C = fn_800BEEB8(3, task->m_msgData, 0x45, task->m_resFile, task->m_scnGroup, task->m_priority, 0,
                                    Heaps::MenuInstance, Heaps::MenuResource, 0, 0, 0xF0);
    }
    task->m_unk64 = 30;
}

int muWifiDataLoadTask::mainStepRemakeMain(muWifiDataLoadTask* task) {
    gfSysRecorder2* recorder = task->m_recorder;
    int nextStep = task->m_step;
    if (recorder->m_busy == 0) {
        if (--task->m_unk64 <= 0) {
            switch (recorder->m_result) {
            case 0:
                task->m_unk6C = true;
                nextStep = 0;
                break;
            case 1:
                task->m_unk68 = 0;
                nextStep = 3;
                break;
            case 2:
                task->m_unk68 = 1;
                nextStep = 3;
                break;
            }
            if (task->m_unk5C != NULL) {
                task->m_unk5C->exit();
                task->m_unk5C = NULL;
            }
        }
    }
    return nextStep;
}

void muWifiDataLoadTask::mainStepRemakeErrorInit(muWifiDataLoadTask* task) {
    if (task->m_unk68 == 0) {
        if (task->m_unk5C == NULL) {
            task->m_unk5C = fn_800BEEB8(0, task->m_msgData, 0x2D, task->m_resFile, task->m_scnGroup, task->m_priority, 0,
                                        Heaps::MenuInstance, Heaps::MenuResource, 0, 0, 0xF0);
        }
    } else {
        int files;
        int blocks;
        task->m_recorder->getRequire(2, &files, &blocks, true);
        int blockCount = blocks;
        if (task->m_unk5C == NULL) {
            char text[0x100];
            u32 length;
            char* data;
            int size = Message::writeTagClear(text);
            Message::getPrintIndexData(task->m_msgData, 0x2E, &data, &length);
            memcpy(text + size, data, length);
            size += length;
            size += sprintf(text + size, "%d", blockCount);
            Message::getPrintIndexData(task->m_msgData, 0x2F, &data, &length);
            memcpy(text + size, data, length);
            size += length;
            text[size] = 0;
            task->m_unk5C = fn_800BF048(0, text, size + 1, task->m_resFile, task->m_scnGroup, task->m_priority, 0,
                                        Heaps::MenuInstance, Heaps::MenuResource, 0, 0, 0xF0);
        }
    }
}

int muWifiDataLoadTask::mainStepRemakeErrorMain(muWifiDataLoadTask* task) {
    int state;
    muNoticeWndTask* wnd = task->m_unk5C;
    if (wnd == NULL) {
        state = 0;
    } else if (wnd->m_step != 3) {
        state = 1;
    } else {
        state = 3;
        if (wnd->m_selected != 0) {
            state = 2;
        }
    }
    if (state != 1) {
        if (wnd != NULL) {
            wnd->exit();
            task->m_unk5C = NULL;
        }
        task->m_result = 1;
        return 4;
    }
    return task->m_step;
}

void muWifiDataLoadTask::mainStepDoneInit(muWifiDataLoadTask* task) {
    if (task->m_unk5C == NULL) {
        task->m_unk5C = fn_800BEEB8(0, task->m_msgData, task->m_result == 0 ? 0x46 : 0x31, task->m_resFile, task->m_scnGroup, task->m_priority, 0,
                                    Heaps::MenuInstance, Heaps::MenuResource, 0, 0, 0xF0);
    }
}

int muWifiDataLoadTask::mainStepDoneMain(muWifiDataLoadTask* task) {
    int state;
    muNoticeWndTask* wnd = task->m_unk5C;
    if (wnd == NULL) {
        state = 0;
    } else if (wnd->m_step != 3) {
        state = 1;
    } else {
        state = 3;
        if (wnd->m_selected != 0) {
            state = 2;
        }
    }
    if (state != 1) {
        if (wnd != NULL) {
            wnd->exit();
            task->m_unk5C = NULL;
        }
        return 5;
    }
    return task->m_step;
}
