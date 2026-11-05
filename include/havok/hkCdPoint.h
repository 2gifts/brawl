#pragma once

#include <havok/hkContactPoint.h>

#include <havok/hkCdBody.h>

// Largest distance a collector starts with (0x7F7FFFEE, a little below FLT_MAX).
#define HK_REAL_MAX 3.40282e38f

// Collision point handed to the collector callbacks: the contact (position, normal, distance in the normal's w)
// and the two bodies of the pair (the collectors walk their m_parent chains to the root).
struct hkCdPoint {
    hkContactPoint m_contact;  // 0x00..0x1F
    const hkCdBody* m_cdBodyA; // 0x20
    const hkCdBody* m_cdBodyB; // 0x24
};

// Receiver of collision points (closest point and linear cast queries). Vtable: 0x08 dtor, 0x0C addCdPoint.
struct hkCdPointCollector {
    hkCdPointCollector() : m_earlyOutDistance(HK_REAL_MAX) {}
    virtual ~hkCdPointCollector() {}
    virtual void addCdPoint(hkCdPoint* point) = 0; // HYPOTHESIS: name from the map

    hkReal m_earlyOutDistance; // 0x04 (the closest-point collectors keep their best distance here)
};
