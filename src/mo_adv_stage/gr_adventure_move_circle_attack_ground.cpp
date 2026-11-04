#include <gr/gr_gimmick_motion_path.h>
#include <mt/mt_vector.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_adventure_move_circle_attack_ground.h>
#include <gf/gf_model.h>
#include <gm/gm_global.h>
#include <sora_adv_stage/gr_adventure_move_ground.h>

extern grGimmickMotionPathInfo lbl_40_data_88F8;
extern char lbl_40_data_88E8[16];

void grAdventureMoveCircleAttackGround::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grGimmick::startup(data, unk1, layerType);
    unk164Path = static_cast<grAdventureMoveGroundData*>(m_gimmickData);
    unk164Path->m_motionPathData.m_motionRatio = unk164Path->m_motionRatios[g_GameGlobal->m_advData->m_difficultyLevel];
    if (m_modelAnims != NULL) {
        m_modelAnims[0]->unbindNodeAnim(m_sceneModels[0]);
    }
    makeCalcuCallback(1, Heaps::StageInstance);
    setCalcuCallbackRoot(3);
    grGimmickMotionPathInfo motionPathInfo = lbl_40_data_88F8;
    motionPathInfo.m_archive = data;
    motionPathInfo.m_motionPathData = &unk164Path->m_motionPathData;
    motionPathInfo.m_0xc = unk164Path->m_defaultSE;
    motionPathInfo.m_0x10 = unk164Path->m_moveSE;
    motionPathInfo.m_0x14 = unk164Path->m_endSE;
    motionPathInfo.m_0x18 = unk164Path->m_defaultEffect;
    motionPathInfo.m_0x1c = unk164Path->m_moveEffect;
    motionPathInfo.m_0x20 = unk164Path->m_endEffect;
    createAttachMotionPath(&motionPathInfo, &unk164->unk1E4, lbl_40_data_88E8);
    Vec2f pos(getPos());
    float radius = unk164->unk24C;
    unk168 = addHitCircle(&pos, unk164->unk248, 0, radius);
    enableHitCircle(1, 0, unk168);
    if (unk164->unk1E4.m_triggerId != 0 && unk164->unk1E4.m_isValid) {
        enableHitCircle(0, 0, unk168);
    }
    stTrigger* trigger = g_stTriggerMng->createTrigger(Gimmick::Area_None, &unk164->unk1E4);
    createGimmickStatusTrigger(this, trigger, &unk164->unk1E4, 1, 0);
}

grAdventureMoveCircleAttackGround* grAdventureMoveCircleAttackGround::create(int mdlIndex, const char* taskName) {
    grAdventureMoveCircleAttackGround* ground = new (Heaps::StageInstance) grAdventureMoveCircleAttackGround(taskName);
    ground->setMdlIndex(mdlIndex);
    return ground;
}

void grAdventureMoveCircleAttackGround::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    grGimmick::updateCallback(0);
    Vec2f pos(getPos());
    setHitCirclePos(&pos, 0, unk168);
}

void grAdventureMoveCircleAttackGround::setInitializeFlag() {
    if (g_stTriggerMng->getTriggerFlag(&unk164->unk1E0, true) == false) {
        if (unk164->unk1E0.m_isValid) {
            setValid(0);
        }
    } else if (!unk164->unk1E0.m_isValid) {
        setValid(0);
    }
    if (g_stTriggerMng->getTriggerFlag(&unk164->unk1E4, false) == true) {
        m_gimmickMotionPath->setFrame(m_gimmickMotionPath->m_frameCount);
        m_gimmickMotionPath->setFrameModelAnim();
    }
}

void grAdventureMoveCircleAttackGround::notifyEventOnTrigger(int flag) {
    enableHitCircle(1, 0, unk168);
}

grAdventureMoveCircleAttackGround::~grAdventureMoveCircleAttackGround() { }
