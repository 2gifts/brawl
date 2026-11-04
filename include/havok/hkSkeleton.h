#pragma once

#include <havok/hkQsTransform.h>

struct hkBone;

// Skeleton (0x1C bytes). Layout and names from main.dol's hkSkeletonClass: three simple arrays (data pointer,
// count). The hkPose TU reads the bone count and the reference pose.
struct hkSkeleton {
    const char* m_name;             // 0x00
    s16* m_parentIndices;           // 0x04
    int m_numParentIndices;         // 0x08
    hkBone** m_bones;               // 0x0C
    int m_numBones;                 // 0x10
    hkQsTransform* m_referencePose; // 0x14
    int m_numReferencePose;         // 0x18
};
