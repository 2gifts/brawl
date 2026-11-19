#define MT_VEC3F_MUL_INLINE
#include <mt/mt_vector.h>
#include <so/kinetic/so_kinetic_energy.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <types.h>

// HYPOTHESIS: the full 3D counterpart of soKineticEnergyNormal (layout and vtable order from the REL's own vtable).
class soKineticEnergy3f : public soKineticEnergy {
public:
    virtual void updateEnergy(soModuleAccesser*);
    virtual Vec2f getSpeed();
    virtual Vec3f getSpeed3f();
    virtual Vec3f getRotation();
    virtual void resetEnergy(int, Vec2f*, Vec3f*, soModuleAccesser*);
    virtual void clearSpeed();
    virtual void clearRotSpeed();
    virtual void mulSpeed(Vec3f*);
    virtual void mulAccel(Vec3f*);
    virtual void reflectSpeed(Vec3f*);
    virtual void reflectAccel(Vec3f*);
    virtual void onConsiderGroundFriction();
    virtual void offConsiderGroundFriction();
    virtual ~soKineticEnergy3f();
    void resetEnergy(int, Vec3f*, Vec3f*, soModuleAccesser*);
    Vec3f m_speed3f;
    Vec3f m_accel3f;
    Vec3f m_targetSpeed3f;
    Vec3f m_brake3f;
    Vec3f m_limit3f;
    bool unk44;
    bool unk45;
};

void soKineticEnergy3f::resetEnergy(int, Vec2f* speed, Vec3f*, soModuleAccesser*) {
    m_speed3f = Vec3f(speed->m_x, speed->m_y, 0.0f);
    m_accel3f = Vec3f(0.0f, 0.0f, 0.0f);
    m_targetSpeed3f = Vec3f(0.0f, 0.0f, 0.0f);
    m_brake3f = Vec3f(0.0f, 0.0f, 0.0f);
    m_limit3f = Vec3f(-1.0f, -1.0f, -1.0f);
}

void soKineticEnergy3f::resetEnergy(int, Vec3f* speed, Vec3f*, soModuleAccesser*) {
    m_speed3f = *speed;
    m_accel3f = Vec3f(0.0f, 0.0f, 0.0f);
    m_targetSpeed3f = Vec3f(0.0f, 0.0f, 0.0f);
    m_brake3f = Vec3f(0.0f, 0.0f, 0.0f);
    m_limit3f = Vec3f(-1.0f, -1.0f, -1.0f);
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

void soKineticEnergy3f::mulSpeed(Vec3f* speed) {
    Vec3fMul(&m_speed3f, &m_speed3f, speed);
}

void soKineticEnergy3f::mulAccel(Vec3f* accel) {
    Vec3fMul(&m_accel3f, &m_accel3f, accel);
}

static inline void reflectVec(Vec3f& out, const Vec3f& v, const Vec3f& n) {
    float dot = n.m_x * v.m_x + n.m_y * v.m_y + n.m_z * v.m_z;
    Vec3f n2 = n * 2.0f;
    Vec3f proj = n2 * dot;
    Vec3f result = out - proj;
    out = result;
}

void soKineticEnergy3f::reflectSpeed(Vec3f* normal) {
    reflectVec(m_speed3f, m_speed3f, *normal);
}

void soKineticEnergy3f::reflectAccel(Vec3f* normal) {
    reflectVec(m_accel3f, m_accel3f, *normal);
}

void soKineticEnergy3f::onConsiderGroundFriction() {
    unk45 = true;
}

void soKineticEnergy3f::offConsiderGroundFriction() {
    unk45 = false;
}

void soKineticEnergy3f::clearRotSpeed() { }

void soKineticEnergy3f::clearSpeed() {
    m_speed3f.m_z = 0.0f;
    m_speed3f.m_y = 0.0f;
    m_speed3f.m_x = 0.0f;
}

Vec3f soKineticEnergy3f::getRotation() {
    return Vec3f(0.0f, 0.0f, 0.0f);
}

Vec3f soKineticEnergy3f::getSpeed3f() {
    return m_speed3f;
}

Vec2f soKineticEnergy3f::getSpeed() {
    return Vec2f(m_speed3f.m_x, m_speed3f.m_y);
}

soKineticEnergy3f::~soKineticEnergy3f() { }
