#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_photo.h>
#include <gf/gf_pad_system.h>
#include <gf/gf_task_scheduler.h>
#include <if/if_photo.h>
#include <if/if_snap_save.h>
#include <memory.h>
#include <mu/mu_adv_name.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// IfSnapSaveTask::getState (map name; still unnamed in if_snap_save.cpp)
extern "C" s32 fn_800F2894(IfSnapSaveTask* task);

// HYPOTHESIS: model records share the menu model table layout.
extern const muAdvNameModel lbl_805A2590[1];
extern const muAdvNameModel lbl_80408B98[5];
extern const muAdvNameModel lbl_80408BC0[5];

void IfPhoto::createModel(nw4r::g3d::ResFile* modelFile, nw4r::g3d::ResFile* resFile,
                          void* msgData, nw4r::g3d::ResFile* resFile2, u32 helpFlags) {
    init();
    m_resFile = *resFile;
    m_msgData = msgData;
    m_resFile2 = *resFile2;
    createModelByTbl(modelFile, lbl_805A2590, 1);
    m_helpFlags = helpFlags;
    if (helpFlags & 3) {
        createModelByTbl(modelFile, lbl_80408B98, 5);
        if (helpFlags & 2) {
            for (int index = 1; index < 6; index++) {
                m_objects[index]->setFrameVisible(1.0f);
            }
        }
    }
    if (helpFlags & 4) {
        createModelByTbl(modelFile, lbl_80408BC0, 5);
    }
    initMsg();
}

void IfPhoto::createModelByTbl(nw4r::g3d::ResFile* modelFile, const void* table, int count) {
    int copies;
    int index;
    int copy;
    const muAdvNameModel* entry = static_cast<const muAdvNameModel*>(table);
    for (index = 0; index < count; index++, entry++) {
        copies = 1;
        if (entry->m_start < entry->m_end) {
            copies = entry->m_end - entry->m_start;
        }
        for (copy = 0; copy < copies; copy++) {
            MuObject* object = MuObject::create(modelFile, entry->m_name, entry->m_priority,
                                               NULL, Heaps::MenuInstance);
            m_objects[entry->m_start + copy] = object;
            object->changeAnimN(entry->m_name);
            m_objects[entry->m_start + copy]->m_modelAnim->setUpdateRate(0.0f);
        }
    }
}

IfPhoto::IfPhoto() {
    init();
}

void IfPhoto::init() {
    m_scnGroup = NULL;
    for (int i = 0; i < 11; i++) {
        m_objects[i] = NULL;
    }
    m_msg = NULL;
    m_step = 0;
    unk38 = 0;
    unk3C = 0;
    unk40 = 0;
    unk44 = 0;
    m_padNo = 0;
    unk4C = 0;
    m_cameraInput = 2;
    m_resFile = nw4r::g3d::ResFile();
    m_msgData = NULL;
    m_snapSaveTask = NULL;
    m_state = 0;
    unk60 = 0;
    unk64 = 1;
    m_resFile2 = nw4r::g3d::ResFile();
    m_helpFlags = 0;
}

void IfPhoto::startMelee(nw4r::g3d::ScnGroup* scnGroup) {
    m_scnGroup = scnGroup;
}

CameraController* IfPhoto::getCameraController() {
    return static_cast<CameraController*>(gfTaskScheduler::getInstance()->getTask("Camera"));
}

gfPadStatus IfPhoto::getPadStatus() {
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(m_padNo, &status);
    return status;
}

void IfPhoto::removeAllObj() {
    nw4r::g3d::ScnGroup* group = m_scnGroup;
    while (group->sceneItemsCount != 0) {
        // nw4r ScnGroup::PopBack
        if (group->sceneItemsCount != 0) {
            group->Remove((u32)(group->sceneItemsCount - 1));
        }
    }
}

bool IfPhoto::haveSnapHelp() {
    return m_helpFlags & 1;
}

bool IfPhoto::haveRetryHelp() {
    return (m_helpFlags >> 1) & 1;
}

bool IfPhoto::haveResetHelp() {
    return (m_helpFlags >> 2) & 1;
}

bool IfPhoto::getSaveEnable(IfSnapSaveMedia* media, u32 size) {
    bool enable = false;
    if (media->m_status == 0 && media->m_free >= size) {
        enable = true;
    }
    return enable;
}

void IfPhoto::photoStepSnapSaveInit() {
    m_snapSaveTask = IfSnapSaveTask::create(&m_resFile, &m_resFile2, m_msgData, m_scnGroup, 0xC4,
            Heaps::MenuInstance, Heaps::MenuInstance, Heaps::Network);
}

s32 IfPhoto::photoStepSnapSaveMain() {
    if (fn_800F2894(m_snapSaveTask) != 0) {
        m_snapSaveTask->exit();
        m_snapSaveTask = NULL;
        return 1;
    }
    return m_step;
}

void IfPhoto::exitPause() {
    if (m_snapSaveTask) {
        m_snapSaveTask->exit();
        m_snapSaveTask = NULL;
    }
    CameraController* camera = getCameraController();
    cmPhotoController* photo = dynamic_cast<cmPhotoController*>(camera->getCameraController(CameraController::Photo));
    if (photo->unk60 <= 0) {
        camera->changeInput(static_cast<CameraController::Kind>(m_cameraInput));
    }
}

void cmPhotoController::clearControl() {
    memset(unk14, 0, sizeof(unk14));
}

void IfPhoto::getMediaState(IfSnapSaveMedia* media, int kind) {
    // HYPOTHESIS: the nonzero media kind leaves the free-size value unset.
    int status;
    u32 size;
    if (kind == 0) {
        size = 0x1000000;
        status = 0;
    } else {
        status = 1;
    }
    switch (status) {
    case 0:
        size >>= 17;
        status = 0;
        break;
    }
    media->m_status = status;
    media->m_free = size;
}
