// hkTransform.o (main.dol 0x802871F4-0x802878F0)
// rigid transform (3x3 rotation + translation) helpers
//   0x802871F4   128  get4x4ColumnMajor
//   0x80287274   128  set4x4ColumnMajor
//   0x802872F4   244  setInverse
//   0x802873E8   536  setMul
//   0x80287600   752  setMulInverseMul
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
    m[15] = 0.0f; // BUG: the original stores a zero to m[15] before the final value (without it the function does not match)
    m[15] = 1.0f;
}

void hkTransform::set4x4ColumnMajor(const hkReal* m) {
    m_rotation.m_col0.x = m[0];
    m_rotation.m_col0.y = m[1];
    m_rotation.m_col0.z = m[2];
    // BUG: the original clears translation.w before the rotation rows; the final 1.0f store overwrites it (without it the function does not match).
    m_translation.w = 0.0f;
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
void hkTransform::setInverse(const hkTransform& t) {
    m_rotation.setTranspose(t.m_rotation);
    hkVector4 negTranslation;
    negTranslation.setNeg4(t.m_translation);
    m_translation._setRotatedDir(m_rotation, negTranslation);
}

// this = a * b. the 3 rotation columns and the translation of b all get rotated by a, then a's translation is added on
void hkTransform::setMul(const hkTransform& a, const hkTransform& b) {
    hkRotation r = a.m_rotation;
    for (int i = 0; i < 4; i++) {
        hkReal* out = &m_rotation.m_col0.x + i * 4;
        const hkReal* in = &b.m_rotation.m_col0.x + i * 4;
        hkReal x = in[0];
        hkReal y = in[1];
        hkReal z = in[2];
        out[0] = x * r.m_col0.x + y * r.m_col1.x + z * r.m_col2.x;
        out[1] = x * r.m_col0.y + y * r.m_col1.y + z * r.m_col2.y;
        out[2] = x * r.m_col0.z + y * r.m_col1.z + z * r.m_col2.z;
        out[3] = 0.0f;
    }
    m_translation.add4(a.m_translation);
}

// this = inverse(a) * b, without ever building the inverse
void hkTransform::setMulInverseMul(const hkTransform& a, const hkTransform& b) {
    // rotation = a.rotation^T * b.rotation: a's columns copied first, then each column of b dotted with them
    {
        hkVector4 columns[3];
        columns[0] = a.m_rotation.m_col0;
        columns[1] = a.m_rotation.m_col1;
        columns[2] = a.m_rotation.m_col2;

        const hkReal* in = &b.m_rotation.m_col0.x;
        hkReal* out = &m_rotation.m_col0.x;
        for (int i = 0; i < 3; i++) {
            const hkReal v0 = in[0];
            const hkReal v1 = in[1];
            const hkReal v2 = in[2];
            out[0] = v0 * columns[0].x + v1 * columns[0].y + v2 * columns[0].z;
            out[1] = v0 * columns[1].x + v1 * columns[1].y + v2 * columns[1].z;
            out[2] = v0 * columns[2].x + v1 * columns[2].y + v2 * columns[2].z;
            out[3] = 0.0f;
            in += 4;
            out += 4;
        }
    }
    hkVector4 h;
    h.setSub4(b.m_translation, a.m_translation);
    m_translation._setRotatedInverseDir(a.m_rotation, h);
}
