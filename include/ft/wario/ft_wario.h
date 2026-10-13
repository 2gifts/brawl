#pragma once
#include <ft/fighter.h>
// HYPOTHESIS: grouping follows getExtendParam and rider parameter accesses.
// Named fields: the bike dismount speed and the frames before a new bike can be summoned (offsets and sizes match the
// published Wario bike parameter table; use in special_s / special_s_escape agrees).
struct ftWarioBikeRiderParam {
    u8 unk0[8];
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
// Partial fighter declarations: module-builder storage remains unreconstructed.
// Used only for the verified inherited Fighter ABI and getExtendParam calls.
class ftWario : public Fighter {
public:
    ftWarioExtendParam* getExtendParam();
};
class ftWarioMan : public Fighter {
public:
    ftWarioExtendParam* getExtendParam();
};
