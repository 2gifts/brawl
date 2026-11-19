#define MT_VEC3F_SCALE_XY_IN_PLACE
#define MT_VEC3F_MUL_INLINE
#include <mt/mt_vector.h>
#include <so/kinetic/so_kinetic_energy.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <so/situation/so_situation_event_presenter.h>
#include <so/ground/so_ground_module_impl.h>
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

// TODO: clean up -- hand-written paired-single multiply; should be a shared Vec3f helper.
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

// TODO: clean up -- MATCH-ONLY paired-single add with the original register assignment; should be Vec3f::operator+= once
// the shared Vec3fAdd helper has a variant with these register lifetimes.
static inline void Vec3fAddLocal(register Vec3f* pOut, register const Vec3f* lhs, register const Vec3f* rhs) {
    register f32 rxy, lxy, sxy, rz, lz, sz;
    asm {
        psq_l  lxy, Vec3f.m_x(lhs), 0, 0
        psq_l  rxy, Vec3f.m_x(rhs), 0, 0
        ps_add sxy, lxy, rxy
        psq_l  lz, Vec3f.m_z(lhs), 1, 0
        psq_l  rz, Vec3f.m_z(rhs), 1, 0
        ps_add sz, lz, rz
        psq_st sxy, Vec3f.m_x(pOut), 0, 0
        psq_st sz, Vec3f.m_z(pOut), 1, 0
    }
}

// TODO: clean up -- Vec3f::lengthSq() with the Z + (X + Y) grouping and no out-of-line call; should use the shared
// MT_VEC3F_LENGTH_SQ_INLINE form of Vec3f::lengthSq.
static inline float lengthSq3(const Vec3f& v) {
    return v.m_z * v.m_z + (v.m_x * v.m_x + v.m_y * v.m_y);
}

// NEAR-MISS: the projection of the ground-normal-followed acceleration copies its temporaries differently, and the
// x-axis brake clamp stores 0.0f where the y/z axes store the target (kept as observed).
void soKineticEnergy3f::updateEnergy(soModuleAccesser* moduleAccesser) {
    if (lengthSq3(m_accel3f) != 0.0f || lengthSq3(m_speed3f) != 0.0f) {
        Vec3f accel = m_accel3f;
        Vec3f brake = m_brake3f;
        if (moduleAccesser->getSituationModule().getKind() == Situation_Ground) {
            if (unk44 == true || unk45 == true) {
                if (moduleAccesser->getGroundModule().isTouch(8, 0)) {
                    if (unk45 == true) {
                        float friction = moduleAccesser->getGroundModule().getDownFriction(0);
                        if (friction > 1.0f) {
                            Vec3fScale(&brake, &brake, friction);
                        } else if (friction < 1.0f) {
                            Vec3fScale(&accel, &accel, friction);
                            Vec3fScale(&brake, &brake, friction);
                        }
                    }
                    if (unk44 == true) {
                        Vec2f normal = moduleAccesser->getGroundModule().getTouchNormal(8, 0);
                        Vec2f flat(accel);
                        Vec2f projected;
                        projected = soKineticUtility::projectionNormalFollow(&flat, &normal);
                        accel.m_x = projected.m_x;
                        accel.m_y = projected.m_y;
                    }
                }
            }
        }
        Vec3fAddLocal(&m_speed3f, &m_speed3f, &accel);
        Vec3f target = m_targetSpeed3f;
        if (target.m_x < 0.0f)
            target.m_x = 0.0f;
        if (target.m_y < 0.0f)
            target.m_y = 0.0f;
        if (m_speed3f.m_x > target.m_x) {
            m_speed3f.m_x -= brake.m_x;
            if (m_speed3f.m_x < target.m_x)
                m_speed3f.m_x = 0.0f;
        } else {
            m_speed3f.m_x += brake.m_x;
            if (m_speed3f.m_x > target.m_x)
                m_speed3f.m_x = target.m_x;
        }
        if (m_speed3f.m_y > target.m_y) {
            m_speed3f.m_y -= brake.m_y;
            if (m_speed3f.m_y < target.m_y)
                m_speed3f.m_y = target.m_y;
        } else {
            m_speed3f.m_y += brake.m_y;
            if (m_speed3f.m_y > target.m_y)
                m_speed3f.m_y = target.m_y;
        }
        if (m_speed3f.m_z > target.m_z) {
            m_speed3f.m_z -= brake.m_z;
            if (m_speed3f.m_z < target.m_z)
                m_speed3f.m_z = target.m_z;
        } else {
            m_speed3f.m_z += brake.m_z;
            if (m_speed3f.m_z > target.m_z)
                m_speed3f.m_z = target.m_z;
        }
        if (m_limit3f.m_x >= 0.0f) {
            if (m_speed3f.m_x > m_limit3f.m_x)
                m_speed3f.m_x = m_limit3f.m_x;
            else if (m_speed3f.m_x < -m_limit3f.m_x)
                m_speed3f.m_x = -m_limit3f.m_x;
        }
        if (m_limit3f.m_y >= 0.0f) {
            if (m_speed3f.m_y > m_limit3f.m_y)
                m_speed3f.m_y = m_limit3f.m_y;
            else if (m_speed3f.m_y < -m_limit3f.m_y)
                m_speed3f.m_y = -m_limit3f.m_y;
        }
        if (m_limit3f.m_z >= 0.0f) {
            if (m_speed3f.m_z > m_limit3f.m_z)
                m_speed3f.m_z = m_limit3f.m_z;
            else if (m_speed3f.m_z < -m_limit3f.m_z)
                m_speed3f.m_z = -m_limit3f.m_z;
        }
    }
}
