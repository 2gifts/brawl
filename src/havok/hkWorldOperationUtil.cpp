// Havok translation unit hkWorldOperationUtil.o (main.dol 0x802F7E24-0x802FB244).
// Work in progress. Functions in address order:
//   0x802F7E24  updateEntityBP, addEntityBP, addPhantomBP, addEntitySI, removeEntityBP, removePhantomBP,
//   removeEntitySI, removeAttachedActionsFromFixedIsland, removeAttachedActionsFromDynamicIsland,
//   addActionsToEntitysIsland, removeIsland, addConstraintToCriticalLockedIsland,
//   removeConstraintFromCriticalLockedIsland, addConstraintImmediately, removeConstraintImmediately,
//   splitSimulationIslands, splitSimulationIslands1, mergeIslands, internalMergeTwoIslands,
//   setRigidBodyMotionType, removeAttachedConstraints, ..., removeIslandFromDirtyList, ...

#include <havok/hkWorldOperationUtil.h>
#include <havok/hkWorldCallbackUtil.h>
#include <havok/hkWorldConstraintUtil.h>
#include <havok/hkSimulationIsland.h>
#include <havok/hkEntity.h>
#include <havok/hkAction.h>
#include <havok/hkSweptTransformUtil.h>
#include <havok/hkWorldAgentUtil.h>
#include <havok/hkLinkedCollidable.h>
#include <havok/hkConstraintInstance.h>
#include <havok/hkLocalArray.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkBroadPhase.h>
#include <havok/hkPhantom.h>
#include <havok/hkSimulation.h>
#include <havok/hkShape.h>
#include <havok/hkTypedBroadPhaseDispatcher.h>
#include <havok/hkRigidBody.h>
#include <havok/hkAgentNnMachine.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkKeyframedRigidMotion.h>
#include <havok/hkFixedRigidMotion.h>
#include <havok/hkString.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkConstraintInternal.h>
#include <havok/hkUnionFind.h>

// HYPOTHESIS: the top two bits of the byte at island+0x26/+0x27 hold the activity state, written as a 2-bit field.
// Local view only; hkSimulationIsland.h does not declare these bytes yet.
struct hkIslandActivityByte {
    u8 activity : 2;
    u8 rest : 6;
};

// world members nobody has laid out yet, and the broadphase pieces behind them (raw offsets from the asm)
template <typename T>
static inline T& at(void* p, int offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<char*>(p) + offset);
}


// Moves the entity's broad-phase box to where its shape is now and hands the pairs that appeared and disappeared
// to the dispatcher (pairs in both lists cancel out). Queued while the world is locked.
void hkWorldOperationUtil::updateEntityBP(hkWorld* world, hkEntity* entity) {
    if (world->m_lockCount != 0) {
        hkWorldOperation::SingleObjectOperation op;
        op.m_type = hkWorldOperation::ENTITY_UPDATE_BROAD_PHASE;
        op.m_object = entity;
        world->queueOperation(&op);
        return;
    }
    world->lockCriticalOperations();
    if (entity->m_collidable.m_shape != NULL) {
        hkLocalArray<hkBroadPhaseHandlePair> newPairs(world->m_unk2E4);
        hkLocalArray<hkBroadPhaseHandlePair> delPairs(world->m_unk2E4);

        hkAabb aabb;
        entity->m_collidable.m_shape->getAabb(*reinterpret_cast<const hkTransform*>(entity->m_collidable.m_motion),
                                              0.5f * world->m_collisionInput->getTolerance(), aabb);
        hkBroadPhaseHandle* handle = &entity->m_collidable.m_broadPhaseHandle;
        world->m_broadPhase->updateAabbs(&handle, &aabb, 1, newPairs, delPairs);

        if (newPairs.getSize() + delPairs.getSize() > 0) {
            hkTypedBroadPhaseDispatcher::removeDuplicates(newPairs, delPairs);
            world->m_broadPhaseDispatcher->removePairs(delPairs.begin(), delPairs.getSize());
            void* filter = world->m_collisionFilter;
            if (filter != NULL) {
                filter = reinterpret_cast<char*>(filter) + 8;
            }
            world->m_broadPhaseDispatcher->addPairs(newPairs.begin(), newPairs.getSize(),
                                                    (const hkCollidableCollidableFilter*)filter);
        }
    }
    world->unlockAndAttemptToExecutePendingOperations();
}

// puts the phantom in the broadphase, then lets the dispatcher react to every pair that showed up
void hkWorldOperationUtil::addPhantomBP(hkWorld* world, hkPhantom* phantom) {
    hkLocalArray<hkBroadPhaseHandlePair> newPairs(at<int>(world, 0x2E4)); // world's broadphase query size

    hkAabb aabb;
    phantom->calcAabb(aabb);

    // world +0x54 is the broadphase
    at<hkBroadPhase*>(world, 0x54)->addObject(&phantom->m_collidable.m_broadPhaseHandle, aabb, newPairs);

    if (newPairs.m_size != 0) {
        void* filter = at<void*>(world, 0x6C);
        if (filter != NULL) {
            filter = reinterpret_cast<char*>(filter) + 8; // the collision filter interface sits 8 bytes in
        }
        at<hkTypedBroadPhaseDispatcher*>(world, 0x58)
            ->addPairs(newPairs.begin(), newPairs.getSize(), (const hkCollidableCollidableFilter*)filter);
    }
}

void hkWorldOperationUtil::removePhantomBP(hkWorld* world, hkPhantom* phantom) {
    hkLocalArray<hkBroadPhaseHandlePair> removedPairs(at<int>(world, 0x2E4));

    // the broadphase takes the object out and reports the pairs that went with it
    at<hkBroadPhase*>(world, 0x54)->removeObject(&phantom->m_collidable.m_broadPhaseHandle, removedPairs);

    if (removedPairs.m_size != 0) {
        at<hkTypedBroadPhaseDispatcher*>(world, 0x58)->removePairs(&removedPairs[0], removedPairs.m_size);
    }
}

// the entity version of addPhantomBP: same broadphase dance, but the box comes from the entity's shape,
// pushed out a bit by half of the world's collision tolerance
void hkWorldOperationUtil::addEntityBP(hkWorld* world, hkEntity* entity) {
    if (entity->m_collidable.m_shape == NULL) {
        return;
    }

    hkLocalArray<hkBroadPhaseHandlePair> newPairs(at<int>(world, 0x2E4));

    // HYPOTHESIS: world +0x68 holds the collision settings and +8 in there is the tolerance; the collidable's
    // motion field points at the entity's transform
    hkAabb aabb;
    entity->m_collidable.m_shape->getAabb(*reinterpret_cast<const hkTransform*>(entity->m_collidable.m_motion),
                                          0.5f * at<float>(at<void*>(world, 0x68), 8), aabb);

    at<hkBroadPhase*>(world, 0x54)->addObject(&entity->m_collidable.m_broadPhaseHandle, aabb, newPairs);

    if (newPairs.m_size > 0) {
        void* filter = at<void*>(world, 0x6C);
        if (filter != NULL) {
            filter = reinterpret_cast<char*>(filter) + 8;
        }
        at<hkTypedBroadPhaseDispatcher*>(world, 0x58)
            ->addPairs(newPairs.begin(), newPairs.getSize(), (const hkCollidableCollidableFilter*)filter);
    }
}

// takes the entity out of the broadphase. the simulation gets to throw away this entity's collision info first
void hkWorldOperationUtil::removeEntityBP(hkWorld* world, hkEntity* entity) {
    hkEntity* entities[1] = {entity};
    if (entity->m_collidable.m_shape == NULL) {
        return;
    }

    world->m_simulation->resetCollisionInformationForEntities(entities, 1, world, true);

    hkLocalArray<hkBroadPhaseHandlePair> removedPairs(at<int>(world, 0x2E4));
    at<hkBroadPhase*>(world, 0x54)->removeObject(&entity->m_collidable.m_broadPhaseHandle, removedPairs);

    if (removedPairs.m_size > 0) {
        at<hkTypedBroadPhaseDispatcher*>(world, 0x58)->removePairs(&removedPairs[0], removedPairs.m_size);
    }
}

void hkWorldOperationUtil::addConstraintToCriticalLockedIsland(hkWorld* world, hkConstraintInstance* constraint) {
    hkWorldConstraintUtil::addConstraint(world, constraint);
    hkWorldCallbackUtil::fireConstraintAdded(world, constraint);
}

void hkWorldOperationUtil::removeConstraintFromCriticalLockedIsland(hkWorld* world, hkConstraintInstance* constraint) {
    if (hkWorldCallbackUtil::listenersAt<hkWorldCallbackUtil::ConstraintListener>(world, 0xFC).m_size != 0) {
        hkWorldCallbackUtil::fireConstraintRemoved(world, constraint);
    }
    hkWorldConstraintUtil::removeConstraint(constraint);
}

void hkWorldOperationUtil::splitSimulationIslands(hkWorld* world) {
    u8 flag = 1;
    splitSimulationIslands1(world, &flag);
}


// Islands whose state changes are queued in the world's dirty list (hkWorld::m_unk40, an hkArray of islands).
// Called out of line by the island split below.
#pragma push
#pragma auto_inline off
void hkWorldOperationUtil::markIslandInactive(hkWorld* world, hkSimulationIsland* island) {
    // Activity state (top two bits of the byte at 0x27) cleared; see hkEntity::getActivationState.
    ((hkIslandActivityByte*)((u8*)island + 0x27))->activity = 0;
    if (*(u16*)((u8*)island + 0x22) == 0xFFFF) {
        *(u16*)((u8*)island + 0x22) = (u16)world->m_unk40.m_size;
        if (world->m_unk40.m_size == (world->m_unk40.m_capacityAndFlags & hkArrayBase::CAPACITY_MASK)) {
            hkArrayUtil::_reserveMore(&world->m_unk40, 4);
        }
        ((hkSimulationIsland**)world->m_unk40.m_data)[world->m_unk40.m_size++] = island;
    }
}
#pragma pop

void hkWorldOperationUtil::markIslandActive(hkWorld* world, hkSimulationIsland* island) {
    u32 activity = 1;
    ((hkIslandActivityByte*)((u8*)island + 0x27))->activity = activity;
    *((u8*)island + 0x25) = 0;
    *((u8*)island + 0x24) = 0;
    if (*(u16*)((u8*)island + 0x22) == 0xFFFF) {
        *(u16*)((u8*)island + 0x22) = (u16)world->m_unk40.m_size;
        if (world->m_unk40.m_size == (world->m_unk40.m_capacityAndFlags & hkArrayBase::CAPACITY_MASK)) {
            hkArrayUtil::_reserveMore(&world->m_unk40, 4);
        }
        ((hkSimulationIsland**)world->m_unk40.m_data)[world->m_unk40.m_size++] = island;
    }
}

// Removes an island from one of the world's island arrays by moving the last entry into its slot.
// The island's index (u16 at 0x20, not recovered in hkSimulationIsland.h) is kept up to date.
static void removeIslandFromArray(hkArray<hkSimulationIsland*>& islands, hkSimulationIsland* island) {
    islands[*(u16*)((u8*)island + 0x20)] = islands[islands.m_size - 1];
    hkSimulationIsland* moved = islands[*(u16*)((u8*)island + 0x20)];
    *(u16*)((u8*)moved + 0x20) = *(u16*)((u8*)island + 0x20);
    islands.m_size--;
}

// BYTES: not an inline candidate (setRigidBodyMotionType calls it out of line).
#pragma push
#pragma auto_inline off
void hkWorldOperationUtil::removeEntitySI(hkWorld* world, hkEntity* entity) {
    entity->m_world = NULL;
    hkSimulationIsland* island = entity->m_simulationIsland;
    island->internalRemoveEntity(entity);
    // an emptied dynamic island goes away
    if ((!(bool)island->isFixed() && island->m_entities.getSize() == 0) && (bool)world->m_wantSimulationIslands) {
        removeIsland(world, island);
        delete island;
    }
}
#pragma pop

// Actions of the entity that sit on the fixed island are collected (with a reference) to move them to the
// entity's new dynamic island.
// BYTES: not an inline candidate (setRigidBodyMotionType calls it out of line).
#pragma push
#pragma auto_inline off
void hkWorldOperationUtil::removeAttachedActionsFromFixedIsland(hkWorld* world, hkEntity* entity,
                                                                hkArray<hkAction*>& actionsToBeMoved) {
    for (int i = 0; i < entity->m_actions.getSize(); i++) {
        hkAction* action = entity->m_actions[i];
        if (action->m_island == world->m_fixedIsland) {
            actionsToBeMoved.pushBack(action);
            action->addReference();
            world->m_fixedIsland->removeAction(action);
            world->m_fixedIsland->m_flags.m_stateB = 1; // the fixed island's action list needs a cleanup
            hkSimulationIsland* island = world->m_fixedIsland;
            if (island->m_dirtyListIndex == 0xFFFF) {
                island->m_dirtyListIndex = world->m_unk40.m_size;
                ((hkArray<hkSimulationIsland*>&)world->m_unk40).pushBack(island);
            }
        }
    }
}
#pragma pop

// Actions whose other entities are all fixed follow the entity to the fixed island: they are collected (with a
// reference) and taken off the entity's dynamic island.
// BYTES: not an inline candidate (setRigidBodyMotionType calls it out of line).
#pragma push
#pragma auto_inline off
void hkWorldOperationUtil::removeAttachedActionsFromDynamicIsland(hkWorld* world, hkEntity* entity,
                                                                  hkArray<hkAction*>& actionsToBeMoved) {
    hkSimulationIsland* island = entity->m_simulationIsland;
    hkInplaceArray<hkEntity*, 16> actionEntities;
    for (int i = 0; i < entity->m_actions.getSize(); i++) {
        hkAction* action = entity->m_actions[i];
        action->getEntities(actionEntities);

        hkBool moveAction = true;
        for (int j = 0; j < actionEntities.getSize(); j++) {
            if (!(bool)actionEntities[j]->isFixed() && actionEntities[j] != entity) {
                moveAction = false;
                break;
            }
        }

        if (moveAction) {
            actionsToBeMoved.pushBack(action);
            action->addReference();
            island->removeAction(action);
            island->m_flags.m_stateB = 1; // the island's action list needs a cleanup
            if (island->m_dirtyListIndex == 0xFFFF) {
                island->m_dirtyListIndex = world->m_unk40.m_size;
                ((hkArray<hkSimulationIsland*>&)world->m_unk40).pushBack(island);
            }
        }
    }
}
#pragma pop

// Puts the actions back on the entity's island (they hold a reference while they are moved) and merges the island
// with the islands of the other movable entities of each action. On the fixed island nothing is merged.
void hkWorldOperationUtil::addActionsToEntitysIsland(hkWorld* world, hkEntity* entity, hkArray<hkAction*>& actions) {
    hkSimulationIsland* island = entity->m_simulationIsland;
    hkInplaceArray<hkEntity*, 16> actionEntities;
    for (int i = 0; i < actions.getSize(); i++) {
        hkAction* action = actions[i];
        if (island != world->m_fixedIsland) {
            island->addAction(action);
            action->removeReference();
            actionEntities.clear();
            action->getEntities(actionEntities);
            for (int j = 0; j < actionEntities.getSize(); j++) {
                if (!(bool)actionEntities[j]->isFixed() && actionEntities[j] != entity) {
                    hkEntity* other = actionEntities[j];
                    if (!(bool)other->isFixed() && !(bool)entity->isFixed() &&
                        other->m_simulationIsland != entity->m_simulationIsland) {
                        mergeIslands(other->m_world, other, entity);
                    }
                }
            }
        } else {
            world->m_fixedIsland->addAction(action);
            action->removeReference();
        }
    }
}

// Detaches every constraint of the entity (with a reference each, without callbacks) into removedConstraints.
void hkWorldOperationUtil::removeAttachedConstraints(hkEntity* entity, hkArray<hkConstraintInstance*>& removedConstraints) {
    hkWorld* world = entity->m_world;
    {
        hkArray<hkConstraintInternal>& constraints = entity->m_constraintsMaster;
        for (int i = constraints.getSize() - 1; i >= 0; i--) {
            hkConstraintInstance* constraint = (hkConstraintInstance*)constraints[i].m_unk00;
            constraint->addReference();
            removeConstraintImmediately(world, constraint, false);
            removedConstraints.pushBack(constraint);
        }
    }
    {
        hkArray<hkConstraintInstance*>& constraints = entity->m_constraintsSlave;
        for (int i = constraints.getSize() - 1; i >= 0; i--) {
            hkConstraintInstance* constraint = constraints[i];
            constraint->addReference();
            removeConstraintImmediately(world, constraint, false);
            removedConstraints.pushBack(constraint);
        }
    }
}

// HYPOTHESIS: names of two hkAgentNnMachine helpers (hkAgent1nMachine.o).
extern "C" void fn_802FF684(hkAgentNnTrack& track, hkAgentNnEntry* entry); // CopyAndRelinkAgentEntry
extern "C" void fn_802FF278(hkAgentNnTrack& track, hkAgentNnEntry* entry); // InternalDeallocateEntry

// Agents to fixed partners move with the entity into agentsRemoved; when the entity becomes fixed, agents that
// sit in its island move to the partner's island.
void hkWorldOperationUtil::removeAttachedAgentsConnectingTheEntityAndAFixedPartnerEntityPlus(
    hkAgentNnTrack& trackToScan, hkEntity* entity, hkAgentNnTrack& agentsRemoved, hkMotion::MotionType newMotionType) {
    for (int i = 0; i < entity->m_collidable.m_collisionEntries.getSize(); i++) {
        hkLinkedCollidable::CollisionEntry& entry = entity->m_collidable.m_collisionEntries[i];
        hkEntity* otherBody = (hkEntity*)((char*)entry.m_partner + entry.m_partner->m_ownerOffset);
        if (otherBody->isFixed()) {
            hkAgentNnEntry* oldAgentEntry = entry.m_agentEntry;
            fn_802FF684(agentsRemoved, oldAgentEntry);
            fn_802FF278(trackToScan, oldAgentEntry);
        } else if (newMotionType == hkMotion::MOTION_FIXED) {
            hkSimulationIsland* islandOfAgent = hkWorldAgentUtil::getIslandFromAgentEntry(
                entry.m_agentEntry, entity->m_simulationIsland, otherBody->m_simulationIsland);
            if (islandOfAgent == entity->m_simulationIsland) {
                hkAgentNnEntry* oldAgentEntry = entry.m_agentEntry;
                fn_802FF684(otherBody->m_simulationIsland->m_agentTrack, oldAgentEntry);
                fn_802FF278(trackToScan, oldAgentEntry);
            }
        }
    }
}

// Processes the islands queued on the world's dirty list (last first; callbacks may add more): removed actions
// are compacted out of the action lists, and islands whose activity changed are moved between the active and
// inactive island arrays.
void hkWorldOperationUtil::cleanupDirtyIslands(hkWorld* world) {
    hkArray<hkSimulationIsland*>& dirtyIslands = (hkArray<hkSimulationIsland*>&)world->m_unk40;
    while (dirtyIslands.getSize()) {
        hkSimulationIsland* island = dirtyIslands[dirtyIslands.getSize() - 1];
        dirtyIslands.popBack();
        if (island) {
            island->m_dirtyListIndex = 0xFFFF;

            if (island->m_flags.m_stateB) {
                hkArray<hkAction*>& actions = island->m_actions;
                int freeSlot = -1;
                int a = 0;
                for (; a < actions.getSize(); a++) {
                    if (actions[a] == NULL) {
                        freeSlot = a;
                        a++;
                        break;
                    }
                }
                for (; a < actions.getSize(); a++) {
                    if (actions[a] != NULL) {
                        actions[freeSlot++] = actions[a];
                    }
                }
                if (freeSlot != -1) {
                    actions.setSize(freeSlot);
                }
                island->m_flags.m_stateB = 0;
            }

            if (island->m_flags27.m_active != island->m_flags.m_stateD) {
                if (island->m_flags27.m_active) {
                    internalActivateIsland(world, island);
                } else {
                    internalDeactivateIsland(world, island);
                }
            }
        }
    }
}

// BYTES: not inline candidates (cleanupDirtyIslands calls both out of line).
#pragma push
#pragma auto_inline off
// Moves the island to the active array (the biggest active island is kept first), restarts its deactivation
// timers and its bodies' time information, and warps its agents from the time of deactivation to now.
void hkWorldOperationUtil::internalActivateIsland(hkWorld* world, hkSimulationIsland* island) {
    removeIslandFromArray(*(hkArray<hkSimulationIsland*>*)&world->m_unk34, island);
    (*(hkArray<hkSimulationIsland*>*)&world->m_unk28).pushBack(island);
    island->m_storageIndex = world->m_unk28.m_size - 1;
    island->m_flags.m_stateD = 1;
    island->m_timeSinceLastHighFrequencyCheck = 0.0f;
    island->m_timeSinceLastLowFrequencyCheck = 0.0f;

    for (int e = 0; e < island->m_entities.getSize(); e++) {
        hkEntity* body = island->m_entities[e];
        hkSweptTransformUtil::setTimeInformation(body->m_motion.m_motionState, 0.0f, 0.0f);
    }

    hkWorldAgentUtil::warpTime(island, island->m_unk6C, world->m_dynamicsStepInfo.m_stepInfo.m_startTime,
                               *world->m_collisionInput);

    // sort the big island to the front
    if (island->m_storageIndex != 0) {
        hkArray<hkSimulationIsland*>& activeIslands = *(hkArray<hkSimulationIsland*>*)&world->m_unk28;
        hkSimulationIsland* first = activeIslands[0];
        if (island->m_entities.getSize() > first->m_entities.getSize()) {
            int index = island->m_storageIndex;
            island->m_storageIndex = 0;
            first->m_storageIndex = index;
            activeIslands[0] = island;
            activeIslands[index] = first;
        }
    }

    hkWorldCallbackUtil::fireIslandActivated(world, island);
}

// Moves the island to the inactive array, freezes its bodies at the time of deactivation and stops them.
void hkWorldOperationUtil::internalDeactivateIsland(hkWorld* world, hkSimulationIsland* island) {
    (*(hkArray<hkSimulationIsland*>*)&world->m_unk34).pushBack(island);
    removeIslandFromArray(*(hkArray<hkSimulationIsland*>*)&world->m_unk28, island);
    island->m_storageIndex = world->m_unk34.m_size - 1;
    island->m_flags.m_stateD = 0;

    // HYPOTHESIS: m_unk6C is the time of deactivation (validates the agents' time information on reactivation)
    island->m_unk6C = world->m_dynamicsStepInfo.m_stepInfo.m_startTime;
    for (int e = 0; e < island->m_entities.getSize(); e++) {
        hkEntity* body = island->m_entities[e];
        hkSweptTransformUtil::freezeMotionState(island->m_unk6C, body->m_motion.m_motionState);
        body->m_motion.setLinearVelocity(hkVector4Zero);
        body->m_motion.setAngularVelocity(hkVector4Zero);
    }
    hkWorldCallbackUtil::fireIslandDeactivated(world, island);
}
#pragma pop

#pragma dont_inline on
void hkWorldOperationUtil::removeIslandFromDirtyList(hkWorld* world, hkSimulationIsland* island) {
    // Dirty-list index of the island: u16 at 0x22 (not recovered in hkSimulationIsland.h).
    u16 index = *(u16*)((u8*)island + 0x22);
    if (index == 0xFFFF) {
        return;
    }
    ((int*)world->m_unk40.m_data)[index] = 0;
    *(u16*)((u8*)island + 0x22) = 0xFFFF;
}
#pragma dont_inline reset

// BYTES: not an inline candidate (removeEntitySI calls it out of line).
#pragma push
#pragma auto_inline off
void hkWorldOperationUtil::removeIsland(hkWorld* world, hkSimulationIsland* island) {
    if ((island->flags26() & 3) == 0) {
        removeIslandFromArray(*(hkArray<hkSimulationIsland*>*)&world->m_unk34, island);
    } else {
        removeIslandFromArray(*(hkArray<hkSimulationIsland*>*)&world->m_unk28, island);
    }
    removeIslandFromDirtyList(world, island);
}
#pragma pop

// BYTES: not an inline candidate (setRigidBodyMotionType calls it out of line).
#pragma push
#pragma auto_inline off
hkConstraintInstance* hkWorldOperationUtil::addConstraintImmediately(hkWorld* world, hkConstraintInstance* constraint, bool fireCallback) {
    world->m_lockCount++;
    hkWorldConstraintUtil::addConstraint(world, constraint);
    if (fireCallback) {
        hkWorldCallbackUtil::fireConstraintAdded(world, constraint);
    }
    world->unlockAndAttemptToExecutePendingOperations();
    return constraint;
}
#pragma pop

// BYTES: not an inline candidate (addActionsToEntitysIsland calls it out of line).
#pragma push
#pragma auto_inline off
void hkWorldOperationUtil::mergeIslands(hkWorld* world, hkEntity* entityA, hkEntity* entityB) {
    hkSimulationIsland* islandA = entityA->m_simulationIsland;
    hkSimulationIsland* islandB = entityB->m_simulationIsland;
    if (world->m_lockCount != 0) {
        hkWorldOperation::BiggestOperation operation;
        operation.m_type = 0x0C;
        operation.m_entityA = entityA;
        operation.m_entityB = entityB;
        world->queueOperation(&operation);
    } else {
        internalMergeTwoIslands(world, islandA, islandB);
    }
}
#pragma pop

// BYTES: not an inline candidate (removeAttachedConstraints calls it out of line).
#pragma push
#pragma auto_inline off
void hkWorldOperationUtil::removeConstraintImmediately(hkWorld* world, hkConstraintInstance* constraint, bool fireCallback) {
    // HYPOTHESIS: the constraint's word at 0x08 is the island whose activity state (bits 0xC0 of 0x26) is set.
    hkSimulationIsland* island = *(hkSimulationIsland**)((u8*)constraint + 0x08);
    ((hkIslandActivityByte*)((u8*)island + 0x26))->activity = 1;
    if (world->areCriticalOperationsLocked()) {
        if (fireCallback && hkWorldCallbackUtil::listenersAt<hkWorldCallbackUtil::ConstraintListener>(world, 0xFC).m_size != 0) {
            hkWorldCallbackUtil::fireConstraintRemoved(world, constraint);
        }
        hkWorldConstraintUtil::removeConstraint(constraint);
    } else {
        world->lockCriticalOperations();
        if (fireCallback && hkWorldCallbackUtil::listenersAt<hkWorldCallbackUtil::ConstraintListener>(world, 0xFC).m_size != 0) {
            hkWorldCallbackUtil::fireConstraintRemoved(world, constraint);
        }
        hkWorldConstraintUtil::removeConstraint(constraint);
        world->unlockAndAttemptToExecutePendingOperations();
    }
}
#pragma pop

// Weak inline copies emitted in this unit; extern "C" placeholders under their fn_ names until the
// instantiating callers are written (shape from https://github.com/jj-link/brawl src/havok/fn_<addr>.cpp).
// 12-byte swap (unnamed in the map)
// TODO: cleanup (shape from jj-link/brawl fn_802FB210)
#pragma push
#pragma auto_inline off
extern "C" void fn_802FB210(int* a, int* b) {
    int t;
    t = a[0]; a[0] = b[0]; b[0] = t;
    t = a[1]; a[1] = b[1]; b[1] = t;
    t = a[2]; a[2] = b[2]; b[2] = t;
}
#pragma pop

// Puts a new entity on an island: fixed entities share the fixed island; with simulation islands every other entity
// starts on an island of its own (active or inactive as asked), otherwise active entities join the one active island.
// BYTES: not an inline candidate (setRigidBodyMotionType calls it out of line).
#pragma push
#pragma auto_inline off
void hkWorldOperationUtil::addEntitySI(hkWorld* world, hkEntity* entity, int activation) {
    entity->m_world = world;
    if (entity->isFixed()) {
        world->m_fixedIsland->internalAddEntity(entity);
        return;
    }
    if (world->m_wantSimulationIslands) {
        hkSimulationIsland* island = new hkSimulationIsland(world);
        if (activation == HK_ENTITY_ACTIVATION_DO_ACTIVATE) {
            island->m_flags27.m_active = 1;
            island->m_flags.m_stateD = 1;
            island->m_storageIndex = world->m_unk28.m_size;
            (*(hkArray<hkSimulationIsland*>*)&world->m_unk28).pushBack(island);
        } else {
            island->m_flags27.m_active = 0;
            island->m_flags.m_stateD = 0;
            island->m_storageIndex = world->m_unk34.m_size;
            (*(hkArray<hkSimulationIsland*>*)&world->m_unk34).pushBack(island);
        }
        island->internalAddEntity(entity);
        return;
    }
    if (activation == HK_ENTITY_ACTIVATION_DO_ACTIVATE) {
        (*(hkArray<hkSimulationIsland*>*)&world->m_unk28)[0]->internalAddEntity(entity);
    }
}
#pragma pop

// Changes the body's motion type. Moving into or out of the fixed state takes the body off its island first: its
// constraints, actions and agents with fixed partners are detached, the motion is replaced, and everything is put
// back (the islands of movable partners merged again). Becoming movable needs the motion saved when the body was
// fixed or keyframed.
void hkWorldOperationUtil::setRigidBodyMotionType(hkRigidBody* body, u8 newMotionType, u8 activation,
                                                  u8 collisionFilterUpdateMode) {
    hkMotion::MotionType newType = (hkMotion::MotionType)newMotionType;
    hkMotion::MotionType oldType = (hkMotion::MotionType)(int)body->m_motion.m_type;
    if (oldType == newType) {
        return;
    }
    bool newIsMovable = newType != hkMotion::MOTION_FIXED && newType != hkMotion::MOTION_KEYFRAMED;
    bool oldIsMovable = oldType != hkMotion::MOTION_FIXED && oldType != hkMotion::MOTION_KEYFRAMED;
    if (newIsMovable && !oldIsMovable && body->m_motion.m_savedMotion == NULL) {
        return;
    }

    body->addReference();
    hkWorld* world = body->m_world;
    bool fixedStateChanges = (oldType == hkMotion::MOTION_FIXED) != (newType == hkMotion::MOTION_FIXED);
    if (world != NULL && fixedStateChanges) {
        hkInplaceArray<hkConstraintInstance*, 16> constraints;
        hkInplaceArray<hkAction*, 16> actions;
        hkAgentNnTrack agentsRemoved(world->m_collisionDispatcher->m_agentEntrySize,
                                     world->m_collisionDispatcher->m_agentSectorSize);
        world->m_unk84 = true;
        world->m_unk85 = false;

        removeAttachedConstraints(body, constraints);
        if (newType != hkMotion::MOTION_FIXED) {
            removeAttachedActionsFromFixedIsland(world, body, actions);
        } else {
            removeAttachedActionsFromDynamicIsland(world, body, actions);
        }
        removeAttachedAgentsConnectingTheEntityAndAFixedPartnerEntityPlus(
            body->m_simulationIsland->m_agentTrack, body, agentsRemoved, newType);
        if (oldType != hkMotion::MOTION_FIXED) {
            hkSimulationIsland* island = body->m_simulationIsland;
            if (island->m_entities.getSize() > 2) {
                island->m_flags.m_stateA = 1; // the island may split now
            }
        }
        removeEntitySI(world, body);
        replaceMotionObject(body, newType, newIsMovable, oldIsMovable, world);
        addEntitySI(world, body, activation);
        addActionsToEntitysIsland(world, body, actions);
        world->m_unk85 = true;

        for (int i = 0; i < constraints.getSize(); i++) {
            hkConstraintInstance* constraint = constraints[i];
            addConstraintImmediately(world, constraint, false);
            constraint->removeReference();
        }

        if (newType != hkMotion::MOTION_FIXED) {
            hkArray<hkLinkedCollidable::CollisionEntry>& entries = body->m_collidable.m_collisionEntries;
            for (int i = 0; i < entries.getSize(); i++) {
                hkLinkedCollidable* partnerCollidable = entries[i].m_partner;
                hkEntity* partner = (hkEntity*)((char*)partnerCollidable + partnerCollidable->m_ownerOffset);
                if (!(bool)body->isFixed() && !(bool)partner->isFixed() &&
                    body->m_simulationIsland != partner->m_simulationIsland) {
                    mergeIslands(body->m_world, body, partner);
                }
            }
        }
        hkAgentNnMachine_AppendTrack(body->m_simulationIsland->m_agentTrack, agentsRemoved);

        if (newType == hkMotion::MOTION_FIXED) {
            hkSweptTransformUtil::freezeMotionState(world->m_dynamicsStepInfo.m_stepInfo.m_startTime,
                                                    body->m_motion.m_motionState);
            world->lockCriticalOperations();
            world->m_simulation->resetCollisionInformationForEntities((hkEntity**)&body, 1, world, false);
            hkSimulation::collideEntitiesBroadPhaseDiscrete((hkEntity**)&body, 1, world);
            hkWorldCallbackUtil::fireInactiveEntityMoved(body->m_world, body);
            world->m_lockCount--;
        }
        world->updateCollisionFilterOnEntity(body, collisionFilterUpdateMode, 1);
        world->m_unk84 = false;
        if (world->m_lockCount == 0 && world->m_pendingOperationsCount != 0 && !(bool)world->m_unk84) {
            world->internal_executePendingOperations();
        }
    } else {
        replaceMotionObject(body, newType, newIsMovable, oldIsMovable, world);
        if (world != NULL) {
            world->updateCollisionFilterOnEntity(body, collisionFilterUpdateMode, 1);
        }
    }
    body->removeReference();
}

// The motion's current rotation (the end rotation of its swept transform).
static inline const hkQuaternion& rotationOf(const hkMotion& motion) {
    return motion.m_motionState.m_sweptTransform.m_rotation1;
}

// Swaps the body's motion in place. Becoming movable again restores the saved dynamic motion (with the current
// state) and, for a dynamic type other than the saved one, rebuilds it as that type. Becoming fixed or keyframed
// builds the new motion over the old one; a movable body keeps a copy of its old motion to come back to.
void hkWorldOperationUtil::replaceMotionObject(hkRigidBody* body, hkMotion::MotionType newType, hkBool newIsMovable,
                                               hkBool oldIsMovable, hkWorld* world) {
    if (newIsMovable) {
        if (!(bool)oldIsMovable) {
            hkKeyframedRigidMotion* motion = &body->m_motion;
            hkMaxSizeMotion* saved = motion->m_savedMotion;
            motion->getMotionStateAndVelocities(saved);
            body->m_collidable.m_broadPhaseHandle.m_objectQualityType = motion->m_savedQualityTypeIndex;
            hkString::memCpy16(motion, saved, sizeof(hkMaxSizeMotion) / 16);
            delete saved;
        }
        if (newType != body->m_motion.m_type && newType != hkMotion::MOTION_DYNAMIC) {
            hkMaxSizeMotion oldMotion;
            hkString::memCpy16(&oldMotion, &body->m_motion, sizeof(hkMaxSizeMotion) / 16);
            hkMatrix3 inertia;
            body->m_motion.getInertiaLocal(inertia);
            hkMotionState& ms = oldMotion.m_motionState;
            const hkVector4& position = ms.m_transform.m_translation;
            hkSimdReal mass = oldMotion.getMass();
            hkRigidBody::createDynamicRigidMotion(newType, position, ms.m_sweptTransform.m_rotation1,
                                                  mass, ms.m_maxLinearVelocity, ms.m_maxAngularVelocity,
                                                  inertia, ms.m_sweptTransform.m_centerOfMassLocal, &body->m_motion);
            hkMotion* motion = &body->m_motion;
            oldMotion.getMotionStateAndVelocities(motion);
            motion->m_motionState.m_linearDamping = ms.m_linearDamping;
            motion->m_motionState.m_angularDamping = ms.m_angularDamping;
        }
    } else {
        hkMaxSizeMotion oldMotion;
        hkString::memCpy16(&oldMotion, &body->m_motion, sizeof(hkMaxSizeMotion) / 16);
        hkKeyframedRigidMotion* motion;
        if (newType == hkMotion::MOTION_FIXED) {
            motion = new (&body->m_motion) hkFixedRigidMotion(body->m_motion.m_motionState.m_transform.m_translation, rotationOf(body->m_motion));
        } else {
            motion = new (&body->m_motion) hkKeyframedRigidMotion(body->m_motion.m_motionState.m_transform.m_translation, rotationOf(body->m_motion));
        }
        if (newType == hkMotion::MOTION_FIXED) {
            motion->m_motionState = oldMotion.m_motionState;
            hkSweptTransform& st = oldMotion.m_motionState.m_sweptTransform;
            hkReal invDeltaTime = st.m_centerOfMass1(3);
            if (invDeltaTime != 0.0f) {
                hkReal time;
                if (world != 0) {
                    time = world->m_simulation->m_unk10;
                } else {
                    time = st.m_centerOfMass0(3) + 1.0f / invDeltaTime;
                }
                hkSweptTransformUtil::freezeMotionState(time, motion->m_motionState);
            }
        } else {
            oldMotion.getMotionStateAndVelocities(motion);
        }
        if (oldIsMovable) {
            hkMaxSizeMotion* saved = new hkMaxSizeMotion();
            u16 memSize = saved->m_memSizeAndFlags;
            hkString::memCpy16(saved, &oldMotion, sizeof(hkMaxSizeMotion) / 16);
            saved->m_memSizeAndFlags = memSize;
            saved->m_referenceCount = 1;
            motion->m_savedMotion = saved;
            motion->m_savedQualityTypeIndex = body->m_collidable.m_broadPhaseHandle.m_objectQualityType;
        } else {
            motion->setStoredMotion(oldMotion.m_savedMotion);
            motion->m_savedQualityTypeIndex = oldMotion.m_savedQualityTypeIndex;
        }
        body->m_solverData = 0;
        body->m_collidable.m_broadPhaseHandle.m_objectQualityType =
            (newType == hkMotion::MOTION_FIXED) ? 1 : 2; // fixed / keyframed collidable quality
    }
    body->m_collidable.m_motion = (hkMotion*)&body->m_motion.m_motionState;
}

// Merges two islands into the bigger one (on a tie, the one earlier in the world's list) and deletes the other:
// its entities, agents, actions and constraints move over, the activity and dirty-list state is combined, and the
// surviving island is moved to the front of the active list when it is now the biggest.
hkSimulationIsland* hkWorldOperationUtil::internalMergeTwoIslands(hkWorld* world, hkSimulationIsland* islandA,
                                                                  hkSimulationIsland* islandB) {
    HK_TIMER_BEGIN("MergeIsle");
    int sizeB = islandB->m_entities.getSize();
    int sizeA = islandA->m_entities.getSize();
    if (sizeA <= sizeB) {
        if (sizeA < sizeB || islandA->m_storageIndex > islandB->m_storageIndex) {
            hkSimulationIsland* tmp = islandA;
            islandA = islandB;
            islandB = tmp;
        }
    }
    world->lockCriticalOperations();

    bool anyActive = islandA->m_flags.m_stateD || islandB->m_flags.m_stateD;
    bool anyMarked = (char)islandA->m_flags27.m_active || (char)islandB->m_flags27.m_active;
    if (anyActive) {
        if (!islandA->m_flags.m_stateD) {
            islandA->m_flags27.m_active = 1;
            internalActivateIsland(world, islandA);
            islandA->m_highFrequencyDeactivationCounter = islandB->m_highFrequencyDeactivationCounter;
            islandA->m_lowFrequencyDeactivationCounter = islandB->m_lowFrequencyDeactivationCounter;
        } else if (!islandB->m_flags.m_stateD) {
            islandB->m_flags27.m_active = 1;
            internalActivateIsland(world, islandB);
        } else {
            u8 high = islandB->m_highFrequencyDeactivationCounter;
            if (islandA->m_highFrequencyDeactivationCounter < high) {
                high = islandA->m_highFrequencyDeactivationCounter;
            }
            islandA->m_highFrequencyDeactivationCounter = high;
            u8 low = islandB->m_lowFrequencyDeactivationCounter;
            if (islandA->m_lowFrequencyDeactivationCounter < low) {
                low = islandA->m_lowFrequencyDeactivationCounter;
            }
            islandA->m_lowFrequencyDeactivationCounter = low;
        }
    }

    hkAgentNnMachine_AppendTrack(islandA->m_agentTrack, islandB->m_agentTrack);

    // entities
    {
        int oldSize = islandA->m_entities.getSize();
        u16 index = (u16)oldSize;
        islandA->m_entities.setSize(oldSize + islandB->m_entities.getSize());
        for (int i = 0; i < islandB->m_entities.getSize(); i++) {
            islandA->m_entities[index] = islandB->m_entities[i];
            islandB->m_entities[i]->m_simulationIsland = islandA;
            islandB->m_entities[i]->m_storageIndex = index;
            index++;
        }
    }

    // actions (empty slots are dropped)
    {
        int numActions = islandA->m_actions.getSize();
        islandA->m_actions.setSize(numActions + islandB->m_actions.getSize());
        for (int i = 0; i < islandB->m_actions.getSize(); i++) {
            hkAction* action = islandB->m_actions[i];
            if (action != NULL) {
                islandA->m_actions[numActions] = action;
                *(hkSimulationIsland**)((u8*)islandA->m_actions[numActions] + 0x0C) = islandA;
                numActions++;
            }
        }
        islandA->m_actions.setSize(numActions);
    }

    // constraints owned by the moved entities
    for (int i = 0; i < islandB->m_entities.getSize(); i++) {
        hkEntity* entity = islandB->m_entities[i];
        hkConstraintInternal* c = entity->m_constraintsMaster.begin();
        for (int j = entity->m_constraintsMaster.getSize() - 1; j >= 0; j--) {
            ((hkConstraintInstance*)c->m_unk00)->m_owner = islandA;
            c++;
        }
    }

    islandA->mergeConstraintInfo(islandB);
    islandA->m_unk1C += islandB->m_unk1C;
    islandB->m_unk1C = 0;

    // take islandB out of its list
    {
        u16 index = islandB->m_storageIndex;
        hkArray<hkSimulationIsland*>& islands = islandB->m_flags.m_stateD
                                                    ? *(hkArray<hkSimulationIsland*>*)&world->m_unk28
                                                    : *(hkArray<hkSimulationIsland*>*)&world->m_unk34;
        if (index < islands.getSize() - 1) {
            islands[index] = islands[islands.getSize() - 1];
            islands[index]->m_storageIndex = index;
        }
        islands.m_size--;
    }

    islandA->m_flags.m_stateA = islandA->m_flags.m_stateA || islandB->m_flags.m_stateA;
    islandA->m_flags.m_stateB = islandA->m_flags.m_stateB || islandB->m_flags.m_stateB;
    islandA->m_flags27.m_active = hkBool(anyMarked);

    if (islandB->m_dirtyListIndex != 0xFFFF && islandA->m_dirtyListIndex == 0xFFFF) {
        islandA->m_dirtyListIndex = world->m_unk40.m_size;
        ((hkArray<hkSimulationIsland*>&)world->m_unk40).pushBack(islandA);
    }
    removeIslandFromDirtyList(world, islandB);
    delete islandB;

    // sort the big island to the front
    if (islandA->m_flags.m_stateD && islandA->m_storageIndex != 0) {
        hkArray<hkSimulationIsland*>& activeIslands = *(hkArray<hkSimulationIsland*>*)&world->m_unk28;
        hkSimulationIsland* first = activeIslands[0];
        if (islandA->m_entities.getSize() > first->m_entities.getSize()) {
            int oldIndex = islandA->m_storageIndex;
            islandA->m_storageIndex = 0;
            first->m_storageIndex = oldIndex;
            activeIslands[0] = islandA;
            activeIslands[oldIndex] = first;
        }
    }

    HK_TIMER_END();
    world->unlockAndAttemptToExecutePendingOperations();
    return islandA;
}

// Grows the array's storage to exactly n elements when it is smaller.
template <typename T>
static inline void reserveExactly(hkArray<T>& array, int n) {
    if (array.getCapacity() < n) {
        hkArrayUtil::_reserve(&array, n, sizeof(T));
    }
}

// Gives heap storage back once an in-place array has shrunk to at most half of it.
template <typename T, int N>
static inline void optimizeCapacity(hkInplaceArray<T, N>& array) {
    if (!(array.m_capacityAndFlags & hkArrayBase::DONT_DEALLOCATE_FLAG)) {
        if (array.m_size < 1 || array.m_size * 2 < array.getCapacity()) {
            hkArrayUtil::_reduce(&array, sizeof(T), array.m_storage, N);
        }
    }
}

// hkSimulationIsland's connectivity check (fn_802EA2D0, isFullyConnected in hkSimulationIsland.o): fills the union
// find with the island's entity graph and reports whether it is one group.
extern "C" hkBool fn_802EA2D0(hkSimulationIsland* island, hkUnionFind& checkConnectivity);

// The island an agent entry belongs to: the first body's island, or the second one's when the first is fixed.
static inline hkSimulationIsland* islandOfAgentEntry(hkAgentNnEntry* entry) {
    hkLinkedCollidable* collA = entry->m_collidable[0];
    hkSimulationIsland* island = ((hkEntity*)((char*)collA + collA->m_ownerOffset))->m_simulationIsland;
    if (island->isFixed()) {
        hkLinkedCollidable* collB = entry->m_collidable[1];
        island = ((hkEntity*)((char*)collB + collB->m_ownerOffset))->m_simulationIsland;
    }
    return island;
}

// Splits every island of the active (or inactive) list that is marked for a split check and is no longer connected:
// each further group of entities gets a new island, and the entities, constraints, actions and agents follow their
// group. Called by hkWorld::addPhysicsSystem with the system's active flag.
extern "C" void fn_802F9098(hkWorld* world, hkBool active) {
    if (world->m_wantSimulationIslands) {
    hkArray<hkSimulationIsland*>& islands = active ? *(hkArray<hkSimulationIsland*>*)&world->m_unk28
                                                   : *(hkArray<hkSimulationIsland*>*)&world->m_unk34;
    for (int i = islands.getSize() - 1; i >= 0; i--) {
        hkSimulationIsland* island = islands[i];
        if (!island->m_flags.m_stateA) {
            continue;
        }
        island->m_flags.m_stateA = 0;

        int numEntities = island->m_entities.getSize();
        hkLocalArray<int> entityInfo(numEntities);
        hkUnionFind checkConnectivity(entityInfo, island->m_entities.getSize());
        if (fn_802EA2D0(island, checkConnectivity)) {
            continue;
        }
        numEntities = island->m_entities.getSize();

        hkInplaceArray<int, 32> sizeOfIslands;
        checkConnectivity.assignGroups(sizeOfIslands);
        int sizeOfFirstIsland = sizeOfIslands[0];

        hkInplaceArray<hkSimulationIsland*, 32> newIslands;
        u8 wasActive = island->m_flags27.m_active;
        newIslands.pushBackUnchecked(island);
        for (int g = 1; g < sizeOfIslands.getSize(); g++) {
            hkSimulationIsland* newIsland = new hkSimulationIsland(world);
            newIsland->m_flags27.m_active = active;
            newIsland->m_flags.m_stateD = active;
            islands.pushBack(newIsland);
            newIsland->m_storageIndex = islands.getSize() - 1;
            if (active && !(char)wasActive) {
                hkWorldOperationUtil::markIslandInactive(world, newIsland);
            }
            newIslands.pushBack(newIsland);
            reserveExactly(newIsland->m_entities, sizeOfIslands[g]);
        }

        // entities, and the constraints they own
        {
            hkEntity** oldEntities = island->m_entities.begin();
            int oldNum = island->m_entities.getSize();
            island->m_entities.m_size = 0;
            for (int e = 0; e < oldNum; e++) {
                hkEntity* entity = oldEntities[e];
                hkSimulationIsland* newIsland = newIslands[entityInfo[e]];
                entity->m_simulationIsland = newIsland;
                entity->m_storageIndex = newIsland->m_entities.getSize();
                newIsland->m_entities.pushBack(entity);
                if (newIsland != island) {
                    hkConstraintInternal* ci = entity->m_constraintsMaster.begin();
                    for (int c = 0; c < entity->m_constraintsMaster.getSize(); c++, ci++) {
                        hkConstraintInfo info;
                        info.m_maxSizeOfJacobians = ci->m_unk14;
                        info.m_sizeOfJacobians = ci->m_unk14;
                        info.m_sizeOfSchemas = ci->m_unk16;
                        info.m_numSolverResults = ci->m_unk18;
                        island->m_unk1C--;
                        island->m_constraintInfo.m_sizeOfJacobians -= info.m_sizeOfJacobians;
                        island->m_constraintInfo.m_sizeOfSchemas -= info.m_sizeOfSchemas;
                        island->m_constraintInfo.m_numSolverResults -= info.m_numSolverResults;
                        info.m_maxSizeOfJacobians = island->m_constraintInfo.m_maxSizeOfJacobians;
                        hkConstraintInternal* internal =
                            (hkConstraintInternal*)((hkConstraintInstance*)ci->m_unk00)->m_internal;
                        if (internal != NULL) {
                            internal->m_unk14 -= info.m_sizeOfJacobians;
                            internal->m_unk16 -= info.m_sizeOfSchemas;
                            internal->m_unk18 -= info.m_numSolverResults;
                        }
                        newIsland->m_unk1C++;
                        ((hkConstraintInstance*)ci->m_unk00)->m_owner = newIsland;
                        newIsland->m_constraintInfo.add(info);
                        internal = (hkConstraintInternal*)((hkConstraintInstance*)ci->m_unk00)->m_internal;
                        if (internal != NULL) {
                            internal->m_unk14 += info.m_sizeOfJacobians;
                            internal->m_unk16 += info.m_sizeOfSchemas;
                            internal->m_unk18 += info.m_numSolverResults;
                        }
                    }
                }
            }
        }

        // actions go to the island of their first movable entity
        {
            hkSimulationIsland* fixedIsland = world->m_fixedIsland;
            hkAction** oldActions = island->m_actions.begin();
            int numActions = island->m_actions.getSize();
            island->m_actions.m_size = 0;
            for (int a = 0; a < numActions; a++) {
                hkAction* action = oldActions[a];
                if (action == NULL) {
                    continue;
                }
                hkSimulationIsland* newIsland = NULL;
                {
                    hkInplaceArray<hkEntity*, 16> actionEntities;
                    action->getEntities(actionEntities);
                    for (int k = 0; k < actionEntities.getSize(); k++) {
                        newIsland = actionEntities[k]->m_simulationIsland;
                        if (newIsland != fixedIsland) {
                            break;
                        }
                    }
                }
                *(hkSimulationIsland**)((u8*)action + 0x0C) = newIsland;
                newIsland->m_actions.pushBack(action);
            }
        }

        // agents: move the few that changed island, or rebuild every track when many entities moved
        if ((numEntities - sizeOfFirstIsland) * 8 < numEntities) {
            hkAgentNnTrack& track = island->m_agentTrack;
            for (int s = 0; s < track.m_sectors.getSize(); s++) {
                hkAgentNnSector* sector = track.getSector(s);
                hkAgentNnEntry* entry = (hkAgentNnEntry*)sector;
                while (entry < (hkAgentNnEntry*)((char*)sector + (s + 1 == track.m_sectors.getSize()
                                                                       ? track.m_bytesUsedInLastSector
                                                                       : track.m_sectorSize))) {
                    hkSimulationIsland* entryIsland = islandOfAgentEntry(entry);
                    if (entryIsland == island) {
                        entry = (hkAgentNnEntry*)((char*)entry + entry->m_size);
                        continue;
                    }
                    fn_802FF684(entryIsland->m_agentTrack, entry);
                    fn_802FF278(track, entry);
                    if (s >= track.m_sectors.getSize()) {
                        break;
                    }
                }
            }
        } else {
            hkCollisionDispatcher* dispatcher = world->m_collisionDispatcher;
            hkAgentNnTrack oldTrack(dispatcher->m_agentEntrySize, dispatcher->m_agentSectorSize);
            hkAgentNnTrack& track = island->m_agentTrack;
            if (track.m_sectors.getSize() == 1) {
                oldTrack.m_sectors.begin()[0] = track.m_sectors.begin()[0];
                oldTrack.m_sectors.m_size = 1;
                track.m_sectors.m_size = 0;
            } else if (track.m_sectors.getSize() > 1) {
                reserveExactly(oldTrack.m_sectors, 2);
                fn_802FB210((int*)&oldTrack.m_sectors, (int*)&track.m_sectors);
            }
            int bytesUsed = oldTrack.m_bytesUsedInLastSector;
            oldTrack.m_bytesUsedInLastSector = track.m_bytesUsedInLastSector;
            track.m_bytesUsedInLastSector = bytesUsed;
            for (int s = 0; s < oldTrack.m_sectors.getSize(); s++) {
                hkAgentNnSector* sector = oldTrack.getSector(s);
                hkAgentNnEntry* entry = (hkAgentNnEntry*)sector;
                hkAgentNnEntry* end = (hkAgentNnEntry*)((char*)sector + (s + 1 == oldTrack.m_sectors.getSize()
                                                                              ? oldTrack.m_bytesUsedInLastSector
                                                                              : oldTrack.m_sectorSize));
                for (; entry < end; entry = (hkAgentNnEntry*)((char*)entry + entry->m_size)) {
                    fn_802FF684(islandOfAgentEntry(entry)->m_agentTrack, entry);
                }
                hkThreadMemory::s_instance->deallocateChunk(sector, oldTrack.m_sectorSize, 0x25);
            }
            oldTrack.m_sectors.m_size = 0;
        }
        optimizeCapacity(island->m_agentTrack.m_sectors);
    }
    }
}
