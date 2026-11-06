#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector4.h>
#include <havok/hkMemory.h>

struct hkWorld;
struct hkEntity;
struct hkAgentNnEntry;
struct hkProcessCollisionInput;
struct hkProcessCollisionOutput;
struct hkStepInfo;

// Simulation driver owned by an hkWorld (layout partly recovered; tail not recovered yet).
// Field offsets from the constructor (0x0C..0x24 region).
struct hkSimulation : hkReferencedObject {
    hkWorld* m_world;     // 0x08 HYPOTHESIS: world that owns this simulation (constructor argument)
    int m_unk0C;          // 0x0C (set to 1 by the constructor)
    hkReal m_unk10;       // 0x10 (set to 0 by the constructor) the time advanced to (advanceTime)
    hkReal m_unk14;       // 0x14 (set to 0 by the constructor) start of the next step (collide adds the delta)
    hkReal m_unk18;       // 0x18 the delta time of the current step (integrate's argument)
    hkReal m_unk1C;       // 0x1C (constructor constant)
    hkReal m_unk20;       // 0x20 (constructor constant)
    // 0x24 (set to 0 by the constructor) the last step result: 0 when the step finished, otherwise the part that
    // ran out of memory (1 integrate, 2 collide); stepDeltaTime also tests 3 before advancing the time.
    int m_unk24;

    static void operator delete(void* p) {
        hkMemory::s_instance->deallocateChunk(p, ((hkReferencedObject*)p)->m_memSizeAndFlags, 0x13);
    }

    // What processAgentsOfEntities does with each agent entry of the entities (called as (this->*func)(entry, input,
    // output)). The input is not const: processAgentCollideDiscrete writes the pair's quality settings into it and its
    // load of the dispatcher stays behind the output stores (a pointer-to-const parameter would let it move up).
    // HYPOTHESIS: the parameter types beyond that.
    typedef void (hkSimulation::*AgentEntryProcessFunction)(hkAgentNnEntry* entry, hkProcessCollisionInput& input,
                                                            hkProcessCollisionOutput& output);

    hkSimulation(hkWorld* world);
    void applyActions();
    // Not written yet (0x802F1480).
    static void collideEntitiesBroadPhaseDiscrete(hkEntity** entities, int numEntities, hkWorld* world);
    // HYPOTHESIS: the last parameter (an int-sized mode compared with 1; collideEntitiesNarrowPhaseDiscrete passes
    // its own through, resetCollisionInformationForEntities passes 0)
    void collideEntitiesNarrowPhaseDiscrete(hkEntity** entities, int numEntities, hkProcessCollisionInput& input,
                                            int mode);
    void processAgentCollideDiscrete(hkAgentNnEntry* entry, hkProcessCollisionInput& input,
                                     hkProcessCollisionOutput& output);
    void processAgentResetCollisionInformation(hkAgentNnEntry* entry, hkProcessCollisionInput& input,
                                               hkProcessCollisionOutput& output);
    void processAgentsOfEntities(hkEntity** entities, int numEntities, hkProcessCollisionInput& input,
                                 AgentEntryProcessFunction func, int mode);
    hkReal snapSimulateTimeAndGetTimeToAdvanceTo();
    int reCollideAfterStepFailure();
    void integrateInternal(const hkStepInfo& stepInfo);
    virtual ~hkSimulation(); // 0x08 (out of line in src/havok/hkSimulation.cpp)
    // Vtable slots from hkSimulation's vtable in the image (0x0C is hkReferencedObject::calcStatistics), names from
    // the map. HYPOTHESIS: the signatures of the slots nothing calls yet (declared without parameters).
    virtual int stepDeltaTime(hkReal physicsDeltaTime); // 0x10 (hkWorld::stepDeltaTime calls it)
    virtual int integrate(hkReal physicsDeltaTime); // 0x14
    virtual int collide();                          // 0x18
    virtual int advanceTime();                      // 0x1C
    // Broad phase then narrow phase for the entities, with the world's critical operations locked.
    // HYPOTHESIS: the last parameter (passed through to collideEntitiesNarrowPhaseDiscrete as its mode)
    virtual void collideEntitiesDiscrete(hkEntity** entities, int numEntities, hkWorld* world,
                                         const hkStepInfo& stepInfo, int mode); // 0x20
    // the hkBool goes by value (the callers pass the address of a one-byte copy)
    virtual void resetCollisionInformationForEntities(hkEntity** entities, int numEntities, hkWorld* world,
                                                      hkBool flag); // 0x24
    // 0x28-0x30 from hkContinuousSimulation's vtable (lbl_80488CF4), which overrides them with its own named
    // functions. HYPOTHESIS: the argument types (unused, the checks are empty in this build).
    virtual void assertThereIsNoCollisionInformationForEntities(const void* entities); // 0x28
    virtual void removeCollisionInformationForAgent(const void* agent);                // 0x2C
    virtual void assertThereIsNoCollisionInformationForAgent(const void* agent);       // 0x30
    // HYPOTHESIS: the name (hkWorldOperationQueue calls it for the reintegrate-and-recollide batch)
    virtual void reintegrateAndRecollideEntities(hkEntity** entities, int numEntities, hkWorld* world); // 0x34
    virtual void collideInternal(const hkStepInfo& stepInfo); // 0x38
    // HYPOTHESIS: the parameter (hkContinuousSimulation's override is warpTime(float))
    virtual void warpTime(hkReal warpDeltaTime); // 0x3C (map: hkSimulation::warpTime, fn_802F0AE0)
};
