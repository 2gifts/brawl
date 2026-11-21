#define MT_VEC3F_MUL_INLINE
#define MT_VEC3F_LENGTH_SQ_INLINE
#include <so/so_kinetic_energy_rot_normal.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <math.h>
#include <types.h>

inline float Vec3f::length() {
    float lengthSquared = lengthSq();
    double absolute = __fabs(lengthSquared);
    float magnitude = absolute;
    if (magnitude <= 1.17549435e-38f) {
        return 0.0f;
    }
    return lengthSquared * rsqrtf(lengthSquared);
}

void soKineticEnergyRotNormal::updateEnergy(soModuleAccesser*) {
    Vec3f accel = m_accel;
    accel = soKineticUtility::brakeSpeed(&m_rotSpeed, &accel, &m_speedTarget, &m_brake);
    if (accel.length() != 0.0f) {
        Vec3fAdd(&m_rotSpeed, &m_rotSpeed, &accel);
    }
    m_rotSpeed = soKineticUtility::limitSpeed(&m_rotSpeed, &m_speedLimit);
}

void soKineticEnergyRotNormal::resetEnergy(int, const Vec2f&, const Vec3f& rotation, soModuleAccesser*) {
    m_rotSpeed = rotation;
}

// MATCH-ONLY: componentwise product in paired singles (the same style as Vec3fAdd).
static inline void Vec3fMul(register Vec3f* pOut, register const Vec3f* lhs, register const Vec3f* rhs) {
    register f32 fr3, fr2, fr1, fr0;
    // clang-format off
    asm {
        psq_l  fr3, Vec3f.m_x(lhs), 0, 0
        psq_l  fr1, Vec3f.m_x(rhs), 0, 0
        psq_l  fr2, Vec3f.m_z(lhs), 1, 0
        psq_l  fr0, Vec3f.m_z(rhs), 1, 0
        ps_mul fr1, fr3, fr1
        ps_mul fr0, fr2, fr0
        psq_st fr1, Vec3f.m_x(pOut), 0, 0
        psq_st fr0, Vec3f.m_z(pOut), 1, 0
    }
    // clang-format on
}

void soKineticEnergyRotNormal::mulSpeed(Vec3f* speed) {
    Vec3fMul(&m_rotSpeed, &m_rotSpeed, speed);
}

void soKineticEnergyRotNormal::mulAccel(Vec3f* accel) {
    Vec3fMul(&m_accel, &m_accel, accel);
}

static inline void reflectVec(Vec3f& out, const Vec3f& v, const Vec3f& n) {
    float dot = n.m_x * v.m_x + n.m_y * v.m_y + n.m_z * v.m_z;
    Vec3f n2 = n * 2.0f;
    Vec3f proj = n2 * dot;
    Vec3f result = out - proj;
    out = result;
}

void soKineticEnergyRotNormal::reflectSpeed(const Vec3f& normal) {
    reflectVec(m_rotSpeed, m_rotSpeed, normal);
}

void soKineticEnergyRotNormal::reflectAccel(const Vec3f& normal) {
    reflectVec(m_accel, m_accel, normal);
}

void soKineticEnergyRotNormal::projectionNormalFollow() { }

void soKineticEnergyRotNormal::clearRotSpeed() {
    m_rotSpeed = Vec3f(0.0f, 0.0f, 0.0f);
}

void soKineticEnergyRotNormal::clearSpeed() { }

Vec3f soKineticEnergyRotNormal::getRotation() const {
    return m_rotSpeed;
}

Vec2f soKineticEnergyRotNormal::getSpeed() const {
    return Vec2f(0.0f, 0.0f);
}

soKineticEnergyRotNormal::~soKineticEnergyRotNormal() { }
