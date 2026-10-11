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
#include <havok/hkLocalArray.h>
#include <havok/hkBroadPhase.h>
#include <havok/hkPhantom.h>

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

// stand-ins for the broadphase dispatcher: it turns new / removed broadphase pairs into collision agents
extern "C" void fn_802CAEE8(void* dispatcher, hkBroadPhaseHandlePair* pairs, int numPairs, void* collisionFilter);
extern "C" void fn_802CAFC0(void* dispatcher, hkBroadPhaseHandlePair* pairs, int numPairs);

// puts the phantom in the broadphase, then lets the dispatcher react to every pair that showed up
void hkWorldOperationUtil::addPhantomBP(hkWorld* world, hkPhantom* phantom) {
    hkLocalArray<hkBroadPhaseHandlePair> newPairs(at<int>(world, 0x2E4)); // world's broadphase query size

    // HYPOTHESIS: vtable 0x1C is calcAabb here, hkPhantom.h has its slots in a different order
    typedef void (*CalcAabbFn)(hkPhantom*, hkAabb*);
    hkAabb aabb;
    reinterpret_cast<CalcAabbFn>((*reinterpret_cast<void***>(phantom))[0x1C / 4])(phantom, &aabb);

    // world +0x54 is the broadphase, vtable 0x10 adds an object (handle is at +0x30 in the phantom)
    void* broadPhase = at<void*>(world, 0x54);
    typedef void (*AddObjectFn)(void*, void*, hkAabb*, hkArray<hkBroadPhaseHandlePair>*);
    reinterpret_cast<AddObjectFn>((*reinterpret_cast<void***>(broadPhase))[0x10 / 4])(broadPhase, reinterpret_cast<char*>(phantom) + 0x30, &aabb, &newPairs);

    if (newPairs.m_size != 0) {
        void* filter = at<void*>(world, 0x6C);
        if (filter != NULL) {
            filter = reinterpret_cast<char*>(filter) + 8; // the collision filter interface sits 8 bytes in
        }
        fn_802CAEE8(at<void*>(world, 0x58), &newPairs[0], newPairs.m_size, filter);
    }
}

void hkWorldOperationUtil::removePhantomBP(hkWorld* world, hkPhantom* phantom) {
    hkLocalArray<hkBroadPhaseHandlePair> removedPairs(at<int>(world, 0x2E4));

    // vtable 0x18 on the broadphase takes an object out and reports the pairs that went with it
    void* broadPhase = at<void*>(world, 0x54);
    typedef void (*RemoveObjectFn)(void*, void*, hkArray<hkBroadPhaseHandlePair>*);
    reinterpret_cast<RemoveObjectFn>((*reinterpret_cast<void***>(broadPhase))[0x18 / 4])(broadPhase, reinterpret_cast<char*>(phantom) + 0x30, &removedPairs);

    if (removedPairs.m_size != 0) {
        fn_802CAFC0(at<void*>(world, 0x58), &removedPairs[0], removedPairs.m_size);
    }
}

// the entity version of addPhantomBP: same broadphase dance, but the box comes from the entity's shape,
// pushed out a bit by half of the world's collision tolerance
void hkWorldOperationUtil::addEntityBP(hkWorld* world, hkEntity* entity) {
    void* shape = at<void*>(entity, 0x1C);
    if (shape == NULL) {
        return;
    }

    hkLocalArray<hkBroadPhaseHandlePair> newPairs(at<int>(world, 0x2E4));

    // HYPOTHESIS: world +0x68 holds the collision settings and +8 in there is the tolerance
    float expansion = 0.5f * at<float>(at<void*>(world, 0x68), 8);
    hkAabb aabb;
    typedef void (*GetAabbFn)(void*, void*, float, hkAabb*);
    reinterpret_cast<GetAabbFn>((*reinterpret_cast<void***>(shape))[0x14 / 4])(shape, at<void*>(entity, 0x24), expansion, &aabb);

    void* broadPhase = at<void*>(world, 0x54);
    typedef void (*AddObjectFn)(void*, void*, hkAabb*, hkArray<hkBroadPhaseHandlePair>*);
    reinterpret_cast<AddObjectFn>((*reinterpret_cast<void***>(broadPhase))[0x10 / 4])(broadPhase, reinterpret_cast<char*>(entity) + 0x30, &aabb, &newPairs);

    if (newPairs.m_size > 0) {
        void* filter = at<void*>(world, 0x6C);
        if (filter != NULL) {
            filter = reinterpret_cast<char*>(filter) + 8;
        }
        fn_802CAEE8(at<void*>(world, 0x58), &newPairs[0], newPairs.m_size, filter);
    }
}

// takes the entity out of the broadphase. the simulation gets to throw away this entity's collision info first
void hkWorldOperationUtil::removeEntityBP(hkWorld* world, hkEntity* entity) {
    if (at<void*>(entity, 0x1C) == NULL) {
        return;
    }

    // HYPOTHESIS: world +8 is the simulation and its vtable 0x24 drops collision info for a list of entities
    hkEntity* entities[1] = {entity};
    u8 flag = 1;
    void* simulation = at<void*>(world, 8);
    typedef void (*DropInfoFn)(void*, hkEntity**, int, hkWorld*, u8*);
    reinterpret_cast<DropInfoFn>((*reinterpret_cast<void***>(simulation))[0x24 / 4])(simulation, entities, 1, world, &flag);

    hkLocalArray<hkBroadPhaseHandlePair> removedPairs(at<int>(world, 0x2E4));
    void* broadPhase = at<void*>(world, 0x54);
    typedef void (*RemoveObjectFn)(void*, void*, hkArray<hkBroadPhaseHandlePair>*);
    reinterpret_cast<RemoveObjectFn>((*reinterpret_cast<void***>(broadPhase))[0x18 / 4])(broadPhase, reinterpret_cast<char*>(entity) + 0x30, &removedPairs);

    if (removedPairs.m_size > 0) {
        fn_802CAFC0(at<void*>(world, 0x58), &removedPairs[0], removedPairs.m_size);
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

void hkWorldOperationUtil::removeIsland(hkWorld* world, hkSimulationIsland* island) {
    if ((island->m_flags26 & 3) == 0) {
        removeIslandFromArray(*(hkArray<hkSimulationIsland*>*)&world->m_unk34, island);
    } else {
        removeIslandFromArray(*(hkArray<hkSimulationIsland*>*)&world->m_unk28, island);
    }
    removeIslandFromDirtyList(world, island);
}

hkConstraintInstance* hkWorldOperationUtil::addConstraintImmediately(hkWorld* world, hkConstraintInstance* constraint, bool fireCallback) {
    world->m_lockCount++;
    hkWorldConstraintUtil::addConstraint(world, constraint);
    if (fireCallback) {
        hkWorldCallbackUtil::fireConstraintAdded(world, constraint);
    }
    if (--world->m_lockCount == 0 && world->m_pendingOperationsCount != 0 && (s8)world->m_unk84 == 0) {
        world->internal_executePendingOperations();
    }
    return constraint;
}

void hkWorldOperationUtil::mergeIslands(hkWorld* world, hkEntity* entityA, hkEntity* entityB) {
    hkSimulationIsland* islandA = entityA->m_simulationIsland;
    hkSimulationIsland* islandB = entityB->m_simulationIsland;
    if (world->m_lockCount != 0) {
        hkWorldOperation::BiggestOperation operation;
        operation.m_kind = 0x0C;
        operation.m_entityA = entityA;
        operation.m_entityB = entityB;
        world->queueOperation(&operation);
    } else {
        internalMergeTwoIslands(world, islandA, islandB);
    }
}

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
        if (--world->m_lockCount == 0 && world->m_pendingOperationsCount != 0 && (s8)world->m_unk84 == 0) {
            world->internal_executePendingOperations();
        }
    }
}
