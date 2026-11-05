#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>
#include <havok/hkCollidable.h>
#include <havok/hkMemory.h>

struct hkAabb;

// Pair of typed broad-phase handles reported by the broad phase (8 bytes).
// HYPOTHESIS: same layout as the pair used by hkPhantom.cpp (which keeps its own copy).
struct hkBroadPhaseHandlePair {
    hkTypedBroadPhaseHandle* m_a; // 0x00
    hkTypedBroadPhaseHandle* m_b; // 0x04
};

// Broad-phase base class. Vtable slots from hk3AxisSweep's vtable (names from the map): 0x08 dtor,
// 0x0C calcStatistics, 0x10 addObject, 0x14 addObjectBatch, 0x18 removeObject, 0x1C removeObjectBatch,
// 0x20 getNumObjects; the later slots are not needed yet.
// HYPOTHESIS: the parameter types (the hk3AxisSweep overrides are not written yet; the callers pass the
// handle, the aabb and the pair list in this order).
struct hkBroadPhase : hkReferencedObject {
    HK_DECLARE_REF_ALLOCATOR(0x1f)

    virtual void addObject(hkBroadPhaseHandle* object, const hkAabb& aabb,
                           hkArray<hkBroadPhaseHandlePair>& pairsOut);                    // 0x10
    virtual void addObjectBatch();                                                         // 0x14 (signature unknown)
    virtual void removeObject(hkBroadPhaseHandle* object, hkArray<hkBroadPhaseHandlePair>& pairsOut); // 0x18
};
