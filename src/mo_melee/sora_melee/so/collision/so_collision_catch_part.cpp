#include <so/collision/so_collision_shield_part.h>
#include <so/collision/templates/so_collision_catch_part.h>

void soCollisionCatchPart::reset() {
    unk0 = 0;
}

void soCollisionCatchPart::set(const soCollisionCatchData* data, soCollisionShieldPartUnit* unit) {
    m_data.m_offset0 = data->m_offset0;
    m_data.m_offset1 = data->m_offset1;
    m_data.m_offset2 = data->m_offset2;
    m_data.m_size = data->m_size;
    m_data.unk10 = data->unk10;
    m_data.unk14 = data->unk14;
    unk0 = 1;
    clTarget& target0 = m_targets.at(0);
    target0.m_4 = 2;
    clTarget& target1 = m_targets.at(1);
    target1.m_4 = (m_data.unk10 >> 4) & 0x3FF;
    clTarget& target2 = m_targets.at(2);
    target2.m_4 = (m_data.unk10 >> 1) & 7;
    clTarget& target3 = m_targets.at(3);
    target3.m_4 = m_data.unk14_0;
    clTarget& target4 = m_targets.at(4);
    target4.m_4 = 1;
    unit->m_targetArray = &m_targets;
}
