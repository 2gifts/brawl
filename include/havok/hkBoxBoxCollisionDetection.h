#pragma once

#include <havok/hkBoxBoxManifold.h>
#include <havok/hkTransform.h>
#include <havok/hkCollisionAgent.h>

struct hkProcessCollisionOutput;
struct hkContactPoint;

// Box-box collision detector (hkBoxBoxCollisionDetection.o). A stack object built by the box agents.
// Layout from the unit's functions (asm); HYPOTHESIS: the member names (Havok style).
struct hkBoxBoxCollisionDetection {
    // Closest-feature result of findClosestPoint (0x40 bytes, 16-byte aligned: the callers keep it on an aligned
    // stack). Its constructor is empty (out-of-line copy at 0x80303700).
    struct hkFeaturePointCache {
        hkVector4 unk0;        // 0x00 not identified
        hkVector4 unk10;       // 0x10 not identified
        hkVector4 m_normal;    // 0x20 separating normal (edge-edge case)
        hkReal m_normalScale;  // 0x30 the face normal is the face axis scaled by -m_normalScale
        hkReal m_distance;     // 0x34 contact distance (the w of the contact normal)
        int m_featureIndex;    // 0x38 0..2 face of A, 3..6 face of B, above: edge-edge (then A's edge feature id)
        int m_featureIndexB;   // 0x3C B's edge feature id (edge-edge)

        hkFeaturePointCache() {}
    };

    u8 unk0[0x0C];                       // 0x00 not identified
    hkContactMgr* m_contactMgr;          // 0x0C receives the removed contact point ids
    hkProcessCollisionOutput* m_result;  // 0x10 output that receives the manifold's contact points
    const hkTransform* m_wTa;            // 0x14 world transform of box A
    const hkTransform* m_wTb;            // 0x18 world transform of box B
    u8 unk1C[0x20 - 0x1C];               // 0x1C not identified
    hkTransform m_aTb;                   // 0x20 B in the frame of A
    hkVector4 m_radiusA;                 // 0x60 half extents of A
    hkVector4 m_radiusB;                 // 0x70 half extents of B
    hkVector4 m_tolerance4;              // 0x80 tolerance in all lanes
    u8 unk90[0xB0 - 0x90];               // 0x90 not identified
    hkReal m_boundaryTolerance;          // 0xB0 limit of the edge-edge separations (only x is read)
    hkReal m_minEdgeNormalLength;        // 0xB4 isValidEdgeEdge rejects edge pairs whose cross product is shorter
    hkVector4 m_dinA;                    // 0xC0 translation of B in A (copy of m_aTb's translation)
    hkVector4 m_dinB;                    // 0xD0 translation of A in B, rotated (initWorkVariables)
    hkVector4 m_sepDist[5];              // 0xE0 separations of the face axes of A, B and the edge axes

    void initWorkVariables();
    void calcManifold(hkBoxBoxManifold& manifold);
    void refreshManifold(hkBoxBoxManifold& manifold);
    hkBool queryManifoldNormalConsistency(hkBoxBoxManifold& manifold);
    void calcManifoldNormal(hkVector4& normal, const hkFeatureContactPoint& fcp, hkFeaturePointCache& cache,
                            const hkBool& isEdgeEdge);
    void checkManifoldNormalConsistency(hkBoxBoxManifold& manifold);
    // Separating axis test over the 15 axes; nonzero when the boxes are separated by more than tolerance.
    int checkIntersection(const hkVector4& tolerance);
    void setvdProj(const hkRotation& bRa, int i, int j, hkVector4& out);
    int findClosestPoint(hkBoxBoxManifold& manifold, hkFeatureContactPoint& fcp, hkFeaturePointCache& cache);
    int addPoint(hkBoxBoxManifold& manifold, hkFeaturePointCache& cache, hkFeatureContactPoint& fcp);
    void findAdditionalManifoldPoints(hkBoxBoxManifold& manifold, hkFeatureContactPoint fcp);
    void tryToAddPointFaceA(hkBoxBoxManifold& manifold, hkFeatureContactPoint fcp, u16 planeMask, hkReal minDist);
    void tryToAddPointFaceB(hkBoxBoxManifold& manifold, hkFeatureContactPoint fcp, u16 planeMask, hkReal minDist);
    void tryToAddPointOnEdge(hkBoxBoxManifold& manifold, int edgeA, int edgeB, int nextA, int nextB,
                             const hkVector4& normalA, const hkVector4& normalB, hkReal minDist);
    void addAdditionalEdgeHelper(hkBoxBoxManifold& manifold, hkFeatureContactPoint& fcp, hkReal minDist);
    hkBool calculateClosestPoint(hkContactPoint& contact);
    hkBool isValidEdgeEdge(hkFeaturePointCache& cache);
    void checkCompleteness(hkBoxBoxManifold& manifold, int planeMaskA, int planeMaskB);
};
