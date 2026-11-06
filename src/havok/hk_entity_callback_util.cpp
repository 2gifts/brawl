// Havok translation unit hk_entity_callback_util.o (main.dol 0x802E4044-0x802E4338).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802E4044   252  fireContactPointConfirmedInternal   [map: hkEntityCallbackUtil__fireContactPointConfirmedInternal]
//   0x802E4140   252  fireContactPointRemovedInternal   [map: hkEntityCallbackUtil__fireContactPointRemovedInternal]
//   0x802E423C   252  fireContactProcessInternal   [map: hkEntityCallbackUtil__fireContactProcessInternal]

#include <havok/hkEntity.h>
#include <havok/hkBase.h>
#include <havok/hkEntityCallbackUtil.h>

// HYPOTHESIS: collision listener interface as used by the entity callbacks (vtable slots 3, 4, 5).
struct hkCollisionListener : hkBaseObject {
    virtual void contactPointConfirmed(struct hkCollisionEventConfirmed* ev); // 0x0C
    virtual void contactPointRemoved(struct hkCollisionEventRemoved* ev);     // 0x10
    virtual void contactProcess(struct hkCollisionEventConfirmed* ev);        // 0x14
};

// HYPOTHESIS: event records; the entity pointer sits at 0x08 (confirmed/process) or 0x10 (removed).
struct hkCollisionEventConfirmed {
    u8 unk00[0x08];
    hkEntity* m_entity; // 0x08
};

struct hkCollisionEventRemoved {
    u8 unk00[0x10];
    hkEntity* m_entity; // 0x10
};


// compacts a listener list by dropping its null entries (the same shape as hkWorldCallbackUtil's)
template <typename T>
static inline void removeNullListeners(hkArray<T*>& list) {
    for (int i = list.m_size - 1; i >= 0; i--) {
        if (list.begin()[i] == 0) {
            list.removeAtAndCopy(i);
        }
    }
}

void hkEntityCallbackUtil::fireContactPointConfirmedInternal(hkEntity* entity, hkCollisionEventConfirmed* ev) {
    ev->m_entity = entity;
    int n = entity->m_collisionListeners.getSize();
    for (int i = n - 1; i >= 0; i--) {
        hkCollisionListener* listener = entity->m_collisionListeners[i];
        if (listener) {
            listener->contactPointConfirmed(ev);
        }
    }
    // Listeners removed during the callbacks left null slots: drop them now, keeping the order.
    removeNullListeners(entity->m_collisionListeners);
}

void hkEntityCallbackUtil::fireContactPointRemovedInternal(hkEntity* entity, hkCollisionEventRemoved* ev) {
    ev->m_entity = entity;
    int n = entity->m_collisionListeners.getSize();
    for (int i = n - 1; i >= 0; i--) {
        hkCollisionListener* listener = entity->m_collisionListeners[i];
        if (listener) {
            listener->contactPointRemoved(ev);
        }
    }
    // Listeners removed during the callbacks left null slots: drop them now, keeping the order.
    removeNullListeners(entity->m_collisionListeners);
}

void hkEntityCallbackUtil::fireContactProcessInternal(hkEntity* entity, hkCollisionEventConfirmed* ev) {
    ev->m_entity = entity;
    int n = entity->m_collisionListeners.getSize();
    for (int i = n - 1; i >= 0; i--) {
        hkCollisionListener* listener = entity->m_collisionListeners[i];
        if (listener) {
            listener->contactProcess(ev);
        }
    }
    // Listeners removed during the callbacks left null slots: drop them now, keeping the order.
    removeNullListeners(entity->m_collisionListeners);
}
