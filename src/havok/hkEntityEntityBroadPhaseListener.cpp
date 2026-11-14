// Havok translation unit hkEntityEntityBroadPhaseListener.o (main.dol 0x802F3420-0x802F34F0).
// The broad-phase listener hkWorld creates for entity pairs (vtable lbl_804886A8); its deleting destructor sits in hkWorld.o.

#include <types.h>
#include <havok/hkWorld.h>
#include <havok/hkEntity.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkLinkedCollidable.h>
#include <havok/hkBroadPhase.h>
#include <havok/hkAgentNnMachine.h>
#include <havok/hkWorldAgentUtil.h>

// Collidable owning a typed broad-phase handle (the handle keeps a signed byte offset back to it).
static inline hkLinkedCollidable* collidableOfHandle(hkTypedBroadPhaseHandle* handle) {
    return (hkLinkedCollidable*)((char*)handle + handle->m_ownerOffset);
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

