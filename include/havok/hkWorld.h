#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>
#include <havok/hkVector4.h>
#include <havok/hkStepInfo.h>
#include <havok/hkSolverInfo.h>
#include <havok/hkWorldMaintenanceMgr.h>

struct hkWorldOperationQueue;
struct hkWorld;
// Listener told when its world is deleted (virtual destructor first, then the callback: hkBroadPhaseBorder's
// secondary vtable).
struct hkWorldDeletionListener {
    virtual ~hkWorldDeletionListener() {}
    virtual void worldDeletedCallback(hkWorld* world) = 0;
};
struct hkWorldMemoryWatchDog;
struct hkPhantom;
struct hkAction;
struct hkEntity;
struct hkConstraintInstance;
struct hkPhysicsSystem;
struct hkCollisionFilter;
struct hkCollisionDispatcher;
struct hkStatisticsCollector;
struct hkAabb;
struct hkProcessCollisionInput;
struct hkBroadPhase;
struct hkSimulation;
struct hkTypedBroadPhaseDispatcher;

// The simulation world (layout partly recovered from hkWorld.cpp asm; the tail after 0x114 is not
// recovered). Fields marked unk are written by the constructor and not read by the code matched so far.
// The world's current step and its solver settings (0x150 bytes). Evidence for the pair: integrateInternal hands
// hkConstraintSolverSetup::solve the address of the step and the address 0x10 past it.
struct hkWorldDynamicsStepInfo {
    hkStepInfo m_stepInfo;     // 0x00
    hkSolverInfo m_solverInfo; // 0x10
};

struct hkSimulationIsland;
struct hkRigidBody;

// Element of the world's array at 0xA8 (16 bytes). HYPOTHESIS: only the block at 0x0C is known.
struct hkWorldUnkA8Entry {
    u32 m_unk00[3]; // 0x00
    void* m_unk0C;  // 0x0C
};

struct hkWorld : hkReferencedObject {
    hkSimulation* m_simulation;                     // 0x08 (hkWorld::stepDeltaTime calls its slot 0x10, stepDeltaTime)
    hkVector4 m_gravity;                            // 0x10
    hkSimulationIsland* m_fixedIsland;              // 0x20 holds the fixed entities (and actions on fixed entities only)
    hkRigidBody* m_fixedRigidBody;                  // 0x24 the body null constraint sides attach to
    hkArray<hkSimulationIsland*> m_unk28;           // 0x28 the active islands
    hkArray<hkSimulationIsland*> m_unk34;           // 0x34 the inactive islands
    hkArray<hkSimulationIsland*> m_unk40;           // 0x40 the islands queued for cleanupDirtyIslands
    hkWorldMaintenanceMgr* m_maintenanceMgr;        // 0x4C (hkSimulation::integrate calls performMaintenance)
    u8 m_unk50[0x04];                               // 0x50 not recovered
    hkBroadPhase* m_broadPhase;                     // 0x54 (hkSimulation::collideEntitiesBroadPhaseDiscrete updates it)
    hkTypedBroadPhaseDispatcher* m_broadPhaseDispatcher; // 0x58 gets the pairs the broad phase reports
    // Three more broad-phase dispatch objects (calcStatistics reports them with the dispatcher).
    hkReferencedObject* m_unk5C;                    // 0x5C
    hkReferencedObject* m_unk60;                    // 0x60
    hkReferencedObject* m_unk64;                    // 0x64
    hkProcessCollisionInput* m_collisionInput;      // 0x68 HYPOTHESIS: name; integrate copies the step info into it
    hkCollisionFilter* m_collisionFilter;           // 0x6C handed to the dispatcher with the new pairs
    hkCollisionDispatcher* m_collisionDispatcher;   // 0x70 HYPOTHESIS: name; hkWorldAgentUtil::removeAgent hands it to
                                                    // hkAgentNnMachine_DestroyAgent
    hkWorldOperationQueue* m_operationQueue;        // 0x74 (allocated 0x20 bytes in the constructor)
    int m_pendingOperationsCount;                                    // 0x78 how many world operations are sitting in the queue
    int m_lockCount;                                // 0x7C incremented around add/remove calls
    int m_unk80;                                    // 0x80 summed with m_lockCount in removePhantom
    hkBool m_unk84;                                 // 0x84 flag: operations are being executed
    u8 m_unk85;                                     // 0x85 (initialised to 1)
    int m_unk88;                                    // 0x88
    int m_pendingOperationQueueCount;               // 0x8C how many executeAllPending calls are on the stack (starts at 1)
    int m_unk90;                                    // 0x90
    u8 m_unk94[0x08];                               // 0x94 .. 0x9B not recovered
    void* m_unk9C;                                  // 0x9C
    void* m_unkA0;                                  // 0xA0
    void* m_unkA4;                                  // 0xA4
    hkArray<hkWorldUnkA8Entry> m_unkA8;             // 0xA8 (the destructor frees each entry's 0x0C block)
    hkBool m_wantSimulationIslands;                 // 0xB4 empty islands are removed only when set
    // hkDefaultWorldMaintenanceMgr::performMaintenance only looks for islands to deactivate when it is set
    hkBool m_unkB5;                                 // 0xB5 HYPOTHESIS: a deactivation switch
    // HYPOTHESIS: an inactive body that is moved by hand gets activated when this is set (and it is not fixed)
    hkBool m_unkB6;                                 // 0xB6
    u8 m_unkB7;                                     // 0xB7
    hkReal m_highFrequencyDeactivationPeriod;       // 0xB8
    hkReal m_lowFrequencyDeactivationPeriod;        // 0xBC
    u8 m_unkC0[0x08];                               // 0xC0 .. 0xC7 not recovered
    int m_unkC8;                                    // 0xC8 (initialised to -1)
    hkArray<hkPhantom*> m_phantoms;                 // 0xCC
    // listener arrays (hkWorldCallbackUtil reads them at these offsets; element types there)
    hkArray<void*> m_actionListeners;               // 0xD8
    hkArray<void*> m_entityListeners;               // 0xE4
    hkArray<void*> m_phantomListeners;              // 0xF0
    hkArray<void*> m_constraintListeners;           // 0xFC
    hkArray<hkWorldDeletionListener*> m_worldDeletionListeners; // 0x108
    hkArray<void*> m_islandActivationListeners;     // 0x114
    hkArray<void*> m_postSimulationListeners;       // 0x120
    // tested before hkWorldCallbackUtil::firePostIntegrateCallback (hkSimulation::integrateInternal)
    hkArray<void*> m_postIntegrateListeners;        // 0x12C HYPOTHESIS: name, from the callback it guards
    hkArray<void*> m_postCollideListeners;          // 0x138
    // tested before hkWorldCallbackUtil::fireIslandPostIntegrateCallback (hkSimulation::integrateInternal)
    hkArray<void*> m_islandPostIntegrateListeners;  // 0x144 HYPOTHESIS: name, from the callback it guards
    // tested before hkWorldCallbackUtil::fireIslandPostCollideCallback (hkSimulation::collideInternal)
    hkArray<void*> m_islandPostCollideListeners;    // 0x150 HYPOTHESIS: name, from the callback it guards
    hkArray<void*> m_contactListeners;              // 0x15C
    hkReferencedObject* m_unk168;                   // 0x168 released by the destructor
    u8 m_unk16C[0x04];                              // 0x16C not recovered
    // the current step and the solver settings (hkSimulation::integrate copies the step in; actions get it in
    // applyActions)
    hkWorldDynamicsStepInfo m_dynamicsStepInfo;     // 0x170 HYPOTHESIS: name
    // 0x2C0 the broadphase extents (min, max) as two vector4s (hkBroadPhaseBorder places its phantoms on them).
    // HYPOTHESIS: Havok's hkVector4 m_broadPhaseExtents[2]; kept as floats so the class layout stays as it is.
    hkReal m_broadPhaseExtents[2][4];               // 0x2C0
    u8 m_unk2E0[0x2E4 - 0x2E0];                     // 0x2E0 not recovered
    int m_unk2E4;                                   // 0x2E4 capacity of hkWorldOperationUtil's local pair arrays
    int m_unk2E8;                                   // 0x2E8 capacity of collideEntitiesBroadPhaseDiscrete's pair arrays

    HK_DECLARE_REF_ALLOCATOR(0x2d)

    virtual ~hkWorld();
    int getMemUsageForIntegration();
    void setGravity(const hkVector4& gravity);
    void queueOperation(const void* operation);
    void addWorldDeletionListener(hkWorldDeletionListener* listener);
    void removeWorldDeletionListener(hkWorldDeletionListener* listener);
    hkPhantom* addPhantom(hkPhantom* phantom);
    void removePhantom(hkPhantom* phantom);
    void removeAction(hkAction* action);
    void removeActionImmediately(hkAction* action);
    void addEntityBatch(hkEntity** entities, int count, bool active);
    hkBool removeEntity(hkEntity* entity);
    virtual void calcStatistics(hkStatisticsCollector* collector) const;
    void removeEntityBatch(hkEntity** entities, int count);
    hkEntity* addEntity(hkEntity* entity, int activation);
    void activateRegion(hkAabb* aabb);
    int stepDeltaTime(hkReal physicsDeltaTime);
    void updateCollisionFilterOnEntity(hkEntity* entity, u8 updateMode, u8 updateShapeCollections);
    void updateCollisionFilterOnPhantom(hkPhantom* phantom, u8 updateShapeCollections);
    void updateCollisionFilterOnWorld(u8 updateMode, u8 updateShapeCollections);
    hkAction* addAction(hkAction* action);
    hkConstraintInstance* addConstraint(hkConstraintInstance* constraint);
    hkBool removeConstraint(hkConstraintInstance* constraint);
    void addPhantomBatch(hkPhantom** phantoms, int count);
    void removePhantomBatch(hkPhantom** phantoms, int count);
    void addPhysicsSystem(hkPhysicsSystem* system);
    void removePhysicsSystem(hkPhysicsSystem* system);
    void lock();
    void unlock();
    void lockIslandForConstraintUpdate();
    void unlockIslandForConstraintUpdate();
    // Runs the queued world operations (map name hkWorld__internal_executePendingOperations, fn_802EFE70).
    void internal_executePendingOperations();
    // drop one lock and, if that was the last one and something got queued meanwhile, run the queue
    // BYTES: the flag goes through hkBool's bool conversion before the negation (hkBool::operator! builds a second
    // hkBool); in the propagation-off hkContinuousSimulation::handleAllToisTill that gives extsb; neg; or; srwi.
    int areCriticalOperationsLocked() const {
        return m_lockCount;
    }
    // Phantom operations are also queued while the phantom-specific lock (0x80) is held.
    int areCriticalOperationsLockedForPhantoms() const {
        return m_lockCount + m_unk80;
    }
    void lockCriticalOperations() {
        m_lockCount++;
    }
    void unlockAndAttemptToExecutePendingOperations() {
        if (--m_lockCount == 0 && m_pendingOperationsCount != 0 && !(bool)m_unk84) {
            internal_executePendingOperations();
        }
    }
};
