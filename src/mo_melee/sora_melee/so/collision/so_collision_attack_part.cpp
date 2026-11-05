#include <so/collision/so_collision_attack_part.h>
#include <cl/cl_collision.h>

void soCollisionAttackPart::reset() {
    m_status = Status_Inactive;
}

void soCollisionAttackPart::resetStatus() {
    m_status = Status_Set;
}

void soCollisionAttackPart::setTargetCategory(soCollision::CategoryMask categoryMask) {
    m_attackData.m_targetCategory = categoryMask;
    m_clTargetArrayVector.at(1).m_4 = categoryMask;
}

void soCollisionAttackPart::set(newCollision* collision) {
    if (m_status == Status_Inactive) {
        m_status = Status_Set;
    }
    {
        clTarget& target = m_clTargetArrayVector.at(0);
        target.m_4 = 0x22;
    }
    if (m_attackData.m_setOffKind >> 1) {
        m_clTargetArrayVector.at(0).m_4 |= 1;
    }
    if (m_attackData.m_isShieldable) {
        m_clTargetArrayVector.at(0).m_4 |= 4;
    }
    {
        clTarget& target = m_clTargetArrayVector.at(5);
        target.m_0 = 1;
    }
    if (m_attackData.m_isReflectable) {
        m_clTargetArrayVector.at(0).m_4 |= 8;
        m_clTargetArrayVector.at(5).m_0 |= 2;
    }
    if (m_attackData.m_isAbsorbable) {
        m_clTargetArrayVector.at(0).m_4 |= 0x10;
        m_clTargetArrayVector.at(5).m_0 |= 4;
    }
    {
        clTarget& target = m_clTargetArrayVector.at(1);
        target.m_4 = m_attackData.m_targetCategory;
    }
    {
        clTarget& target = m_clTargetArrayVector.at(2);
        target.m_4 = m_attackData.m_targetSituation;
    }
    {
        clTarget& target = m_clTargetArrayVector.at(3);
        target.m_4 = m_attackData.m_targetPart;
    }
    {
        clTarget& target = m_clTargetArrayVector.at(4);
        target.m_4 = 3;
    }
    if (m_attackData.m_isInvalidXlu) {
        m_clTargetArrayVector.at(4).m_4 |= 4;
    }
    collision->m_targets = &m_clTargetArrayVector;
    collision->m_isInfinite = false;
}
