// Havok translation unit hkWorld.o (main.dol 0x802EA88C-0x802F004C).
// Not yet decompiled. Functions in address order:
//   0x802EA88C  2140  __dt
//   0x802EB0E8    92  __dt
//   0x802EB144    92  __dt
//   0x802EB1A0   512  hkWorld_setupContactMgrFactories
//   0x802EB3A0    92  __dt
//   0x802EB3FC  1044  updateCollisionFilterOnWorld
//   0x802EB810   364  hkWorld_updateFilterOnSinglePhantom
//   0x802EB97C   820  updateCollisionFilterOnPhantom
//   0x802EBCB0  1296  updateCollisionFilterOnEntity
//   0x802EC1C0   608  addEntity
//   0x802EC420  1952  addEntityBatch
//   0x802ECBC0   552  removeEntity
//   0x802ECDE8  1232  removeEntityBatch
//   0x802ED2B8   308  activateRegion
//   0x802ED3EC   356  addConstraint
//   0x802ED550   224  removeConstraint
//   0x802ED630   492  addAction
//   0x802ED81C    68  removeAction
//   0x802ED860   616  removeActionImmediately
//   0x802EDAC8   296  addPhantom
//   0x802EDBF0   972  addPhantomBatch
//   0x802EDFBC   316  removePhantom
//   0x802EE0F8   812  removePhantomBatch
//   0x802EE424   268  addPhysicsSystem
//   0x802EE530   264  removePhysicsSystem
//   0x802EE638    36  setGravity
//   0x802EE65C   108  addWorldDeletionListener
//   0x802EE6C8    80  removeWorldDeletionListener
//   0x802EE718   276  stepDeltaTime
//   0x802EE82C  1772  calcStatistics
//   0x802EEF18  3620  __ct
//   0x802EFD3C   308  setCollisionFilter
//   0x802EFE70   188  internal_executePendingOperations
//   0x802EFF2C     8  queueOperation
//   0x802EFF34   108  getMemUsageForIntegration
//   0x802EFFA0     4  lock
//   0x802EFFA4     4  unlock
//   0x802EFFA8     4  lockIslandForConstraintUpdate
//   0x802EFFAC     4  unlockIslandForConstraintUpdate
//   0x802EFFB0     8  createContactMgr
//   0x802EFFB8     4  cleanup
//   0x802EFFBC     8  reserveContactPoints
//   0x802EFFC4     4  processToi
//   0x802EFFC8     4  removeToi
//   0x802EFFCC     8  addToi
//   0x802EFFD4     4  processContact
//   0x802EFFD8     4  removeContactPoint
//   0x802EFFDC     8  addContactPoint
//   0x802EFFE4    92  __dt
//   0x802F0040    12  __sinit_\hkWorld_cpp

#include <havok/hkWorld.h>
#include <havok/hkPhysicsSystem.h>
#include <havok/hkPhantom.h>
#include <havok/hkWorldCallbackUtil.h>
#include <havok/hkWorldOperationUtil.h>
#include <havok/hkWorldOperationQueue.h>
#include <havok/hkMemory.h>
#include <havok/hkString.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkEntity.h>
#include <havok/hkBroadPhase.h>
#include <havok/hkConstraintChainInstance.h>
#include <havok/hkAction.h>
#include <havok/hkSimulationIsland.h>
#include <havok/hkAgentNnMachine.h>
#include <havok/hkWorldAgentUtil.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkCollisionFilter.h>
#include <havok/hkLinkedCollidable.h>
#include <havok/hkLocalArray.h>
#include <havok/hkTypedBroadPhaseDispatcher.h>
#include <havok/hkSweptTransformUtil.h>
#include <havok/hkEntityCallbackUtil.h>
#include <havok/hkSimulation.h>

void hkWorld::lock() {}

void hkWorld::unlock() {}

void hkWorld::lockIslandForConstraintUpdate() {}

void hkWorld::unlockIslandForConstraintUpdate() {}

// Shape collection mode of the filter updates: also re-filter the children of shape collections.
enum hkUpdateCollectionFilterMode {
    HK_UPDATE_COLLECTION_FILTER_IGNORE_SHAPE_COLLECTIONS = 0,
    HK_UPDATE_COLLECTION_FILTER_PROCESS_SHAPE_COLLECTIONS = 1,
};

// Re-runs the collision filter over the whole world. A full check re-filters every entity, then every phantom;
// otherwise each existing agent is checked: agents the filter (or the collision quality table) now rejects are
// removed and their island is marked for a split check, the others refresh their shape collection filter if asked.
void hkWorld::updateCollisionFilterOnWorld(u8 updateMode, u8 updateShapeCollections) {
    if (areCriticalOperationsLocked()) {
        struct {
            u8 m_type;
            u8 m_updateMode;
            u8 m_updateShapeCollections;
        } op;
        op.m_type = hkWorldOperation::UPDATE_FILTER_WORLD;
        op.m_updateMode = updateMode;
        op.m_updateShapeCollections = updateShapeCollections;
        queueOperation(&op);
        return;
    }
    m_unk84 = true;
    HK_TIMER_BEGIN("UpdateFilterOnWorld");
    if (updateMode == 0) {
        hkArray<hkSimulationIsland*>& activeIslands = *(hkArray<hkSimulationIsland*>*)&m_unk28;
        for (int i = 0; i < activeIslands.getSize(); i++) {
            hkSimulationIsland* island = activeIslands[i];
            for (int j = 0; j < island->m_entities.getSize(); j++) {
                updateCollisionFilterOnEntity(island->m_entities[j], 0, updateShapeCollections);
            }
        }
        hkArray<hkSimulationIsland*>& inactiveIslands = *(hkArray<hkSimulationIsland*>*)&m_unk34;
        for (int i = 0; i < inactiveIslands.getSize(); i++) {
            hkSimulationIsland* island = inactiveIslands[i];
            for (int j = 0; j < island->m_entities.getSize(); j++) {
                updateCollisionFilterOnEntity(island->m_entities[j], 0, updateShapeCollections);
            }
        }
    } else {
        lockCriticalOperations();
        hkArray<hkSimulationIsland*>* islandArrays[2];
        islandArrays[0] = (hkArray<hkSimulationIsland*>*)&m_unk28;
        islandArrays[1] = (hkArray<hkSimulationIsland*>*)&m_unk34;
        for (int a = 0; a < 2; a++) {
            hkInplaceArray<hkAgentNnEntry*, 32> agentsToRemove;
            for (int i = 0; i < islandArrays[a]->getSize(); i++) {
                hkSimulationIsland* island = (*islandArrays[a])[i];
                agentsToRemove.m_size = 0;
                hkAgentNnTrack& track = island->m_agentTrack;
                for (int s = 0; s < track.getNumSectors();) {
                    hkAgentNnEntry* entry = (hkAgentNnEntry*)track.getSector(s);
                    s++;
                    hkAgentNnEntry* end = (s == track.getNumSectors())
                                              ? (hkAgentNnEntry*)((char*)entry + track.m_bytesUsedInLastSector)
                                              : (hkAgentNnEntry*)((char*)entry + track.m_sectorSize);
                    for (; entry < end; entry = (hkAgentNnEntry*)((char*)entry + entry->m_size)) {
                        if (m_collisionFilter->isCollisionEnabled(*entry->m_collidable[0], *entry->m_collidable[1])) {
                            if (m_collisionDispatcher->getCollisionQualityIndex(
                                    entry->m_collidable[0]->m_broadPhaseHandle.m_objectQualityType,
                                    entry->m_collidable[1]->m_broadPhaseHandle.m_objectQualityType)) {
                                if ((hkUpdateCollectionFilterMode)updateShapeCollections ==
                                    HK_UPDATE_COLLECTION_FILTER_PROCESS_SHAPE_COLLECTIONS) {
                                    hkAgentNnMachine_UpdateShapeCollectionFilter(entry, *m_collisionInput);
                                }
                                continue;
                            }
                        }
                        agentsToRemove.pushBack(entry);
                        island->m_flags.m_stateA = 1;
                    }
                }
                while (agentsToRemove.getSize()) {
                    hkAgentNnEntry* entry = (agentsToRemove.begin() + agentsToRemove.getSize())[-1];
                    agentsToRemove.m_size--;
                    hkWorldAgentUtil::removeAgentAndItsToiEvents(entry);
                }
            }
        }
        m_lockCount--;
    }
    if (updateMode == 0) {
        for (int i = 0; i < m_phantoms.getSize(); i++) {
            updateCollisionFilterOnPhantom(m_phantoms[i], updateShapeCollections);
        }
    }
    m_unk84 = false;
    if (m_lockCount == 0 && m_pendingOperationsCount != 0 && !(bool)m_unk84) {
        internal_executePendingOperations();
    }
    HK_TIMER_END();
}

// Single-phantom filter update (hkWorld_updateFilterOnSinglePhantom), defined further down.
struct Obj;
struct Param;
struct ObjB;
extern "C" void fn_802EB810(Obj* a, Param* p, ObjB* b);

// Re-filters one phantom against everything its broad-phase handle overlaps, then optionally its shape collections.
void hkWorld::updateCollisionFilterOnPhantom(hkPhantom* phantom, u8 updateShapeCollections) {
    if (areCriticalOperationsLocked()) {
        struct {
            u8 m_type;
            hkPhantom* m_phantom;
            u8 m_updateShapeCollections;
        } op;
        op.m_type = hkWorldOperation::UPDATE_FILTER_PHANTOM;
        op.m_phantom = phantom;
        op.m_updateShapeCollections = updateShapeCollections;
        queueOperation(&op);
        return;
    }
    lockCriticalOperations();
    HK_TIMER_BEGIN_LIST("UpdateFilterOnPhantom", "broadphase");
    hkLocalArray<hkBroadPhaseHandlePair> pairsOut(m_unk2E4);
    m_broadPhase->reQuerySingleObject(&phantom->m_collidable.m_broadPhaseHandle, pairsOut);
    HK_TIMER_SPLIT_LIST("UpdateOverlaps");
    hkTypedBroadPhaseHandle* thisHandle = &phantom->m_collidable.m_broadPhaseHandle;
    for (int i = 0; i < pairsOut.getSize(); i++) {
        hkTypedBroadPhaseHandle* other = (hkTypedBroadPhaseHandle*)pairsOut[i].m_b;
        if (other == thisHandle) {
            continue;
        }
        hkCollidable* collidable = (hkCollidable*)((char*)other + other->m_ownerOffset);
        fn_802EB810((Obj*)phantom, (Param*)collidable, (ObjB*)m_collisionFilter);
    }
    if (updateShapeCollections) {
        HK_TIMER_SPLIT_LIST("collectionFilter");
        phantom->updateShapeCollectionFilter();
    }
    unlockAndAttemptToExecutePendingOperations();
    HK_TIMER_END_LIST();
}

// Broad-phase type of a collidable owned by a phantom.
enum { HK_BROAD_PHASE_PHANTOM = 2 };

// Re-filters one entity. A full check queries the broad phase again: phantoms in the result are updated directly,
// pairs that already have an agent are dropped, and the rest get new agents. Every existing agent is then checked
// against the filter and the collision quality table; rejected agents are removed and the island marked for a
// split check.
void hkWorld::updateCollisionFilterOnEntity(hkEntity* entity, u8 updateMode, u8 updateShapeCollections) {
    if (areCriticalOperationsLocked()) {
        struct {
            u8 m_type;
            hkEntity* m_entity;
            u8 m_updateMode;
            u8 m_updateShapeCollections;
        } op;
        op.m_type = hkWorldOperation::UPDATE_FILTER_ENTITY;
        op.m_entity = entity;
        op.m_updateMode = updateMode;
        op.m_updateShapeCollections = updateShapeCollections;
        queueOperation(&op);
        return;
    }
    HK_TIMER_BEGIN_LIST("UpdateFilterOnEntity", "init");
    lockCriticalOperations();
    {
    hkInplaceArray<hkBroadPhaseHandlePair, 128> newPairs;
    if (updateMode == 0) {
        HK_TIMER_SPLIT_LIST("broadphase");
        m_broadPhase->reQuerySingleObject(&entity->m_collidable.m_broadPhaseHandle, newPairs);
        HK_TIMER_SPLIT_LIST("phantom");
        for (int i = 0; i < newPairs.getSize(); i++) {
            hkTypedBroadPhaseHandle* handle = newPairs[i].m_b;
            hkCollidable* collidable = (hkCollidable*)((char*)handle + handle->m_ownerOffset);
            hkPhantom* phantom;
            if (collidable->m_broadPhaseHandle.m_type == HK_BROAD_PHASE_PHANTOM) {
                phantom = (hkPhantom*)((char*)collidable + collidable->m_ownerOffset);
            } else {
                phantom = NULL;
            }
            if (phantom != NULL) {
                fn_802EB810((Obj*)phantom, (Param*)&entity->m_collidable, (ObjB*)m_collisionFilter);
                if (updateShapeCollections) {
                    phantom->updateShapeCollectionFilter();
                }
                newPairs.removeAt(i);
                i--;
            }
        }
        {
            hkInplaceArray<hkBroadPhaseHandlePair, 128> existingPairs;
            for (int i = 0; i < entity->m_collidable.m_collisionEntries.getSize(); i++) {
                hkBroadPhaseHandlePair& pair = existingPairs.expandOne();
                pair.m_a = &entity->m_collidable.m_collisionEntries[i].m_agentEntry->m_collidable[0]->m_broadPhaseHandle;
                pair.m_b = &entity->m_collidable.m_collisionEntries[i].m_agentEntry->m_collidable[1]->m_broadPhaseHandle;
            }
            hkTypedBroadPhaseDispatcher::removeDuplicates(existingPairs, newPairs);
        }
    }
    HK_TIMER_SPLIT_LIST("checkAgts");
    {
        int entityQuality = entity->m_collidable.m_broadPhaseHandle.m_objectQualityType;
        for (int i = 0; i < entity->m_collidable.m_collisionEntries.getSize(); i++) {
            hkLinkedCollidable::CollisionEntry& entry = entity->m_collidable.m_collisionEntries[i];
            if (m_collisionFilter->isCollisionEnabled(entity->m_collidable, *entry.m_partner)) {
                if (m_collisionDispatcher->m_collisionQualityIndex[entityQuality]
                                                                  [entry.m_partner->m_broadPhaseHandle.m_objectQualityType]) {
                    if ((hkUpdateCollectionFilterMode)updateShapeCollections ==
                        HK_UPDATE_COLLECTION_FILTER_PROCESS_SHAPE_COLLECTIONS) {
                        hkAgentNnMachine_UpdateShapeCollectionFilter(entry.m_agentEntry, *m_collisionInput);
                    }
                    continue;
                }
            }
            hkWorldAgentUtil::removeAgentAndItsToiEvents(entry.m_agentEntry);
            entity->m_simulationIsland->m_flags.m_stateA = 1;
            i--;
        }
    }
    if (newPairs.getSize() > 0) {
        HK_TIMER_SPLIT_LIST("addAgts");
        const hkCollidableCollidableFilter* filter = m_collisionFilter;
        m_broadPhaseDispatcher->addPairs(newPairs.begin(), newPairs.getSize(), filter);
    }
    unlockAndAttemptToExecutePendingOperations();
    }
    HK_TIMER_END_LIST();
}

// Adds an entity: it gets a motion state pointer if loaded without one, a fresh unique id and an island, then its
// broad-phase entry (which creates its agents) and the added callbacks. Queued while the world is locked.
hkEntity* hkWorld::addEntity(hkEntity* entity, int activation) {
    if (areCriticalOperationsLocked()) {
        struct {
            u8 m_type;
            hkEntity* m_entity;
            int m_activation;
        } op;
        op.m_type = hkWorldOperation::ENTITY_ADD;
        op.m_entity = entity;
        op.m_activation = activation;
        queueOperation(&op);
        return NULL;
    }
    HK_TIMER_BEGIN_LIST("AddEntity", "Island");
    if (entity->m_collidable.m_motion == NULL) {
        entity->m_collidable.m_motion = (hkMotion*)entity->getMotionState();
    }
    hkSweptTransformUtil::setTimeInformation(entity->m_motion.m_motionState, 0.0f, 0.0f);
    m_unk85 = false;
    entity->m_uid = ++m_unkC8;
    entity->addReference();
    hkWorldOperationUtil::addEntitySI(this, entity, activation);
    m_unk85 = true;
    lockCriticalOperations();
    HK_TIMER_SPLIT_LIST("Broadphase");
    hkWorldOperationUtil::addEntityBP(this, entity);
    HK_TIMER_SPLIT_LIST("Callbacks");
    hkWorldCallbackUtil::fireEntityAdded(this, entity);
    hkEntityCallbackUtil::fireEntityAdded(entity);
    unlockAndAttemptToExecutePendingOperations();
    HK_TIMER_END_LIST();
    return entity;
}

// Adds a batch of entities: movable ones share one new island (or the single active island without simulation
// islands), fixed ones go to the fixed island; then all are added to the broad phase at once, the new pairs get
// agents and the added callbacks run. Queued while the world is locked.
void hkWorld::addEntityBatch(hkEntity** entities, int count, bool active) {
    if (count == 0) {
        return;
    }
    if (areCriticalOperationsLocked()) {
        struct {
            u8 m_type;
            hkEntity** m_entities;
            u16 m_count;
            u8 m_active;
        } op;
        op.m_type = hkWorldOperation::ENTITY_BATCH_ADD;
        op.m_entities = entities;
        op.m_count = count;
        op.m_active = active;
        queueOperation(&op);
        return;
    }
    HK_TIMER_BEGIN_LIST("AddEntities", "init");
    lockCriticalOperations();
    hkLocalArray<hkBroadPhaseHandle*> collidablesForBroadphase(count);
    hkLocalArray<hkAabb> aabbsForBroadphase(count);
    hkSimulationIsland* newIsland;
    bool usedNewIsland = false;
    if (m_wantSimulationIslands) {
        newIsland = new hkSimulationIsland(this);
        if ((hkEntityActivation)active == HK_ENTITY_ACTIVATION_DO_ACTIVATE) {
            newIsland->m_storageIndex = m_unk28.m_size;
            newIsland->m_flags.m_stateA = 1;
            newIsland->m_flags.m_stateD = 1;
            newIsland->m_flags27.m_active = 1;
        } else {
            newIsland->m_storageIndex = m_unk34.m_size;
            newIsland->m_flags.m_stateA = 1;
            newIsland->m_flags.m_stateD = 0;
            newIsland->m_flags27.m_active = 0;
        }
    } else {
        newIsland = ((hkSimulationIsland**)m_unk28.m_data)[0];
        newIsland->m_entities.reserveSmart(newIsland->m_entities.getSize() + count);
    }
    hkReal extraRadius = 0.5f * m_collisionInput->getTolerance();
    for (int i = 0; i < count; i++) {
        hkEntity* entity = entities[i];
        entity->m_uid = ++m_unkC8;
        entity->addReference();
        if (entity->m_collidable.m_motion == NULL) {
            entity->m_collidable.m_motion = (hkMotion*)entity->getMotionState();
        }
        hkSweptTransformUtil::setTimeInformation(entity->m_motion.m_motionState, 0.0f, 0.0f);
        entity->m_world = this;
        if (entity->isFixed()) {
            m_fixedIsland->internalAddEntity(entity);
        } else {
            usedNewIsland = true;
            newIsland->internalAddEntity(entity);
        }
        if (entity->m_collidable.m_shape != NULL) {
            hkAabb& aabb = *aabbsForBroadphase.expandByUnchecked(1);
            entity->m_collidable.m_shape->getAabb(*(const hkTransform*)entity->m_collidable.m_motion, extraRadius,
                                                  aabb);
            collidablesForBroadphase.pushBackUnchecked(&entity->m_collidable.m_broadPhaseHandle);
        }
    }
    if (m_wantSimulationIslands) {
        if (usedNewIsland) {
            hkArray<hkSimulationIsland*>& islands = ((hkEntityActivation)active == HK_ENTITY_ACTIVATION_DO_ACTIVATE) ? *(hkArray<hkSimulationIsland*>*)&m_unk28
                                                                     : *(hkArray<hkSimulationIsland*>*)&m_unk34;
            islands.pushBack(newIsland);
        } else {
            delete newIsland;
        }
    }
    hkLocalArray<hkBroadPhaseHandlePair> newPairs(m_unk2E4);
    HK_TIMER_SPLIT_LIST("Broadphase");
    m_broadPhase->addObjectBatch(collidablesForBroadphase, aabbsForBroadphase, newPairs);
    HK_TIMER_SPLIT_LIST("CreateAgents");
    const hkCollidableCollidableFilter* filter = m_collisionFilter;
    m_broadPhaseDispatcher->addPairs(newPairs.begin(), newPairs.getSize(), filter);
    HK_TIMER_SPLIT_LIST("AddedCb");
    for (int i = 0; i < count; i++) {
        hkEntity* entity = entities[i];
        hkWorldCallbackUtil::fireEntityAdded(this, entity);
        hkEntityCallbackUtil::fireEntityAdded(entity);
    }
    unlockAndAttemptToExecutePendingOperations();
    HK_TIMER_END_LIST();
}

// Removes an entity: its broad-phase entry and agents first, then the removed callbacks and its island. A loaded
// entity (memory size 0) frees its internal arrays before the world drops its reference. Queued while locked.
hkBool hkWorld::removeEntity(hkEntity* entity) {
    if (areCriticalOperationsLocked()) {
        struct {
            u8 m_type;
            hkEntity* m_entity;
        } op;
        op.m_type = hkWorldOperation::ENTITY_REMOVE;
        op.m_entity = entity;
        queueOperation(&op);
        return false;
    }
    lockCriticalOperations();
    HK_TIMER_BEGIN_LIST("RemEntity", "Broadphase");
    hkWorldOperationUtil::removeEntityBP(this, entity);
    HK_TIMER_SPLIT_LIST("Callbacks");
    hkWorldCallbackUtil::fireEntityRemoved(this, entity);
    hkEntityCallbackUtil::fireEntityRemoved(entity);
    m_unk85 = false;
    HK_TIMER_SPLIT_LIST("Island");
    hkWorldOperationUtil::removeEntitySI(this, entity);
    if (entity->m_memSizeAndFlags == 0) {
        entity->deallocateInternalArrays();
    }
    entity->removeReference();
    m_unk85 = true;
    unlockAndAttemptToExecutePendingOperations();
    HK_TIMER_END_LIST();
    return true;
}

// Removes a batch of entities: their collision information is reset, they leave the broad phase together (the
// lost pairs' agents are deleted), then each gets the removed callbacks, leaves its island and is released.
void hkWorld::removeEntityBatch(hkEntity** entities, int count) {
    if (count < 1) {
        return;
    }
    if (areCriticalOperationsLocked()) {
        struct {
            u8 m_type;
            hkEntity** m_entities;
            u16 m_count;
        } op;
        op.m_type = hkWorldOperation::ENTITY_BATCH_REMOVE;
        op.m_entities = entities;
        op.m_count = count;
        queueOperation(&op);
        return;
    }
    lockCriticalOperations();
    HK_TIMER_BEGIN_LIST("RemEntities", "Init+CallBck");
    m_simulation->resetCollisionInformationForEntities(entities, count, this, true);
    {
        hkLocalArray<hkBroadPhaseHandle*> collidablesForBroadphase(count);
        for (hkEntity **e = entities, **end = entities + count; e != end; e++) {
            hkEntity* entity = *e;
            if (entity->m_collidable.m_shape != NULL) {
                collidablesForBroadphase.pushBackUnchecked(&entity->m_collidable.m_broadPhaseHandle);
            }
        }
        HK_TIMER_SPLIT_LIST("BroadPhase");
        hkLocalArray<hkBroadPhaseHandlePair> pairsOut(m_unk2E4);
        m_broadPhase->removeObjectBatch(collidablesForBroadphase, pairsOut);
        HK_TIMER_SPLIT_LIST("DelAgents");
        m_broadPhaseDispatcher->removePairs(pairsOut.begin(), pairsOut.getSize());
    }
    HK_TIMER_SPLIT_LIST("RemoveCb");
    hkEntity** end = entities + count;
    for (hkEntity** e = entities; e != end; e++) {
        hkWorldCallbackUtil::fireEntityRemoved(this, *e);
        hkEntityCallbackUtil::fireEntityRemoved(*e);
        hkWorldOperationUtil::removeEntitySI(this, *e);
        if ((*e)->m_memSizeAndFlags == 0) {
            (*e)->deallocateInternalArrays();
        }
        (*e)->removeReference();
    }
    HK_TIMER_END_LIST();
    unlockAndAttemptToExecutePendingOperations();
}

// Statistics collector slots the world reports through (local view). HYPOTHESIS: the names.
struct hkWorldStatisticsCollector {
    virtual void unk08();
    virtual void beginObject(const char* name, int type, const void* object);                    // 0x0C
    virtual void addArray(const char* name, int type, const void* data, int size, int capacity); // 0x10
    virtual void addChildObject(const char* name, int type, const void* object);                 // 0x14
    virtual void pushDir(const char* name);                                                      // 0x18
    virtual void popDir();                                                                       // 0x1C
    virtual void endObject();                                                                    // 0x20

    // An array's storage counts only when the array owns it.
    void addArrayStorage(const char* name, int type, const hkArrayBase& array, int elemSize) {
        if (!(array.m_capacityAndFlags & hkArrayBase::DONT_DEALLOCATE_FLAG)) {
            addArray(name, type, array.m_data, array.m_size * elemSize,
                     (array.m_capacityAndFlags & hkArrayBase::CAPACITY_MASK) * elemSize);
        }
    }
};

// The world's listener arrays and pieces not laid out in hkWorld.h, by offset.
static inline const hkArrayBase& worldArrayAt(const hkWorld* world, int offset) {
    return *(const hkArrayBase*)((const char*)world + offset);
}
static inline const void* worldPointerAt(const hkWorld* world, int offset) {
    return *(void* const*)((const char*)world + offset);
}

void hkWorld::calcStatistics(hkStatisticsCollector* collector) const {
    hkWorldStatisticsCollector* c = (hkWorldStatisticsCollector*)collector;
    c->beginObject("hkWorld", 2, this);
    for (int i = m_unk28.m_size - 1; i >= 0; i--) {
        c->addChildObject("Active", 8, ((hkSimulationIsland**)m_unk28.m_data)[i]);
    }
    for (int i = m_unk34.m_size - 1; i >= 0; i--) {
        c->addChildObject("Inactive", 8, ((hkSimulationIsland**)m_unk34.m_data)[i]);
    }
    c->addChildObject("Fixed", 8, m_fixedIsland);
    c->addArrayStorage("PhantomPtrs", 4, m_phantoms, 4);
    for (int i = 0; i < m_phantoms.getSize(); i++) {
        c->addChildObject("Phantoms", 2, m_phantoms[i]);
    }
    c->pushDir("Internal");
    c->addChildObject("Simulation", 4, m_simulation);
    c->addArrayStorage("DirtyIslPtr", 8, m_unk40, 4);
    c->addChildObject("MaintnceMgr", 4, m_maintenanceMgr);
    c->addArray("OpQueue", 4, m_operationQueue, 0x20, 0);
    c->pushDir("OpQueue");
    c->addArrayStorage("Operations", 8, m_operationQueue->m_operations0, sizeof(hkWorldOperation::BiggestOperation));
    c->popDir();
    c->pushDir("Collide");
    c->addChildObject("Broadphase", 4, m_broadPhase);
    c->addArray("BpDispatch", 4, m_broadPhaseDispatcher, 0x104, 0);
    c->addArray("BpDispatch", 4, worldPointerAt(this, 0x60), 0xC, 0);
    c->addArray("BpDispatch", 4, worldPointerAt(this, 0x5C), 8, 0);
    c->addArray("BpDispatch", 4, worldPointerAt(this, 0x64), 8, 0);
    c->addArray("CollInput", 4, m_collisionInput, 0x30, 0);
    c->addChildObject("Filter", 1, m_collisionFilter);
    c->addChildObject("Dispatcher", 4, m_collisionDispatcher);
    c->popDir();
    c->popDir();
    c->pushDir("Listeners");
    c->addArrayStorage("EntLisPtr", 4, worldArrayAt(this, 0xE4), 4);
    c->addArrayStorage("PhantLisPtr", 4, worldArrayAt(this, 0xF0), 4);
    c->addArrayStorage("ConstrLisPtr", 4, worldArrayAt(this, 0xFC), 4);
    c->addArrayStorage("WldDelLisPtr", 4, m_worldDeletionListeners, 4);
    c->addArrayStorage("IslActLisPtr", 4, worldArrayAt(this, 0x114), 4);
    c->addArrayStorage("PstSimLisPtr", 4, m_postSimulationListeners, 4);
    c->addArrayStorage("PstIntLisPtr", 4, m_postIntegrateListeners, 4);
    c->addArrayStorage("PstColLisPtr", 4, m_postCollideListeners, 4);
    c->addArrayStorage("PstIntLisPtr", 4, m_islandPostIntegrateListeners, 4);
    c->addArrayStorage("CollLisPtr", 4, worldArrayAt(this, 0x15C), 4);
    c->popDir();
    c->endObject();
}

void hkWorld::setGravity(const hkVector4& gravity) {
    m_gravity = gravity;
}

#pragma dont_inline on
void hkWorld::queueOperation(const void* operation) { // out of line in the original
    m_operationQueue->queueOperation(*static_cast<const hkWorldOperation::BaseOperation*>(operation));
}
#pragma dont_inline reset

void hkWorld::addWorldDeletionListener(hkWorldDeletionListener* listener) {
    m_worldDeletionListeners.pushBack(listener);
}

void hkWorld::removeWorldDeletionListener(hkWorldDeletionListener* listener) {
    // HYPOTHESIS: an entry is cleared in place (no compaction).
    int index = m_worldDeletionListeners.indexOf(listener);
    m_worldDeletionListeners[index] = NULL;
}


// returns NULL when the add got queued because the world was locked

hkConstraintInstance* hkWorld::addConstraint(hkConstraintInstance* constraint) {
    if (areCriticalOperationsLocked()) {
        hkWorldOperationRecord op;
        op.m_type = hkWorldOperation::CONSTRAINT_ADD;
        op.m_object = constraint;
        queueOperation(&op);
        return NULL;
    }

    // Older assets leave the child sizes of a breakable constraint at 0: fill them on the first add.
    if (constraint->getData()->getType() == 12) {
        hkBreakableConstraintData* data = static_cast<hkBreakableConstraintData*>(constraint->getData());
        if (data->m_childRuntimeSize == 0) {
            hkConstraintRuntimeInfo info;
            data->m_constraintData->getRuntimeInfo(true, info);
            data->m_childRuntimeSize = info.unk00;
            data->m_childNumSolverResults = info.unk04;
        }
    }

    hkConstraintInstance* result;
    m_unk84 = true;
    {
        constraint->pointNullsToFixedRigidBody();
        result = hkWorldOperationUtil::addConstraintImmediately(this, constraint, true);
        if (constraint->getType() == hkConstraintInstance::TYPE_CHAIN) {
            hkConstraintChainInstance* chain = static_cast<hkConstraintChainInstance*>(constraint);
            if (chain->m_action->m_world == NULL) {
                addAction(chain->m_action);
            }
            if (chain->m_chainedEntities.getSize() - 1 < chain->getData()->getNumConstraintInfos()) {
                // (a warning in debug builds)
            }
        }
    }
    m_unk84 = false;
    if (m_lockCount == 0 && m_pendingOperationsCount != 0 && !(bool)m_unk84) {
        internal_executePendingOperations();
    }
    return result;
}

// BYTES: not an inline candidate (removePhysicsSystem calls it out of line).
#pragma push
#pragma auto_inline off
hkBool hkWorld::removeConstraint(hkConstraintInstance* constraint) {
    if (areCriticalOperationsLocked()) {
        hkWorldOperationRecord op;
        op.m_type = hkWorldOperation::CONSTRAINT_REMOVE;
        op.m_object = constraint;
        queueOperation(&op);
        return false;
    }

    lockCriticalOperations();
    if (constraint->getType() == hkConstraintInstance::TYPE_CHAIN) {
        // the chain's action goes with it
        hkAction* action = static_cast<hkConstraintChainInstance*>(constraint)->m_action;
        if (action->m_world == this) {
            removeActionImmediately(action);
        }
    }
    hkWorldOperationUtil::removeConstraintImmediately(this, constraint, true);
    unlockAndAttemptToExecutePendingOperations();
    return true;
}
#pragma pop

hkPhantom* hkWorld::addPhantom(hkPhantom* phantom) {
    if (areCriticalOperationsLockedForPhantoms()) {
        hkWorldOperationRecord op;
        op.m_type = hkWorldOperation::PHANTOM_ADD;
        op.m_object = phantom;
        queueOperation(&op);
        return NULL;
    }

    lockCriticalOperations();

    // a phantom that came out of a packfile may not have its collidable pointed at a motion state yet
    if (phantom->m_collidable.m_motion == NULL) {
        phantom->m_collidable.m_motion = phantom->getMotionState();
    }

    phantom->m_world = this;
    phantom->addReference();
    m_phantoms.pushBack(phantom);

    hkWorldOperationUtil::addPhantomBP(this, phantom);

    hkWorldCallbackUtil::firePhantomAdded(this, phantom);
    phantom->firePhantomAdded();

    unlockAndAttemptToExecutePendingOperations();
    return phantom;
}

// Adds a batch of phantoms: each gets its motion state pointer, world and reference, joins the world's list and
// gets the added callbacks; then they enter the broad phase together and the new pairs get agents.
void hkWorld::addPhantomBatch(hkPhantom** phantoms, int count) {
    if (areCriticalOperationsLockedForPhantoms()) {
        struct {
            u8 m_type;
            hkPhantom** m_phantoms;
            u16 m_count;
        } op;
        op.m_type = hkWorldOperation::PHANTOM_BATCH_ADD;
        op.m_phantoms = phantoms;
        op.m_count = count;
        queueOperation(&op);
        return;
    }
    lockCriticalOperations();
    hkLocalArray<hkBroadPhaseHandle*> collidablesForBroadphase(count);
    hkLocalArray<hkAabb> aabbsForBroadphase(count);
    aabbsForBroadphase.m_size = count;
    collidablesForBroadphase.m_size = count;
    for (int i = 0; i < count; i++) {
        hkPhantom* phantom = phantoms[i];
        if (phantom->m_collidable.m_motion == NULL) {
            phantom->m_collidable.m_motion = phantom->getMotionState();
        }
        phantom->m_world = this;
        collidablesForBroadphase[i] = &phantom->m_collidable.m_broadPhaseHandle;
        phantom->calcAabb(aabbsForBroadphase[i]);
        phantom->addReference();
        m_phantoms.pushBack(phantom);
        hkWorldCallbackUtil::firePhantomAdded(this, phantom);
        phantom->firePhantomAdded();
    }
    hkLocalArray<hkBroadPhaseHandlePair> newPairs(m_unk2E4);
    m_broadPhase->addObjectBatch(collidablesForBroadphase, aabbsForBroadphase, newPairs);
    const hkCollidableCollidableFilter* filter = m_collisionFilter;
    m_broadPhaseDispatcher->addPairs(newPairs.begin(), newPairs.getSize(), filter);
    unlockAndAttemptToExecutePendingOperations();
}

// Removes a batch of phantoms: the removed callbacks run first, then they leave the broad phase together (the lost
// pairs' agents are deleted) and finally each leaves the world's list and is released.
void hkWorld::removePhantomBatch(hkPhantom** phantoms, int count) {
    if (areCriticalOperationsLockedForPhantoms()) {
        struct {
            u8 m_type;
            hkPhantom** m_phantoms;
            u16 m_count;
        } op;
        op.m_type = hkWorldOperation::PHANTOM_BATCH_REMOVE;
        op.m_phantoms = phantoms;
        op.m_count = count;
        queueOperation(&op);
        return;
    }
    lockCriticalOperations();
    hkLocalArray<hkBroadPhaseHandle*> collidablesForBroadphase(count);
    for (int i = 0; i < count; i++) {
        hkPhantom* phantom = phantoms[i];
        collidablesForBroadphase.pushBackUnchecked(&phantom->m_collidable.m_broadPhaseHandle);
        hkWorldCallbackUtil::firePhantomRemoved(this, phantom);
        phantom->firePhantomRemoved();
    }
    {
        hkLocalArray<hkBroadPhaseHandlePair> pairsOut(m_unk2E4);
        m_broadPhase->removeObjectBatch(collidablesForBroadphase, pairsOut);
        m_broadPhaseDispatcher->removePairs(pairsOut.begin(), pairsOut.getSize());
    }
    for (int i = 0; i < count; i++) {
        hkPhantom* phantom = phantoms[i];
        phantom->m_world = NULL;
        m_phantoms.removeAt(m_phantoms.indexOf(phantom));
        if (phantom->m_memSizeAndFlags == 0) {
            phantom->deallocateInternalArrays();
        }
        phantom->removeReference();
    }
    unlockAndAttemptToExecutePendingOperations();
}

void hkWorld::removePhantom(hkPhantom* phantom) {
    if (areCriticalOperationsLockedForPhantoms()) {
        hkWorldOperationRecord op;
        op.m_type = hkWorldOperation::PHANTOM_REMOVE;
        op.m_object = phantom;
        queueOperation(&op);
        return;
    }

    lockCriticalOperations();

    hkWorldCallbackUtil::firePhantomRemoved(this, phantom);
    phantom->firePhantomRemoved();

    hkWorldOperationUtil::removePhantomBP(this, phantom);

    // take it out of the list: last one gets moved into the hole
    m_phantoms.removeAt(m_phantoms.indexOf(phantom));
    phantom->m_world = NULL;

    // loaded from a packfile (memSize 0) means it might still own empty arrays that need freeing
    if (phantom->m_memSizeAndFlags == 0) {
        phantom->deallocateInternalArrays();
    }
    phantom->removeReference();

    unlockAndAttemptToExecutePendingOperations();
}

// raw field access for the world's members that nobody has laid out yet (offsets are from the asm)
template <typename T>
static inline T& worldField(void* w, int offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<char*>(w) + offset);
}


// wakes up every entity in the box. while the world is locked the box is copied and queued instead
void hkWorld::activateRegion(hkAabb* aabb) {
    if (m_lockCount != 0) {
        hkWorldOperation::SingleObjectOperation op;
        op.m_type = hkWorldOperation::ACTIVATE_REGION;
        op.m_object = hkMemory::getInstance().allocateChunk(0x20, 4);
        hkString::memCpy(op.m_object, aabb, 0x20);
        queueOperation(&op);
        return;
    }

    hkArray<hkBroadPhaseHandlePair> hits;
    // HYPOTHESIS: world +0x54 is the broadphase
    worldField<hkBroadPhase*>(this, 0x54)->querySingleAabb(*aabb, hits);

    for (int i = 0; i < hits.m_size; i++) {
        // handle -> collidable (a signed byte offset stored in the handle) -> owner, only if it is an entity (type 1)
        char* handle = reinterpret_cast<char*>(hits[i].m_b);
        char* collidable = handle + static_cast<s8>(handle[5]);
        hkEntity* entity;
        if (collidable[0x18] == 1) {
            entity = reinterpret_cast<hkEntity*>(collidable + *reinterpret_cast<int*>(collidable + 0x10));
        } else {
            entity = NULL;
        }
        if (entity != NULL) {
            entity->activate();
        }
    }
}

// The two virtual calls made by stepDeltaTime. HYPOTHESIS: names from what the calls do (step the
// simulation, ask the watchdog to free memory); only slot 0x10 of each vtable is used.
struct hkWorldStepSimulation : hkReferencedObject {
    virtual int stepDeltaTime(hkReal physicsDeltaTime); // 0x10
};
struct hkWorldFreeMemoryWatchDog : hkReferencedObject {
    int m_memoryLimit; // 0x08
    virtual void freeMemory(hkWorld* world); // 0x10
};

// Runs the simulation for one step. When that succeeded and the memory in use (hkMemory 0x28 + 0x14) is over
// the watchdog's limit, the watchdog is asked to free memory, bracketed by a timer.
// MATCH-ONLY: the watchdog timer strings are writable data in the original (the data and small data sections),
// not part of the unit's read-only string pool.
static char s_watchDogTimerBegin[] = "TtWatchDog:FreeMem";
static char s_watchDogTimerEnd[] = "Et";

int hkWorld::stepDeltaTime(hkReal physicsDeltaTime) {
    int result = ((hkWorldStepSimulation*)m_simulation)->stepDeltaTime(physicsDeltaTime);
    if (result == 0) {
        hkWorldFreeMemoryWatchDog* watchDog = worldField<hkWorldFreeMemoryWatchDog*>(this, 0x50);
        if (watchDog != NULL) {
            hkMemory& memory = hkMemory::getInstance();
            if (worldField<int>(&memory, 0x28) + worldField<int>(&memory, 0x14) > watchDog->m_memoryLimit) {
                HK_MONITOR_TIMER_COMMAND(s_watchDogTimerBegin);
                watchDog->freeMemory(this);
                HK_MONITOR_TIMER_COMMAND(s_watchDogTimerEnd);
            }
        }
    }
    return result;
}

hkAction* hkWorld::addAction(hkAction* action) {
    if (areCriticalOperationsLocked()) {
        hkWorldOperationRecord op;
        op.m_type = hkWorldOperation::ACTION_ADD;
        op.m_object = action;
        queueOperation(&op);
        return NULL;
    }

    action->addReference();
    lockCriticalOperations();

    hkInplaceArray<hkEntity*, 4> entities;
    action->getEntities(entities);
    action->m_world = this;

    hkEntity* firstMovableEntity = NULL;
    for (int i = 0; i < entities.getSize(); ++i) {
        entities[i]->m_actions.pushBack(action);
        hkSimulationIsland* island = entities[i]->m_simulationIsland;
        if (!(bool)island->isFixed()) {
            if (firstMovableEntity == NULL) {
                firstMovableEntity = entities[i];
                island->addAction(action);
            } else if (firstMovableEntity->m_simulationIsland != entities[i]->m_simulationIsland) {
                // the action joins the islands of its entities
                hkWorldOperationUtil::mergeIslands(this, firstMovableEntity, entities[i]);
            }
        }
    }

    // When all entities are fixed, the action goes to the fixed island.
    if (firstMovableEntity == NULL) {
        entities[0]->m_simulationIsland->addAction(action);
    }

    unlockAndAttemptToExecutePendingOperations();
    hkWorldCallbackUtil::fireActionAdded(this, action);
    return action;
}

void hkWorld::removeActionImmediately(hkAction* action) {
    // temporary reference while the callbacks run
    action->addReference();
    lockCriticalOperations();
    hkWorldCallbackUtil::fireActionRemoved(this, action);
    {
        hkInplaceArray<hkEntity*, 4> entities;
        action->getEntities(entities);
        for (int i = 0; i < entities.getSize(); ++i) {
            int idx = entities[i]->m_actions.indexOf(action);
            entities[i]->m_actions.removeAt(idx);
        }

        hkSimulationIsland* island = action->m_island;
        island->removeAction(action);
        action->m_world = NULL;
        action->removeReference();

        // put the island on the world's dirty island list (an array at 0x40) for the end-of-step bookkeeping
        hkWorld* world = island->m_world;
        if (island->m_dirtyListIndex == 0xFFFF) {
            island->m_dirtyListIndex = world->m_unk40.m_size;
            ((hkArray<hkSimulationIsland*>&)world->m_unk40).pushBack(island);
        }
    }
    unlockAndAttemptToExecutePendingOperations();
    action->removeReference();
}

#pragma dont_inline on
void hkWorld::removeAction(hkAction* action) {
    if (m_lockCount != 0) {
        hkWorldOperationRecord op;
        op.m_type = hkWorldOperation::ACTION_REMOVE;
        op.m_object = action;
        queueOperation(&op);
    } else {
        removeActionImmediately(action);
    }
}
#pragma dont_inline reset

// HYPOTHESIS: post-add step of the physics system, not recovered (called with the system's active flag).
extern "C" void fn_802F9098(hkWorld* world, hkBool active);

void hkWorld::addPhysicsSystem(hkPhysicsSystem* system) {
    if (system->m_rigidBodies.m_size > 0) {
        addEntityBatch((hkEntity**)system->m_rigidBodies.m_data, system->m_rigidBodies.m_size, system->m_active);
    }
    if (system->m_phantoms.m_size > 0) {
        addPhantomBatch((hkPhantom**)system->m_phantoms.m_data, system->m_phantoms.m_size);
    }
    for (int i = 0; i < system->m_actions.m_size; i++) {
        hkAction* action = ((hkAction**)system->m_actions.m_data)[i];
        if (action != NULL) {
            addAction(action);
        }
    }
    for (int i = 0; i < system->m_constraints.m_size; i++) {
        addConstraint(((hkConstraintInstance**)system->m_constraints.m_data)[i]);
    }
    if (system->m_rigidBodies.m_size > 0) {
        fn_802F9098(this, system->m_active);
    }
}

void hkWorld::removePhysicsSystem(hkPhysicsSystem* system) {
    for (int i = 0; i < system->m_constraints.m_size; i++) {
        removeConstraint(((hkConstraintInstance**)system->m_constraints.m_data)[i]);
    }
    for (int i = 0; i < system->m_actions.m_size; i++) {
        removeAction(((hkAction**)system->m_actions.m_data)[i]);
    }
    int rigidBodyCount = system->m_rigidBodies.m_size;
    if (rigidBodyCount < 4) {
        for (int i = 0; i < system->m_rigidBodies.m_size; i++) {
            removeEntity(((hkEntity**)system->m_rigidBodies.m_data)[i]);
        }
    } else {
        removeEntityBatch((hkEntity**)system->m_rigidBodies.m_data, system->m_rigidBodies.m_size);
    }
    if (system->m_phantoms.m_size > 0) {
        removePhantomBatch((hkPhantom**)system->m_phantoms.m_data, system->m_phantoms.m_size);
    }
}

// Per-entry record of the 0x28 array (HYPOTHESIS: field names and types, only the read offsets are known).
struct hkWorldMemoryEntry {
    u8 m_unk00[0x0C];  // 0x00
    int m_unk0C;       // 0x0C
    int m_unk10;       // 0x10
    int m_unk14;       // 0x14 (scaled by 4)
    u8 m_unk18[0x24];  // 0x18 .. 0x3B
    int m_unk3C;       // 0x3C (scaled by 128)
};

int hkWorld::getMemUsageForIntegration() {
    int result = 0;
    for (int i = m_unk28.m_size - 1; i >= 0; i--) {
        hkWorldMemoryEntry* entry = ((hkWorldMemoryEntry**)m_unk28.m_data)[i];
        int part = (entry->m_unk14 << 2) + entry->m_unk10;
        int value = entry->m_unk0C + (entry->m_unk3C << 7) + part;
        value += 0x9C;
        result = (result > value) ? result : value;
    }
    return result;
}


// Null contact manager and its factory. Declared locally (no header yet). The vtables are lbl_80488348
// (dtor, calcStatistics, then the eight slots in the order below) and lbl_80488330 (dtor, calcStatistics,
// createContactMgr); their deleting destructors free class ids 0x20 and 0x04. Every manager method is a no-op
// returning a fixed value; the factory hands out the manager embedded at +0x08.
// HYPOTHESIS: the real bases are hkContactMgr / hkContactMgrFactory (hkCollisionAgent.h, whose slot parameters
// are not known well enough to override them here).
struct hkNullContactMgr : hkReferencedObject {
    HK_DECLARE_REF_ALLOCATOR(0x20)
    virtual bool addContactPoint();      // 0x10
    virtual bool reserveContactPoints(); // 0x14
    virtual void removeContactPoint();   // 0x18
    virtual void processContact();       // 0x1C
    virtual void cleanup();              // 0x20
    virtual bool addToi();               // 0x24
    virtual void removeToi();            // 0x28
    virtual void processToi();           // 0x2C
};

struct hkNullContactMgrFactory : hkReferencedObject {
    hkNullContactMgr m_contactMgr; // 0x08

    HK_DECLARE_REF_ALLOCATOR(0x04)
    virtual hkNullContactMgr* createContactMgr(); // 0x10
};

hkNullContactMgr* hkNullContactMgrFactory::createContactMgr() {
    return &m_contactMgr;
}

void hkNullContactMgr::cleanup() {}

bool hkNullContactMgr::reserveContactPoints() {
    return false;
}

void hkNullContactMgr::processToi() {}

void hkNullContactMgr::removeToi() {}

bool hkNullContactMgr::addToi() {
    return true;
}

void hkNullContactMgr::processContact() {}

void hkNullContactMgr::removeContactPoint() {}

bool hkNullContactMgr::addContactPoint() {
    return false;
}

// Unnamed static flag (lbl_805A0F20); initialised through hkBool(bool) so the TU gets a __sinit.
// HYPOTHESIS: a debug/config switch of the unit; nothing in the written code reads it yet.
static hkBool s_hkWorld_unk(false);

// Deleting destructors of the two broad-phase listeners hkWorld creates (classes not modelled yet; sized delete,
// allocation class 0x1f). Shape from https://github.com/jj-link/brawl src/havok/fn_802EB0E8.cpp / fn_802EB144.cpp.
// hkEntityEntityBroadPhaseListener::~hkEntityEntityBroadPhaseListener
// TODO: cleanup (shape from jj-link/brawl fn_802EB0E8)
extern "C" hkReferencedObject* fn_802EB0E8(hkReferencedObject* self, int deleting) {
    if (self != 0 && deleting > 0) {
        hkMemory::s_instance->deallocateChunk(self, self->m_memSizeAndFlags, 0x1f);
    }
    return self;
}
// hkBroadPhaseBorderListener::~hkBroadPhaseBorderListener
// TODO: cleanup (shape from jj-link/brawl fn_802EB144)
extern "C" hkReferencedObject* fn_802EB144(hkReferencedObject* self, int deleting) {
    if (self != 0 && deleting > 0) {
        hkMemory::s_instance->deallocateChunk(self, self->m_memSizeAndFlags, 0x1f);
    }
    return self;
}

// fn_802EB810: body from https://github.com/jj-link/brawl src/havok/fn_802EB810.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802EB810 {
struct Param {
    char padding0[0x10];
    int offset;
    char padding1[4];
    signed char state;
};

struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6(Param*);
    virtual unsigned int v7();
    virtual void v8(Param*);
};

struct Target {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6(void*);
    virtual unsigned int v7(void*);
    virtual void v8(void*);
};

struct ObjB {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual unsigned int v18(void*, Param*);
};

// TODO: cleanup (shape from jj-link/brawl fn_802EB810)
extern "C" void fn_802EB810(Obj* a, Param* p, ObjB* b) {
    unsigned int c1 = a->v7() >> 24;
    void* field = (char*)a + 0x1c;

    if ((signed char)(b->v18(field, p) >> 24) != 0) {
        if ((signed char)c1 == 0) {
            a->v6(p);
        }
        if (p->state == 2) {
            if ((signed char)(((Target*)((char*)p + p->offset))->v7(field) >> 24) == 0) {
                ((Target*)((char*)p + p->offset))->v6(field);
            }
        }
    } else {
        if ((signed char)c1 != 0) {
            a->v8(p);
        }
        if (p->state == 2) {
            if ((signed char)(((Target*)((char*)p + p->offset))->v7(field) >> 24) != 0) {
                ((Target*)((char*)p + p->offset))->v8(field);
            }
        }
    }
}
} // namespace jj_fn_802EB810

// TODO: cleanup (shape from jj-link/brawl fn_802EFD3C)
// fn_802EFD3C: body from https://github.com/jj-link/brawl src/havok/fn_802EFD3C.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
#include <havok/hkWorld.h>
namespace jj_fn_802EFD3C {
#pragma use_lmw_stmw on
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
class SoHeapAllocator {
public:
 virtual void vf0();
 virtual void vf1();
 virtual void vf2();
 virtual void vf3();
 virtual void* alloc(int, int);
};
extern "C" char lbl_80487320[];
struct Ref {
 virtual void destroy(int);
 u16 active;
 s16 refs;
 void* a;
 void* b;
 void* c;
 void* d;
};
struct Other { void* a; void* b; };
struct Self { u8 pad[0x68]; Other* other; Ref* ref; };
extern "C" void fn_802EFD3C(Self* self, Ref* arg, char* flag, u8 x, u8 y) {
 if (!arg) {
  Ref* p = (Ref*)((SoHeapAllocator*)hkMemory::s_instance)->alloc(0x18, 0x25);
  p->active = 0x18;
  arg = p;
  if (p) {
   p->refs = 1;
   *(void**)p = lbl_80487320;
   p->a = lbl_80487320 + 0x10;
   p->b = lbl_80487320 + 0x20;
   p->c = lbl_80487320 + 0x30;
   p->d = lbl_80487320 + 0x40;
  }
 } else if (arg->active) {
  arg->refs = arg->refs + 1;
 }
 Ref* p = self->ref;
 if (p->active) {
  p->refs = p->refs - 1;
  if (p->refs == 0 && p != 0) p->destroy(1);
 }
 self->ref = arg;
 void* ptr = arg;
 if (arg) ptr = (char*)arg + 0xc;
 char f = *flag;
 self->other->b = ptr;
 if (f) ((hkWorld*)(self))->updateCollisionFilterOnWorld(x, y);
}
} // namespace jj_fn_802EFD3C

// TODO: cleanup (shape from jj-link/brawl fn_802EB1A0)
// fn_802EB1A0: body from https://github.com/jj-link/brawl src/havok/fn_802EB1A0.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
#include <havok/hkWorld.h>
#include <havok/hkSimpleConstraintContactMgr.h>
#include <havok/hkReportContactMgr.h>
#include <havok/hkMemory.h>
namespace jj_fn_802EB1A0 {
class Alloc {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void* alloc(int size, int align);
};
class Object_vptr_0 {
public:
    virtual void slot0(int);
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    unsigned short size;
    short refs;
    void* unk8;
    short pad12;
    short unkE;
};
extern "C" char lbl_80488330[];
extern "C" char lbl_80488348[];
extern "C" void fn_802CC8DC(void*, void*, int);
extern "C" void fn_802EB1A0(int arg0, void* arg1) {
    Object_vptr_0* p1 = (Object_vptr_0*)new hkSimpleConstraintContactMgr::Factory((hkWorld*)arg0);
    Object_vptr_0* p2 = (Object_vptr_0*)new hkReportContactMgr::Factory((hkWorld*)arg0);
    Object_vptr_0* p3 = (Object_vptr_0*)((Alloc*)hkMemory::s_instance)->alloc(0x10, 4);
    p3->size = 0x10;
    if (p3 != 0) {
        p3->refs = 1;
        *(void**)p3 = lbl_80488330;
        p3->unkE = 1;
        p3->unk8 = lbl_80488348;
    }
    fn_802CC8DC(arg1, p1, 1);
    fn_802CC8DC(arg1, p2, 2);
    fn_802CC8DC(arg1, p3, 3);
    if (p1->size != 0) {
        if (--p1->refs == 0 && p1 != 0) {
            p1->slot0(1);
        }
    }
    if (p2->size != 0) {
        if (--p2->refs == 0 && p2 != 0) {
            p2->slot0(1);
        }
    }
    if (p3->size != 0) {
        if (--p3->refs == 0 && p3 != 0) {
            p3->slot0(1);
        }
    }
}
} // namespace jj_fn_802EB1A0

// Blocks of the world freed with this helper (memory class 0x13). HYPOTHESIS: what they hold.
static inline void freeWorldBlock(void* block) {
    if (block != 0) {
        hkMemory::getInstance().deallocateChunk(block, 1, 0x13);
    }
}

// Removes everything still in the world (phantoms, entities, the fixed body and the fixed island), tells the
// deletion listeners, then releases the broad phase, the dispatchers, the filter, the collision input, the simulation
// and the operation queue.
hkWorld::~hkWorld() {
    m_maintenanceMgr->removeReference();
    m_maintenanceMgr = NULL;
    if (m_unk9C != NULL) {
        freeWorldBlock(m_unk9C);
        m_unk9C = NULL;
    }
    if (m_unkA4 != NULL) {
        freeWorldBlock(m_unkA4);
        m_unkA4 = NULL;
        for (int i = 0; i < m_unkA8.getSize(); i++) {
            void* block = m_unkA8[i].m_unk0C;
            if (block != NULL) {
                hkMemory::getInstance().deallocateChunk(block, 1, 0x13);
            }
        }
    }
    m_unkA8.clear();
    if (m_unkA0 != NULL) {
        freeWorldBlock(m_unkA0);
        m_unkA0 = NULL;
    }
    hkReferencedObject*& watchDog = worldField<hkReferencedObject*>(this, 0x50);
    if (watchDog != NULL) {
        watchDog->removeReference();
        watchDog = NULL;
    }

    while (m_phantoms.getSize() > 0) {
        removePhantom(m_phantoms[0]);
    }
    while (m_unk28.getSize() > 0 && m_unk28[0]->m_entities.getSize() != 0) {
        removeEntity(m_unk28[0]->m_entities[0]);
    }
    while (m_unk34.getSize() > 0) {
        removeEntity(m_unk34[0]->m_entities[0]);
    }
    if (!(bool)m_wantSimulationIslands) {
        hkWorldOperationUtil::removeIslandFromDirtyList(this, m_unk28[m_unk28.getSize() - 1]);
        delete m_unk28[m_unk28.getSize() - 1];
        m_unk28.m_size--;
    }
    removeEntity((hkEntity*)m_fixedRigidBody);
    m_fixedRigidBody = NULL;
    while (m_fixedIsland->m_entities.getSize() > 0) {
        removeEntity(m_fixedIsland->m_entities[0]);
    }
    hkWorldOperationUtil::removeIslandFromDirtyList(this, m_fixedIsland);
    delete m_fixedIsland;
    m_fixedIsland = NULL;

    hkWorldCallbackUtil::fireWorldDeleted(this);

    m_broadPhase->removeReference();
    m_broadPhase = NULL;
    m_collisionDispatcher->removeReference();
    m_collisionDispatcher = NULL;
    delete m_broadPhaseDispatcher;
    if (m_unk168 != NULL) {
        m_unk168->removeReference();
    }
    delete m_unk5C;
    delete m_unk60;
    delete m_unk64;
    m_collisionFilter->removeReference();
    hkMemory::getInstance().deallocateChunk(*(void**)((char*)m_collisionInput + 0x20), 8, 0x1d);
    hkMemory::getInstance().deallocateChunk(m_collisionInput, 0x30, 0x1d);
    m_simulation->removeReference();
    delete m_operationQueue;
}
