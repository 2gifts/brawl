#pragma once

#include <havok/hkMatrix3.h>
#include <havok/hkQuaternion.h>
#include <havok/hkVector4.h>

struct hkMathUtil {
    // result of splitting a 4x4 matrix M into translation * rotation * scale (* skew). 0xD0 bytes
    struct Decomposition {
        hkVector4 m_translation;            // 0x00
        hkQuaternion m_rotation;            // 0x10
        hkVector4 m_scale;                  // 0x20
        hkBool m_hasScale;                  // 0x30 scale isn't (1,1,1)
        hkBool m_hasSkew;                   // 0x31 translation * rotation * scale doesn't give back M
        hkBool m_flips;                     // 0x32 determinant is negative (a mirror)
        hkMatrix3 m_scaleAndSkew;           // 0x40
        hkMatrix3 m_skew;                   // 0x70
        hkMatrix3 m_basis;                  // 0xA0 rotation matrix, may have a flipped axis
    };

    // matrixIn is 16 floats, column major
    static void decompose4x4ColTransform(const hkReal* matrixIn, Decomposition& decomposition);
};
