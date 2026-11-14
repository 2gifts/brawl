// Havok translation unit hkWorldOperationUtil.o (main.dol 0x802F7E24-0x802FB244).
// Work in progress. Functions in address order (method names from the Havok TU map):
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
    if (world->m_lockCount != 0) {
        if (fireCallback && hkWorldCallbackUtil::listenersAt<hkWorldCallbackUtil::ConstraintListener>(world, 0xFC).m_size != 0) {
            hkWorldCallbackUtil::fireConstraintRemoved(world, constraint);
        }
        hkWorldConstraintUtil::removeConstraint(constraint);
    } else {
        world->m_lockCount++;
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
extern "C" void fn_802FB210(int* a, int* b) {
    int t;
    t = a[0]; a[0] = b[0]; b[0] = t;
    t = a[1]; a[1] = b[1]; b[1] = t;
    t = a[2]; a[2] = b[2]; b[2] = t;
}

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
