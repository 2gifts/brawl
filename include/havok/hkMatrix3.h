#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector4.h>
#include <StaticAssert.h>

struct hkRotation;
struct hkQuaternion;

// 3x3 matrix stored as three column vectors (column-major): entry (row r, column c) is m_col<c>[r].
// The fourth lanes are retained by scalar/add operations and cleared by matrix products and setTranspose.
// Column members (not a float array): main.dol has an out-of-line weak hkMatrix3::__as that copies the 12 floats
// through hkVector4's float-wise operator=, and hkRotation (which is an hkMatrix3) is read by column everywhere.
// 16-byte alignment: main.dol's hkClassMember::TYPE_PROPERTIES gives hkMatrix3 alignment 16.
struct hkMatrix3 {
    hkVector4 m_col0; // 0x00
    hkVector4 m_col1; // 0x10
    hkVector4 m_col2; // 0x20

    // One zero vector copied to the three columns (inlined in hkKeyframedRigidMotion and hkMassProperties' ctor).
    void setZero() {
        hkVector4 zero;
        zero.setZero4();
        m_col0 = zero;
        m_col1 = zero;
        m_col2 = zero;
    }

    void transpose();
    void setTranspose(const hkMatrix3& other);
    void setMul(const hkMatrix3& lhs, const hkMatrix3& rhs);
    void setMul(float scalar, const hkMatrix3& other);
    void setMulInverse(const hkMatrix3& lhs, const hkMatrix3& rhs);
    hkBool isApproximatelyEqual(float epsilon, const hkMatrix3& other) const;
    hkResult invert(float epsilon);
    void invertSymmetric();
    void add(const hkMatrix3& other);
    void mul(float scalar);
    // HYPOTHESIS: callers establish the basis storage, but not the source type name.
    void changeBasis(const hkRotation& basis);
} __attribute__((aligned(16)));

static_assert(sizeof(hkMatrix3) == 0x30, "hkMatrix3 size");

void hkMatrix3_setMulMat3Mat3(hkMatrix3* dst, const hkMatrix3* lhs, const hkMatrix3* rhs);
void hkMatrix3_invertSymmetric(hkMatrix3* matrix);
