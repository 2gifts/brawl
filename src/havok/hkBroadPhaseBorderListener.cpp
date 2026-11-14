// Havok translation unit hkBroadPhaseBorderListener.o (main.dol 0x802F3290-0x802F3420).
// The broad-phase listener hkWorld creates for its border phantoms (vtable lbl_80488690); its deleting destructor sits in hkWorld.o.

#include <types.h>
#include <havok/hkWorld.h>
#include <havok/hkLinkedCollidable.h>
#include <havok/hkBroadPhase.h>
#include <havok/hkPhantom.h>

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
