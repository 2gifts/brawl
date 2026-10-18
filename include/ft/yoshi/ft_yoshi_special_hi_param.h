#pragma once

// Partial Egg Throw record at extendParam[1]. Descriptive names are hypotheses
// from airborne boost and egg-launch code; native accesses are scalar floats.
struct ftYoshiSpecialHiParam {
    float speedY;
    float gravity;
    float fallSpeedMax;
    float repeatSpeedMul;
    float aerialUseLimit;
    float throwStickFull;
    float throwStickAngle;
    float throwAngleDeadzone;
    float throwBaseAngle;
    float throwSpeed;
    float throwStrengthSpeed;
    float throwOffsetX;
    float throwOffsetY;
    float throwSpin;
    float throwStrengthSpin;
};
