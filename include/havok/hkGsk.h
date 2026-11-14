#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector4.h>

// Closest-distance result of hkGsk::convertFeatureToClosestDistance. HYPOTHESIS: names from what is written
// (the plane as normal and distance, a point on the closest feature and a weight 1, 0 or 0.5).
struct hkGskClosestDistance {
    hkVector4 m_normal;   // 0x00
    hkVector4 m_position; // 0x10
    hkReal m_distance;    // 0x20
    hkReal m_weight;      // 0x24
};

// GJK / separating-axis solver state (map name hkGsk). Only the members touched by the functions recovered so far are
// named; the object size and the rest of the layout are not known yet.
#include <havok/hkGskCache.h>
struct hkConvexShape;

struct hkGsk {
    int m_dimA;    // 0x00 HYPOTHESIS: number of simplex vertices on body A (1: the closest feature is a vertex of A)
    int m_dimB;    // 0x04 HYPOTHESIS: the same for body B
    int unk8;      // 0x08 copied from the cache's unkA
    int unkC;      // 0x0C copied from the cache's unkB
    u8 unk10; // 0x10 cleared by the constructor
    int unk14; // 0x14 HYPOTHESIS: feature-change flag, written by setFeatureChange

    hkGsk() __attribute__((never_inline));

    // Takes the simplex of the cache and fetches its vertices from the two shapes (A's at 0x20, B's at 0xE0).
    void init(const hkConvexShape* shapeA, const hkConvexShape* shapeB, const hkGskCache& cache);
    hkVector4* verticesA() { return (hkVector4*)((char*)this + 0x20); }
    hkVector4* verticesB() { return (hkVector4*)((char*)this + 0xE0); }

    void setFeatureChange(int value);
    // Turns the separating plane of the closest feature into a closest-distance result (see the .cpp).
    void convertFeatureToClosestDistance(const hkVector4& plane, hkGskClosestDistance& out) const;
    // Vector stored at a byte offset of the solver state (the vertex arrays are not laid out yet).
    const hkVector4& vectorAt(int offset) const { return *(const hkVector4*)((const char*)this + offset); }
    // HYPOTHESIS: exports the cache when the feature-change flag is set (tail call to exitAndExportCacheImpl).
    void checkForChangesAndUpdateCache(void* arg);
    // HYPOTHESIS: argument not identified; passed through unchanged by checkForChangesAndUpdateCache.
    void exitAndExportCacheImpl(void* arg);
};
