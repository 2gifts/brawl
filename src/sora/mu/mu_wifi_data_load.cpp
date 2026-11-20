#include <gf/gf_task.h>
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

// TODO: clean up, give variables real names
namespace jj_800DACB0 {
extern "C" {
void loadWifi__14gfSysRecorder2FQ25Heaps8HeapType(void* a, int b);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void fn_800DACB0(void* p)
{
    loadWifi__14gfSysRecorder2FQ25Heaps8HeapType(*(void**)((char*)p + 0x60), 0x2b);
}
}
}

namespace jj_800DAD80 {
extern "C" {
int fn_800BEEB8(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12);
}

struct Fn5CObj {
    u8 pad0[0x40];
    int m40;
    int m44;
    int m48;
    int m4C;
    int m50;
    int m54;
    int m58;
    int m5C;
};

// TODO: clean up, give variables real names
extern "C" void fn_800DAD80(Fn5CObj* self) {
    if (self->m5C == 0) {
        self->m5C = fn_800BEEB8(1, self->m4C, 0x44, self->m40, self->m44, self->m58, 0, 0x2A, 0x2B, 0, 0, 0xF0);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_800DAC20 {
struct Menu {
    unsigned char padding[0x50];
    union {
        int state;
        unsigned int stateIndex;
    };
    int previousState;
};

typedef void (*EnterFn)(Menu*);
typedef int (*UpdateFn)(Menu*);
struct StateEntry {
    EnterFn enter;
    UpdateFn update;
};

extern "C" {
extern const StateEntry lbl_80408550[];

// TODO: clean up, give variables real names
void fn_800DAC20(Menu* self) {
    if (self->previousState != self->state) {
        if (lbl_80408550[self->stateIndex].enter)
            lbl_80408550[self->stateIndex].enter(self);
        self->previousState = self->state;
    }
    if (lbl_80408550[self->stateIndex].update)
        self->state = lbl_80408550[self->stateIndex].update(self);
}
}
}

// TODO: clean up, give variables real names
namespace jj_800DADF8 {
struct Task : gfTask {
    char pad[0xc0];
    unsigned char flag;
    char pad2[3];
    int mode;
};
struct State {
    char pad[0x48];
    int field48;
    char pad2[4];
    int result;
    char pad3[8];
    Task* task;
};
// TODO: clean up, give variables real names
extern "C" int fn_800DADF8(State* self) {
    int result;
    int status;
    Task* task = self->task;
    if (task == 0) {
        status = 0;
    } else if (task->mode != 3) {
        status = 1;
    } else {
        status = 3;
        if (task->flag != 0) status = 2;
    }
    switch (status) {
    case 1:
        result = self->result;
        break;
    case 2:
        result = 2;
        break;
    case 3:
        result = 4;
        self->field48 = 1;
        break;
    }
    if (result != self->result) {
        if (self->task != 0) {
            self->task->exit();
            self->task = 0;
        }
    }
    return result;
}
}

