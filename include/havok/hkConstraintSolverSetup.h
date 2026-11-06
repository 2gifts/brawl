#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector4.h>

// Constraint solver setup helpers (hkConstraintSolverSetup TU). Only the members implemented so far.
struct hkEntity;
struct hkStepInfo;
struct hkSolverInfo;
struct hkSimulationIsland;

// The step values solve works with (0x60 bytes), built on the stack by hkSimulation::integrateInternal from the
// step and the world's solver settings (hkSolverInfo, offsets named after its fields).
// HYPOTHESIS: the name and the grouping. The aligned first member is what the stack copy shows (16-byte aligned).
struct hkSolverStepValues {
    hkReal m_unk00 __attribute__((aligned(16))); // 0x00 solver m_unk124
    hkReal m_unk04;                              // 0x04 solver m_unk124 * m_unk130
    hkReal m_unk08;                              // 0x08 solver m_unk128
    hkReal m_deltaTime;                          // 0x0C the step's delta time
    hkReal m_invDeltaTime;                       // 0x10 the step's inverse delta time
    hkReal m_unk14;                              // 0x14 solver m_unk138
    hkReal m_unk18;                              // 0x18 solver m_unk138 * m_unk134
    hkReal m_unk1C;                              // 0x1C solver m_unk54 * m_unk128
    hkReal m_unk20;                              // 0x20 solver m_unk08
    hkReal m_unk24;                              // 0x24 solver m_unk5C * m_unk128
    u8 m_unk28[0x10];                            // 0x28 not written by integrateInternal
    hkReal m_unk38;                              // 0x38 solver m_unk04
    hkReal m_unk3C;                              // 0x3C solver m_unk08
    u8 m_unk40[0x20];                            // 0x40 not written by integrateInternal (size from its frame)
};

struct hkConstraintSolverSetup {
    void integrate(void* a, void* b, void* c);
    void oneStepIntegrate(hkEntity** entities, int count, u8* base);
    void shutdownSolver();
    // Solves and integrates the entities of one island (map name). HYPOTHESIS: the parameter types.
    static void solve(const hkStepInfo& stepInfo, const hkSolverInfo& solverInfo, const hkSolverStepValues& values,
                      hkSimulationIsland* island, hkEntity** entities, int numEntities);
};
