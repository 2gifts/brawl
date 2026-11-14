// Havok translation unit hkBuildJacobianFromAtoms.o (main.dol 0x80289D0C-0x8028D8A0): the jacobian builders of the
// constraint atoms, each after its atom's numSolverResults.
// Not yet decompiled. Functions in address order:
//   0x80289D0C     8  numSolverResults (hkSetLocalTranslationsConstraintAtom)
//   0x80289D14   812  setLocalRotationsFromAtom
//   0x8028A040     8  numSolverResults (hkSetLocalRotationsConstraintAtom)
//   0x8028A048  1252  buildJacobianFromBallSocketAtom
//   0x8028A52C     8  numSolverResults
//   0x8028A534   444  fn_8028A534
//   0x8028A6F0     8  numSolverResults
//   0x8028A6F8   168  fn_8028A6F8
//   0x8028A7A0     8  numSolverResults
//   0x8028A7A8   184  buildJacobianFromLinSoftAtom
//   0x8028A860     8  numSolverResults
//   0x8028A868   172  buildJacobianFromLinLimitAtom
//   0x8028A914     8  numSolverResults
//   0x8028A91C   204  buildJacobianFromLinFrictionAtom
//   0x8028A9E8    32  hkSkipSolverResults
//   0x8028AA08     8  numSolverResults
//   0x8028AA10   456  buildJacobianFromLinMotorAtom
//   0x8028ABD8     8  numSolverResults
//   0x8028ABE0   320  buildJacobianFrom2dAngAtom
//   0x8028AD20     8  numSolverResults
//   0x8028AD28   268  buildJacobianFromAngAtom
//   0x8028AE34     8  numSolverResults
//   0x8028AE3C   684  buildJacobianFromAngLimitAtom
//   0x8028B0E8     8  numSolverResults
//   0x8028B0F0   568  buildJacobianFromConeLimitAtom
//   0x8028B328     8  numSolverResults
//   0x8028B330   680  bulidJacobianFromTwistLimitAtom
//   0x8028B5D8     8  numSolverResults
//   0x8028B5E0   140  buildJacobianFromAngFrictionAtom
//   0x8028B66C     8  numSolverResults
//   0x8028B674  1620  buildJacobianFromAngMotorAtom
//   0x8028BCC8     8  numSolverResults
//   0x8028BCD0  2472  buildJacobianFromRagdollMotorAtom
//   0x8028C678     8  numSolverResults
//   0x8028C680  2340  buildJacobianFromPulleyAtom
//   0x8028CFA4     8  numSolverResults
//   0x8028CFAC   472  buildJacobianFromSoftContactModifier
//   0x8028D184     8  numSolverResults (hkSoftContactModifierConstraintAtom)
//   0x8028D18C   736  fn_8028D18C
//   0x8028D46C     8  numSolverResults (hkMassChangerModifierConstraintAtom)
//   0x8028D474   340  fn_8028D474
//   0x8028D5C8     8  fn_8028D5C8
//   0x8028D5D0   712  fn_8028D5D0
//   0x8028D898     8  fn_8028D898

#include <havok/hkSetLocalTranslationsConstraintAtom.h>
#include <havok/hkSetLocalRotationsConstraintAtom.h>
#include <havok/hkLinSoftConstraintAtom.h>
#include <havok/hkConstraintAtom.h>
#include <havok/hkConstraintQueryIn.h>
#include <havok/hkJacobianSchema.h>
#include <havok/hkRotation.h>

int hkSetLocalTranslationsConstraintAtom::numSolverResults() const {
    return 0;
}

int hkSetLocalRotationsConstraintAtom::numSolverResults() const {
    return 0;
}

// Helper in another TU (not yet named): takes the scratch block, the caller's context and a pass-through pointer.
extern "C" void fn_8028E8E8(void* scratch, void* ctx, void* pass);

struct hkLinSoftAtomScratch {
    hkVector4 m_a; // 0x00
    hkVector4 m_b; // 0x10
    hkVector4 m_c; // 0x20
    float m_d;     // 0x30
    float m_e;     // 0x34
};

void hkLinSoftConstraintAtom::buildJacobianFromLinSoftAtom(void* a, const hkVector4* b, const hkVector4* table, void* d) {
    hkLinSoftAtomScratch s;
    s.m_a = b[3];
    s.m_b = table[3];
    s.m_c = table[m_axisIndex];
    s.m_d = m_tau;
    s.m_e = m_damping;
    fn_8028E8E8(&s, a, d);
}

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

int hkSoftContactModifierConstraintAtom::numSolverResults() const {
    return 0;
}

int hkMassChangerModifierConstraintAtom::numSolverResults() const {
    return 0;
}
