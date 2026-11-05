#include <havok/hkMatrix3.h>
#include <havok/hkRotation.h>
#include <havok/hkVector4.h>

#include <math.h>

void hkMatrix3::transpose() {
    float* m = &m_col0.x;

    float value4 = m[4];
    m[4] = m[1];
    m[1] = value4;

    float value8 = m[8];
    m[8] = m[2];
    m[2] = value8;

    float value9 = m[9];
    m[9] = m[6];
    m[6] = value9;
}

// Same code as hkMatrix3::setMul (the C entry point of the product).
void hkMatrix3_setMulMat3Mat3(hkMatrix3* dst, const hkMatrix3* lhs, const hkMatrix3* rhs) {
    dst->_setMul(*lhs, *rhs);
}

void hkMatrix3_invertSymmetric(hkMatrix3* matrix) {
    matrix->invertSymmetric();
}

void hkMatrix3::setTranspose(const hkMatrix3& other) {
    const float* src = &other.m_col0.x;
    float* dst = &m_col0.x;

    dst[0] = src[0];
    dst[5] = src[5];
    dst[10] = src[10];
    dst[3] = 0.0f;
    dst[7] = 0.0f;
    dst[11] = 0.0f;
    dst[1] = src[4];
    dst[4] = src[1];
    dst[2] = src[8];
    dst[8] = src[2];
    dst[6] = src[9];
    dst[9] = src[6];
}

void hkMatrix3::setMul(const hkMatrix3& lhs, const hkMatrix3& rhs) {
    _setMul(lhs, rhs);
}

void hkMatrix3::setMul(float scalar, const hkMatrix3& other) {
    const float* src = &other.m_col0.x;
    float* dst = &m_col0.x;

    dst[0] = scalar * src[0];
    dst[1] = scalar * src[1];
    dst[2] = scalar * src[2];
    dst[3] = scalar * src[3];
    dst[4] = scalar * src[4];
    dst[5] = scalar * src[5];
    dst[6] = scalar * src[6];
    dst[7] = scalar * src[7];
    dst[8] = scalar * src[8];
    dst[9] = scalar * src[9];
    dst[10] = scalar * src[10];
    dst[11] = scalar * src[11];
}

void hkMatrix3::setMulInverse(const hkMatrix3& lhs, const hkMatrix3& rhs) {
    hkMatrix3 transposed;
    transposed.setTranspose(rhs);
    _setMul(lhs, transposed);
}

hkBool hkMatrix3::isApproximatelyEqual(float epsilon, const hkMatrix3& other) const {
    return m_col0.equals3(other.m_col0, epsilon) && m_col1.equals3(other.m_col1, epsilon) &&
           m_col2.equals3(other.m_col2, epsilon);
}

hkResult hkMatrix3::invert(float epsilon) {
    hkVector4 r0;
    r0.setCross(m_col1, m_col2);
    hkVector4 r1;
    r1.setCross(m_col2, m_col0);
    hkVector4 r2;
    r2.setCross(m_col0, m_col1);

    const hkReal determinant = m_col0.dot3(r0);
    if ((hkReal)fabs(determinant) > epsilon * epsilon * epsilon) {
        const hkReal invDeterminant = 1.0f / determinant;
        m_col0.setMul4(invDeterminant, r0);
        m_col1.setMul4(invDeterminant, r1);
        m_col2.setMul4(invDeterminant, r2);
        transpose();
        return HK_SUCCESS;
    }
    return HK_FAILURE;
}

void hkMatrix3::invertSymmetric() {
    hkVector4 r0;
    r0.setCross(m_col1, m_col2);
    hkVector4 r1;
    r1.setCross(m_col2, m_col0);
    hkVector4 r2;
    r2.setCross(m_col0, m_col1);

    hkReal determinant = m_col0.dot3(r0);
    const hkReal eps = 1.6940659e-21f;
    determinant = (eps > determinant) ? eps : determinant;

    hkReal invDeterminant = 1.0f / determinant;
    m_col0.setMul4(invDeterminant, r0);
    m_col1.setMul4(invDeterminant, r1);
    m_col2.setMul4(invDeterminant, r2);
}

void hkMatrix3::add(const hkMatrix3& other) {
    m_col0.add4(other.m_col0);
    m_col1.add4(other.m_col1);
    m_col2.add4(other.m_col2);
}

void hkMatrix3::mul(float scalar) {
    float* m = &m_col0.x;

    m[0] = m[0] * scalar;
    m[1] = m[1] * scalar;
    m[2] = m[2] * scalar;
    m[3] = m[3] * scalar;
    m[4] = m[4] * scalar;
    m[5] = m[5] * scalar;
    m[6] = m[6] * scalar;
    m[7] = m[7] * scalar;
    m[8] = m[8] * scalar;
    m[9] = m[9] * scalar;
    m[10] = m[10] * scalar;
    m[11] = m[11] * scalar;
}

void hkMatrix3::changeBasis(const hkRotation& basis) {
    hkMatrix3 temp;
    const hkMatrix3& rotation = basis;
    temp.setMulInverse(*this, rotation);
    setMul(rotation, temp);
}
