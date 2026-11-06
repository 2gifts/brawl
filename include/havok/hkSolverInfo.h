#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector4.h>

// Solver settings of the world (hkWorld 0x180; hkConstraintSolverSetup::solve takes them). hkSimulation's
// collideInternal and integrateInternal derive the per-step values from the step. The name is the map's (its
// debug printer sits in hkSolver.o); offsets in the comments are within the struct (world offset in brackets).
struct hkSolverInfo {
    hkReal m_unk00;            // 0x00 [0x180] not recovered
    hkReal m_unk04;            // 0x04 [0x184]
    hkReal m_unk08;            // 0x08 [0x188]
    u8 m_unk0C[0x04];          // 0x0C [0x18C] not recovered
    hkVector4 m_unk10;         // 0x10 [0x190] gravity * m_unk124
    hkVector4 m_unk20;         // 0x20 [0x1A0] gravity * step delta time (applied to single objects)
    u8 m_unk30[0x54 - 0x30];   // 0x30 [0x1B0] not recovered
    hkReal m_unk54;            // 0x54 [0x1D4]
    u8 m_unk58[0x04];          // 0x58 [0x1D8] not recovered
    hkReal m_unk5C;            // 0x5C [0x1DC]
    u8 m_unk60[0x124 - 0x60];  // 0x60 [0x1E0] not recovered
    hkReal m_unk124;           // 0x124 [0x2A4] step delta time * m_unk138
    hkReal m_unk128;           // 0x128 [0x2A8] inverse step delta time * m_unk12C
    int m_unk12C;              // 0x12C [0x2AC]
    int m_unk130;              // 0x130 [0x2B0]
    hkReal m_unk134;           // 0x134 [0x2B4]
    hkReal m_unk138;           // 0x138 [0x2B8]

    // empty in the original (hkSolver.o)
    void hkDebugPrintfAccumulators();
};
