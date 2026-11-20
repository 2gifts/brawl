#include <so/collision/so_collision_hit_part.h>
#include <so/collision/so_collision_shield_part.h>
#include <cl/cl_collision.h>
#include <types.h>

soCollisionHitPart::soCollisionHitPart(soCollision::Category selfCategory, u32 opponentCategory) : m_clTargetArrayVector(6, 0) {
    {
        clTarget& target = m_clTargetArrayVector.at(0);
        target.m_0 = 2;
    }
    {
        clTarget& target = m_clTargetArrayVector.at(1);
        target.m_0 = 1 << selfCategory;
    }
    {
        clTarget& target = m_clTargetArrayVector.at(2);
        target.m_0 = 0;
    }
    {
        clTarget& target = m_clTargetArrayVector.at(3);
        target.m_0 = 0;
    }
    {
        clTarget& target = m_clTargetArrayVector.at(4);
        target.m_0 = 0;
    }
    m_clTargetArrayVector.at(1).m_4 = opponentCategory;
}

soCollisionHitPart::~soCollisionHitPart() { }

void soCollisionHitPart::reset(newCollision* collision) {
    collision->m_targets = &m_clTargetArrayVector;
    int status = m_hitData.m_0x1c_3;
    m_isUpdated = false;
    m_status = unk4 = status;
}

// NONMATCHING: the original loads all eight words before storing them (register order r10..r5, f0, r0).
void soCollisionHitPart::setData(soCollisionHitData* data) {
    m_hitData = *data;
    clTarget& target = m_clTargetArrayVector.at(3);
    target.m_0 = 1 << data->m_part;
}

void soCollisionHitPart::setCheckCatch(bool checkCatch) {
    if (checkCatch == true) {
        m_clTargetArrayVector.at(0).m_4 |= 0x40;
    } else {
        m_clTargetArrayVector.at(0).m_4 &= ~0x40;
    }
}

void soCollisionHitPart::setOpponentCategory(u32 categoryMask, bool enable) {
    if (enable == true) {
        m_clTargetArrayVector.at(0).m_4 |= categoryMask;
    } else {
        m_clTargetArrayVector.at(0).m_4 &= ~categoryMask;
    }
}

void soCollisionHitPart::setSelfCategory(soCollision::Category category) {
    clTarget& target = m_clTargetArrayVector.at(1);
    target.m_0 = 1 << category;
}

bool soCollisionHitPart::update(soModuleAccesser* moduleAccesser, soCollisionShieldPartUnit* unit, int minStatus, u32 situation,
                                u32 arg, u32 situationMask, float scale) {
    if (m_status != 3) {
        Vec3f start;
        start.m_x = m_hitData.m_startOffsetPos.m_x;
        start.m_y = m_hitData.m_startOffsetPos.m_y;
        start.m_z = m_hitData.m_startOffsetPos.m_z;
        Vec3f end;
        end.m_x = m_hitData.m_endOffsetPos.m_x;
        end.m_y = m_hitData.m_endOffsetPos.m_y;
        end.m_z = m_hitData.m_endOffsetPos.m_z;
        soCollisionUtil::updateCollision(moduleAccesser, unit, m_hitData.m_shapeType, !m_isUpdated, m_hitData.m_nodeIndex,
                                         &start, &end, arg, false, m_hitData.m_size, scale);
        {
            clTarget& target = m_clTargetArrayVector.at(2);
            target.m_0 = (1 << situation) | situationMask;
        }
        if (m_status > minStatus) {
            minStatus = m_status;
        }
        {
            clTarget& target = m_clTargetArrayVector.at(4);
            target.m_0 = 1 << minStatus;
        }
        unit->m_isUpdated = true;
        m_isUpdated = true;
        return true;
    }
    m_isUpdated = false;
    unit->m_isUpdated = false;
    return false;
}

int soCollisionHitPart::getNodeIndex() {
    if (m_status != 3) {
        return m_hitData.m_nodeIndex;
    }
    return -1;
}
