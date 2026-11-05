// Havok translation unit hkRotation.o (main.dol 0x80285A6C-0x80285CCC).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x80285A6C   196  set   [map: hkRotation__set]
//   0x80285B30   412  isOrthonormal   [map: hkRotation__isOrthonormal]
#include <havok/hkRotation.h>
#include <math.h>

void hkRotation::set(const hkQuaternion& q) {
    hkReal x = q.m_vec.x;
    hkReal y = q.m_vec.y;
    hkReal z = q.m_vec.z;
    hkReal w = q.m_vec.w;
    hkReal x2 = x + x;
    hkReal y2 = y + y;
    hkReal z2 = z + z;
    hkReal yy = y2 * y;
    hkReal zz = z2 * z;
    hkReal xx = x2 * x;
    hkReal xy = y2 * x;
    hkReal zw = z2 * w;
    hkReal xz = z2 * x;
    hkReal yw = y2 * w;
    hkReal yz = z2 * y;
    hkReal xw = x2 * w;

    m_col0.x = 1.0f - (yy + zz);
    m_col0.y = xy + zw;
    m_col0.z = xz - yw;
    m_col0.w = 0.0f;
    m_col1.x = xy - zw;
    m_col1.y = 1.0f - (xx + zz);
    m_col1.z = yz + xw;
    m_col1.w = 0.0f;
    m_col2.x = xz + yw;
    m_col2.y = yz - xw;
    m_col2.z = 1.0f - (xx + yy);
    m_col2.w = 0.0f;
}

hkBool hkRotation::isOrthonormal(hkReal epsilon) const {
    if ((hkReal)fabs(m_col0.lengthSquared3() - 1.0f) > epsilon) {
        return false;
    }
    if ((hkReal)fabs(m_col1.lengthSquared3() - 1.0f) > epsilon) {
        return false;
    }
    if ((hkReal)fabs(m_col2.lengthSquared3() - 1.0f) > epsilon) {
        return false;
    }
    hkVector4 cross;
    cross.setCross(m_col0, m_col1);
    hkVector4 eps;
    eps.setAll(epsilon);
    hkVector4 diff;
    diff.setSub4(cross, m_col2);
    diff.setAbs4(diff);
    return (eps.compareLessThan4(diff) & 0xE) == 0;
}
