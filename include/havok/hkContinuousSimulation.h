#pragma once

#include <havok/hkSimulation.h>
#include <havok/hkArray.h>
#include <havok/hkToiResourceMgr.h>

struct hkEntity;
struct hkAgentNnEntry;
struct hkProcessCollisionOutput;

// One pending time-of-impact event (0x40 bytes, stored in an hkArray). Layout partly recovered from addToiEvent.
// Layout from addToiEvent and the removal code. The copy below moves the fields one by one; 0x1C is not copied.
struct hkToiEvent {
    hkReal unk00;            // 0x00
    int unk04[2];            // 0x04 the two entities
    hkReal unk0C;            // 0x0C
    // 0x10 the impact's properties (see hkProcessCollisionData::ToiInfo); copies move them as two words.
    int unk10[2];            // 0x10
    u32 unk18;               // 0x18 (matched against the agent key at 0x10 when an agent's events are removed)
    hkVector4 unk20;         // 0x20 (0x1C is alignment padding)
    hkVector4 unk30;         // 0x30
};

// Continuous (time-of-impact) simulation (layout partly recovered; tail not recovered yet).
// Fields from the constructor/destructor (0x28..0x38 region).
struct hkContinuousSimulation : hkSimulation {
    hkArray<hkToiEvent> m_toiEvents;         // 0x28 (ctor: empty array with DONT_DEALLOCATE flag)
    hkToiResourceMgr* m_toiResourceMgr;      // 0x34 (owned, allocated by the constructor)
    int m_unk38;                             // 0x38 (set to 0 by the constructor)

    hkContinuousSimulation(hkWorld* world);
    virtual ~hkContinuousSimulation(); // 0x08
    virtual int advanceTime(); // 0x1C
    virtual void reintegrateAndRecollideEntities(hkEntity** entities, int numEntities, hkWorld* world); // 0x34
    // Steps the world through one time-of-impact event (vtable 0x40, map: simulateToi, fn_80328124).
    // HYPOTHESIS: the parameter types (handleAllToisTill passes the world, its stack copy of the event and the
    // step's delta time).
    virtual void simulateToi(hkWorld* world, hkToiEvent& event, hkReal physicsDeltaTime); // 0x40
    // Processes the pending time-of-impact events up to the given time; a non-zero result is a failed step.
    // HYPOTHESIS: the parameter (advanceTime passes the snapped time to advance to).
    int handleAllToisTill(hkReal minTime);
    // Queues the time of impact the agent of entry reported in output.
    void addToiEvent(const hkProcessCollisionOutput& output, const hkAgentNnEntry& entry);
    // Drops the pending events of an entity, telling their contact managers.
    void removeToiEventsOfEntity(hkEntity* entity);
    void removeToiEventsOfEntities(hkEntity** entities, int numEntities);
    void processAgentCollideContinuous(hkAgentNnEntry* entry, hkProcessCollisionInput& input,
                                       hkProcessCollisionOutput& output);

    // Debug-only consistency checks (vtable 0x28, 0x30); empty in this build. Argument types are HYPOTHESIS (unused).
    void assertThereIsNoCollisionInformationForEntities(const void* entities);
    void assertThereIsNoCollisionInformationForAgent(const void* agent);
    void warpTime(hkReal dt);
    void removeCollisionInformationForAgent(const void* agent);
};
