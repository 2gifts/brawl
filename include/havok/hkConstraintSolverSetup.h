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

struct hkConstraintQueryIn;

// One region of the solver scratch buffer: where it starts and where it ends.
struct hkSolverBufferRegion {
    char* m_begin; // 0x00
    char* m_end;   // 0x04
};

// The solver's working memory, carved out of one buffer by initializeSolverState (0x5C bytes). The buffer is split
// into six 16-byte aligned regions (10, 4, 30, 10 and 6 sixty-fourths of the usable size, the last region takes the
// rest); each region also gets a write cursor. HYPOTHESIS: all names and the meaning of the regions.
struct hkConstraintSolverResources {
    const hkStepInfo* m_stepInfo;                // 0x00
    const hkSolverInfo* m_solverInfo;            // 0x04
    const hkConstraintQueryIn* m_constraintQueryIn; // 0x08
    char* m_region0Begin;                        // 0x0C
    char* m_region0End;                          // 0x10
    char* m_region1Begin;                        // 0x14
    char* m_region1End;                          // 0x18
    char* m_region2Begin;                        // 0x1C
    char* m_region2End;                          // 0x20
    char* m_region3Begin;                        // 0x24
    char* m_region3End;                          // 0x28
    char* m_region4Begin;                        // 0x2C
    char* m_region4End;                          // 0x30
    char* m_region4Cursor;                       // 0x34
    char* m_region4Cursor2;                      // 0x38
    char* m_region5Begin;                        // 0x3C
    char* m_region5End;                          // 0x40 end of the usable buffer
    char* m_region5Cursor;                       // 0x44
    char* m_region5Cursor2;                      // 0x48
    char* m_region0Cursor;                       // 0x4C
    char* m_region2Cursor;                       // 0x50
    char* m_region3Cursor;                       // 0x54
    char* m_region3Cursor2;                      // 0x58
};

struct hkConstraintSolverSetup {
    void integrate(void* a, void* b, void* c);
    void oneStepIntegrate(hkEntity** entities, int count, u8* base);
    void shutdownSolver();
    static void initializeSolverState(const hkStepInfo& stepInfo, const hkSolverInfo& solverInfo,
                                      const hkConstraintQueryIn& constraintQueryIn, char* buffer, int bufferSize,
                                      hkConstraintSolverResources& s);
    // Solves and integrates the entities of one island (map name). HYPOTHESIS: the parameter types.
    static void solve(const hkStepInfo& stepInfo, const hkSolverInfo& solverInfo, const hkSolverStepValues& values,
                      hkSimulationIsland* island, hkEntity** entities, int numEntities);
};
