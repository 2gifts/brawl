#pragma once

#include <havok/hkMatrix3.h>
#include <havok/hkQuaternion.h>

// 3x3 rotation matrix: an hkMatrix3 (three columns) with no fields of its own.
struct hkRotation : hkMatrix3 {
    void set(const hkQuaternion& q);
    // tolerance is passed in f1 (HYPOTHESIS: a caller-supplied epsilon).
    bool isOrthonormal(hkReal tolerance) const;
};

#ifndef HK_VECTOR4_OUTLINE_SETMUL3
inline void hkVector4::_setMul3(const hkRotation& m, const hkVector4& v) {
    hkReal vx = v.x;
    hkReal vy = v.y;
    hkReal vz = v.z;
    x = vx * m.m_col0.x + vy * m.m_col1.x + vz * m.m_col2.x;
    y = vx * m.m_col0.y + vy * m.m_col1.y + vz * m.m_col2.y;
    z = vx * m.m_col0.z + vy * m.m_col1.z + vz * m.m_col2.z;
    w = 0.0f;
}
#endif
