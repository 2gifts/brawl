#pragma once

#include <havok/hkShapeRayCastInput.h>

struct hkCdBody;

// Receiver of the ray hits of hkShape::castRayWithCollector. Vtable: 0x08 addRayHit, 0x0C dtor (the order of the
// height field forwarding collector's vtable). No allocator: its deleting dtor calls the global delete.
struct hkShapeRayCastCollector {
    virtual void addRayHit(const hkCdBody& body, const hkShapeRayCastOutput& hit) = 0;
    virtual ~hkShapeRayCastCollector() {}

    hkReal m_earlyOutHitFraction; // 0x04
};
