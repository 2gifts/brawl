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

// Local algorithm helper (hkAlgorithm in the Havok TU map). Used only by the operation queue.
// the original keeps quickSort as its own function, so no inlining while these are defined
#pragma dont_inline on
struct hkAlgorithm {
    template <typename T, typename Less>
    static void quickSortRecursive(T* elements, int lo, int hi, Less less) {
        for (;;) {
            int i = lo;
            int j = hi;
            T pivot = elements[(lo + hi) >> 1];
            do {
                while (less(elements[i], pivot)) {
                    i++;
                }
                while (less(pivot, elements[j])) {
                    j--;
                }
                if (j < i) {
                } else if (j == i) {
                    i++;
                    j--;
                } else {
                    T tmp = elements[i];
                    elements[i] = elements[j];
                    elements[j] = tmp;
                    i++;
                    j--;
                }
            } while (i <= j);
            if (lo < j) {
                quickSortRecursive(elements, lo, j, less);
            }
            if (i >= hi) {
                return;
            }
            lo = i;
        }
    }

    template <typename T, typename Less>
    static void quickSort(T* elements, int n, Less less) {
        if (n > 1) {
            quickSortRecursive(elements, 0, n - 1, less);
        }
    }
};
#pragma dont_inline reset

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

    hkWorldOperation::BaseOperation& op = reinterpret_cast<hkWorldOperation::BaseOperation&>(slot);
    hkString::memCpy(&op, &operation, sizeof(hkWorldOperation::BiggestOperation));

    switch (op.m_type) {
        QUEUE_SINGLE_CASE(ENTITY_ADD)
        QUEUE_SINGLE_CASE(ENTITY_REMOVE)
        QUEUE_SINGLE_CASE(ENTITY_UPDATE_BROAD_PHASE)
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

        case hkWorldOperation::PHANTOM_UPDATE_BROAD_PHASE: {
            hkWorldOperation::UpdatePhantomBP& cop = static_cast<hkWorldOperation::UpdatePhantomBP&>(op);
            const hkWorldOperation::UpdatePhantomBP& src = static_cast<const hkWorldOperation::UpdatePhantomBP&>(operation);
            addReferenceTo(cop.m_phantom);
            // HYPOTHESIS: the queue keeps its own 0x20 byte copy of the aabb (5.1 does a `new hkAabb`)
            cop.m_aabb = static_cast<hkAabb*>(hkMemory::getInstance().allocateChunk(0x20, 0x25));
            hkString::memCpy(cop.m_aabb, src.m_aabb, 0x20);
            break;
        }

        QUEUE_SINGLE_CASE(UPDATE_FILTER_ENTITY)
        QUEUE_SINGLE_CASE(UPDATE_FILTER_PHANTOM)

        case hkWorldOperation::UPDATE_FILTER_WORLD:
            break;

        QUEUE_SINGLE_CASE(UPDATE_MOVED_BODY_INFO)
        QUEUE_BATCH_CASE(ENTITY_BATCH_REINTEGRATE_AND_RECOLLIDE)
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
    bool result = false;
    u16 keyA = *(u16*)((char*)a.m_entityA->m_simulationIsland + 0x20); // HYPOTHESIS: island key at +0x20
    u16 keyB = *(u16*)((char*)b.m_entityA->m_simulationIsland + 0x20);
    if (keyA < keyB) {
        result = true;
    } else if (keyA == keyB) {
        u16 tieA = *(u16*)((char*)a.m_entityB->m_simulationIsland + 0x20);
        u16 tieB = *(u16*)((char*)b.m_entityB->m_simulationIsland + 0x20);
        if (tieA < tieB) {
            result = true;
        }
    }
    return hkBool(result);
}

// the executor pokes at a few classes nobody has laid out yet (constraint instances, actions, the simulation).
// raw offsets for now, each one is from the asm
template <typename T>
static inline T& at(void* p, int offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<char*>(p) + offset);
}

// call a virtual function by its vtable byte offset, for slots whose class has no header yet
template <typename R, typename A, typename B>
static inline R vcall2(void* obj, int slot, A a, B b) {
    typedef R (*Fn)(void*, A, B);
    return reinterpret_cast<Fn>((*reinterpret_cast<void***>(obj))[slot / 4])(obj, a, b);
}

static inline hkMotion* motionOf(hkRigidBody* body) { return reinterpret_cast<hkMotion*>(body->m_motion); }

// the pointer lists the batch ops carry get freed the same way they were allocated: count * 4 bytes, class 4
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
    hkInplaceArray<hkWorldOperation::BiggestOperation, 16> stolenPending;
    stolenPending.swap(m_operations0);
    m_world->m_pendingOperationQueueCount++;

    for (int i = 0; i < stolenPending.m_size; i++) {
        hkWorldOperation::BaseOperation& op = reinterpret_cast<hkWorldOperation::BaseOperation&>(stolenPending[i]);
        void* o = &op;
        switch (op.m_type) {
            case hkWorldOperation::ENTITY_ADD: {
                hkEntity* entity = at<hkEntity*>(o, 4);
                if (entity->m_world == NULL) {
                    m_world->addEntity(entity, at<int>(o, 8));
                }
                removeReferenceFrom(entity);
                break;
            }
            case hkWorldOperation::ENTITY_REMOVE: {
                hkEntity* entity = at<hkEntity*>(o, 4);
                if (entity->m_world == m_world) {
                    m_world->removeEntity(entity);
                }
                removeReferenceFrom(entity);
                break;
            }
            case hkWorldOperation::RIGIDBODY_SET_MOTION_TYPE: {
                hkRigidBody* body = at<hkRigidBody*>(o, 4);
                body->setMotionType(at<u8>(o, 8), at<u8>(o, 9), at<u8>(o, 10));
                removeReferenceFrom(body);
                break;
            }
            case hkWorldOperation::WORLD_OBJECT_SET_SHAPE: {
                hkWorldObject* object = at<hkWorldObject*>(o, 4);
                // HYPOTHESIS: setShape is a virtual here (vtable 0x10), hkWorldObject.h has it as a plain member
                vcall2<void, hkShape*, int>(object, 0x10, at<hkShape*>(o, 8), 0);
                removeReferenceFrom(object);
                removeReferenceFrom(at<hkShape*>(o, 8));
                break;
            }
            case hkWorldOperation::ENTITY_BATCH_ADD: {
                // no check that these were already added, the caller is trusted
                hkEntity** entities = at<hkEntity**>(o, 4);
                int count = at<u16>(o, 8);
                m_world->addEntityBatch(entities, count, at<u8>(o, 10));
                for (hkEntity** e = entities; e < entities + count; e++) {
                    removeReferenceFrom(*e);
                }
                freeBatch(entities, count);
                break;
            }
            case hkWorldOperation::ENTITY_BATCH_REMOVE: {
                hkEntity** entities = at<hkEntity**>(o, 4);
                int count = at<u16>(o, 8);
                m_world->removeEntityBatch(entities, count);
                for (hkEntity** e = entities; e < entities + count; e++) {
                    removeReferenceFrom(*e);
                }
                freeBatch(entities, count);
                break;
            }
            case hkWorldOperation::CONSTRAINT_ADD: {
                void* constraint = at<void*>(o, 4);
                // only adds it if it isn't owned yet and both of its entities are in this world
                if (at<void*>(constraint, 8) == NULL && at<hkEntity*>(constraint, 0x14)->m_world == m_world &&
                    at<hkEntity*>(constraint, 0x18)->m_world == m_world) {
                    m_world->addConstraint(static_cast<hkConstraintInstance*>(constraint));
                }
                removeReferenceFrom(constraint);
                break;
            }
            case hkWorldOperation::CONSTRAINT_REMOVE: {
                void* constraint = at<void*>(o, 4);
                if (at<void*>(constraint, 8) != NULL) {
                    m_world->removeConstraint(static_cast<hkConstraintInstance*>(constraint));
                }
                removeReferenceFrom(constraint);
                break;
            }
            case hkWorldOperation::ACTION_ADD: {
                void* action = at<void*>(o, 4);
                if (at<void*>(action, 8) == NULL) {
                    // an action can only go in once every entity it touches is already in this world
                    hkArray<hkEntity*> actionEntities;
                    vcall2<void, hkArray<hkEntity*>*, int>(action, 0x14, &actionEntities, 0);
                    bool allInWorld = true;
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
                void* action = at<void*>(o, 4);
                if (at<void*>(action, 0xC) != NULL) {
                    m_world->removeAction(static_cast<hkAction*>(action));
                }
                removeReferenceFrom(action);
                break;
            }
            case hkWorldOperation::ISLAND_MERGE: {
                hkEntity* a = at<hkEntity*>(o, 4);
                hkEntity* b = at<hkEntity*>(o, 8);
                if (a->m_world == m_world && b->m_world == m_world) {
                    // a body may have gotten pinned (motion type 7 is fixed) since this was queued, then the merge is just skipped
                    bool bothMovable = at<u8>(a, 0xA8) != 7 && at<u8>(b, 0xA8) != 7;
                    if (bothMovable && a->m_simulationIsland != b->m_simulationIsland) {
                        hkWorldOperationUtil::mergeIslands(a->m_world, a, b);
                    }
                }
                removeReferenceFrom(a);
                removeReferenceFrom(b);
                break;
            }
            case hkWorldOperation::PHANTOM_ADD: {
                hkPhantom* phantom = at<hkPhantom*>(o, 4);
                if (phantom->m_world == NULL) {
                    m_world->addPhantom(phantom);
                }
                removeReferenceFrom(phantom);
                break;
            }
            case hkWorldOperation::PHANTOM_REMOVE: {
                hkPhantom* phantom = at<hkPhantom*>(o, 4);
                if (phantom->m_world == m_world) {
                    m_world->removePhantom(phantom);
                }
                removeReferenceFrom(phantom);
                break;
            }
            case hkWorldOperation::PHANTOM_BATCH_ADD: {
                hkPhantom** phantoms = at<hkPhantom**>(o, 4);
                int count = at<u16>(o, 8);
                m_world->addPhantomBatch(phantoms, count);
                for (hkPhantom** p = phantoms; p < phantoms + count; p++) {
                    removeReferenceFrom(*p);
                }
                freeBatch(phantoms, count);
                break;
            }
            case hkWorldOperation::PHANTOM_BATCH_REMOVE: {
                hkPhantom** phantoms = at<hkPhantom**>(o, 4);
                int count = at<u16>(o, 8);
                m_world->removePhantomBatch(phantoms, count);
                for (hkPhantom** p = phantoms; p < phantoms + count; p++) {
                    removeReferenceFrom(*p);
                }
                freeBatch(phantoms, count);
                break;
            }
            case hkWorldOperation::ENTITY_UPDATE_BROAD_PHASE: {
                hkEntity* entity = at<hkEntity*>(o, 4);
                if (entity->m_world == m_world) {
                    hkWorldOperationUtil::updateEntityBP(m_world, entity);
                }
                removeReferenceFrom(entity);
                break;
            }
            case hkWorldOperation::PHANTOM_UPDATE_BROAD_PHASE: {
                hkPhantom* phantom = at<hkPhantom*>(o, 4);
                void* aabb = at<void*>(o, 8);
                if (phantom->m_world == m_world) {
                    phantom->updateBroadPhase(aabb);
                }
                removeReferenceFrom(phantom);
                hkMemory::getInstance().deallocateChunk(aabb, 0x20, 0x25);
                break;
            }
            case hkWorldOperation::UPDATE_FILTER_ENTITY: {
                hkEntity* entity = at<hkEntity*>(o, 4);
                if (entity->m_world == m_world) {
                    m_world->updateCollisionFilterOnEntity(entity, at<u8>(o, 8), at<u8>(o, 9));
                }
                removeReferenceFrom(entity);
                break;
            }
            case hkWorldOperation::UPDATE_FILTER_PHANTOM: {
                hkPhantom* phantom = at<hkPhantom*>(o, 4);
                if (phantom->m_world == m_world) {
                    m_world->updateCollisionFilterOnPhantom(phantom, at<u8>(o, 8));
                }
                removeReferenceFrom(phantom);
                break;
            }
            case hkWorldOperation::UPDATE_FILTER_WORLD:
                m_world->updateCollisionFilterOnWorld(at<u8>(o, 1), at<u8>(o, 2));
                break;
            case hkWorldOperation::UPDATE_MOVED_BODY_INFO: {
                hkRigidBody* body = at<hkRigidBody*>(o, 4);
                body->updateBroadphaseAndResetCollisionInformationOfWarpedBody();
                removeReferenceFrom(body);
                break;
            }
            case hkWorldOperation::RIGIDBODY_SET_POSITION_AND_ROTATION: {
                hkRigidBody* body = at<hkRigidBody*>(o, 4);
                hkVector4* posAndRot = at<hkVector4*>(o, 8);
                body->setPositionAndRotation(posAndRot[0], reinterpret_cast<hkQuaternion&>(posAndRot[1]));
                hkMemory::getInstance().deallocateChunk(posAndRot, 0x20, 4);
                removeReferenceFrom(body);
                break;
            }
            case hkWorldOperation::RIGIDBODY_SET_LINEAR_VELOCITY: {
                hkRigidBody* body = at<hkRigidBody*>(o, 4);
                hkVector4 v(at<float>(o, 8), at<float>(o, 12), at<float>(o, 16), 0.0f);
                body->activate();
                motionOf(body)->setLinearVelocity(v);
                removeReferenceFrom(body);
                break;
            }
            case hkWorldOperation::RIGIDBODY_SET_ANGULAR_VELOCITY: {
                hkRigidBody* body = at<hkRigidBody*>(o, 4);
                hkVector4 v(at<float>(o, 8), at<float>(o, 12), at<float>(o, 16), 0.0f);
                body->activate();
                motionOf(body)->setAngularVelocity(v);
                removeReferenceFrom(body);
                break;
            }
            case hkWorldOperation::RIGIDBODY_APPLY_LINEAR_IMPULSE: {
                hkRigidBody* body = at<hkRigidBody*>(o, 4);
                hkVector4 v(at<float>(o, 8), at<float>(o, 12), at<float>(o, 16), 0.0f);
                body->activate();
                motionOf(body)->applyLinearImpulse(v);
                removeReferenceFrom(body);
                break;
            }
            case hkWorldOperation::RIGIDBODY_APPLY_POINT_IMPULSE: {
                hkRigidBody* body = at<hkRigidBody*>(o, 4);
                hkVector4* pointAndImpulse = at<hkVector4*>(o, 8);
                body->activate();
                motionOf(body)->applyPointImpulse(pointAndImpulse[1], pointAndImpulse[0]);
                hkMemory::getInstance().deallocateChunk(pointAndImpulse, 0x20, 4);
                removeReferenceFrom(body);
                break;
            }
            case hkWorldOperation::RIGIDBODY_APPLY_ANGULAR_IMPULSE: {
                hkRigidBody* body = at<hkRigidBody*>(o, 4);
                hkVector4 v(at<float>(o, 8), at<float>(o, 12), at<float>(o, 16), 0.0f);
                body->activate();
                motionOf(body)->applyAngularImpulse(v);
                removeReferenceFrom(body);
                break;
            }
            case hkWorldOperation::ACTIVATE_REGION: {
                void* aabb = at<void*>(o, 4);
                m_world->activateRegion(static_cast<hkAabb*>(aabb));
                hkMemory::getInstance().deallocateChunk(aabb, 0x20, 4);
                break;
            }
            case hkWorldOperation::ACTIVATE_ENTITY: {
                hkEntity* entity = at<hkEntity*>(o, 4);
                entity->activate();
                removeReferenceFrom(entity);
                break;
            }
            case hkWorldOperation::DEACTIVATE_ENTITY: {
                hkEntity* entity = at<hkEntity*>(o, 4);
                if (entity->m_world == m_world) {
                    entity->deactivate();
                }
                removeReferenceFrom(entity);
                break;
            }
            case hkWorldOperation::ENTITY_BATCH_REINTEGRATE_AND_RECOLLIDE: {
                hkEntity** entities = at<hkEntity**>(o, 4);
                int count = at<u16>(o, 8);
                // HYPOTHESIS: world +8 is the simulation and its vtable 0x34 is reintegrateAndRecollideEntities
                typedef void (*ReintegrateFn)(void*, hkEntity**, int, hkWorld*, int);
                void* simulation = m_world->m_unk08;
                reinterpret_cast<ReintegrateFn>((*reinterpret_cast<void***>(simulation))[0x34 / 4])(simulation, entities, count, m_world, at<u8>(o, 10));
                for (hkEntity** e = entities; e < entities + count; e++) {
                    removeReferenceFrom(*e);
                }
                freeBatch(entities, count);
                break;
            }
            case hkWorldOperation::USER_CALLBACK: {
                void* callback = at<void*>(o, 4);
                // HYPOTHESIS: the callback interface has one function after the destructor and calcStatistics (vtable 0x10)
                vcall2<void, int, int>(callback, 0x10, at<int>(o, 8), 0);
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

