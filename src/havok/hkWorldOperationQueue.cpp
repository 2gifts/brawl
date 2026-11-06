// Havok translation unit hkWorldOperationQueue.o (main.dol 0x802F5D5C-0x802F7910).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802F5D5C    44  __ct   [map: hkWorldOperationQueue____ct]
//   0x802F5D88   188  __dt   [map: hkWorldOperationQueue____dt]
//   0x802F5E44  2088  queueOperation   [map: hkWorldOperationQueue__queueOperation]
//   0x802F666C    84  islandLess   [map: hkWorldOperation16BiggestOperationRCQ216hkWorldOperation16BiggestOperation__islandLess]
//   0x802F66C0  4200  executeAllPending   [map: hkWorldOperationQueue__executeAllPending]
//   0x802F7728    28  quickSort<Q216hkWorldOperation16BiggestOperation,PFRCQ216   [map: hkAlgorithm__quickSort_Q216hkWorldOperation16BiggestOperation_PFRCQ216hkWorldOperation16BiggestOperationRCQ216hkW___]
//   0x802F7744   460  quickSortRecursive<Q216hkWorldOperation16BiggestOperation   [map: hkAlgorithm__quickSortRecursive_Q216hkWorldOperation16BiggestOperation_PFRCQ216hkWorldOperation16BiggestOperation___]

#include <havok/hkWorldOperationQueue.h>
#include <havok/hkSimulationIsland.h>
#include <havok/hkEntity.h>
#include <havok/hkMemory.h>
#include <havok/hkString.h>
#include <havok/hkWorld.h>
#include <havok/hkWorldOperationUtil.h>
#include <havok/hkRigidBody.h>
#include <havok/hkPhantom.h>
#include <havok/hkMotion.h>
#include <havok/hkSimulation.h>
#include <havok/hkAction.h>

#include <havok/hkAlgorithm.h>

hkWorldOperationQueue::hkWorldOperationQueue(hkWorld* world) {
    m_world = world;
    m_unk1C = 0;
}

hkWorldOperationQueue::~hkWorldOperationQueue() {}

// every queued operation holds a reference on what it points at until it runs
static inline void addReferenceTo(void* obj) { static_cast<hkReferencedObject*>(obj)->addReference(); }
static inline void removeReferenceFrom(void* obj) { static_cast<hkReferencedObject*>(obj)->removeReference(); }

// batches own a private copy of the caller's pointer list (memory class 4), plus a reference on each pointer.
// the original has this spelled out in every batch case instead of calling a helper, so it's a macro here
#define QUEUE_BATCH_CASE(NAME)                                                                       \
    case hkWorldOperation::NAME: {                                                                  \
        hkWorldOperation::BatchOperation& cop = static_cast<hkWorldOperation::BatchOperation&>(op); \
        const hkWorldOperation::BatchOperation& src =                                               \
            static_cast<const hkWorldOperation::BatchOperation&>(operation);                        \
        cop.m_objects = static_cast<void**>(hkMemory::getInstance().allocateChunk(cop.m_count * 4, 4)); \
        hkString::memCpy(cop.m_objects, src.m_objects, src.m_count * 4);                            \
        void** p = cop.m_objects;                                                                   \
        void** pEnd = p + cop.m_count;                                                              \
        while (p < pEnd) {                                                                          \
            addReferenceTo((p++)[0]);                                                               \
        }                                                                                           \
        break;                                                                                      \
    }

// most operations just point at one thing at +4 that has to stay alive
#define QUEUE_SINGLE_CASE(NAME)                                                                          \
    case hkWorldOperation::NAME:                                                                         \
        addReferenceTo(static_cast<hkWorldOperation::SingleObjectOperation&>(op).m_object);              \
        break;

void hkWorldOperationQueue::queueOperation(const hkWorldOperation::BaseOperation& operation) {
    hkWorldOperation::BiggestOperation& slot = m_operations0.expandOne();
    m_world->m_pendingOperationsCount++;

    // BYTES: the target has copy propagation off here (the m_unk1C hkBool test is materialized, the grow test reads
    // the capacity word into its own register). HYPOTHESIS: an aligned local is the trigger (TIPS); which local the
    // original has is unknown. It beats `#pragma opt_propagation off` (99.70) on this function.
    hkWorldOperation::BaseOperation* opp __attribute__((aligned(4))) = reinterpret_cast<hkWorldOperation::BaseOperation*>(&slot);
    hkWorldOperation::BaseOperation& op = *opp;
    hkString::memCpy(&op, &operation, sizeof(hkWorldOperation::BiggestOperation));

    switch (op.m_type) {
        QUEUE_SINGLE_CASE(ENTITY_ADD)
        QUEUE_SINGLE_CASE(ENTITY_REMOVE)
        QUEUE_SINGLE_CASE(RIGIDBODY_SET_MOTION_TYPE)

        case hkWorldOperation::WORLD_OBJECT_SET_SHAPE: {
            hkWorldOperation::SetWorldObjectShape& cop = static_cast<hkWorldOperation::SetWorldObjectShape&>(op);
            addReferenceTo(cop.m_worldObject);
            addReferenceTo(cop.m_shape);
            break;
        }

        QUEUE_BATCH_CASE(ENTITY_BATCH_ADD)
        QUEUE_BATCH_CASE(ENTITY_BATCH_REMOVE)

        QUEUE_SINGLE_CASE(CONSTRAINT_ADD)
        QUEUE_SINGLE_CASE(CONSTRAINT_REMOVE)
        QUEUE_SINGLE_CASE(ACTION_ADD)
        QUEUE_SINGLE_CASE(ACTION_REMOVE)

        case hkWorldOperation::ISLAND_MERGE: {
            hkWorldOperation::MergeIslands& cop = static_cast<hkWorldOperation::MergeIslands&>(op);
            addReferenceTo(cop.m_entities[0]);
            addReferenceTo(cop.m_entities[1]);
            if (m_unk1C) {
                // while the world is splitting islands the merges go on their own list so they can be replayed first
                m_operations1.pushBack(slot);
                m_operations0.m_size--;
            }
            break;
        }

        QUEUE_SINGLE_CASE(PHANTOM_ADD)
        QUEUE_SINGLE_CASE(PHANTOM_REMOVE)
        QUEUE_BATCH_CASE(PHANTOM_BATCH_ADD)
        QUEUE_BATCH_CASE(PHANTOM_BATCH_REMOVE)
        // the target places this case here, next to the phantom one (jump table order)
        QUEUE_SINGLE_CASE(ENTITY_UPDATE_BROAD_PHASE)

        case hkWorldOperation::PHANTOM_UPDATE_BROAD_PHASE: {
            hkWorldOperation::UpdatePhantomBP& cop = static_cast<hkWorldOperation::UpdatePhantomBP&>(op);
            const hkWorldOperation::UpdatePhantomBP& src = static_cast<const hkWorldOperation::UpdatePhantomBP&>(operation);
            addReferenceTo(cop.m_phantom);
            // the queue keeps its own copy of the aabb (the target null-checks the allocation before the copy)
            cop.m_aabb = new hkAabb(src.m_aabb->m_min, src.m_aabb->m_max);
            break;
        }

        QUEUE_SINGLE_CASE(UPDATE_FILTER_ENTITY)
        QUEUE_SINGLE_CASE(UPDATE_FILTER_PHANTOM)

        case hkWorldOperation::UPDATE_FILTER_WORLD:
            break;

        QUEUE_BATCH_CASE(ENTITY_BATCH_REINTEGRATE_AND_RECOLLIDE)
        QUEUE_SINGLE_CASE(UPDATE_MOVED_BODY_INFO)
        QUEUE_SINGLE_CASE(RIGIDBODY_SET_POSITION_AND_ROTATION)
        QUEUE_SINGLE_CASE(RIGIDBODY_SET_LINEAR_VELOCITY)
        QUEUE_SINGLE_CASE(RIGIDBODY_SET_ANGULAR_VELOCITY)
        QUEUE_SINGLE_CASE(RIGIDBODY_APPLY_LINEAR_IMPULSE)
        QUEUE_SINGLE_CASE(RIGIDBODY_APPLY_POINT_IMPULSE)
        QUEUE_SINGLE_CASE(RIGIDBODY_APPLY_ANGULAR_IMPULSE)

        case hkWorldOperation::WORLD_OBJECT_ADD_REFERENCE:
            // not really queued, just a deferred reference grab. do it now and forget the slot
            addReferenceTo(static_cast<hkWorldOperation::SingleObjectOperation&>(op).m_object);
            m_operations0.m_size--;
            m_world->m_pendingOperationsCount--;
            break;

        case hkWorldOperation::WORLD_OBJECT_REMOVE_REFERENCE:
            removeReferenceFrom(static_cast<hkWorldOperation::SingleObjectOperation&>(op).m_object);
            m_operations0.m_size--;
            m_world->m_pendingOperationsCount--;
            break;

        case hkWorldOperation::ACTIVATE_REGION:
            break;

        QUEUE_SINGLE_CASE(ACTIVATE_ENTITY)
        QUEUE_SINGLE_CASE(DEACTIVATE_ENTITY)
        QUEUE_SINGLE_CASE(USER_CALLBACK)

        default:
            break;
    }
}

hkBool hkWorldOperation::BiggestOperation::islandLess(const BiggestOperation& a, const BiggestOperation& b) {
    u16 keyA = *(u16*)((char*)a.m_entityA->m_simulationIsland + 0x20); // HYPOTHESIS: island key at +0x20
    u16 keyB = *(u16*)((char*)b.m_entityA->m_simulationIsland + 0x20);
    return hkBool(keyA < keyB || (keyA == keyB && *(u16*)((char*)a.m_entityB->m_simulationIsland + 0x20) < *(u16*)((char*)b.m_entityB->m_simulationIsland + 0x20)));
}

// the executor pokes at a few classes nobody has laid out yet (constraint instances, actions, the simulation).
// raw offsets for now, each one is from the asm
template <typename T>
static inline T& at(void* p, int offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<char*>(p) + offset);
}

// View of the user callback's slot (its class has no virtuals declared yet; MWCC: hkReferencedObject's dtor is
// 0x08, calcStatistics 0x0C). HYPOTHESIS: the name, from what the case does.
struct hkWorldOperationUserCallback : hkReferencedObject {
    virtual void worldOperationUserCallback(int userData); // 0x10
};

static inline hkMotion* motionOf(hkRigidBody* body) { return reinterpret_cast<hkMotion*>(body->m_motion); }

// the pointer lists the batch ops carry get freed the same way they were allocated: count * 4 bytes, class 4
// motion type 7 is fixed. HYPOTHESIS: an inline hkBool query on the entity's motion (byte 0xA8)
static inline hkBool isFixed(hkEntity* entity) {
    return hkBool(at<u8>(entity, 0xA8) == 7);
}
// HYPOTHESIS: the byte-sized mode fields are stored as enums in a byte (Havok's hkEnum); reading one converts it.
struct hkEnumU8 {
    u8 m_storage;
    operator int() const { return m_storage; }
};
static inline void removeReferencesFrom(void** objects, int count) {
    void** pEnd = objects + count;
    void** p = objects;
    while (p < pEnd) {
        removeReferenceFrom((p++)[0]);
    }
}
static inline void freeBatch(void* objects, int count) {
    hkMemory::getInstance().deallocateChunk(objects, count * 4, 4);
}

void hkWorldOperationQueue::executeAllPending() {
    // island merges are kept on their own list, they go first and sorted so the result is always the same
    if (m_operations1.m_size != 0) {
        hkAlgorithm::quickSort(static_cast<hkWorldOperation::BiggestOperation*>(m_operations1.m_data), m_operations1.m_size,
                               hkWorldOperation::BiggestOperation::islandLess);
        m_operations0.insertAt(0, m_operations1);
        m_operations1.m_size = 0;
    }

    // take everything that's pending right now, so anything queued while this runs lands in a fresh list
    // BYTES: an aligned local puts this function in the propagation-off mode the target shows (N-form hkBool
    // tests, the inplace element ctor loop not unrolled). HYPOTHESIS: which local carries it in the original.
    hkInplaceArray<hkWorldOperation::BiggestOperation, 16> stolenPending __attribute__((aligned(4)));
    stolenPending.swap(m_operations0);
    m_world->m_pendingOperationQueueCount++;

    for (int i = 0; i < stolenPending.m_size; i++) {
        hkWorldOperation::BaseOperation& op = reinterpret_cast<hkWorldOperation::BaseOperation&>(stolenPending[i]);
        void* o = &op;
        // BYTES: with copy propagation off the target reads an operation field again after every call, so the
        // pointers below are references into the operation (T*&), not copies; the cases that keep a value across
        // a call (the rigid body ones, the merge) use a plain local.
        switch (op.m_type) {
            case hkWorldOperation::ENTITY_ADD: {
                hkEntity*& entity = at<hkEntity*>(o, 4);
                if (entity->m_world == NULL) {
                    m_world->addEntity(entity, at<int>(o, 8));
                }
                removeReferenceFrom(entity);
                break;
            }
            case hkWorldOperation::ENTITY_REMOVE: {
                hkEntity*& entity = at<hkEntity*>(o, 4);
                if (entity->m_world == m_world) {
                    m_world->removeEntity(entity);
                }
                removeReferenceFrom(entity);
                break;
            }
            case hkWorldOperation::RIGIDBODY_SET_MOTION_TYPE: {
                hkRigidBody*& body = at<hkRigidBody*>(o, 4);
                body->setMotionType(at<hkEnumU8>(o, 8), at<hkEnumU8>(o, 9), at<hkEnumU8>(o, 10));
                removeReferenceFrom(body);
                break;
            }
            case hkWorldOperation::WORLD_OBJECT_SET_SHAPE: {
                hkWorldObject*& object = at<hkWorldObject*>(o, 4);
                object->setShape(at<hkShape*>(o, 8));
                removeReferenceFrom(object);
                removeReferenceFrom(at<hkShape*>(o, 8));
                break;
            }
            case hkWorldOperation::ENTITY_BATCH_ADD: {
                // no check that these were already added, the caller is trusted
                hkWorldOperation::BatchOperation& batch = static_cast<hkWorldOperation::BatchOperation&>(op);
                bool active = at<bool>(o, 10);
                m_world->addEntityBatch(reinterpret_cast<hkEntity**>(batch.m_objects), batch.m_count, active);
                removeReferencesFrom(batch.m_objects, batch.m_count);
                freeBatch(batch.m_objects, batch.m_count);
                break;
            }
            case hkWorldOperation::ENTITY_BATCH_REMOVE: {
                hkWorldOperation::BatchOperation& batch = static_cast<hkWorldOperation::BatchOperation&>(op);
                m_world->removeEntityBatch(reinterpret_cast<hkEntity**>(batch.m_objects), batch.m_count);
                removeReferencesFrom(batch.m_objects, batch.m_count);
                freeBatch(batch.m_objects, batch.m_count);
                break;
            }
            case hkWorldOperation::CONSTRAINT_ADD: {
                void*& constraint = at<void*>(o, 4);
                // only adds it if it isn't owned yet and both of its entities are in this world
                if (at<void*>(constraint, 8) == NULL && at<hkEntity*>(constraint, 0x14)->m_world == m_world &&
                    at<hkEntity*>(constraint, 0x18)->m_world == m_world) {
                    m_world->addConstraint(static_cast<hkConstraintInstance*>(constraint));
                }
                removeReferenceFrom(constraint);
                break;
            }
            case hkWorldOperation::CONSTRAINT_REMOVE: {
                void*& constraint = at<void*>(o, 4);
                if (at<void*>(constraint, 8) != NULL) {
                    m_world->removeConstraint(static_cast<hkConstraintInstance*>(constraint));
                }
                removeReferenceFrom(constraint);
                break;
            }
            case hkWorldOperation::ACTION_ADD: {
                void*& action = at<void*>(o, 4);
                if (at<void*>(action, 8) == NULL) {
                    // an action can only go in once every entity it touches is already in this world
                    hkArray<hkEntity*> actionEntities;
                    hkBool allInWorld = true;
                    static_cast<hkAction*>(action)->getEntities(actionEntities);
                    for (int k = 0; k < actionEntities.m_size; k++) {
                        if (actionEntities[k]->m_world != m_world) {
                            allInWorld = false;
                            break;
                        }
                    }
                    if (allInWorld) {
                        m_world->addAction(static_cast<hkAction*>(action));
                    }
                }
                removeReferenceFrom(action);
                break;
            }
            case hkWorldOperation::ACTION_REMOVE: {
                void*& action = at<void*>(o, 4);
                if (at<void*>(action, 0xC) != NULL) {
                    m_world->removeAction(static_cast<hkAction*>(action));
                }
                removeReferenceFrom(action);
                break;
            }
            case hkWorldOperation::ISLAND_MERGE: {
                hkWorldOperation::MergeIslands& merge = static_cast<hkWorldOperation::MergeIslands&>(op);
                hkEntity* a = merge.m_entities[0];
                if (a->m_world == m_world) {
                    hkEntity* b = merge.m_entities[1];
                    if (b->m_world == m_world) {
                        // a body may have gotten pinned since this was queued, then the merge is just skipped
                        bool bothMovable = !bool(isFixed(a)) && !bool(isFixed(b));
                        bool doMerge = bothMovable && a->m_simulationIsland != b->m_simulationIsland;
                        if (doMerge) {
                            hkWorldOperationUtil::mergeIslands(a->m_world, a, b);
                        }
                    }
                }
                removeReferenceFrom(merge.m_entities[0]);
                removeReferenceFrom(merge.m_entities[1]);
                break;
            }
            case hkWorldOperation::PHANTOM_ADD: {
                hkPhantom*& phantom = at<hkPhantom*>(o, 4);
                if (phantom->m_world == NULL) {
                    m_world->addPhantom(phantom);
                }
                removeReferenceFrom(phantom);
                break;
            }
            case hkWorldOperation::PHANTOM_REMOVE: {
                hkPhantom*& phantom = at<hkPhantom*>(o, 4);
                if (phantom->m_world == m_world) {
                    m_world->removePhantom(phantom);
                }
                removeReferenceFrom(phantom);
                break;
            }
            case hkWorldOperation::PHANTOM_BATCH_ADD: {
                hkWorldOperation::BatchOperation& batch = static_cast<hkWorldOperation::BatchOperation&>(op);
                m_world->addPhantomBatch(reinterpret_cast<hkPhantom**>(batch.m_objects), batch.m_count);
                removeReferencesFrom(batch.m_objects, batch.m_count);
                freeBatch(batch.m_objects, batch.m_count);
                break;
            }
            case hkWorldOperation::PHANTOM_BATCH_REMOVE: {
                hkWorldOperation::BatchOperation& batch = static_cast<hkWorldOperation::BatchOperation&>(op);
                m_world->removePhantomBatch(reinterpret_cast<hkPhantom**>(batch.m_objects), batch.m_count);
                removeReferencesFrom(batch.m_objects, batch.m_count);
                freeBatch(batch.m_objects, batch.m_count);
                break;
            }
            case hkWorldOperation::ENTITY_UPDATE_BROAD_PHASE: {
                hkEntity*& entity = at<hkEntity*>(o, 4);
                if (entity->m_world == m_world) {
                    hkWorldOperationUtil::updateEntityBP(m_world, entity);
                }
                // this one goes through hkWorldObject's own (out-of-line) removeReference
                entity->removeReference();
                break;
            }
            case hkWorldOperation::PHANTOM_UPDATE_BROAD_PHASE: {
                hkPhantom*& phantom = at<hkPhantom*>(o, 4);
                void*& aabb = at<void*>(o, 8);
                if (phantom->m_world == m_world) {
                    phantom->updateBroadPhase(aabb);
                }
                removeReferenceFrom(phantom);
                hkMemory::getInstance().deallocateChunk(aabb, 0x20, 0x25);
                break;
            }
            case hkWorldOperation::UPDATE_FILTER_ENTITY: {
                hkEntity*& entity = at<hkEntity*>(o, 4);
                if (entity->m_world == m_world) {
                    m_world->updateCollisionFilterOnEntity(entity, at<hkEnumU8>(o, 8), at<hkEnumU8>(o, 9));
                }
                removeReferenceFrom(entity);
                break;
            }
            case hkWorldOperation::UPDATE_FILTER_PHANTOM: {
                hkPhantom*& phantom = at<hkPhantom*>(o, 4);
                if (phantom->m_world == m_world) {
                    m_world->updateCollisionFilterOnPhantom(phantom, at<hkEnumU8>(o, 8));
                }
                removeReferenceFrom(phantom);
                break;
            }
            case hkWorldOperation::UPDATE_FILTER_WORLD:
                m_world->updateCollisionFilterOnWorld(at<hkEnumU8>(o, 1), at<hkEnumU8>(o, 2));
                break;
            case hkWorldOperation::UPDATE_MOVED_BODY_INFO: {
                hkRigidBody*& body = at<hkRigidBody*>(o, 4);
                body->updateBroadphaseAndResetCollisionInformationOfWarpedBody();
                removeReferenceFrom(body);
                break;
            }
            case hkWorldOperation::RIGIDBODY_SET_POSITION_AND_ROTATION: {
                hkRigidBody*& body = at<hkRigidBody*>(o, 4);
                hkVector4*& posAndRot = at<hkVector4*>(o, 8);
                body->setPositionAndRotation(posAndRot[0], reinterpret_cast<hkQuaternion&>(posAndRot[1]));
                hkMemory::getInstance().deallocateChunk(posAndRot, 0x20, 4);
                removeReferenceFrom(body);
                break;
            }
            case hkWorldOperation::RIGIDBODY_SET_LINEAR_VELOCITY: {
                hkVector4 v(at<float>(o, 8), at<float>(o, 12), at<float>(o, 16), 0.0f);
                hkRigidBody* body = at<hkRigidBody*>(o, 4);
                body->activate();
                motionOf(body)->setLinearVelocity(v);
                removeReferenceFrom(at<hkRigidBody*>(o, 4));
                break;
            }
            case hkWorldOperation::RIGIDBODY_SET_ANGULAR_VELOCITY: {
                hkVector4 v(at<float>(o, 8), at<float>(o, 12), at<float>(o, 16), 0.0f);
                hkRigidBody* body = at<hkRigidBody*>(o, 4);
                body->activate();
                motionOf(body)->setAngularVelocity(v);
                removeReferenceFrom(at<hkRigidBody*>(o, 4));
                break;
            }
            case hkWorldOperation::RIGIDBODY_APPLY_LINEAR_IMPULSE: {
                hkVector4 v(at<float>(o, 8), at<float>(o, 12), at<float>(o, 16), 0.0f);
                hkRigidBody* body = at<hkRigidBody*>(o, 4);
                body->activate();
                motionOf(body)->applyLinearImpulse(v);
                removeReferenceFrom(at<hkRigidBody*>(o, 4));
                break;
            }
            case hkWorldOperation::RIGIDBODY_APPLY_POINT_IMPULSE: {
                // the point and the impulse travel in one 0x20 byte block: point first, impulse second
                hkVector4* pointAndImpulse = at<hkVector4*>(o, 8);
                hkRigidBody* body = at<hkRigidBody*>(o, 4);
                const hkVector4& impulse = pointAndImpulse[1];
                body->activate();
                motionOf(body)->applyPointImpulse(impulse, pointAndImpulse[0]);
                hkMemory::getInstance().deallocateChunk(at<hkVector4*>(o, 8), 0x20, 4);
                removeReferenceFrom(at<hkRigidBody*>(o, 4));
                break;
            }
            case hkWorldOperation::RIGIDBODY_APPLY_ANGULAR_IMPULSE: {
                hkVector4 v(at<float>(o, 8), at<float>(o, 12), at<float>(o, 16), 0.0f);
                hkRigidBody* body = at<hkRigidBody*>(o, 4);
                body->activate();
                motionOf(body)->applyAngularImpulse(v);
                removeReferenceFrom(at<hkRigidBody*>(o, 4));
                break;
            }
            case hkWorldOperation::ACTIVATE_REGION: {
                void*& aabb = at<void*>(o, 4);
                m_world->activateRegion(static_cast<hkAabb*>(aabb));
                hkMemory::getInstance().deallocateChunk(aabb, 0x20, 4);
                break;
            }
            case hkWorldOperation::ACTIVATE_ENTITY: {
                hkEntity*& entity = at<hkEntity*>(o, 4);
                entity->activate();
                removeReferenceFrom(entity);
                break;
            }
            case hkWorldOperation::DEACTIVATE_ENTITY: {
                hkEntity*& entity = at<hkEntity*>(o, 4);
                if (entity->m_world == m_world) {
                    entity->deactivate();
                }
                removeReferenceFrom(entity);
                break;
            }
            case hkWorldOperation::ENTITY_BATCH_REINTEGRATE_AND_RECOLLIDE: {
                hkWorldOperation::BatchOperation& batch = static_cast<hkWorldOperation::BatchOperation&>(op);
                // HYPOTHESIS: world +8 is the simulation
                reinterpret_cast<hkSimulation*>(m_world->m_unk08)
                    ->reintegrateAndRecollideEntities(reinterpret_cast<hkEntity**>(batch.m_objects), batch.m_count, m_world);
                removeReferencesFrom(batch.m_objects, batch.m_count);
                freeBatch(batch.m_objects, batch.m_count);
                break;
            }
            case hkWorldOperation::USER_CALLBACK: {
                void*& callback = at<void*>(o, 4);
                reinterpret_cast<hkWorldOperationUserCallback*>(callback)->worldOperationUserCallback(at<int>(o, 8));
                removeReferenceFrom(callback);
                break;
            }
            default:
                break;
        }

        // running that may have queued more, deal with those before moving on
        if (m_operations0.m_size != 0) {
            executeAllPending();
        }
    }

    stolenPending.swap(m_operations0);
    m_operations0.m_size = 0;
    m_world->m_pendingOperationQueueCount--;
}

typedef hkBool (*BiggestOperationLess)(const hkWorldOperation::BiggestOperation&, const hkWorldOperation::BiggestOperation&);

