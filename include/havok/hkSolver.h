#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector8.h>
#include <havok/hkVelocityAccumulator.h>
#include <havok/hkSolverInfo.h>

typedef float hkReal;

// Constraint solver. Most members are empty in the original (the stage hooks are no-ops here).
struct hkSolver {
    static void hkSolveUpload();
    void loadVelocityAccumulators();
    void popVelocityAccumulators();
    void storeVelocityAccumulators();
    void prefetchVelocityAccumulators();
    void storeDelayedResult();
    void loadFixedRegisters();
    void applyVelField(hkVector4* vel, u8* rec);
    void applyAngularImpulse(hkReal impulse, hkVector8* jac, hkVelocityAccumulator* a, hkVelocityAccumulator* b, hkReal* sum);
};

