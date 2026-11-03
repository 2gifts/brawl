#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_adv.h>
#include <snd/snd_system.h>
#include <sora_adv_stage/gr_adventure_door.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/st_adventure2.h>

extern stAdventure2* lbl_40_bss_15C;

void grAdventureDoor::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    switch (unk150State) {
    case 0:
        unk154 = 0.0f;
        break;
    case 1:
        if (unk154 > unk15C) {
            unk150State = 2;
        }
        if (m_data->unk44 != 5) {
            cmAdventureController* camera = dynamic_cast<cmAdventureController*>(CameraController::getInstance()->m_currentController);
            if (camera != NULL && camera->unk9 == 0) {
                float distance;
                camera->unkE8 = Vec2f(0.0f, 0.0f);
                distance = camera->unkDC;
                camera->unkDC = distance - 1.0f;
            }
        }
        unk154 += deltaFrame;
        break;
    case 2:
        lbl_40_bss_15C->requestStepJump(unk164Data);
        disableArea();
        unk150State = 0;
        break;
    }
    grGimmick::updateCallback(0);
}

grAdventureDoor::grAdventureDoor(const char* taskName) : grYakumono(taskName) {
    unk150State = 0;
    unk154 = 0.0f;
    unk158 = 0.0f;
    unk15C = 50.0f;
    m_data = NULL;
    unk164Data = 0;
    unk168 = 40;
    unk194 = 0;
    unk198 = 0;
}

grAdventureDoor* grAdventureDoor::create(int mdlIndex, u32 jumpData, const char* taskName) {
    grAdventureDoor* door = new (Heaps::StageInstance) grAdventureDoor(taskName);
    door->setJumpData(jumpData);
    if (door != NULL) {
        door->setMdlIndex(mdlIndex);
    }
    return door;
}

void grAdventureDoor::openTheDoor() {
    u32 hasAnimation = m_modelAnims[0]->m_resFile.GetResAnmChrNumEntries() != 0;
    if (hasAnimation == 1) {
        grGimmick::changeNodeAnim(0, 0);
    }
    if (m_data->unk37 != 0) {
        EachDoorTypeEffect();
    }
}

void grAdventureDoor::EachDoorTypeEffect() {
    if (m_data->unk37 != 0) {
        if (m_data->unk48 > 0) {
            g_sndSystem->playSE(static_cast<SndID>(m_data->unk48), -1, 0, 0, -1);
        } else {
            switch (m_data->unk44) {
            case 0:
                g_sndSystem->playSE(static_cast<SndID>(0x2262), -1, 0, 0, -1);
                break;
            case 1:
                g_sndSystem->playSE(static_cast<SndID>(0x2289), -1, 0, 0, -1);
                break;
            case 2:
                g_sndSystem->playSE(static_cast<SndID>(0x228A), -1, 0, 0, -1);
                break;
            case 3:
                g_sndSystem->playSE(static_cast<SndID>(0x228B), -1, 0, 0, -1);
                break;
            case 4:
                g_sndSystem->playSE(static_cast<SndID>(0x228C), -1, 0, 0, -1);
                break;
            }
        }
    }
    switch (m_data->unk44) {
    case 5:
        startGimmickEffect(0);
        break;
    }
}

grAdventureDoor::~grAdventureDoor() { }

void grAdventureDoor::setInitializeFlag() {
    if (g_stTriggerMng->getTriggerFlag(&m_data->m_validTrigger, true) == false) {
        if (m_data->m_validTrigger.m_isValid) {
            setValid(0);
        }
    } else if (!m_data->m_validTrigger.m_isValid) {
        setValid(0);
    }
}
