// hkTransform.o (main.dol 0x802871F4-0x802878F0)
// rigid transform (3x3 rotation + translation) helpers
//   0x802871F4   128  get4x4ColumnMajor
//   0x80287274   128  set4x4ColumnMajor
//   0x802872F4   244  setInverse
//   0x802873E8   536  setMul
//   0x80287600   752  setMulInverseMul
#pragma fp_contract on
#include <havok/hkTransform.h>

// Column-major 4x4: m[col * 4 + row], with the w entries 0 and the translation column last.
void hkTransform::get4x4ColumnMajor(hkReal* m) const {
    m[0] = m_rotation.m_col0.x;
    m[1] = m_rotation.m_col0.y;
    m[2] = m_rotation.m_col0.z;
    m[3] = 0.0f;
    m[4] = m_rotation.m_col1.x;
    m[5] = m_rotation.m_col1.y;
    m[6] = m_rotation.m_col1.z;
    m[7] = 0.0f;
    m[8] = m_rotation.m_col2.x;
    m[9] = m_rotation.m_col2.y;
    m[10] = m_rotation.m_col2.z;
    m[11] = 0.0f;
    m[12] = m_translation.x;
    m[13] = m_translation.y;
    m[14] = m_translation.z;
    m[15] = 1.0f;
}

void hkTransform::set4x4ColumnMajor(const hkReal* m) {
    m_rotation.m_col0.x = m[0];
    m_rotation.m_col0.y = m[1];
    m_rotation.m_col0.z = m[2];
    m_rotation.m_col0.w = 0.0f;
    m_rotation.m_col1.x = m[4];
    m_rotation.m_col1.y = m[5];
    m_rotation.m_col1.z = m[6];
    m_rotation.m_col1.w = 0.0f;
    m_rotation.m_col2.x = m[8];
    m_rotation.m_col2.y = m[9];
    m_rotation.m_col2.z = m[10];
    m_rotation.m_col2.w = 0.0f;
    m_translation.x = m[12];
    m_translation.y = m[13];
    m_translation.z = m[14];
    m_translation.w = 1.0f;
}

// Inverse of a rigid transform: rotation transposed, translation -(R^T t).
// hkMatrix3 is declared in its own header, which the hkRotation.h here shadows, so only its transpose entry point is declared locally.
struct hkMatrix3 {
    void setTranspose(const hkMatrix3& other);
};

void hkTransform::setInverse(const hkTransform& t) {
    reinterpret_cast<hkMatrix3&>(m_rotation).setTranspose(reinterpret_cast<const hkMatrix3&>(t.m_rotation));

    // translation = this rotation applied to -t.translation (inlined rotated-direction product).
    hkVector4 negTranslation;
    negTranslation.set(-t.m_translation.x, -t.m_translation.y, -t.m_translation.z, -t.m_translation.w);
    const hkRotation& r = m_rotation;
    m_translation.x = negTranslation.z * r.m_col2.x + (negTranslation.x * r.m_col0.x + negTranslation.y * r.m_col1.x);
    m_translation.y = negTranslation.z * r.m_col2.y + (negTranslation.x * r.m_col0.y + negTranslation.y * r.m_col1.y);
    m_translation.z = negTranslation.z * r.m_col2.z + (negTranslation.x * r.m_col0.z + negTranslation.y * r.m_col1.z);
    m_translation.w = 0.0f;
}

// v rotated by r: col0 * x + col1 * y + col2 * z, w ends up 0
static inline void rotateDir(hkVector4& out, const hkRotation& r, const hkVector4& v) {
    out.x = r.m_col2.x * v.z + (r.m_col0.x * v.x + r.m_col1.x * v.y);
    out.y = r.m_col2.y * v.z + (r.m_col0.y * v.x + r.m_col1.y * v.y);
    out.z = r.m_col2.z * v.z + (r.m_col0.z * v.x + r.m_col1.z * v.y);
    out.w = 0.0f;
}

// v rotated by the inverse of r (so r transposed): each lane is v dotted with one column
static inline void rotateInverseDir(hkVector4& out, const hkRotation& r, const hkVector4& v) {
    out.x = r.m_col0.z * v.z + (r.m_col0.x * v.x + r.m_col0.y * v.y);
    out.y = r.m_col1.z * v.z + (r.m_col1.x * v.x + r.m_col1.y * v.y);
    out.z = r.m_col2.z * v.z + (r.m_col2.x * v.x + r.m_col2.y * v.y);
    out.w = 0.0f;
}

// this = a * b. the 3 rotation columns and the translation of b all get rotated by a, then a's translation is added on
void hkTransform::setMul(const hkTransform& a, const hkTransform& b) {
    const hkVector4* in = &b.m_rotation.m_col0;
    hkVector4* out = &m_rotation.m_col0;
    for (int i = 0; i < 4; i++) {
        rotateDir(out[i], a.m_rotation, in[i]);
    }
    m_translation.add4(a.m_translation);
}

// this = inverse(a) * b, without ever building the inverse
void hkTransform::setMulInverseMul(const hkTransform& a, const hkTransform& b) {
    const hkVector4* in = &b.m_rotation.m_col0;
    hkVector4* out = &m_rotation.m_col0;
    for (int i = 0; i < 3; i++) {
        rotateInverseDir(out[i], a.m_rotation, in[i]);
    }
    hkVector4 h;
    h.setSub4(b.m_translation, a.m_translation);
    rotateInverseDir(m_translation, a.m_rotation, h);
}
