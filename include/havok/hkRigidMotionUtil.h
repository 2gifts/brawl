#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector4.h>

struct hkStepInfo;
struct hkVector4;
struct hkEntity;

// Rigid motion helpers from the hkRigidMotionUtil TU. Parameter types are HYPOTHESIS: the callers pass the
// solver object, a motion or entity pointer, a pointer to a small record (its first word is the motion
// pointer), a count-like word, a stride-like word and the motion pointer again.
struct hkRigidMotionUtil {
    static void hkRigidMotionUtilApplyAccumulators(void* self, void* p1, void* p2, u32 p3, u32 p4, void* p5);
    // Steps the motions of the entities on their own (hkSimulation::integrateInternal, islands without
    // constraints): the step, the velocity change from gravity over it, the entities, and the offset of the motion
    // in an entity. HYPOTHESIS: the parameter types.
    static void hkRigidMotionUtilApplyForcesAndStep(const hkStepInfo& stepInfo, const hkVector4& deltaVelocity,
                                                    hkEntity** entities, int numEntities, int motionOffset);
};
