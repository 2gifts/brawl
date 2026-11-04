#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_adventure_move_attack_trigger_once.h>

grAdventureMoveAttackTriggerOnce* grAdventureMoveAttackTriggerOnce::create(int mdlIndex, const char* taskName) {
    grAdventureMoveAttackTriggerOnce* ground = new (Heaps::StageInstance) grAdventureMoveAttackTriggerOnce(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

void grAdventureMoveAttackTriggerOnce::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grAdventureMoveAttackGround::startup(data, unk1, layerType);
    grYakumono::disableAttack(0);
    grYakumono::setTeamYakumono(0x11, -1);
}

grAdventureMoveAttackTriggerOnce::~grAdventureMoveAttackTriggerOnce() { }

void grAdventureMoveAttackTriggerOnce::frowStandby(float deltaFrame) { }

void grAdventureMoveAttackTriggerOnce::frowStart(float deltaFrame) {
    grYakumono::setAttackGimmick(0, 0, 0, &static_cast<grGimmickMoveAttackTriggerOnceData*>(m_moveAttackData)->m_attackData, NULL);
    m_state = 2;
}

void grAdventureMoveAttackTriggerOnce::frowMoving(float deltaFrame) {
    if (m_gimmickMotionPath != NULL && m_gimmickMotionPath->isMoveEnd()) {
        m_state = 3;
    }
}

void grAdventureMoveAttackTriggerOnce::frowStopped(float deltaFrame) {
    grYakumono::disableAttack(0);
}
