#pragma once

#include <havok/hkQuaternion.h>
#include <havok/hkVector4.h>

// hkMatrix3 and hkRotation are defined twice in this codebase (hkMatrix3.h vs hkRotation.h) and can't be
// included together, so the matrices inside Decomposition are plain storage. hkMathUtil.cpp views them as hkMatrix3
struct hkMatrix3Storage {
    float v[12];
} __attribute__((aligned(16)));

struct hkMathUtil {
    // result of splitting a 4x4 matrix M into translation * rotation * scale (* skew). 0xD0 bytes
    struct Decomposition {
        hkVector4 m_translation;            // 0x00
        hkQuaternion m_rotation;            // 0x10
        hkVector4 m_scale;                  // 0x20
        hkBool m_hasScale;                  // 0x30 scale isn't (1,1,1)
        hkBool m_hasSkew;                   // 0x31 translation * rotation * scale doesn't give back M
        hkBool m_flips;                     // 0x32 determinant is negative (a mirror)
        hkMatrix3Storage m_scaleAndSkew;    // 0x40
        hkMatrix3Storage m_skew;            // 0x70
        hkMatrix3Storage m_basis;           // 0xA0 rotation matrix, may have a flipped axis
    };

    // matrixIn is 16 floats, column major
    static void decompose4x4ColTransform(const hkReal* matrixIn, Decomposition& decomposition);
};
