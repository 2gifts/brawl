#include <gr/gr_gimmick_motion_path.h>
#include <mt/mt_vector.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_adventure_move_circle_attack_ground.h>

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
