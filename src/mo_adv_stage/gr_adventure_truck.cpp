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

#include <gr/gr_gimmick.h>
#include <gf/gf_3d_scene.h>
// TODO: clean up, give variables real names
namespace jj_2D248 {
extern "C" const float lbl_40_rodata_4F0[];
struct Base { char pad[0x3c]; };
class grYakumono : public Base {
public:
#define V(x) virtual void x();
V(processDefault) V(processBegin) V(processAnim) V(processUpdate) V(processPreMapCorrection) V(processMapCorrection) V(fn_40_2D3A8) V(processPreCollision) V(processCollision) V(processCatch)
V(processHit) V(processCamera) V(processFixCamera) V(processEffect) V(processGameProc) V(processEnd) V(renderPre) V(renderOpa) V(renderXlu) V(processDebug)
V(renderDebug) V(init) V(__dt__16grAdventureTruckFv) V(fn_40_2D248) V(loadGroundData) V(bindData) V(setupMelee) V(setVisibility) V(setVisibilityByClipping__16grAdventureTruckFUl) V(setVisibilityAttachedEffect__16grAdventureTruckFUl)
V(receiveCollMsg) V(receiveCollMsg_Landing) V(receiveCollMsg_Heading) V(receiveCollMsg_Wall) V(receiveCollMsg_Attack) V(unloadData) V(getModelCount) V(fn_40_2C760) V(setNode) V(setStageData)
V(getStageData) V(initStageData) V(setMdlIndex) V(getMdlIndex) V(isEnableCalcCollision) V(enableCalcCollision) V(disableCalcCollision) V(getNodeIndex) V(getNodePosition) V(getNodePosition_2)
V(getNodeMatrix) V(getNodeMatrix_2) V(setNodeVisibility) V(setNodeVisibility_2) V(setNodeVisibilityAll) V(isNodeVisible) V(setNodeCollision) V(setNodeCollision_2) V(getNodeScale) V(getNodeScale_2)
V(setValid) V(setValidAttachedEffect) V(setInitializeFlag__16grAdventureTruckFv) V(setInitializeInfo) V(getInitializeInfo) V(setMotionRatio) V(setMotionFrame) V(getMotionFrame) V(setMotionLoop) V(setMatAlphaMul)
V(setMatAlpha) V(updateG3dProcCalcWorld) V(preExit) V(adventureEventGetItem) V(invalidatedByCameraClipping) V(setTransparencyFlag) V(updateCallback) V(fixedPosition) V(setTgtNode) V(getTgtNode)
V(setGimmickData) V(getGimmickData) V(changeNodeAnim) V(createFadeVisibleProduction) V(createSoundEffectVisibleProductionForExcel) V(createEffectVisibleProductionForExcel) V(setSimpleEffectVisibleProduction) V(dbDispInvalidatedByCameraClippingSphere) V(setTransparency) V(getTransparencyFlag)
V(onDamage) V(onInflict) V(onInflictEach) V(onGimmickEvent__16grAdventureTruckFP18soGimmickEventArgsPi) V(enableYakumono) V(disableYakumono) V(enableHit) V(disableHit)
virtual void disableAttack(unsigned long);
#undef V
};
extern "C" void fn_40_2D248(grYakumono* obj, float delta) {
    unsigned char* s = (unsigned char*)obj;
    const float* k = lbl_40_rodata_4F0;
    ((grGimmick*)obj)->grGimmick::update(delta);
    if (s[0x170]) *(float*)(s+0x160) -= k[13];
    else *(float*)(s+0x160) += k[13];
    if (*(float*)(s+0x160) < k[11]) {
        if (s[0x1c0]) {
            ((grGimmick*)obj)->startGimmickSE(0);
            s[0x170] = 0;
        }
        *(float*)(s+0x160) = k[11];
    }
    if (*(float*)(s+0x160) > k[0]) *(float*)(s+0x160) = k[0];
    unsigned char* p = *(unsigned char**)(s+0xd4);
    *(float*)(p+0x94) = *(float*)(s+0x160);
    *(float*)(p+0x98) = *(float*)(s+0x164);
    *(float*)(p+0x9c) = *(float*)(s+0x168);
    if (s[0x158] == 3) {
        obj->disableAttack(0);
        *(unsigned long*)(s+0x154) = 1;
        g_gfSceneRoot->remove(*(nw4r::g3d::ScnMdl**)(*(char**)(s+0x44)));
        *(unsigned long*)(*(char**)(s+0xd4)+0x84) = 0;
        g_gfSceneRoot->add(gfSceneRoot::Layer_Ground, *(nw4r::g3d::ScnMdl**)(*(char**)(s+0x44)+4));
        ((grGimmick*)obj)->startGimmickSE(8);
        ((grGimmick*)obj)->startGimmickEffect(5);
        s[0x158] = 4;
    }
    ((grGimmick*)obj)->grGimmick::updateCallback(*(unsigned long*)(s+0x154));
}
}

