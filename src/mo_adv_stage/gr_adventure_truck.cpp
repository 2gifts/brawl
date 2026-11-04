#include <ft/ft_manager.h>
#include <gr/gr_gimmick_motion_path.h>
#include <so/so_external_value_accesser.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_adventure_truck.h>
#include <sora_adv_stage/st_adventure2.h>

extern stAdventure2* lbl_40_bss_15C;

grAdventureTruck* grAdventureTruck::create(int mdlIndex, const char* taskName) {
    grAdventureTruck* ground = new (Heaps::StageInstance) grAdventureTruck(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureTruck::~grAdventureTruck() { }

void grAdventureTruck::setInitializeFlag() {
    if (g_stTriggerMng->getTriggerFlag(&m_data->m_validTrigger, true) == false) {
        if (m_data->m_validTrigger.m_isValid) {
            setValid(0);
        }
    } else if (!m_data->m_validTrigger.m_isValid) {
        setValid(0);
    }
}

void grAdventureTruck::setVisibilityByClipping(u32 visible) {
    if (m_resFile.ptr()) {
        Ground::setVisibilityByClipping(visible, unk154);
        m_isVisibleByClipping = visible;
    }
}

void grAdventureTruck::setVisibilityAttachedEffect(u32 visible) {
    if (visible == 1) {
        addSceneRoot(m_sceneModels[unk154], 0);
    } else {
        g_gfSceneRoot->remove(m_sceneModels[unk154]);
    }
    setVisibility(visible);
}

void grAdventureTruck::startTruck() {
    m_gimmickMotionPath->startMove();
    changeNodeAnim(0, 0);
    m_modelAnims[0]->setUpdateRate(m_data->unk0_float);
    Fighter* fighter = g_ftManager->getFighter(g_ftManager->getEntryId(0), -1);
    if (fighter != NULL) {
        int team = soExternalValueAccesser::getTeamNo(fighter);
        u32 owner = soExternalValueAccesser::getTeamOwnerId(fighter);
        grYakumono::setTeamYakumono(team, owner);
    }
    u32 nodeIndex = 0;
    getNodeIndex(&nodeIndex, 0, "FrontPosition");
    grYakumono::setAttackGimmick(0, 0, nodeIndex, &m_data->unk1EC, NULL);
    startGimmickSE(1);
    startGimmickSE(2);
    startGimmickSE(3);
    startGimmickSE(4);
    startGimmickSE(5);
    startGimmickSE(6);
    startGimmickSE(7);
    startGimmickSE(10);
    startGimmickEffect(0);
    startGimmickEffect(1);
    startGimmickEffect(2);
    startGimmickEffect(3);
    startGimmickEffect(4);
    g_stTriggerMng->setTriggerFlag(&m_data->unk270);
}

void grAdventureTruck::onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId) {
    Gimmick::EventKind kind = eventInfo->m_kind;
    s8 player = getPlayerNumber(taskId);
    if (kind == Gimmick::Event_Exit) {
        if (player == 0) {
            lbl_40_bss_15C->stopCameraAdvCameraOffset();
        }
        return;
    }
    if (kind <= 0x1B || kind >= 0x22) {
        return;
    }
    switch (kind) {
        case Gimmick::Truck_Event_In:
            if (unk158 == 0) {
                Vec3f pos = getPos();
                unk170 = 0;
                unk158 = 1;
                unk15C = 0.0f;
                if (player == 0) {
                    lbl_40_bss_15C->setFighterPos(1, &pos);
                }
                startGimmickSE(0);
                lbl_40_bss_15C->setCameraAdvCameraOffset(&m_data->unk274);
                static_cast<soGimmickTruckEventArgs*>(eventInfo)->m_pos = pos;
            }
            unk1C0++;
            break;
        case Gimmick::Truck_Event_Ride:
            break;
        case Gimmick::Truck_Event_Exit:
            lbl_40_bss_15C->stopCameraAdvCameraOffset();
            unk170 = 1;
            unk1C0--;
            break;
    }
}
