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
    // BYTES: mutable (some member of hkGsk is: its const members keep their loads after stores to the outputs).
    // HYPOTHESIS: this one.
    mutable int unk14; // 0x14 HYPOTHESIS: feature-change flag, written by setFeatureChange
    // The simplex vertices fetched from the shapes and two more vector arrays (hkGskBaseAgent's getPenetrations
    // constructs four arrays of four vectors from 0x20 to 0x120). HYPOTHESIS: the names, and the 0x60 bytes after
    // them (the object is 0x180 bytes in staticGetPenetrations' frame).
    hkVector4 m_verticesA[4]; // 0x20
    hkVector4 unk60[4];       // 0x60
    hkVector4 unkA0[4];       // 0xA0
    hkVector4 m_verticesB[4]; // 0xE0
    u8 unk120[0x60];          // 0x120

    // BYTES: inline (hkGskBaseAgent::staticGetPenetrations inlines it). The out-of-line copy in hkContinuousGsk.o
    // comes from a call there that is not inlined (hk4dGskCollideCalcToi, not written yet), so until that caller
    // exists hkContinuousGsk.cpp defines the constructor out of line itself.
    // TODO: clean up: drop HK_GSK_OUTLINE_CTOR once hk4dGskCollideCalcToi is written.
#ifdef HK_GSK_OUTLINE_CTOR
    hkGsk() __attribute__((never_inline));
#else
    hkGsk() : unk10(0) {}
#endif

    // Takes the simplex of the cache and fetches its vertices from the two shapes (A's at 0x20, B's at 0xE0).
    void init(const hkConvexShape* shapeA, const hkConvexShape* shapeB, const hkGskCache& cache);
    hkVector4* verticesA() { return m_verticesA; }
    hkVector4* verticesB() { return m_verticesB; }

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
