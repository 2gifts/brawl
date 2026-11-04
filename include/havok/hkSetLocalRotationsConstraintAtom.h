#pragma once

#include <havok/hkConstraintAtom.h>

// Constraint atom that sets the local rotations of a constraint's bodies (0x70 bytes). Layout and names from
// main.dol's hkSetLocalRotationsConstraintAtomClass (the atom header is shared, see hkConstraintAtom.h).
struct hkSetLocalRotationsConstraintAtom : hkConstraintAtom {
    hkRotation m_rotationA; // 0x10
    hkRotation m_rotationB; // 0x40

    int numSolverResults() const;
};
