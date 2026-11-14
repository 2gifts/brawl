#pragma once

#include <havok/hkConvexShape.h>
#include <havok/hkMemory.h>
#include <havok/hkWorldCinfo.h>

// Capsule shape (0x30 bytes): segment endpoints vertexA (0x10) and vertexB (0x20), radius from the convex base.
// Layout from hkCapsuleShapeClass.cpp. Overrides most of the hkShape/hkConvexShape virtuals.
struct hkCapsuleShape : hkConvexShape {
    enum RayHitType {
        HIT_CAP0 = 0,
        HIT_CAP1 = 1,
        HIT_BODY = 2,
    };

    hkVector4 m_vertexA; // 0x10
    hkVector4 m_vertexB; // 0x20

    hkCapsuleShape(hkFinishLoadedObjectFlag flag) {} // finish-loading ctor: only the vtable is written
    // The cylinder agent passes the cylinder's axis ends and the radius.
    hkCapsuleShape(const hkVector4& vertexA, const hkVector4& vertexB, hkReal radius);
    virtual ~hkCapsuleShape();

    static void finishLoadedObjecthkCapsuleShape(void* p);
    static void cleanupLoadedObjecthkCapsuleShape(void* p);
    static const void* getVtablehkCapsuleShape();

    virtual void calcStatistics(hkStatisticsCollector* collector) const;     // 0x0C
    virtual int getType() const;                                             // 0x10
    virtual void getAabb(const hkTransform& xf, hkReal expansion, hkAabb& out) const; // 0x14
    virtual hkBool castRay(const hkShapeRayCastInput& input, hkShapeRayCastOutput& results) const; // 0x1C
    virtual void getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const; // 0x28
    virtual hkVector4* getCollisionSpheres(hkVector4* out) const;            // 0x2C
    virtual void getSupportingVertex(const hkVector4& direction, hkVector4& out) const; // 0x30
    virtual void convertVertexIdsToVertices(const u16* ids, int numIds, hkVector4* out) const; // 0x34
    virtual void getFirstVertex(hkVector4& out) const;                       // 0x38
    virtual int getNumVertices() const;                                      // 0x3C

    // Closest point of the segment a..b to p (never inlined: castRay calls it out of line).
    static void closestPointLineSeg(const hkVector4& p, const hkVector4& a, const hkVector4& b, hkVector4& out)
        __attribute__((never_inline));
    // Closest points of the lines a + s*dA and b + t*dB: the squared distance (in: the largest accepted), the two
    // parameters and the two points. HYPOTHESIS: the parameter meanings beyond the call shape.
    static void closestInfLineSegInfLineSeg(const hkVector4& a, const hkVector4& dA, const hkVector4& b,
                                            const hkVector4& dB, hkReal& distSquared, hkReal& tA, hkReal& tB,
                                            hkVector4& pointA, hkVector4& pointB);
};
