#pragma once

#include <havok/hkBase.h>

struct hkEntity;
struct hkCollisionEventConfirmed;
struct hkCollisionEventRemoved;

// Calls the listeners registered on an entity (map names hkEntityCallbackUtil__*).
struct hkEntityCallbackUtil {
    // hkEntity's destructor calls it first (fn_802E3ECC). HYPOTHESIS: the parameter type.
    static void fireEntityDeleted(hkEntity* entity);
    static void fireContactPointConfirmedInternal(hkEntity* entity, hkCollisionEventConfirmed* ev);
    static void fireContactPointRemovedInternal(hkEntity* entity, hkCollisionEventRemoved* ev);
    static void fireContactProcessInternal(hkEntity* entity, hkCollisionEventConfirmed* ev);
};
