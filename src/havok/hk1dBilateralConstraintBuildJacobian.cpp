#include <types.h>
// Havok translation unit hk1dBilateralConstraintBuildJacobian.o (main.dol 0x8028D8BC-0x8028F9AC): the
// one-dimensional Jacobian row builders the atom builders of hkBuildJacobianFromAtoms call, and the schema writers
// of the inverse-mass and velocity modifiers. Names from the DebugMode map. Functions in address order:
//   0x8028D8BC     4  hkLoadVelocityAccumulators
//   0x8028D8C0   988  hk1dAngularBilateralConstraintBuildJacobian
//   0x8028DC9C   996  hk1dAngularFrictionBuildJacobian
//   0x8028E080  1060  hk1dLinearFrictionBuildJacobian
//   0x8028E4A4  1092  hk1dLinearBilateralConstraintBuildJacobian
//   0x8028E8E8  1112  hk1dLinearBilateralConstraintUserTauBuildJacobian
//   0x8028ED40  1128  hk1dLinearLimitBuildJacobian
//   0x8028F1A8  1868  hkStabilizedBallSocketConstraintBuildJacobian
//   0x8028F8F4   108  hkSetInvMassBuildJacobian
//   0x8028F960    76  hkAddVelocityBuildJacobian

#include <havok/hkVector4.h>
#include <havok/hkConstraintQueryIn.h>

// Empty on this platform (the chain constraint datas still call it). HYPOTHESIS: parameters.
void hkLoadVelocityAccumulators(const hkConstraintQueryIn& in) {}

// Schema header word: the schema type in the top byte, the schema size in the low half.
struct hkModifierSchemaHeader {
    u32 m_tag; // 0x00
};

// Writes the inverse-mass modifier schema (type 0x10, size 4) and the two bodies' inverse-mass vectors into the
// Jacobian stream. HYPOTHESIS: parameter names; the query input is not read.
void hkSetInvMassBuildJacobian(const hkVector4& invMassA, const hkVector4& invMassB, const hkConstraintQueryIn& in,
                               hkConstraintQueryOut& out) {
    hkReal* jac = (hkReal*)(hkJacobianElement*)out.m_jacobians;
    hkModifierSchemaHeader* schema = (hkModifierSchemaHeader*)(hkJacobianSchema*)out.m_jacobianSchemas;
    schema->m_tag = 0x10050004;
    jac[0] = invMassA.x;
    jac[1] = invMassA.y;
    jac[2] = invMassA.z;
    jac[3] = invMassA.w;
    jac[4] = invMassB.x;
    jac[5] = invMassB.y;
    jac[6] = invMassB.z;
    jac[7] = invMassB.w;
    out.m_jacobianSchemas = (hkJacobianSchema*)(schema + 1);
    out.m_jacobians = (hkJacobianElement*)((char*)(hkJacobianElement*)out.m_jacobians + 0x20);
}

// Writes the velocity modifier schema (type 0x11, size 4) and the velocity to add into the Jacobian stream.
// HYPOTHESIS: parameter names; the query input is not read.
void hkAddVelocityBuildJacobian(const hkVector4& velocity, const hkConstraintQueryIn& in, hkConstraintQueryOut& out) {
    hkReal* jac = (hkReal*)(hkJacobianElement*)out.m_jacobians;
    hkModifierSchemaHeader* schema = (hkModifierSchemaHeader*)(hkJacobianSchema*)out.m_jacobianSchemas;
    schema->m_tag = 0x11040004;
    jac[0] = velocity.x;
    jac[1] = velocity.y;
    jac[2] = velocity.z;
    jac[3] = velocity.w;
    out.m_jacobianSchemas = (hkJacobianSchema*)(schema + 1);
    out.m_jacobians = (hkJacobianElement*)((char*)(hkJacobianElement*)out.m_jacobians + 0x10);
}
