#pragma once
#include <types.h>

// HYPOTHESIS: grouping follows getExtendParam and rider parameter accesses.
// Named fields: the bike dismount speed and the frames before a new bike can be summoned (offsets and sizes match the
// published Wario bike parameter table; use in special_s / special_s_escape agrees).
struct ftWarioBikeRiderParam {
    float gravity; // Custom kinetic mode 0x64 applies its negative as gravity.
    float unk4;    // Copied to the gravity energy's clamp field at +0x1C.
    float unk8;  // HYPOTHESIS: a damage reaction threshold (compared with the damage reaction in checkDamage)
    float unkC;  // HYPOTHESIS: a damage reaction threshold (compared with the damage reaction in checkDamage)
    float escapeSpeedX;
    float escapeSpeedY;
    int remountCooldownFrames;
};
// Corkscrew (up special) parameters. HYPOTHESIS: grouping follows SpecialHiStart/Jump reads of the same record.
struct ftWarioSpecialHiParam {
    float startSpeedXMul;
    float startSpeedYAdd;
    float startGravity;
    float stickThreshold;
    float jumpTiltDegrees;
    float endSpeedX;
    float endSpeedY;
};
// Wario Waft (down special) charge parameters: charge thresholds are in seconds (the status multiplies by 60).
// HYPOTHESIS: the jump speed names follow the interpolation between two charge levels in execStatus.
struct ftWarioSpecialLwParam {
    int chargeSecondsMax;
    int chargeSecondsLevel1;
    int chargeSecondsLevel2;
    float powerMulLevel1;
    float powerMulLevel2;
    float jumpSpeedYLevel2Min;
    float jumpSpeedYLevel2Max;
    float jumpSpeedYLevel3;
};
struct ftWarioExtendParam {
    void* unk0;
    ftWarioBikeRiderParam* bikeRider;
    ftWarioSpecialHiParam* specialHi;
    ftWarioSpecialLwParam* specialLw;
};
// Wario-Man's gauge: HYPOTHESIS: the names follow ftWarioMan::change and notifyEventOnDamage (how long the transformation
// lasts, and what a hit takes off the gauge).
struct ftWarioManParam {
    float durationSeconds; // 0x00 multiplied by 60 into frames
    float unk4;            // 0x04 scales a constant (0xdb2) taken off the duration
    float damageToGauge;   // 0x08 a hit takes this much per percent of damage off the gauge
    float unkC;            // 0x0C the gauge level below which the end warning plays
};
// The parameter accessor of Wario-Man reached through the fighter (its other tables belong to the builder).
struct ftWarioManExtendParamAccesser {
    u8 unk0[0x7C];
    ftWarioExtendParam m_param; // 0x7C
    ftWarioManParam* m_manParam; // 0x8C
};
