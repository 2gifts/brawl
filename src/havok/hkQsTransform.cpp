// hkQsTransform.o (main.dol 0x80284DB0-0x8028564C)
// translation + rotation (quaternion) + scale. the 4x4 conversions are from havok 5.1. setMul isn't in the 5.1
// source we have (it's an inline there that got lost) so that one is standard qs-transform math checked against the asm
//   0x80284DB0   380  get4x4ColumnMajor
//   0x80284F2C   192  set4x4ColumnMajor
//   0x80284FEC   640  setInverse
//   0x8028526C   900  setMul
//   0x802855F0    92  setMulInverseMul
#include <havok/hkMathUtil.h>
#include <havok/hkQsTransform.h>
#include <havok/hkTransform.h>

// hkMatrix3 can't be included next to hkRotation.h (they define hkRotation differently), so just the bit we call
struct hkMatrix3 {
    void setMul(const hkMatrix3& lhs, const hkMatrix3& rhs);
};

// v rotated by quaternion q. sign = -1 does the inverse rotation instead (same thing as conjugating q)
static inline void rotateByQuat(hkVector4& out, const hkQuaternion& q, const hkVector4& v, hkReal sign) {
    hkReal ux = q.m_vec.x * sign;
    hkReal uy = q.m_vec.y * sign;
    hkReal uz = q.m_vec.z * sign;
    hkReal r = q.m_vec.w;

    // t = 2 * (u x v)
    hkReal tx = 2.0f * (uy * v.z - uz * v.y);
    hkReal ty = 2.0f * (uz * v.x - ux * v.z);
    hkReal tz = 2.0f * (ux * v.y - uy * v.x);

    // v + r * t + u x t
    out.x = v.x + r * tx + (uy * tz - uz * ty);
    out.y = v.y + r * ty + (uz * tx - ux * tz);
    out.z = v.z + r * tz + (ux * ty - uy * tx);
    out.w = 0.0f;
}

// Build the 4x4 as (rotation * scale, translation) via an hkTransform, then let it write the floats
void hkQsTransform::get4x4ColumnMajor(hkReal* m) const {
    hkRotation rotMatrix;
    rotMatrix.set(m_rotation);

    hkRotation scaMatrix;
    for (int i = 0; i < 12; i++) {
        (&scaMatrix.m_col0.x)[i] = 0.0f;
    }
    scaMatrix.m_col0.x = m_scale.x;
    scaMatrix.m_col1.y = m_scale.y;
    scaMatrix.m_col2.z = m_scale.z;

    hkRotation rotSca;
    reinterpret_cast<hkMatrix3&>(rotSca).setMul(reinterpret_cast<hkMatrix3&>(rotMatrix), reinterpret_cast<hkMatrix3&>(scaMatrix));

    hkTransform temp;
    temp.m_rotation = rotSca;
    temp.m_translation = m_translation;
    temp.get4x4ColumnMajor(m);
}

// the bool it returns is "no skew", so false means the matrix couldn't be represented exactly
hkBool hkQsTransform::set4x4ColumnMajor(const hkReal* p) {
    hkMathUtil::Decomposition decomposition;
    hkMathUtil::decompose4x4ColTransform(p, decomposition);

    m_translation = decomposition.m_translation;
    m_rotation = decomposition.m_rotation;
    m_scale = decomposition.m_scale;

    return !decomposition.m_hasSkew;
}

void hkQsTransform::setInverse(const hkQsTransform& t) {
    rotateByQuat(m_translation, t.m_rotation, t.m_translation, -1.0f);
    m_translation.mul4(-1.0f);

    m_rotation.m_vec.set(-t.m_rotation.m_vec.x, -t.m_rotation.m_vec.y, -t.m_rotation.m_vec.z, t.m_rotation.m_vec.w);

    // scale just flips to 1/scale, w gets cleared
    m_scale.set(1.0f / t.m_scale.x, 1.0f / t.m_scale.y, 1.0f / t.m_scale.z, 0.0f);
}

// this = a * b: b's translation gets scaled and rotated by a and then moved by a's translation,
// rotations multiply, scales multiply per axis
void hkQsTransform::setMul(const hkQsTransform& a, const hkQsTransform& b) {
    hkVector4 scaled;
    scaled.set(a.m_scale.x * b.m_translation.x, a.m_scale.y * b.m_translation.y, a.m_scale.z * b.m_translation.z, 0.0f);
    hkVector4 rotated;
    rotateByQuat(rotated, a.m_rotation, scaled, 1.0f);
    m_translation.set(a.m_translation.x + rotated.x, a.m_translation.y + rotated.y, a.m_translation.z + rotated.z,
                      a.m_translation.w + rotated.w);

    // quaternion product, a then b
    const hkVector4& qa = a.m_rotation.m_vec;
    const hkVector4& qb = b.m_rotation.m_vec;
    m_rotation.m_vec.set(qa.w * qb.x + qb.w * qa.x + (qa.y * qb.z - qa.z * qb.y),
                         qa.w * qb.y + qb.w * qa.y + (qa.z * qb.x - qa.x * qb.z),
                         qa.w * qb.z + qb.w * qa.z + (qa.x * qb.y - qa.y * qb.x),
                         qa.w * qb.w - (qa.x * qb.x + qa.y * qb.y + qa.z * qb.z));

    m_scale.set(a.m_scale.x * b.m_scale.x, a.m_scale.y * b.m_scale.y, a.m_scale.z * b.m_scale.z, 0.0f);
}

// this = inverse(a) * b
// HYPOTHESIS: the original calls setInverse/setMul here instead of inlining them. i couldn't get that without
// wrecking the other two (dont_inline turns off the quat helper inlining too), so this one is left as is
void hkQsTransform::setMulInverseMul(const hkQsTransform& a, const hkQsTransform& b) {
    hkQsTransform inv;
    inv.setInverse(a);
    setMul(inv, b);
}
