#pragma once

#include <havok/hkConstraintAtom.h>

// Constraint atom that sets the local transforms of a constraint's bodies (0x90 bytes). Layout and names from
// main.dol's hkSetLocalTransformsConstraintAtomClass (the atom header is shared, see hkConstraintAtom.h).
struct hkSetLocalTransformsConstraintAtom : hkConstraintAtom {
    hkTransform m_transformA; // 0x10
    hkTransform m_transformB; // 0x50
};
