#pragma once

#include <havok/hkConstraintAtom.h>

// Constraint atom that sets the local translations of a constraint's bodies (0x30 bytes). Layout and names from
// main.dol's hkSetLocalTranslationsConstraintAtomClass (the atom header is shared, see hkConstraintAtom.h).
struct hkSetLocalTranslationsConstraintAtom : hkConstraintAtom {
    hkVector4 m_translationA; // 0x10
    hkVector4 m_translationB; // 0x20

    int numSolverResults() const;
};
