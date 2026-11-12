#pragma once

#include <havok/hkBase.h>

struct hkConstraintInfo;

// Runtime info record written by getRuntimeInfo (two words; the ragdoll writes 0x70 and 0xC).
struct hkConstraintRuntimeInfo {
    u32 unk00;
    u32 unk04;
};

// Solver size summary of a constraint (0x10 bytes). Layout and names from main.dol's hkConstraintInfoClass;
// the modifier atoms add their schema and Jacobian sizes to it (hkConstraintAtom.cpp).
struct hkConstraintInfo {
    int m_maxSizeOfJacobians; // 0x00
    int m_sizeOfJacobians;    // 0x04
    int m_sizeOfSchemas;      // 0x08
    int m_numSolverResults;   // 0x0C
};

// getConstraintInfo fills two more words after the reflected summary: the first atom of the constraint and the
// byte size of its atoms (the two values getConstraintInfoUtil takes).
// HYPOTHESIS: a separate record type; the getConstraintInfo symbols take an hkConstraintInfo*.
struct hkConstraintInfoWithAtoms : hkConstraintInfo {
    void* unk10; // 0x10 first atom
    u32 unk14;   // 0x14 size of the atoms in bytes
};

// Base constraint description. Reflection layout: hkConstraintDataClass.cpp (size 0xC, userData at 0x8).
// Virtual slot numbers follow the target vtable: slot 2 dtor, slot 3 calcStatistics (hkReferencedObject),
// slot 4 isValid and slot 5 getType (the derived data vtables), slots 6..8 are not yet identified, slot 9 is
// buildJacobian (checked against the bridge atom call thunk).
struct hkConstraintData : hkReferencedObject {
    u32 m_userData; // 0x08

    hkConstraintData();
    hkConstraintData(hkFinishLoadedObjectFlag flag) {} // finish-loading ctor: the base ctors set refcount
    virtual ~hkConstraintData() {} // empty in the target (no base dtor call in the derived dtors)

    virtual hkBool isValid() const;
    virtual int getType() const; // 11 = the contact constraint (hkConstrainedSystemFilter skips those)
    virtual void unk06();
    virtual void unk07();
    virtual void unk08();
    virtual void buildJacobian();
};
