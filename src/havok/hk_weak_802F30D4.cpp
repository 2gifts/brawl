// Havok translation unit hk_weak_802F30D4.o (main.dol 0x802F30D4-0x802F34F0), carved from an unclaimed gap.
// It holds the hkSimulation virtuals that follow hkSimulation.o's text (reintegrateAndRecollideEntities and the three
// empty collision-information checks of this build), then the two broad-phase listeners hkWorld creates: the border
// listener (vtable lbl_80488690) and the entity-entity listener (vtable lbl_804886A8). Their deleting destructors
// sit in hkWorld.o.
// HYPOTHESIS: the unit boundary (the listener functions may belong to units of their own).

#include <types.h>
#include <havok/hkSimulation.h>
#include <havok/hkWorld.h>
#include <havok/hkEntity.h>
#include <havok/hkStepInfo.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkLinkedCollidable.h>
#include <havok/hkBroadPhase.h>
#include <havok/hkPhantom.h>
#include <havok/hkAgentNnMachine.h>
#include <havok/hkWorldAgentUtil.h>
#include <havok/hkWorldOperationQueue.h>
#include <havok/hkSweptTransformUtil.h>
#include <havok/hkRigidMotionUtil.h>
#include <havok/hkClass.h>

// Moves the entities back to the simulation time, re-integrates them over the current step and collides them again
// (broad phase, then the narrow phase with fresh time-of-impact information). Queued while the world is locked.
void hkSimulation::reintegrateAndRecollideEntities(hkEntity** entities, int numEntities, hkWorld* world) {
    if (world->m_lockCount != 0) {
        hkWorldOperation::BatchOperation op;
        op.m_type = hkWorldOperation::ENTITY_BATCH_REINTEGRATE_AND_RECOLLIDE;
        op.m_objects = (void**)entities;
        op.m_count = numEntities;
        world->queueOperation(&op);
        return;
    }
    world->lockCriticalOperations();
    hkStepInfo stepInfo(world->m_simulation->m_unk10, world->m_simulation->m_unk14);
    world->m_collisionInput->m_stepInfo = stepInfo;
    for (int i = 0; i < numEntities; i++) {
        hkSweptTransformUtil::backStepMotionState(world->m_simulation->m_unk10,
                                                  entities[i]->m_motion.m_motionState);
    }
    hkRigidMotionUtil::hkRigidMotionUtilStep(stepInfo, entities, numEntities, HK_OFFSET_OF(hkEntity, m_motion));
    collideEntitiesBroadPhaseDiscrete(entities, numEntities, world);
    for (int i = 0; i < numEntities; i++) {
        hkWorldAgentUtil::invalidateTim(entities[i], *world->m_collisionInput);
    }
    collideEntitiesNarrowPhaseDiscrete(entities, numEntities, *world->m_collisionInput, 0);
    world->unlockAndAttemptToExecutePendingOperations();
}

// The collision-information checks are empty in this build (hkContinuousSimulation overrides them).
void hkSimulation::assertThereIsNoCollisionInformationForAgent(const void* agent) {}

void hkSimulation::removeCollisionInformationForAgent(const void* agent) {}

void hkSimulation::assertThereIsNoCollisionInformationForEntities(const void* entities) {}

// Collidable owning a typed broad-phase handle (the handle keeps a signed byte offset back to it).
static inline hkLinkedCollidable* collidableOfHandle(hkTypedBroadPhaseHandle* handle) {
    return (hkLinkedCollidable*)((char*)handle + handle->m_ownerOffset);
}

// Broad-phase type of the border phantoms (hkWorldObject's types: entity 1, phantom 2, border 3).
enum {
    HK_BROAD_PHASE_BORDER_ID = 3,
};

// Tells the border phantom on either side of a pair about the other side (pairs of two border phantoms are dropped).
struct hkBroadPhaseBorderListener : hkReferencedObject {
    HK_DECLARE_REF_ALLOCATOR(0x1f)

    virtual ~hkBroadPhaseBorderListener();
    virtual void addCollisionPair(hkBroadPhaseHandlePair& pair);
    virtual void removeCollisionPair(hkBroadPhaseHandlePair& pair);
};

static inline hkPhantom* phantomOfCollidable(hkCollidable* collidable) {
    return (hkPhantom*)((char*)collidable + collidable->m_ownerOffset);
}

void hkBroadPhaseBorderListener::addCollisionPair(hkBroadPhaseHandlePair& pair) {
    if (pair.m_a->m_type == HK_BROAD_PHASE_BORDER_ID && pair.m_b->m_type == HK_BROAD_PHASE_BORDER_ID) {
        return;
    }
    if (pair.m_a->m_type == HK_BROAD_PHASE_BORDER_ID) {
        hkCollidable* collA = collidableOfHandle(pair.m_a);
        phantomOfCollidable(collA)->addOverlappingCollidable(collidableOfHandle(pair.m_b));
    }
    if (pair.m_b->m_type == HK_BROAD_PHASE_BORDER_ID) {
        hkCollidable* collB = collidableOfHandle(pair.m_b);
        phantomOfCollidable(collB)->addOverlappingCollidable(collidableOfHandle(pair.m_a));
    }
}

void hkBroadPhaseBorderListener::removeCollisionPair(hkBroadPhaseHandlePair& pair) {
    if (pair.m_a->m_type == HK_BROAD_PHASE_BORDER_ID && pair.m_b->m_type == HK_BROAD_PHASE_BORDER_ID) {
        return;
    }
    if (pair.m_a->m_type == HK_BROAD_PHASE_BORDER_ID) {
        hkCollidable* collA = collidableOfHandle(pair.m_a);
        phantomOfCollidable(collA)->removeOverlappingCollidable(collidableOfHandle(pair.m_b));
    }
    if (pair.m_b->m_type == HK_BROAD_PHASE_BORDER_ID) {
        hkCollidable* collB = collidableOfHandle(pair.m_b);
        phantomOfCollidable(collB)->removeOverlappingCollidable(collidableOfHandle(pair.m_a));
    }
}

// Creates and destroys the agents of entity pairs the broad phase reports.
struct hkEntityEntityBroadPhaseListener : hkReferencedObject {
    HK_DECLARE_REF_ALLOCATOR(0x1f)

    hkEntityEntityBroadPhaseListener(hkWorld* world);
    virtual ~hkEntityEntityBroadPhaseListener();
    virtual void addCollisionPair(hkBroadPhaseHandlePair& pair);
    virtual void removeCollisionPair(hkBroadPhaseHandlePair& pair);

    hkWorld* m_world; // 0x08
};

hkEntityEntityBroadPhaseListener::hkEntityEntityBroadPhaseListener(hkWorld* world) {
    m_world = world;
}

// A pair whose quality types combine to no collision quality gets no agent; otherwise the input takes that quality's
// predictive setting first.
void hkEntityEntityBroadPhaseListener::addCollisionPair(hkBroadPhaseHandlePair& pair) {
    hkLinkedCollidable* collA = collidableOfHandle(pair.m_a);
    hkLinkedCollidable* collB = collidableOfHandle(pair.m_b);
    hkProcessCollisionInput* input = m_world->m_collisionInput;
    hkCollisionDispatcher* dispatcher = input->m_dispatcher;
    int quality = dispatcher->getCollisionQualityIndex(collA->m_broadPhaseHandle.m_objectQualityType,
                                                       collB->m_broadPhaseHandle.m_objectQualityType);
    if (quality == 0) {
        return;
    }
    input->m_createPredictiveAgents = dispatcher->getCollisionQualityInfo(quality)->unk10;
    hkWorldAgentUtil::addAgent(collA, collB, *input);
}

void hkEntityEntityBroadPhaseListener::removeCollisionPair(hkBroadPhaseHandlePair& pair) {
    hkLinkedCollidable* collA = collidableOfHandle(pair.m_a);
    hkLinkedCollidable* collB = collidableOfHandle(pair.m_b);
    hkAgentNnEntry* entry = hkAgentNnMachine_FindAgent(collA, collB);
    if (entry != 0) {
        hkWorldAgentUtil::removeAgent(entry);
    }
}
