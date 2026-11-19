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

#include <gr/gr_gimmick.h>
// TODO: clean up, give variables real names
namespace jj_31E1C {


typedef void (grGimmick::*Handler)(float);

struct Table {
    Handler entries[4];
};

extern "C" const Table lbl_40_rodata_5F8;

extern "C" void fn_40_31E1C(grGimmick* self, float delta) {
    unsigned char* child = *(unsigned char**)((char*)self + 0xCC);
    if (child != 0 && (int)child[0x14C] == 1) {
        *(int*)((char*)self + 0x168) = 1;
    }

    Table table = lbl_40_rodata_5F8;
    (self->*table.entries[*(int*)((char*)self + 0x168)])(delta);
    self->grGimmick::update(delta);
    self->grGimmick::updateCallback(0);
}
}

