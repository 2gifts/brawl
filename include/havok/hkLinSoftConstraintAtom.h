#pragma once

#include <havok/hkConstraintAtom.h>
#include <havok/hkVector4.h>

// Linear soft constraint atom (0x0C bytes). Layout and names from main.dol's hkLinSoftConstraintAtomClass: the axis
// index at 0x02 selects a 16-byte entry in the caller's table; tau and damping are copied into the scratch block.
struct hkLinSoftConstraintAtom : hkConstraintAtom {
    u8 m_axisIndex;   // 0x02
    hkReal m_tau;     // 0x04
    hkReal m_damping; // 0x08

    int numSolverResults() const; // in hkBuildJacobianFromAtoms.cpp

    void buildJacobianFromLinSoftAtom(void* a, const hkVector4* b, const hkVector4* table, void* d);
};
