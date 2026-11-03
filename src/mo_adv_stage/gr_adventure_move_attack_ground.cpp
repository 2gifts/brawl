#include <gr/gr_gimmick_motion_path.h>
#include <sora_adv_stage/gr_adventure_move_attack_ground.h>

grAdventureMoveAttackGround* grAdventureMoveAttackGround::create(int mdlIndex, const char* taskName) {
    grAdventureMoveAttackGround* ground = new (Heaps::StageInstance) grAdventureMoveAttackGround(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureMoveAttackGround::grAdventureMoveAttackGround(const char* taskName) : grAdventureMoveGround(taskName) {
    m_effectHandle = 0;
    m_moveAttackData = NULL;
    // HYPOTHESIS: unk15C is ykData.
    reinterpret_cast<ykData*>(unk15C)->m_dataGroupNum = 0;
    reinterpret_cast<ykData*>(unk15C)->m_dataGroups = NULL;
    m_effectId = static_cast<EfID>(0);
    m_transparencySettings = 0x1F;
    unkF8 = 50.0f;
}

grAdventureMoveAttackGround::~grAdventureMoveAttackGround() { }

void grAdventureMoveAttackGround::setAttackMoveGround() {
    grGimmickMoveAttackGroundData* data = static_cast<grGimmickMoveAttackGroundData*>(m_moveAttackData);
    grYakumono::setAttackGimmick(0, 0, data->m_attackData.m_nodeIndex, &data->m_attackData, NULL);
}

void grAdventureMoveAttackGround::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_effectHandle != 0) {
        ecMgr::getInstance()->setPos(m_effectHandle, &getPos());
        ecMgr::getInstance()->setRot(m_effectHandle, &getRot());
    }
    if (!m_gimmickMotionPath->isMoveEnd()) {
        // HYPOTHESIS: unk0[5] is grGimmickMotionPathData::m_pathMode (Path_Loop == 1).
        if (static_cast<grGimmickMoveAttackGroundData*>(m_moveAttackData)->unk0[5] == 1) {
            if (m_gimmickMotionPath->isStartFrame()) {
                setValid(1);
            }
            if (m_gimmickMotionPath->isEndFrame()) {
                setValid(0);
            }
        }
    }
    grGimmick::updateCallback(0);
}

void grAdventureMoveAttackGround::setEffect(EfID effectId) {
    m_effectId = effectId;
    m_effectHandle = g_ecMgr->setEffect(effectId);
}

void grAdventureMoveAttackGround::setValid(u32 unk1) {
    grYakumono::setValid(unk1);
    if (unk1 == 1) {
        grYakumono::disableAttack(0);
        grYakumono::setAttackGimmick(0, 0, 0, &static_cast<grGimmickMoveAttackGroundData*>(m_moveAttackData)->m_attackData,
                                     NULL);
    }
}

void grAdventureMoveAttackGround::setValidAttachedEffect(u32 unk1) {
    grYakumono::setValidAttachedEffect(unk1);
    if (unk1 == 1) {
        grYakumono::disableAttack(0);
        grYakumono::setAttackGimmick(0, 0, 0, &static_cast<grGimmickMoveAttackGroundData*>(m_moveAttackData)->m_attackData,
                                     NULL);
    }
}

void grAdventureMoveAttackGround::setVisibilityByClipping(u32 unk1) {
    Ground::setVisibilityByClipping(unk1, 0);
    if (unk1 != 0 && !m_isVisibleByClipping) {
        if (m_effectId != 0) {
            m_effectHandle = g_ecMgr->setEffect(m_effectId);
        }
    } else if (unk1 == 0 && m_isVisibleByClipping) {
        if (m_effectHandle != 0) {
            g_ecMgr->endEffect(m_effectHandle);
        }
    }
    m_isVisibleByClipping = unk1;
}
