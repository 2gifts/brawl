#pragma once

#include <havok/hkShape.h>
#include <havok/hkShapeContainer.h>
#include <havok/hkCdBody.h>

// Bounding volume shape (0x18 bytes). Layout from the reflection-free asm: the bounding volume shape pointer
// (reference counted) at 0x0C and the single child container (vtable at 0x10, child shape at 0x14).
struct hkBvShape : hkShape {
    hkShape* m_boundingVolumeShape; // 0x0C
    hkSingleShapeContainer m_childContainer; // 0x10

    hkBvShape(hkFinishLoadedObjectFlag flag) : m_childContainer(flag) {} // finish-loading ctor
    virtual ~hkBvShape();

    static void finishLoadedObjecthkBvShape(void* p);
    static void cleanupLoadedObjecthkBvShape(void* p);
    static const void* getVtablehkBvShape();

    virtual void calcStatistics(hkStatisticsCollector* collector) const;     // 0x0C
    virtual int getType() const;                                             // 0x10
    virtual void getAabb(const hkTransform& xf, hkReal expansion, hkAabb& out) const; // 0x14
    // Rays go straight to the child shape (key 0); the bounding volume only matters to the agents.
    virtual hkBool castRay(const hkShapeRayCastInput& input, hkShapeRayCastOutput& results) const; // 0x1C
    virtual void castRayWithCollector(const hkShapeRayCastInput& input, const hkCdBody& body,
                                      hkShapeRayCastCollector& collector) const; // 0x20
    virtual hkShapeContainer* getContainer() const;                          // 0x24
};
