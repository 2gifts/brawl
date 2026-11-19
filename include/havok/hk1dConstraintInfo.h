#pragma once

// Inputs of the one-dimensional Jacobian row builders of hk1dBilateralConstraintBuildJacobian.o, built on the
// stack by the atom builders (hkBuildJacobianFromAtoms.o) and the generic constraint commands
// (hkGenericConstraintData.o). HYPOTHESIS: all member names; layouts from the callers' stack stores.

#include <havok/hkVector4.h>
#include <havok/hkConstraintQueryIn.h>
#include <havok/hkContactPointMaterial.h>

// Input of the angular friction rows.
struct hk1dAngularFrictionInfo {
    const hkVector4* m_constrainedDofW; // 0x00 first friction axis (a column of base A)
    void* m_solverResults;              // 0x04 from the query input
    hkReal m_maxFrictionTorque;         // 0x08
    int m_numFriction;                  // 0x0C
};

// Builds the friction rows of the angular friction axes (fn_8028DC9C).
void hk1dAngularFrictionBuildJacobian(const hk1dAngularFrictionInfo& info, const hkConstraintQueryIn& in,
                                      hkConstraintQueryOut& out);

// Input of the one-dimensional angular rows (0x30 bytes).
struct hk1dAngularBilateralInfo {
    hkVector4 m_zeroErrorAxisAinW;      // 0x00
    hkVector4 m_perpZeroErrorAxisAinW;  // 0x10
    hkVector4 m_constrainedDofBinW;     // 0x20
};
void hk1dAngularBilateralConstraintBuildJacobian(const hk1dAngularBilateralInfo& info, const hkConstraintQueryIn& in,
                                                 hkConstraintQueryOut& out);

// Input of the one-dimensional linear rows: the two pivots and the constrained direction (0x30 bytes).
struct hk1dLinearBilateralInfo {
    hkVector4 m_pivotA;           // 0x00
    hkVector4 m_pivotB;           // 0x10
    hkVector4 m_constrainedDofW;  // 0x20
};
void hk1dLinearBilateralConstraintBuildJacobian(const hk1dLinearBilateralInfo& info, const hkConstraintQueryIn& in,
                                                hkConstraintQueryOut& out);

// Input of the one-dimensional linear limit rows (0x38 bytes).
struct hk1dLinearLimitInfo : hk1dLinearBilateralInfo {
    hkReal m_min; // 0x30
    hkReal m_max; // 0x34
};
void hk1dLinearLimitBuildJacobian(const hk1dLinearLimitInfo& info, const hkConstraintQueryIn& in,
                                  hkConstraintQueryOut& out);

// Input of the linear friction row (0x38 bytes): the pivot offset, the friction direction, the maximum force and
// the solver results of the row.
struct hk1dLinearFrictionInfo {
    hkVector4 m_pivot;           // 0x00
    hkVector4 m_constrainedDofW; // 0x10
    hkReal m_maxFrictionForce;   // 0x20
    void* m_solverResults;       // 0x24
};

// Input of the angular limit row (0x20 bytes).
struct hk1dAngularLimitInfo {
    hkVector4 m_constrainedDofW; // 0x00
    hkReal m_min;                // 0x10
    hkReal m_max;                // 0x14
    hkReal m_computedAngle;      // 0x18
    hkReal m_tau;                // 0x1C
};

// Output of hkCalcMotorData, read by the velocity motor commit functions (0x18 bytes). HYPOTHESIS: names and
// the meaning of the members.
struct hk1dAngularVelocityMotorInfo {
    hkReal m_rhsFactor;      // 0x00
    hkReal m_maxForce;       // 0x04
    hkReal m_tauFactor;      // 0x08 scaled by in.unk04
    hkReal m_dampFactor;     // 0x0C scaled by in.unk04
    hkReal m_minForce;       // 0x10
    hkReal m_velocityFactor; // 0x14
};

// Input record of hkCalcMotorData (0x18 bytes). The first word is written by the motor Jacobian begin functions.
// HYPOTHESIS: the names (hkCalcMotorData reads the step info's words 3 and 4 and the three trailing members).
struct hkConstraintMotorInput {
    hkReal unk00;                          // 0x00
    const hkConstraintQueryIn* m_stepInfo; // 0x04
    hkSolverResults m_lastResults;         // 0x08
    hkReal m_deltaTarget;                  // 0x10
    hkReal m_positionError;                // 0x14
};

void hk1dAngularVelocityMotorCommitJacobian(const hk1dAngularVelocityMotorInfo& info, const hkConstraintQueryIn& in,
                                            hkConstraintQueryOut& out);

// Not yet named in symbols.txt (their callers' placeholders use the fn_ labels).
// TODO: clean up: rename to hk1dLinearFrictionBuildJacobian / hk1dAngularLimitBuildJacobian (C++ linkage) once
// every caller uses these declarations.
extern "C" void fn_8028E080(const hk1dLinearFrictionInfo& info, const hkConstraintQueryIn& in,
                            hkConstraintQueryOut& out);
extern "C" void fn_80287FE8(const hk1dAngularLimitInfo& info, const hkConstraintQueryIn& in,
                            hkConstraintQueryOut& out);
// hk1dAngularVelocityMotorBeginJacobian (hkBuildJacobianFromAtoms.o). HYPOTHESIS: the parameters.
extern "C" void fn_802887A8(const hkVector4& axis, const hkConstraintQueryIn& in, hkJacobianElement* jac,
                            hkConstraintMotorInput* motorInput);
