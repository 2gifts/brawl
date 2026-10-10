// hkMatrix6.o (main.dol 0x80283EB4-0x80284DB0)
// 6x6 matrix helpers, which is just a 2x2 grid of hkMatrix3 blocks. the solver uses these.
// the havok 5.1 source we have lines up w/ this really well (same function names even) but this
// dol is an older build so i'm checking everything against the asm instead of trusting it
//   0x80283EB4   908  hkMatrix6Sub
//   0x80284240   360  hkMatrix6SetMul
//   0x802843A8   620  hkMatrix6SetMulV
//   0x80284614    92  hkMatrix6SetTranspose
//   0x80284670  1856  hkMatrix6SetInvert
#include <havok/hkMatrix6.h>

// each hkMatrix3 is 3 columns of 4 floats, so treat a block as 3 hkVector4s
static inline hkVector4* cols(hkMatrix3& m) { return reinterpret_cast<hkVector4*>(m.elements); }
static inline const hkVector4* cols(const hkMatrix3& m) { return reinterpret_cast<const hkVector4*>(m.elements); }

// a -= b, all 12 floats (yes the w lanes too, the asm does them)
static inline void subBlock(hkMatrix3& a, const hkMatrix3& b) {
    cols(a)[0].setSub4(cols(a)[0], cols(b)[0]);
    cols(a)[1].setSub4(cols(a)[1], cols(b)[1]);
    cols(a)[2].setSub4(cols(a)[2], cols(b)[2]);
}

// a += b, same deal
static inline void addBlock(hkMatrix3& a, const hkMatrix3& b) {
    cols(a)[0].add4(cols(b)[0]);
    cols(a)[1].add4(cols(b)[1]);
    cols(a)[2].add4(cols(b)[2]);
}

// out = m * v on xyz, w gets zeroed (the asm stores a 0.0f constant there)
static inline void setMul3(hkVector4& out, const hkMatrix3& m, const hkVector4& v) {
    const float* e = m.elements;
    out.x = e[0] * v.x + e[4] * v.y + e[8] * v.z;
    out.y = e[1] * v.x + e[5] * v.y + e[9] * v.z;
    out.z = e[2] * v.x + e[6] * v.y + e[10] * v.z;
    out.w = 0.0f;
}

// hkVector8 is basically two hkVector4s back to back, linear first then angular
static inline hkVector4& lin(hkVector8& v) { return *reinterpret_cast<hkVector4*>(&v.m_v[0]); }
static inline hkVector4& ang(hkVector8& v) { return *reinterpret_cast<hkVector4*>(&v.m_v[4]); }
static inline const hkVector4& lin(const hkVector8& v) { return *reinterpret_cast<const hkVector4*>(&v.m_v[0]); }
static inline const hkVector4& ang(const hkVector8& v) { return *reinterpret_cast<const hkVector4*>(&v.m_v[4]); }

// dst += r * src on all four lanes
static inline void addMul4(hkVector4& dst, hkReal r, const hkVector4& src) {
    dst.x += r * src.x;
    dst.y += r * src.y;
    dst.z += r * src.z;
    dst.w += r * src.w;
}

static inline void zeroBlock(hkMatrix3& m) {
    for (int i = 0; i < 12; i++) {
        m.elements[i] = 0.0f;
    }
}

static inline void identityBlock(hkMatrix3& m) {
    zeroBlock(m);
    m.elements[0] = 1.0f;
    m.elements[5] = 1.0f;
    m.elements[10] = 1.0f;
}

void hkMatrix6::sub(const hkMatrix6& other) {
    subBlock(m_m[0][0], other.m_m[0][0]);
    subBlock(m_m[0][1], other.m_m[0][1]);
    subBlock(m_m[1][0], other.m_m[1][0]);
    subBlock(m_m[1][1], other.m_m[1][1]);
}

// this = a * b, one block at a time: m[i][j] = a[i][0]*b[0][j] + a[i][1]*b[1][j]
void hkMatrix6::setMul(const hkMatrix6& a, const hkMatrix6& b) {
    hkMatrix3 tmp;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            hkMatrix3_setMulMat3Mat3(&tmp, &a.m_m[i][0], &b.m_m[0][j]);
            hkMatrix3_setMulMat3Mat3(&m_m[i][j], &a.m_m[i][1], &b.m_m[1][j]);
            addBlock(m_m[i][j], tmp);
        }
    }
}

// gauss-jordan on the columns: scale a column so its diagonal is 1, then use it to wipe that row out
// of every other column. do the same moves on the identity and you end up with the inverse.
// 5.1 asserts if a diagonal is zero, i didn't see anything like that in the asm so no check here
void hkMatrix6::setInvert(const hkMatrix6& tmpIn) {
    hkMatrix6& out = *this;
    hkMatrix6 in = tmpIn;

    identityBlock(out.m_m[0][0]);
    zeroBlock(out.m_m[0][1]);
    zeroBlock(out.m_m[1][0]);
    identityBlock(out.m_m[1][1]);

    for (int row2 = 0; row2 < 2; row2++) {
        for (int row3 = 0; row3 < 3; row3++) {
            hkReal multiplier = 1.0f / in.m_m[row2][row2].elements[row3 * 4 + row3];

            cols(in.m_m[0][row2])[row3].mul4(multiplier);
            cols(in.m_m[1][row2])[row3].mul4(multiplier);
            cols(out.m_m[0][row2])[row3].mul4(multiplier);
            cols(out.m_m[1][row2])[row3].mul4(multiplier);

            for (int col2 = 0; col2 < 2; col2++) {
                for (int col3 = 0; col3 < 3; col3++) {
                    if (col2 != row2 || col3 != row3) {
                        hkReal m = -in.m_m[row2][col2].elements[col3 * 4 + row3];

                        addMul4(cols(in.m_m[0][col2])[col3], m, cols(in.m_m[0][row2])[row3]);
                        addMul4(cols(in.m_m[1][col2])[col3], m, cols(in.m_m[1][row2])[row3]);
                        addMul4(cols(out.m_m[0][col2])[col3], m, cols(out.m_m[0][row2])[row3]);
                        addMul4(cols(out.m_m[1][col2])[col3], m, cols(out.m_m[1][row2])[row3]);
                    }
                }
            }
        }
    }
}

// diagonal blocks transpose in place, the two off-diagonal ones swap places
void hkMatrix6::setTranspose(const hkMatrix6& other) {
    m_m[0][0].setTranspose(other.m_m[0][0]);
    m_m[0][1].setTranspose(other.m_m[1][0]);
    m_m[1][0].setTranspose(other.m_m[0][1]);
    m_m[1][1].setTranspose(other.m_m[1][1]);
}

extern "C" {

void hkMatrix6Sub(hkMatrix6& out, const hkMatrix6& b) {
    out.sub(b);
}

void hkMatrix6SetMul(hkMatrix6& out, const hkMatrix6& a, const hkMatrix6& b) {
    out.setMul(a, b);
}

// 6-vector version: lin = a00*b.lin + a01*b.ang, ang = a10*b.lin + a11*b.ang
void hkMatrix6SetMulV(hkVector8& out, const hkMatrix6& a, const hkVector8& b) {
    hkVector4 tmp[2];

    setMul3(tmp[0], a.m_m[0][0], lin(b));
    setMul3(lin(out), a.m_m[0][1], ang(b));

    setMul3(tmp[1], a.m_m[1][0], lin(b));
    setMul3(ang(out), a.m_m[1][1], ang(b));

    lin(out).add4(tmp[0]);
    ang(out).add4(tmp[1]);
}

void hkMatrix6SetTranspose(hkMatrix6& out, const hkMatrix6& in) {
    out.setTranspose(in);
}

void hkMatrix6SetInvert(hkMatrix6& out, const hkMatrix6& in) {
    out.setInvert(in);
}

}  // extern "C"
