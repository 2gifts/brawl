#pragma once

#include <havok/hkMatrix3.h>
#include <havok/hkQuaternion.h>

// 3x3 rotation matrix: an hkMatrix3 (three columns) with no fields of its own.
struct hkRotation : hkMatrix3 {
    void set(const hkQuaternion& q);
    // tolerance is passed in f1 (HYPOTHESIS: a caller-supplied epsilon).
    bool isOrthonormal(hkReal tolerance) const;
};

inline void hkVector4::_setRotatedDir(const hkRotation& r, const hkVector4& v) {
    hkReal vz = v.z;
    hkReal vy = v.y;
    hkReal vx = v.x;
    x = vx * r.m_col0.x + vy * r.m_col1.x + vz * r.m_col2.x;
    y = vx * r.m_col0.y + vy * r.m_col1.y + vz * r.m_col2.y;
    z = vx * r.m_col0.z + vy * r.m_col1.z + vz * r.m_col2.z;
    w = 0.0f;
}

inline void hkVector4::_setRotatedInverseDir(const hkRotation& r, const hkVector4& v) {
    hkReal vx = v.x;
    hkReal vy = v.y;
    hkReal vz = v.z;
    x = vx * r.m_col0.x + vy * r.m_col0.y + vz * r.m_col0.z;
    y = vx * r.m_col1.x + vy * r.m_col1.y + vz * r.m_col1.z;
    z = vx * r.m_col2.x + vy * r.m_col2.y + vz * r.m_col2.z;
    w = 0.0f;
}

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
