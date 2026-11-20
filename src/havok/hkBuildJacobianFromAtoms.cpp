#include <types.h>
// Havok translation unit hkBuildJacobianFromAtoms.o (main.dol 0x80287B04-0x8028D8BC): the shared Jacobian helpers,
// the atom-list builder and the jacobian builders of the constraint atoms, each followed by its atom's
// numSolverResults. Names from the DebugMode map. Functions in address order:
//   0x80287B04  1252  hkBallSocketConstraintBuildJacobian
//   0x80287FE8   992  hk1dAngularLimitBuildJacobian
//   0x802883C8   992  _hk1dAngularLimitBuildJacobian
//   0x802887A8   772  hk1dAngularVelocityMotorBeginJacobian
//   0x80288AAC   116  hk1dAngularVelocityMotorCommitJacobian
//   0x80288B20    32  hkReplacePreviousRhs
//   0x80288B40    68  hkBeginConstraints
//   0x80288B84    68  _hkBeginConstraints (out-of-line copy of the inline)
//   0x80288BC8  2708  hkSolverBuildJacobianFromAtomsNotContact
//   0x8028965C   172  hkConstraintQueryIn copy constructor (weak)
//   0x80289708   260  hkTransform copy constructor (weak)
//   0x8028980C     8  hkPadSpu<const hkTransform*>::val (weak)
//   0x80289814     8  hkPadSpu<void*>::val (weak)
//   0x8028981C     8  hkEnum<hkConstraintAtom::AtomType, u16> conversion (weak)
//   0x80289824    32  hkBridgeConstraintAtom::callBridgeFromAtom
//   0x80289844    12  hkBridgeConstraintAtom::numSolverResults
//   0x80289850   940  hkSetLocalTransformsConstraintAtom::setLocalTransformsFromAtom
//   0x80289BFC     8  numSolverResults (hkSetLocalTransformsConstraintAtom)
//   0x80289C04   264  hkSetLocalTranslationsConstraintAtom::setLocalTranslationsFromAtom
//   0x80289D0C     8  numSolverResults (hkSetLocalTranslationsConstraintAtom)
//   0x80289D14   812  hkSetLocalRotationsConstraintAtom::setLocalRotationsFromAtom
//   0x8028A040     8  numSolverResults (hkSetLocalRotationsConstraintAtom)
//   0x8028A048  1252  buildJacobianFromBallSocketAtom
//   0x8028A52C     8  numSolverResults
//   0x8028A534   444  fn_8028A534
//   0x8028A6F0     8  numSolverResults
//   0x8028A6F8   168  hkLinConstraintAtom::buildJacobianFromLinAtom
//   0x8028A7A0     8  numSolverResults
//   0x8028A7A8   184  buildJacobianFromLinSoftAtom
//   0x8028A860     8  numSolverResults
//   0x8028A868   172  hkLinLimitConstraintAtom::buildJacobianFromLinLimitAtom
//   0x8028A914     8  numSolverResults
//   0x8028A91C   204  buildJacobianFromLinFrictionAtom
//   0x8028A9E8    32  hkSkipSolverResults
//   0x8028AA08     8  numSolverResults
//   0x8028AA10   456  buildJacobianFromLinMotorAtom
//   0x8028ABD8     8  numSolverResults
//   0x8028ABE0   320  buildJacobianFrom2dAngAtom
//   0x8028AD20     8  numSolverResults
//   0x8028AD28   268  buildJacobianFromAngAtom (fn_8028AD28 below)
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
//   0x8028D8A0     4  hkEndConstraints
//   0x8028D8A4     8  hkAddByteOffsetCpuPtr<void*> (weak)
//   0x8028D8AC     8  hkAddByteOffsetConst<const hkConstraintAtom> (weak)
//   0x8028D8B4     8  hkPadSpu<void*>::operator= (weak)

#include <havok/hkSetLocalTranslationsConstraintAtom.h>
#include <havok/hkSetLocalRotationsConstraintAtom.h>
#include <havok/hkLinSoftConstraintAtom.h>
#include <havok/hkConstraintAtom.h>
#include <havok/hkConstraintQueryIn.h>
#include <havok/hk1dConstraintInfo.h>
#include <havok/hkJacobianSchema.h>
#include <havok/hkRotation.h>

#include <havok/hkSetLocalTransformsConstraintAtom.h>
#include <havok/hkTransform.h>
#include <havok/hkMath.h>

// Motor schema (type 5, 0x1C bytes). HYPOTHESIS: member names.
struct hkJacobianMotorSchemaData {
    u32 m_tag;         // 0x00
    hkReal m_one;      // 0x04
    hkReal m_tau;      // 0x08
    hkReal m_damping;  // 0x0C
    hkReal m_maxForce; // 0x10
    hkReal m_minForce; // 0x14
    hkReal m_velocity; // 0x18
};

// Header schema written in front of every constraint's rows (type 1, 0x18 bytes). HYPOTHESIS: member names.
struct hkJacobianHeaderSchemaData {
    u32 m_tag;                // 0x00
    int m_jacobianOffset;     // 0x04 the rows' offset from the Jacobian base
    u32 m_accumulatorAOffset; // 0x08
    u32 m_accumulatorBOffset; // 0x0C
    int m_solverResultStride; // 0x10 HYPOTHESIS
    void* m_solverResults;    // 0x14
};

// Byte distance from base to ptr.
static inline int hkGetByteOffsetInt(const void* base, const void* ptr) {
    return (const char*)ptr - (const char*)base;
}

// Ends the motor rows: writes the motor schema and the rhs of the last row. Not matched yet: the target keeps the
// loads after the preceding stores.
void hk1dAngularVelocityMotorCommitJacobian(const hk1dAngularVelocityMotorInfo& info, const hkConstraintQueryIn& in,
                                            hkConstraintQueryOut& out) {
    hkJacobianMotorSchemaData* schema = (hkJacobianMotorSchemaData*)(hkJacobianSchema*)out.m_jacobianSchemas;
    hkReal* jac = (hkReal*)(hkJacobianElement*)out.m_jacobians;
    hkReal s = in.unk04;
    schema->m_tau = info.m_tauFactor * s;
    schema->m_damping = info.m_dampFactor * s;
    schema->m_maxForce = info.m_maxForce;
    schema->m_minForce = info.m_minForce;
    schema->m_velocity = info.m_velocityFactor;
    schema->m_one = 1.0f;
    schema->m_tag = 0x050A001C;
    jac[7] = info.m_rhsFactor * in.m_builderParam;
    out.m_jacobians = (hkJacobianElement*)(jac + 8);
    out.m_jacobianSchemas = (hkJacobianSchema*)(schema + 1);
}

// Overwrites the rhs (word 3) of the row `rowsBack` rows (0x30 bytes each) behind the Jacobian cursor.
// HYPOTHESIS: parameter order and names.
void hkReplacePreviousRhs(const hkConstraintQueryIn& in, hkConstraintQueryOut& out, int rowsBack, hkReal rhs) {
    char* row = (char*)(hkJacobianElement*)out.m_jacobians;
    row -= (s16)rowsBack * 0x30;
    ((hkReal*)row)[3] = rhs * in.m_builderParam;
}

// Writes the header schema of a constraint. The out-of-line copy of the inline follows hkBeginConstraints (the
// atom-list builder calls it).
static inline void hkBeginConstraintsBody(const hkConstraintQueryIn& in, hkConstraintQueryOut& out,
                                          void* solverResults, int solverResultStride) {
    hkJacobianHeaderSchemaData* h = (hkJacobianHeaderSchemaData*)(hkJacobianSchema*)out.m_jacobianSchemas;
    int offset = hkGetByteOffsetInt((hkJacobianElement*)out.m_jacobiansBase, (hkJacobianElement*)out.m_jacobians);
    u32 accA = in.m_accumulatorAOffset;
    h->m_tag = 0x01010018;
    h->m_jacobianOffset = offset;
    h->m_accumulatorAOffset = accA;
    h->m_accumulatorBOffset = in.m_accumulatorBOffset;
    h->m_solverResults = solverResults;
    h->m_solverResultStride = solverResultStride;
    out.m_jacobianSchemas = (hkJacobianSchema*)(h + 1);
}

void hkBeginConstraints(const hkConstraintQueryIn& in, hkConstraintQueryOut& out, void* solverResults,
                        int solverResultStride) {
    hkBeginConstraintsBody(in, out, solverResults, solverResultStride);
}

// The same body written out (the target's copy of the inline is register-allocated as a function of its own).
void _hkBeginConstraints(const hkConstraintQueryIn& in, hkConstraintQueryOut& out, void* solverResults,
                         int solverResultStride) {
    hkJacobianHeaderSchemaData* h = (hkJacobianHeaderSchemaData*)(hkJacobianSchema*)out.m_jacobianSchemas;
    int offset = hkGetByteOffsetInt((hkJacobianElement*)out.m_jacobiansBase, (hkJacobianElement*)out.m_jacobians);
    u32 accA = in.m_accumulatorAOffset;
    h->m_tag = 0x01010018;
    h->m_jacobianOffset = offset;
    h->m_accumulatorAOffset = accA;
    h->m_accumulatorBOffset = in.m_accumulatorBOffset;
    h->m_solverResults = solverResults;
    h->m_solverResultStride = solverResultStride;
    out.m_jacobianSchemas = (hkJacobianSchema*)(h + 1);
}

// The bridge's builder takes the constraint data; the header written for the bridge atom is taken back first.
typedef void (*hkBridgeBuildJacobianCall)(hkConstraintData* data, const hkConstraintQueryIn& in,
                                          hkConstraintQueryOut& out);

void hkBridgeConstraintAtom::callBridgeFromAtom(const hkConstraintQueryIn& in, hkTransform& baseA, hkTransform& baseB,
                                                hkConstraintQueryOut& out) const {
    out.m_jacobianSchemas = (hkJacobianSchema*)((char*)(hkJacobianSchema*)out.m_jacobianSchemas - 0x18);
    ((hkBridgeBuildJacobianCall)m_buildJacobianFunc)(m_constraintData, in, out);
}

int hkBridgeConstraintAtom::numSolverResults() const {
    return 100000;
}

// this = a * b with b's rotation columns and translation all rotated by a (the body of hkTransform::setMul, inlined
// here; b is read through a separate pointer indexed per row, which keeps its base out of the loop addressing).
static inline void hkTransform_setMul(hkTransform& t, const hkTransform& a, const hkTransform& b) {
    hkRotation r = a.m_rotation;
    const hkTransform* bp = &b;
    for (int i = 0; i < 4; i++) {
        hkReal* out = &t.m_rotation.m_col0.x + i * 4;
        hkReal x = (&bp->m_rotation.m_col0.x)[i * 4 + 0];
        hkReal y = (&bp->m_rotation.m_col0.x)[i * 4 + 1];
        hkReal z = (&bp->m_rotation.m_col0.x)[i * 4 + 2];
        out[0] = x * r.m_col0.x + y * r.m_col1.x + z * r.m_col2.x;
        out[1] = x * r.m_col0.y + y * r.m_col1.y + z * r.m_col2.y;
        out[2] = x * r.m_col0.z + y * r.m_col1.z + z * r.m_col2.z;
        out[3] = 0.0f;
    }
    t.m_translation.add4(a.m_translation);
}

// Moves the constraint space of both bodies: base = world transform * atom's local transform.
void hkSetLocalTransformsConstraintAtom::setLocalTransformsFromAtom(const hkConstraintQueryIn& in, hkTransform& baseA,
                                                                    hkTransform& baseB) const {
    hkTransform_setMul(baseA, *in.m_transformA.m_value, m_transformA);
    hkTransform_setMul(baseB, *in.m_transformB.m_value, m_transformB);
}

int hkSetLocalTransformsConstraintAtom::numSolverResults() const {
    return 0;
}

// this = t * v (the body of hkVector4::setTransformedPos with the components read in x, y, z order).
static inline void hkVector4_setTransformedPos(hkVector4& o, const hkTransform& t, const hkVector4& v) {
    const hkRotation& r = t.m_rotation;
    hkReal vx = v.x;
    hkReal vy = v.y;
    hkReal vz = v.z;
    o.x = t.m_translation.x + (vx * r.m_col0.x + vy * r.m_col1.x + vz * r.m_col2.x);
    o.y = t.m_translation.y + (vx * r.m_col0.y + vy * r.m_col1.y + vz * r.m_col2.y);
    o.z = t.m_translation.z + (vx * r.m_col0.z + vy * r.m_col1.z + vz * r.m_col2.z);
    o.w = 0.0f;
}

// Moves the constraint pivots: base translation = world transform * atom's local translation.
void hkSetLocalTranslationsConstraintAtom::setLocalTranslationsFromAtom(const hkConstraintQueryIn& in,
                                                                        hkTransform& baseA, hkTransform& baseB) const {
    hkVector4_setTransformedPos(baseA.m_translation, *in.m_transformA, m_translationA);
    hkVector4_setTransformedPos(baseB.m_translation, *in.m_transformB, m_translationB);
}

int hkSetLocalTranslationsConstraintAtom::numSolverResults() const {
    return 0;
}

// rotation = a's rotation * b's rotation: the column loop of hkTransform::setMul without the translation.
// HYPOTHESIS: the target runs the loop four times (two unrolled iterations of two), as here.
static inline void hkRotation_setMul(hkTransform& t, const hkTransform& a, const hkTransform& b) {
    hkRotation r = a.m_rotation;
    const hkTransform* bp = &b;
    for (int i = 0; i < 4; i++) {
        hkReal* out = &t.m_rotation.m_col0.x + i * 4;
        hkReal z = (&bp->m_rotation.m_col0.x)[i * 4 + 2];
        hkReal y = (&bp->m_rotation.m_col0.x)[i * 4 + 1];
        hkReal x = (&bp->m_rotation.m_col0.x)[i * 4 + 0];
        out[0] = x * r.m_col0.x + y * r.m_col1.x + z * r.m_col2.x;
        out[1] = x * r.m_col0.y + y * r.m_col1.y + z * r.m_col2.y;
        out[2] = x * r.m_col0.z + y * r.m_col1.z + z * r.m_col2.z;
        out[3] = 0.0f;
    }
}

// Turns the constraint space of both bodies: base rotation = world rotation * atom's local rotation.
void hkSetLocalRotationsConstraintAtom::setLocalRotationsFromAtom(const hkConstraintQueryIn& in, hkTransform& baseA,
                                                                  hkTransform& baseB) const {
    hkRotation_setMul(baseA, *in.m_transformA.m_value, (const hkTransform&)m_rotationA);
    hkRotation_setMul(baseB, *in.m_transformB.m_value, (const hkTransform&)m_rotationB);
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

// TODO: clean up, give variables real names
extern "C" int fn_8028A52C(void) {
    return 3;
}

// Keeps the pivots m_length apart: one linear row along the current pivot separation (the first base axis of the
// identity when the pivots coincide), with the rhs of that row replaced by the length error.
void hkStiffSpringConstraintAtom::buildJacobianFromStiffSpringAtom(const hkConstraintQueryIn& in,
                                                                   const hkTransform& baseA, const hkTransform& baseB,
                                                                   hkConstraintQueryOut& out) const {
    hk1dLinearBilateralInfo info;
    info.m_pivotA = baseA.m_translation;
    info.m_pivotB = baseB.m_translation;
    hkVector4 separation;
    separation.setSub4(info.m_pivotA, info.m_pivotB);
    hkReal length = separation.length3();
    separation.mul4(1.0f / length);
    if (length > 0.0f) {
        info.m_constrainedDofW = separation;
    } else {
        info.m_constrainedDofW = hkTransform::getIdentity().getColumn(0);
        length = 0.0f;
    }
    hk1dLinearBilateralConstraintBuildJacobian(info, in, out);
    hkReal* row = (hkReal*)((char*)(hkJacobianElement*)out.m_jacobians - 0x30);
    row[3] = (m_length - length) * in.m_builderParam;
}

// Cone limit: the angle between A's twist axis and B's reference axis, measured from alignment or from
// perpendicularity depending on the mode, limited around the axis perpendicular to both. Returns 1 (no row) when
// the axes are (nearly) parallel. 96%: register colouring of the cross product only.
// The reciprocal square root with the estimate held as a double first (the target keeps frsqrte and frsp in
// separate registers here, unlike hkVector4_sqrtInverse).
static inline hkReal coneSqrtInverse(hkReal x) {
    if (x <= 0.0f) {
        u32 bits = 0x7f800000;
        return *(hkReal*)&bits;
    }
    double e = __frsqrte(x);
    hkReal r = (hkReal)e;
    return (0.5f * r) * -(r * (x * r) - 3.0f);
}
static const hkReal s_coneAngleScale[2] = {1.0f, -1.0f};
static const hkReal s_coneAngleOffset[2] = {0.0f, 1.5707964f};

int hkConeLimitConstraintAtom::buildJacobianFromConeLimitAtom(const hkConstraintQueryIn& in, const hkRotation& baseA,
                                                               const hkRotation& baseB, hk1dAngularLimitInfo& info,
                                                               hkConstraintQueryOut& out) const {
    const hkVector4& twistAxisA = baseA.getColumn(m_twistAxisInA);
    const hkVector4& refAxisB = baseB.getColumn(m_refAxisInB);
    hkVector4 perp;
    perp.setCross(twistAxisA, refAxisB);
    hkSimdReal sinAngleSq = perp.lengthSquared3();
    if (sinAngleSq < 1.1920929e-07f) {
        return 1;
    }
    hkReal sinAngle = 1.0f / coneSqrtInverse(sinAngleSq);
    hkReal cosAngle = twistAxisA.dot3(refAxisB);
    int mode = (u8)m_angleMeasurementMode;
    hkReal angle = hkMathTypes::hkMath_atan2fApproximation(sinAngle, cosAngle);
    hkReal computedAngle = angle * s_coneAngleScale[mode] + s_coneAngleOffset[mode];
    info.m_tau = m_angularLimitsTauFactor * in.unk38;
    hkReal sign = s_coneAngleScale[1 - (u8)m_angleMeasurementMode];
    perp.mul4(coneSqrtInverse(sinAngleSq));
    info.m_constrainedDofW.setMul4(sign, perp);
    info.m_computedAngle = computedAngle;
    info.m_min = m_minAngle;
    info.m_max = m_maxAngle;
    return 0;
}

// Twist limit: the twist angle around the half-way axis of the two twist axes, measured from B's reference axis
// projected into the plane perpendicular to that axis. The tau is scaled down by the squared cosine of the half
// angle between the twist axes. HYPOTHESIS: the original name keeps the map's spelling ("bulid").
// TODO: clean up: draft; the target reloads the axis from the stack after the atan2 call (ours keeps axis.x in
// f30 across it), which shifts the FPR colouring of the frame products.
int hkTwistLimitConstraintAtom::bulidJacobianFromTwistLimitAtom(const hkConstraintQueryIn& in, const hkRotation& baseA,
                                                                const hkRotation& baseB, hk1dAngularLimitInfo& info,
                                                                hkConstraintQueryOut& out) const {
    const hkVector4& twistAxisA = baseA.getColumn(m_twistAxis);
    const hkVector4& twistAxisB = baseB.getColumn(m_twistAxis);
    info.m_constrainedDofW = twistAxisA;
    info.m_min = m_minAngle;
    info.m_max = m_maxAngle;
    hkVector4 axis;
    axis.setAdd4(twistAxisB, twistAxisA);
    hkSimdReal lengthSq = axis.lengthSquared3();
    const hkVector4& refAxisB = baseB.getColumn(m_refAxis);
    const hkVector4& refAxisA = baseA.getColumn(m_refAxis);
    hkReal length = 1.0f / hkVector4_sqrtInverse(lengthSq);
    double cosHalfAngle;
    if (length > 1e-16f) {
        axis.mul4(1.0f / length);
        cosHalfAngle = 0.5f * length;
    } else {
        axis = twistAxisB;
        cosHalfAngle = 0.0f;
    }
    hkRotation frame;
    frame.m_col0 = axis;
    frame.m_col1.setCross(axis, refAxisB);
    frame.m_col2.setCross(frame.m_col1, axis);
    hkReal angle = hkMathTypes::hkMath_atan2fApproximation(frame.m_col1.dot3(refAxisA), frame.m_col2.dot3(refAxisA));
    info.m_computedAngle = angle;
    info.m_constrainedDofW = axis;
    hkReal c = (hkReal)cosHalfAngle;
    info.m_tau = c * (c * (m_angularLimitsTauFactor * in.unk38));
    return 0;
}

extern "C" int fn_8028A6F0(void) {
    return 1;
}

// Keeps the pivots on a plane: one linear row along the axis of body B's base picked by the atom.
void hkLinConstraintAtom::buildJacobianFromLinAtom(const hkConstraintQueryIn& in, const hkTransform& baseA,
                                                   const hkTransform& baseB, hkConstraintQueryOut& out) const {
    hk1dLinearBilateralInfo info;
    info.m_pivotA = baseA.m_translation;
    info.m_pivotB = baseB.m_translation;
    info.m_constrainedDofW = baseB.getColumn(m_axisIndex);
    hk1dLinearBilateralConstraintBuildJacobian(info, in, out);
}

// Keeps the pivots' distance along body B's first base axis within [m_min, m_max]. The target reads column 0 of
// the base, not the column of m_axisIndex.
void hkLinLimitConstraintAtom::buildJacobianFromLinLimitAtom(const hkConstraintQueryIn& in, const hkTransform& baseA,
                                                             const hkTransform& baseB, hkConstraintQueryOut& out) const {
    hk1dLinearLimitInfo info;
    info.m_pivotA = baseA.m_translation;
    info.m_pivotB = baseB.m_translation;
    info.m_min = m_min;
    info.m_max = m_max;
    info.m_constrainedDofW = baseB.getColumn(0);
    hk1dLinearLimitBuildJacobian(info, in, out);
}

extern "C" int fn_8028A7A0(void) {
    return 0x1;
}

extern "C" int fn_8028D5C8(void) {
    return 0;
}

extern "C" int fn_8028D898(void) {
    return 0;
}

// TODO: clean up, give variables real names
namespace jj_8028A91C {
struct Vec4 { f32 x, y, z, w; };
struct Marker { u32 tag; u8 kind; u8 pad[3]; };
struct Buffer { u32 x0; u32 x4; struct Marker *cursor; };
struct A { char pad0[2]; u8 flag; u8 idx; f32 t; };
struct B { char pad[0x44]; void *ptr; };
struct C { char pad[0x30]; struct Vec4 v; };
struct Local { struct Vec4 a; struct Vec4 b; f32 s; void *p; };

extern f32 lbl_805A3D90;
extern void fn_8028E080(struct Local *loc, struct B *b, struct Buffer *e);

extern "C" void fn_8028A91C(struct A *a, struct B *b, struct C *c, struct Vec4 *d, struct Buffer *e) {
    f32 d3, d2, d1, d0, c3, c2, c1, c0, t;
    if (a->flag != 0 && (t = a->t) > lbl_805A3D90) {
        struct Local loc __attribute__((aligned(16)));
        struct Vec4 *v = d + a->idx;
        void *p;
        c0 = c->v.x;
        c1 = c->v.y;
        c2 = c->v.z;
        c3 = c->v.w;
        d0 = v->x;
        d1 = v->y;
        d2 = v->z;
        d3 = v->w;
        p = b->ptr;
        loc.a.x = c0;
        loc.a.y = c1;
        loc.a.z = c2;
        loc.a.w = c3;
        loc.b.x = d0;
        loc.b.y = d1;
        loc.b.z = d2;
        loc.b.w = d3;
        loc.s = t;
        loc.p = p;
        fn_8028E080(&loc, b, e);
        return;
    }
    {
        struct Marker *m = e->cursor;
        m->tag = 0x13020008;
        m->kind = 1;
        e->cursor = m + 1;
    }
}
}

namespace jj_8028AD28 {
typedef struct Vec4 { float x, y, z, w; } Vec4;
typedef struct Triangle { Vec4 b, a, c; } __attribute__((aligned(16))) Triangle;
extern const unsigned char lbl_8040F958[12];
extern void fn_8028D8C0(Triangle *, void *, void *);

extern "C" void fn_8028AD28(unsigned char *ctx, void *arg2, const void *arg3,
                 const void *arg4, void *arg5, int i)
{
    Triangle t;
    const unsigned char *base = lbl_8040F958;
    for (i = ctx[2]; i < ctx[2] + ctx[3]; ++i) {
        const Vec4 *a, *b, *c;
        b = (const Vec4 *)((const char *)arg3 + ((const unsigned char *)(i + (unsigned int)base))[1]);
        c = (const Vec4 *)((const char *)arg4 + ((const unsigned char *)(i + (unsigned int)base))[2]);
        a = (const Vec4 *)((const char *)arg3 + base[i]);
        t.a.x = a->x; t.a.y = a->y; t.a.z = a->z; t.a.w = a->w;
        t.b.x = b->x; t.b.y = b->y; t.b.z = b->z; t.b.w = b->w;
        t.c.x = c->x; t.c.y = c->y; t.c.z = c->z; t.c.w = c->w;
        fn_8028D8C0(&t, arg2, arg5);
    }
}
}

namespace jj_8028D474 {
typedef unsigned short u16;
typedef unsigned int u32;
typedef float f32;

typedef struct Blob {
    f32 f0, f4, f8, fC, f10, f14, f18, f1C, f20, f24;
    u32 v28, v2C, v30, v34;
    f32 f38, f3C;
    u32 v40, v44, v48, v4C, v50;
} Blob;

typedef struct Node {
    u16 kind;
    u16 pad2;
    u16 id;
    u16 pad6;
    struct Node *next;
} Node;

typedef struct Context {
    u32 a, b, c;
} Context;

extern void fn_80295148(void *);
extern void fn_80293CBC(Node *, Blob *, Context *);
extern void fn_80288BC8(Node *, u16, Blob *, Context *);

static inline void copy_blob(Blob *dst, const Blob *src)
{
    dst->f0 = src->f0;
    dst->f4 = src->f4;
    dst->f8 = src->f8;
    dst->fC = src->fC;
    dst->f10 = src->f10;
    dst->f14 = src->f14;
    dst->f18 = src->f18;
    dst->f1C = src->f1C;
    dst->f20 = src->f20;
    dst->f24 = src->f24;
    dst->v28 = src->v28;
    dst->v2C = src->v2C;
    dst->v30 = src->v30;
    dst->v34 = src->v34;
    dst->f38 = src->f38;
    dst->f3C = src->f3C;
    dst->v40 = src->v40;
    dst->v44 = src->v44;
    dst->v48 = src->v48;
    dst->v4C = src->v4C;
    dst->v50 = src->v50;
}

static inline signed char is_kind(Node *p)
{
    return p->kind == 0x16;
}

extern "C" void fn_8028D474(Node * __restrict self, const Blob *src,
                 void *a, void *b, Context *ctx)
{
    Blob local __attribute__((aligned(16)));
    Node *p;
    u16 id;

    ctx->c -= 0x18;
    copy_blob(&local, src);
    p = self->next;

    while ((int)p->kind >= 0x17) {
        p = p->next;
    }

    fn_80295148((char *)p + 8);
    p = self->next;
    id = self->id;

    if (is_kind(p)) {
        fn_80293CBC(p, &local, ctx);
    } else {
        fn_80288BC8(p, id, &local, ctx);
    }
}
}

// The end of a constraint's rows writes nothing. HYPOTHESIS: parameters.
void hkEndConstraints() {}

// Out-of-line copies of templates the atom-list builder calls (hkAddByteOffsetCpuPtr<void*>,
// hkAddByteOffsetConst<const hkConstraintAtom>, hkPadSpu<void*>::operator=); they keep their map labels.
extern "C" void* fn_8028D8A4(void* base, int offset) {
    return (char*)base + offset;
}

extern "C" const void* fn_8028D8AC(const void* base, int offset) {
    return (const char*)base + offset;
}

extern "C" void** fn_8028D8B4(void** slot, void* value) {
    *slot = value;
    return slot;
}

// More out-of-line copies the atom-list builder calls: hkPadSpu<const hkTransform*>::val, hkPadSpu<void*>::val and
// the hkEnum<AtomType, u16> conversion. They keep their map labels. (The hkConstraintQueryIn copy constructor at
// 0x8028965C copies member by member with float loads; not reproduced.)
extern "C" const hkTransform* fn_8028980C(const hkPadSpu<const hkTransform*>* p) {
    return p->m_value;
}

extern "C" void* fn_80289814(const hkPadSpu<void*>* p) {
    return p->m_value;
}

extern "C" int fn_8028981C(const hkEnum<hkConstraintAtom::AtomType, u16>* e) {
    return e->m_storage;
}

// TODO: clean up, give variables real names
namespace jj_80288BC8 {
#pragma use_lmw_stmw on

typedef unsigned int u32;

extern void fn_8028965C(void *, void *);
extern u32 fn_8028D8AC(u32, int);
extern int fn_8028980C(void *);
extern void __ct__11hkTransformFRC11hkTransform(void *, int);
extern int fn_80289814(void *);
extern void _hkBeginConstraints__FRC19hkConstraintQueryInR20hkConstraintQueryOutPvi(void *, int, int, int);
extern u32 fn_8028981C(u32);
extern int fn_8028D8A4(int, int);
extern void fn_8028D8B4(void *, int);
extern void hkEndConstraints__Fv(void);
extern void fn_802883C8(void *, void *, int);
extern void hkSkipSolverResults__FiR20hkConstraintQueryOut(int, int);

#define DECLARE_HANDLER(handler, length) \
    extern void handler(u32, void *, void *, void *, int); \
    extern int length(u32)

DECLARE_HANDLER(callBridgeFromAtom__22hkBridgeConstraintAtomCFRC19hkConstraintQueryInR11hkTransformR11hkTransformR20hkConstraintQueryOut, numSolverResults__22hkBridgeConstraintAtomCFv);
DECLARE_HANDLER(setLocalTransformsFromAtom__34hkSetLocalTransformsConstraintAtomCFRC19hkConstraintQueryInR11hkTransformR11hkTransform, numSolverResults__34hkSetLocalTransformsConstraintAtomCFv);
DECLARE_HANDLER(setLocalTranslationsFromAtom__36hkSetLocalTranslationsConstraintAtomCFRC19hkConstraintQueryInR11hkTransformR11hkTransform, numSolverResults__36hkSetLocalTranslationsConstraintAtomCFv);
DECLARE_HANDLER(setLocalRotationsFromAtom__33hkSetLocalRotationsConstraintAtomCFRC19hkConstraintQueryInR11hkTransformR11hkTransform, numSolverResults__33hkSetLocalRotationsConstraintAtomCFv);
DECLARE_HANDLER(fn_8028A048, fn_8028A52C);
DECLARE_HANDLER(buildJacobianFromStiffSpringAtom__27hkStiffSpringConstraintAtomCFRC19hkConstraintQueryInRC11hkTransformRC11hkTransformR20hkConstraintQueryOut, fn_8028A6F0);
DECLARE_HANDLER(buildJacobianFromLinAtom__19hkLinConstraintAtomCFRC19hkConstraintQueryInRC11hkTransformRC11hkTransformR20hkConstraintQueryOut, fn_8028A7A0);
DECLARE_HANDLER(buildJacobianFromLinSoftAtom__23hkLinSoftConstraintAtomFPvPC9hkVector4PC9hkVector4Pv, numSolverResults__23hkLinSoftConstraintAtomCFv);
DECLARE_HANDLER(buildJacobianFromLinLimitAtom__24hkLinLimitConstraintAtomCFRC19hkConstraintQueryInRC11hkTransformRC11hkTransformR20hkConstraintQueryOut, numSolverResults__24hkLinLimitConstraintAtomCFv);
DECLARE_HANDLER(fn_8028A91C, numSolverResults__27hkLinFrictionConstraintAtomCFv);
DECLARE_HANDLER(fn_8028AA10, numSolverResults__24hkLinMotorConstraintAtomCFv);
DECLARE_HANDLER(fn_8028ABE0, numSolverResults__21hk2dAngConstraintAtomCFv);
DECLARE_HANDLER(fn_8028AD28, numSolverResults__19hkAngConstraintAtomCFv);
DECLARE_HANDLER(buildJacobianFromAngFrictionAtom__27hkAngFrictionConstraintAtomCFRC19hkConstraintQueryInRC10hkRotationRC10hkRotationR20hkConstraintQueryOut, numSolverResults__27hkAngFrictionConstraintAtomCFv);
DECLARE_HANDLER(fn_8028B674, numSolverResults__24hkAngMotorConstraintAtomCFv);
DECLARE_HANDLER(fn_8028BCD0, numSolverResults__28hkRagdollMotorConstraintAtomCFv);
DECLARE_HANDLER(fn_8028C680, numSolverResults__22hkPulleyConstraintAtomCFv);
DECLARE_HANDLER(fn_8028CFAC, numSolverResults__35hkSoftContactModifierConstraintAtomCFv);
DECLARE_HANDLER(fn_8028D18C, numSolverResults__35hkMassChangerModifierConstraintAtomCFv);
DECLARE_HANDLER(fn_8028D474, fn_8028D5C8);
DECLARE_HANDLER(fn_8028D5D0, fn_8028D898);

extern int fn_8028AE3C(u32, void *, void *, void *, void *, int);
extern int numSolverResults__24hkAngLimitConstraintAtomCFv(u32);
extern int buildJacobianFromConeLimitAtom__25hkConeLimitConstraintAtomCFRC19hkConstraintQueryInRC10hkRotationRC10hkRotationR20hk1dAngularLimitInfoR20hkConstraintQueryOut(u32, void *, void *, void *, void *, int);
extern int numSolverResults__25hkConeLimitConstraintAtomCFv(u32);
extern int bulidJacobianFromTwistLimitAtom__26hkTwistLimitConstraintAtomCFRC19hkConstraintQueryInRC10hkRotationRC10hkRotationR20hk1dAngularLimitInfoR20hkConstraintQueryOut(u32, void *, void *, void *, void *, int);
extern int numSolverResults__26hkTwistLimitConstraintAtomCFv(u32);

#define ADVANCE(length, amount) \
    { \
        int n; \
        old = p; \
        p += amount; \
        n = length(old) * 8; \
        fn_8028D8B4(st.f4, fn_8028D8A4(fn_80289814(st.f4), n)); \
        fn_8028D8B4(st.f100, fn_8028D8A4(fn_80289814(st.f100), n)); \
    }

#define HANDLE(value, handler, length, amount) \
    case value: \
        handler(p, st.copy, st.mat1, st.mat2, context); \
        ADVANCE(length, amount); \
        break

#define HANDLE_RESULT(value, handler, length, amount) \
    case value: \
        result = handler(p, st.copy, st.mat1, st.mat2, st.tmp, context); \
        ADVANCE(length, amount); \
        if (result) goto pass; else goto fail; \
        break

extern "C" void fn_80288BC8(u32 p, int size, void *input, int context)
{
    struct {
        char tmp[0x20];
        char mat2[0x40];
        char mat1[0x40];
        char copy[0x30];
        int e0;
        char e4[0x10];
        char f4[0xc];
        char f100[0xc];
    } st __attribute__((aligned(16)));
    u32 end, old;
    int result;

    fn_8028965C(st.copy, input);
    end = fn_8028D8AC(p, size);
    __ct__11hkTransformFRC11hkTransform(st.mat1, fn_8028980C(&st.e0));
    __ct__11hkTransformFRC11hkTransform(st.mat2, fn_8028980C(st.e4));
    _hkBeginConstraints__FRC19hkConstraintQueryInR20hkConstraintQueryOutPvi(st.copy, context, fn_80289814(st.f4), 8);

    while (p < end) {
again:
        switch (fn_8028981C(p)) {
        case 0:
            p = (p + 15) & ~15;
            goto again;
        HANDLE(1, callBridgeFromAtom__22hkBridgeConstraintAtomCFRC19hkConstraintQueryInR11hkTransformR11hkTransformR20hkConstraintQueryOut, numSolverResults__22hkBridgeConstraintAtomCFv, 12);
        HANDLE(2, setLocalTransformsFromAtom__34hkSetLocalTransformsConstraintAtomCFRC19hkConstraintQueryInR11hkTransformR11hkTransform, numSolverResults__34hkSetLocalTransformsConstraintAtomCFv, 144);
        HANDLE(3, setLocalTranslationsFromAtom__36hkSetLocalTranslationsConstraintAtomCFRC19hkConstraintQueryInR11hkTransformR11hkTransform, numSolverResults__36hkSetLocalTranslationsConstraintAtomCFv, 48);
        HANDLE(4, setLocalRotationsFromAtom__33hkSetLocalRotationsConstraintAtomCFRC19hkConstraintQueryInR11hkTransformR11hkTransform, numSolverResults__33hkSetLocalRotationsConstraintAtomCFv, 112);
        HANDLE(5, fn_8028A048, fn_8028A52C, 2);
        HANDLE(6, buildJacobianFromStiffSpringAtom__27hkStiffSpringConstraintAtomCFRC19hkConstraintQueryInRC11hkTransformRC11hkTransformR20hkConstraintQueryOut, fn_8028A6F0, 8);
        HANDLE(7, buildJacobianFromLinAtom__19hkLinConstraintAtomCFRC19hkConstraintQueryInRC11hkTransformRC11hkTransformR20hkConstraintQueryOut, fn_8028A7A0, 4);
        HANDLE(8, buildJacobianFromLinSoftAtom__23hkLinSoftConstraintAtomFPvPC9hkVector4PC9hkVector4Pv, numSolverResults__23hkLinSoftConstraintAtomCFv, 12);
        HANDLE(9, buildJacobianFromLinLimitAtom__24hkLinLimitConstraintAtomCFRC19hkConstraintQueryInRC11hkTransformRC11hkTransformR20hkConstraintQueryOut, numSolverResults__24hkLinLimitConstraintAtomCFv, 12);
        HANDLE(10, fn_8028A91C, numSolverResults__27hkLinFrictionConstraintAtomCFv, 8);
        HANDLE(11, fn_8028AA10, numSolverResults__24hkLinMotorConstraintAtomCFv, 16);
        HANDLE(12, fn_8028ABE0, numSolverResults__21hk2dAngConstraintAtomCFv, 4);
        HANDLE(13, fn_8028AD28, numSolverResults__19hkAngConstraintAtomCFv, 4);
        HANDLE_RESULT(14, fn_8028AE3C, numSolverResults__24hkAngLimitConstraintAtomCFv, 16);
        HANDLE_RESULT(16, buildJacobianFromConeLimitAtom__25hkConeLimitConstraintAtomCFRC19hkConstraintQueryInRC10hkRotationRC10hkRotationR20hk1dAngularLimitInfoR20hkConstraintQueryOut, numSolverResults__25hkConeLimitConstraintAtomCFv, 20);
        HANDLE_RESULT(15, bulidJacobianFromTwistLimitAtom__26hkTwistLimitConstraintAtomCFRC19hkConstraintQueryInRC10hkRotationRC10hkRotationR20hk1dAngularLimitInfoR20hkConstraintQueryOut, numSolverResults__26hkTwistLimitConstraintAtomCFv, 20);
        HANDLE(17, buildJacobianFromAngFrictionAtom__27hkAngFrictionConstraintAtomCFRC19hkConstraintQueryInRC10hkRotationRC10hkRotationR20hkConstraintQueryOut, numSolverResults__27hkAngFrictionConstraintAtomCFv, 12);
        HANDLE(18, fn_8028B674, numSolverResults__24hkAngMotorConstraintAtomCFv, 20);
        HANDLE(19, fn_8028BCD0, numSolverResults__28hkRagdollMotorConstraintAtomCFv, 80);
        HANDLE(20, fn_8028C680, numSolverResults__22hkPulleyConstraintAtomCFv, 64);
        HANDLE(23, fn_8028CFAC, numSolverResults__35hkSoftContactModifierConstraintAtomCFv, 32);
        HANDLE(24, fn_8028D18C, numSolverResults__35hkMassChangerModifierConstraintAtomCFv, 32);
        HANDLE(25, fn_8028D474, fn_8028D5C8, 16);
        HANDLE(26, fn_8028D5D0, fn_8028D898, 32);
        }
        continue;
fail:
        fn_802883C8(st.tmp, st.copy, context);
        continue;
pass:
        hkSkipSolverResults__FiR20hkConstraintQueryOut(1, context);
    }
    hkEndConstraints__Fv();
}
}

// TODO: clean up, give variables real names
namespace jj_8028965C {
typedef float f32;

struct CopyStruct {
    f32 f0;
    f32 f4;
    f32 f8;
    f32 fC;
    f32 f10;
    f32 f14;
    f32 f18;
    f32 f1C;
    f32 f20;
    f32 f24;
    int v28;
    int v2C;
    int v30;
    int v34;
    f32 f38;
    f32 f3C;
    int v40;
    int v44;
    int v48;
    int v4C;
    int v50;
};

extern "C" void fn_8028965C(struct CopyStruct* dst, const struct CopyStruct* src) {
    dst->f0 = src->f0;
    dst->f4 = src->f4;
    dst->f8 = src->f8;
    dst->fC = src->fC;
    dst->f10 = src->f10;
    dst->f14 = src->f14;
    dst->f18 = src->f18;
    dst->f1C = src->f1C;
    dst->f20 = src->f20;
    dst->f24 = src->f24;
    dst->v28 = src->v28;
    dst->v2C = src->v2C;
    dst->v30 = src->v30;
    dst->v34 = src->v34;
    dst->f38 = src->f38;
    dst->f3C = src->f3C;
    dst->v40 = src->v40;
    dst->v44 = src->v44;
    dst->v48 = src->v48;
    dst->v4C = src->v4C;
    dst->v50 = src->v50;
}
}

