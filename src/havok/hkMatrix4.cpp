// hkMatrix4.o (main.dol 0x80283ACC-0x80283EB4)
// the regular 4x4 matrix. the havok 5.1 source we have covers all of this, though the dol's version
// only has these 5 functions out of line, the rest must've been inlined everywhere
//   0x80283ACC   100  transpose
//   0x80283B30   416  setMul
//   0x80283CD0   172  get4x4ColumnMajor
//   0x80283D7C   172  set4x4ColumnMajor
//   0x80283E28   140  set(hkTransform)
#include <havok/hkMatrix4.h>

static inline void swapf(hkReal& a, hkReal& b) {
    hkReal t = a;
    a = b;
    b = t;
}

static inline hkReal dot4(const hkVector4& a, const hkVector4& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// m(r, c) with the usual column-major layout: column c, lane r
static inline hkReal& at(hkMatrix4& m, int r, int c) { return (&(&m.m_col0)[c].x)[r]; }

void hkMatrix4::transpose() {
    hkMatrix4& m = *this;
    swapf(at(m, 0, 1), at(m, 1, 0));
    swapf(at(m, 0, 2), at(m, 2, 0));
    swapf(at(m, 0, 3), at(m, 3, 0));

    swapf(at(m, 1, 2), at(m, 2, 1));
    swapf(at(m, 1, 3), at(m, 3, 1));
    swapf(at(m, 2, 3), at(m, 3, 2));
}

// this = a * b. a gets transposed into a copy so each result is just a column dot a column
void hkMatrix4::setMul(const hkMatrix4& a, const hkMatrix4& b) {
    hkMatrix4 aTrans = a;
    aTrans.transpose();

    hkMatrix4& result = *this;

    for (int c = 0; c < 4; c++) {
        for (int r = 0; r < 4; r++) {
            at(result, r, c) = dot4((&aTrans.m_col0)[r], (&b.m_col0)[c]);
        }
    }
}

void hkMatrix4::get4x4ColumnMajor(hkReal* p) const {
    hkVector4* out = reinterpret_cast<hkVector4*>(p);
    out[0] = m_col0;
    out[1] = m_col1;
    out[2] = m_col2;
    out[3] = m_col3;
}

void hkMatrix4::set4x4ColumnMajor(const hkReal* p) {
    const hkVector4* in = reinterpret_cast<const hkVector4*>(p);
    m_col0 = in[0];
    m_col1 = in[1];
    m_col2 = in[2];
    m_col3 = in[3];
}

void hkMatrix4::set(const hkTransform& t) {
    m_col0 = t.m_rotation.m_col0;
    m_col1 = t.m_rotation.m_col1;
    m_col2 = t.m_rotation.m_col2;
    m_col0.w = 0.0f;
    m_col1.w = 0.0f;
    m_col2.w = 0.0f;

    m_col3 = t.m_translation;
    m_col3.w = 1.0f;
}
