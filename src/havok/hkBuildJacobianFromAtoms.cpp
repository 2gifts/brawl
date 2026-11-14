// Havok translation unit hkBuildJacobianFromAtoms.o (main.dol 0x8028A860-0x8028D184).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x8028A860     8  numSolverResults   [map: hkLinSoftConstraintAtom__numSolverResults]
//   0x8028A868   172  buildJacobianFromLinLimitAtom   [map: hkLinLimitConstraintAtom__buildJacobianFromLinLimitAtom]
//   0x8028A914     8  numSolverResults   [map: hkLinLimitConstraintAtom__numSolverResults]
//   0x8028A91C   204  buildJacobianFromLinFrictionAtom   [map: hkLinFrictionConstraintAtom__buildJacobianFromLinFrictionAtom]
//   0x8028A9E8    32  hkSkipSolverResults   [map: hkConstraintQueryOut__hkSkipSolverResults]
//   0x8028AA08     8  numSolverResults   [map: hkLinFrictionConstraintAtom__numSolverResults]
//   0x8028AA10   456  buildJacobianFromLinMotorAtom   [map: hkLinMotorConstraintAtom__buildJacobianFromLinMotorAtom]
//   0x8028ABD8     8  numSolverResults   [map: hkLinMotorConstraintAtom__numSolverResults]
//   0x8028ABE0   320  buildJacobianFrom2dAngAtom   [map: hk2dAngConstraintAtom__buildJacobianFrom2dAngAtom]
//   0x8028AD20     8  numSolverResults   [map: hk2dAngConstraintAtom__numSolverResults]
//   0x8028AD28   268  buildJacobianFromAngAtom   [map: hkAngConstraintAtom__buildJacobianFromAngAtom]
//   0x8028AE34     8  numSolverResults   [map: hkAngConstraintAtom__numSolverResults]
//   0x8028AE3C   684  buildJacobianFromAngLimitAtom   [map: hkAngLimitConstraintAtom__buildJacobianFromAngLimitAtom]
//   0x8028B0E8     8  numSolverResults   [map: hkAngLimitConstraintAtom__numSolverResults]
//   0x8028B0F0   568  buildJacobianFromConeLimitAtom   [map: hkConeLimitConstraintAtom__buildJacobianFromConeLimitAtom]
//   0x8028B328     8  numSolverResults   [map: hkConeLimitConstraintAtom__numSolverResults]
//   0x8028B330   680  bulidJacobianFromTwistLimitAtom   [map: hkTwistLimitConstraintAtom__bulidJacobianFromTwistLimitAtom]
//   0x8028B5D8     8  numSolverResults   [map: hkTwistLimitConstraintAtom__numSolverResults]
//   0x8028B5E0   140  buildJacobianFromAngFrictionAtom   [map: hkAngFrictionConstraintAtom__buildJacobianFromAngFrictionAtom]
//   0x8028B66C     8  numSolverResults   [map: hkAngFrictionConstraintAtom__numSolverResults]
//   0x8028B674  1620  buildJacobianFromAngMotorAtom   [map: hkAngMotorConstraintAtom__buildJacobianFromAngMotorAtom]
//   0x8028BCC8     8  numSolverResults   [map: hkAngMotorConstraintAtom__numSolverResults]
//   0x8028BCD0  2472  buildJacobianFromRagdollMotorAtom   [map: hkRagdollMotorConstraintAtom__buildJacobianFromRagdollMotorAtom]
//   0x8028C678     8  numSolverResults   [map: hkRagdollMotorConstraintAtom__numSolverResults]
//   0x8028C680  2340  buildJacobianFromPulleyAtom   [map: hkPulleyConstraintAtom__buildJacobianFromPulleyAtom]
//   0x8028CFA4     8  numSolverResults   [map: hkPulleyConstraintAtom__numSolverResults]
//   0x8028CFAC   472  buildJacobianFromSoftContactModifier   [map: hkSoftContactModifierConstraintAtom__buildJacobianFromSoftContactModifier]

#include <havok/hkConstraintAtom.h>
#include <havok/hkLinSoftConstraintAtom.h>
#include <havok/hkConstraintQueryIn.h>
#include <havok/hkJacobianSchema.h>
#include <havok/hkRotation.h>

// Schema that makes the solver step over solver results nobody writes (8 bytes).
// HYPOTHESIS: the names; 0x13 is the schema type and 2 a flag of the header word.
struct hkJacobianSkipSchema : hkJacobianSchema {
    u8 m_numSolverResults; // 0x04
};

enum {
    HK_JACOBIAN_SKIP_SCHEMA_TAG = 0x13020008,
};

void hkSkipSolverResults(int numSolverResults, hkConstraintQueryOut& out) {
    hkJacobianSkipSchema* schema = (hkJacobianSkipSchema*)(hkJacobianSchema*)out.m_jacobianSchemas;
    schema->m_tag = HK_JACOBIAN_SKIP_SCHEMA_TAG;
    schema->m_numSolverResults = numSolverResults;
    out.m_jacobianSchemas = (hkJacobianSchema*)(schema + 1);
}

// Input of the angular friction rows (built on the stack by the atom builder).
// HYPOTHESIS: the names.
struct hk1dAngularFrictionInfo {
    const hkVector4* m_constrainedDofW; // 0x00 first friction axis (a column of base A)
    void* m_solverResults;              // 0x04 from the query input
    hkReal m_maxFrictionTorque;         // 0x08
    int m_numFriction;                  // 0x0C
};

// Builds the friction rows of the angular friction axes (fn_8028DC9C). HYPOTHESIS: the name.
void hk1dAngularFrictionBuildJacobian(const hk1dAngularFrictionInfo& info, const hkConstraintQueryIn& in,
                                      hkConstraintQueryOut& out);

// Without friction the atom's solver results are skipped.
void hkAngFrictionConstraintAtom::buildJacobianFromAngFrictionAtom(const hkConstraintQueryIn& in,
                                                                   const hkRotation& baseA, const hkRotation& baseB,
                                                                   hkConstraintQueryOut& out) const {
    if (m_isEnabled && m_maxFrictionTorque != 0.0f) {
        hk1dAngularFrictionInfo info;
        info.m_constrainedDofW = &(&baseA.m_col0)[m_firstFrictionAxis];
        info.m_maxFrictionTorque = m_maxFrictionTorque;
        info.m_numFriction = m_numFrictionAxes;
        info.m_solverResults = in.unk44;
        hk1dAngularFrictionBuildJacobian(info, in, out);
    } else {
        hkSkipSolverResults(m_numFrictionAxes, out);
    }
}

int hkLinSoftConstraintAtom::numSolverResults() const {
    return 1;
}

int hkLinLimitConstraintAtom::numSolverResults() const {
    return 1;
}

int hkLinFrictionConstraintAtom::numSolverResults() const {
    return 1;
}

int hkLinMotorConstraintAtom::numSolverResults() const {
    return 1;
}

int hk2dAngConstraintAtom::numSolverResults() const {
    return 2;
}

int hkAngConstraintAtom::numSolverResults() const {
    return m_numConstrainedAxes;
}

int hkAngLimitConstraintAtom::numSolverResults() const {
    return 1;
}

int hkConeLimitConstraintAtom::numSolverResults() const {
    return 1;
}

int hkTwistLimitConstraintAtom::numSolverResults() const {
    return 1;
}

int hkAngFrictionConstraintAtom::numSolverResults() const {
    return m_numFrictionAxes;
}

int hkAngMotorConstraintAtom::numSolverResults() const {
    return 1;
}

int hkRagdollMotorConstraintAtom::numSolverResults() const {
    return 3;
}

int hkPulleyConstraintAtom::numSolverResults() const {
    return 1;
}
