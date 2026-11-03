#include <types.h>

#include <cm/cm_camera_controller.h>
#include <gf/gf_camera.h>
#include <gf/gf_camera_controller.h>
#include <gf/gf_homemenu.h>
#include <gf/gf_sys_recorder2.h>
#include <gm/gm_global.h>
#include <memory.h>

#include <mu/mu_delete_nand_task.h>

// The heap of the task and of gfSysRecorder2.
static Heaps::HeapType s_heap = Heaps::Invalid;

static muDeleteNandTask* s_instance;
static gfSysRecorder2* s_recorder;
static int s_step;
// HYPOTHESIS: counts the frames of Step_Wait; nothing reads it.
static int s_frame;
static int s_kind;
// GameGlobal::getEnableSaveLoad when the task started: 0 means the save data is not written at all.
static u8 s_enableSaveLoad;
// HYPOTHESIS: why the write failed (0: it did not, 1: saving is off, 2: no gfSysRecorder2, 3: no buffer, 4: the
// request was refused, 5..7: gfSysRecorder2 failed); nothing in this module reads it.
static int s_error;

muDeleteNandTask::~muDeleteNandTask() {
    s_instance = NULL;
}

muDeleteNandTask* muDeleteNandTask::create(HeapType heap, int kind) {
    if (s_instance != NULL) {
        return s_instance;
    }
    s_heap = heap;
    s_instance = new (heap) muDeleteNandTask();
    s_kind = kind;
    s_instance->init();
    return s_instance;
}

void muDeleteNandTask::init() {
    s_step = Step_Start;
    s_frame = 0;
    s_error = 0;
    s_enableSaveLoad = g_GameGlobal->getEnableSaveLoad();
}

// Has gfSysRecorder2 renew one kind of its save data.
// HYPOTHESIS: an inline helper of the unit (the same code is repeated for each kind, each with its own return value).
static inline bool requestRenew(int kind) {
    if (!gfSysRecorder2::isEnable(0)) {
        s_error = 1;
        return false;
    }
    s_recorder = gfSysRecorder2::createInstance(s_heap);
    if (s_recorder == NULL) {
        s_error = 2;
        return false;
    }
    if (!s_recorder->requestRenew(s_heap, kind)) {
        s_error = 4;
        return false;
    }
    return true;
}

void muDeleteNandTask::processDefault() {
    switch (s_step) {
    case Step_Start:
        s_frame = 0;
        s_step = Step_Reset;
        break;
    case Step_Reset:
        switch (s_kind) {
        case Kind_HiScore:
            g_GameGlobal->InitHiScore();
            break;
        case Kind_BattleRecord:
            g_GameGlobal->InitBattleRecord();
            break;
        case Kind_All: {
            g_GameGlobal->InitAllRecord();
            // The screen setting is reset too, so the camera follows it
            float aspect = g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen == 0 ? 1.35f : 1.73232f;
            CameraController::getInstance()->m_currentController->getCameraManager()->getCamera(0)->m_aspect = aspect;
            break;
        }
        }
        s_step = s_enableSaveLoad == 0 ? Step_Done : Step_BanHome;
        break;
    case Step_BanHome:
        if (g_gfHomeMenu->setBan(1, 0) == true) {
            s_step = Step_Request;
        }
        break;
    case Step_Request: {
        bool requested = false;
        switch (s_kind) {
        case Kind_HiScore:
        case Kind_BattleRecord:
            requested = setupSysSave();
            break;
        case Kind_Adventure:
            requested = requestRenew(1);
            break;
        case Kind_Unk3:
            requested = requestRenew(3);
            break;
        case Kind_All:
            requested = requestRenew(8);
            break;
        }
        s_step = requested == true ? Step_Wait : Step_Finish;
        break;
    }
    case Step_Wait:
        s_frame++;
        if (s_recorder->m_busy == 0) {
            s_step = Step_Result;
        }
        break;
    case Step_Result:
        switch (s_recorder->m_result) {
        case 3:
        default:
            s_error = 5;
            break;
        case 1:
            s_error = 6;
            break;
        case 2:
            s_error = 7;
            break;
        case 0:
            s_error = 0;
            break;
        }
        s_step = Step_Finish;
        break;
    case Step_Finish:
        if (s_recorder != NULL) {
            gfSysRecorder2::dropInstance();
            s_recorder = NULL;
        }
        g_gfHomeMenu->setBan(0, 0);
        s_step = Step_Done;
        break;
    case Step_Done:
        break;
    }
}

bool muDeleteNandTask::setupSysSave() {
    if (!gfSysRecorder2::isEnable(0)) {
        s_error = 1;
        return false;
    }
    s_recorder = gfSysRecorder2::createInstance(s_heap);
    if (s_recorder == NULL) {
        s_error = 2;
        return false;
    }
    void* buffer = s_recorder->getBuffer(0);
    if (buffer == NULL) {
        s_error = 3;
        return false;
    }
    g_GameGlobal->getSaveData(buffer);
    if (!s_recorder->saveSystem()) {
        s_error = 4;
        return false;
    }
    return true;
}

int muDeleteNandTask::getStep() {
    return s_step;
}
