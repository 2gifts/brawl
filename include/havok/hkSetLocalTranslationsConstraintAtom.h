#pragma once

#include <havok/hkConstraintAtom.h>

struct hkConstraintQueryIn;
struct hkTransform;

// Constraint atom that sets the local translations of a constraint's bodies (0x30 bytes). Layout and names from
// main.dol's hkSetLocalTranslationsConstraintAtomClass (the atom header is shared, see hkConstraintAtom.h).
struct hkSetLocalTranslationsConstraintAtom : hkConstraintAtom {
    hkVector4 m_translationA; // 0x10
    hkVector4 m_translationB; // 0x20

    void setLocalTranslationsFromAtom(const hkConstraintQueryIn& in, hkTransform& baseA, hkTransform& baseB) const;
    int numSolverResults() const;
};
