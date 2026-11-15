#pragma once

#include <ft/ft_common_data_accesser.h>
#include <so/collision/so_collision_shield_group.h>

// Lucario's record of the fighter data: the common part and the fields the Lucario module reads after it.
// HYPOTHESIS: member names (from their uses in ft_lucario.cpp); the two article data pointers that follow (aura ball
// 0xA0, Aura Sphere 0xA4) are read by ftCommonDataAccesser::getWeaponData in sora_melee.
struct ftLucarioData : ftData {
    void* unk94;
    const float* m_auraHpModeParam;             // 0x98: aura power curve of stamina matches (getAuraPowerRateHpMode)
    soCollisionShieldGroupData* m_shieldGroup; // 0x9C: installed as shield group 1 by the constructor
};
