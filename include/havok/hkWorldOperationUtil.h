#pragma once

#include <havok/hkBase.h>
#include <havok/hkWorld.h>
#include <havok/hkMotion.h>
#include <havok/hkWorldOperationQueue.h>

struct hkSimulationIsland;
struct hkConstraintInstance;
struct hkEntity;
struct hkRigidBody;
struct hkPhantom;
struct hkAction;
struct hkAgentNnTrack;

// Operations on the world's islands, entities, phantoms and constraints, run while the world is locked
// or when queued operations are executed.
// Activation argument of the entity add functions.
enum {
    HK_ENTITY_ACTIVATION_DO_NOT_ACTIVATE = 0,
    HK_ENTITY_ACTIVATION_DO_ACTIVATE = 1,
};

struct hkWorldOperationUtil {
    static void addEntitySI(hkWorld* world, hkEntity* entity, int activation);
    static void removeEntitySI(hkWorld* world, hkEntity* entity);
    static void removeAttachedActionsFromFixedIsland(hkWorld* world, hkEntity* entity,
                                                       hkArray<hkAction*>& actionsToBeMoved);
    static void removeAttachedActionsFromDynamicIsland(hkWorld* world, hkEntity* entity,
                                                       hkArray<hkAction*>& actionsToBeMoved);
    static void addActionsToEntitysIsland(hkWorld* world, hkEntity* entity, hkArray<hkAction*>& actions);
    static void addConstraintToCriticalLockedIsland(hkWorld* world, hkConstraintInstance* constraint);
    static void removeConstraintFromCriticalLockedIsland(hkWorld* world, hkConstraintInstance* constraint);
    static void splitSimulationIslands(hkWorld* world);
    // (map name; hkSimulation::integrateInternal runs it before the actions). HYPOTHESIS: what it cleans up.
    static void removeAttachedConstraints(hkEntity* entity, hkArray<hkConstraintInstance*>& removedConstraints);
    static void removeAttachedAgentsConnectingTheEntityAndAFixedPartnerEntityPlus(hkAgentNnTrack& trackToScan,
                                                                                hkEntity* entity,
                                                                                hkAgentNnTrack& agentsRemoved,
                                                                                hkMotion::MotionType newMotionType);
    static void cleanupDirtyIslands(hkWorld* world);
    static void internalActivateIsland(hkWorld* world, hkSimulationIsland* island);
    static void internalDeactivateIsland(hkWorld* world, hkSimulationIsland* island);
    static void splitSimulationIslands1(hkWorld* world, u8* flag);
    static void removeIslandFromDirtyList(hkWorld* world, hkSimulationIsland* island);
    static void removeIsland(hkWorld* world, hkSimulationIsland* island);
    static void mergeIslands(hkWorld* world, hkEntity* entityA, hkEntity* entityB);
    static void internalMergeTwoIslands(hkWorld* world, hkSimulationIsland* islandA, hkSimulationIsland* islandB);
    static void removeConstraintImmediately(hkWorld* world, hkConstraintInstance* constraint, bool fireCallback);
    static hkConstraintInstance* addConstraintImmediately(hkWorld* world, hkConstraintInstance* constraint, bool fireCallback);
    static void markIslandInactive(hkWorld* world, hkSimulationIsland* island);
    static void markIslandActive(hkWorld* world, hkSimulationIsland* island);
    static void updateEntityBP(hkWorld* world, hkEntity* entity);
    static void addEntityBP(hkWorld* world, hkEntity* entity);
    static void removeEntityBP(hkWorld* world, hkEntity* entity);
    static void addPhantomBP(hkWorld* world, hkPhantom* phantom);
    static void removePhantomBP(hkWorld* world, hkPhantom* phantom);
    static void setRigidBodyMotionType(hkRigidBody* body, u8 newType, u8 activation, u8 collisionFilterUpdateMode);
    // Swaps the body's motion object for one of the new type (the movable flags go by value).
    static void replaceMotionObject(hkRigidBody* body, hkMotion::MotionType newType, hkBool newIsMovable,
                                    hkBool oldIsMovable, hkWorld* world);
};
