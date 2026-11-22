#define MT_VEC3F_SCALE_XY_IN_PLACE
#define MT_VEC3F_MUL_INLINE
#define MT_VEC3F_OPERATOR_MUL_FRESH_REGS
#define MT_VEC3F_SUB_FRESH_REGS
#include <it/kinetic/it_kinetic_energy.h>
#include <mt/mt_vector.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <so/so_world.h>
#include <types.h>

void itKineticEnergy::updateEnergy(soModuleAccesser* moduleAccesser) {
    Vec2f accel = m_accel;
    Vec2f brake = m_brake;
    if (m_considerGroundFriction || m_isFollowGroundNormal) {
        if (moduleAccesser->getSituationModule().getKind() == Situation_Ground) {
            if (moduleAccesser->getGroundModule().isTouch(0xFF, 0)) {
                if (m_considerGroundFriction == true) {
                    float friction = moduleAccesser->getGroundModule().getDownFriction(0);
                    if (friction != 1.0f) {
                        accel.m_x *= friction;
                        accel.m_y *= friction;
                        brake.m_x *= friction;
                        brake.m_y *= friction;
                    }
                }
                if (m_isFollowGroundNormal == true) {
                    Vec2f normal = moduleAccesser->getGroundModule().getTouchNormal(8, 0);
                    accel = soKineticUtility::projectionNormalFollow(accel, normal);
                }
            }
        }
    }
    m_speed.m_x += accel.m_x;
    m_speed.m_y += accel.m_y;
    Vec2f stableSpeed = m_stableSpeed;
    if (stableSpeed.m_x < 0.0f) {
        stableSpeed.m_x = 0.0f;
    }
    if (stableSpeed.m_y < 0.0f) {
        stableSpeed.m_y = 0.0f;
    }
    if (m_speed.m_x > stableSpeed.m_x) {
        m_speed.m_x -= brake.m_x;
        if (m_speed.m_x < stableSpeed.m_x) {
            m_speed.m_x = stableSpeed.m_x;
        }
    } else {
        m_speed.m_x += brake.m_x;
        if (m_speed.m_x > stableSpeed.m_x) {
            m_speed.m_x = stableSpeed.m_x;
        }
    }
    if (m_speed.m_y > stableSpeed.m_y) {
        m_speed.m_y -= brake.m_y;
        if (m_speed.m_y < stableSpeed.m_y) {
            m_speed.m_y = stableSpeed.m_y;
        }
    } else {
        m_speed.m_y += brake.m_y;
        if (m_speed.m_y > stableSpeed.m_y) {
            m_speed.m_y = stableSpeed.m_y;
        }
    }
    if (m_limitFlags & Limit_Length) {
        float limit = 0.0f;
        if (m_limit.m_x >= limit) {
            limit += m_limit.m_x;
        }
        if (m_limit.m_y >= limit) {
            limit += m_limit.m_y;
        }
        if (limit >= 0.0f) {
            float length = m_speed.length();
            if (length > limit) {
                if (limit <= 0.0f) {
                    m_speed.m_x = m_speed.m_y = 0.0f;
                } else {
                    m_speed.normalize();
                    m_speed.m_x *= limit;
                    m_speed.m_y *= limit;
                }
            }
        }
    } else {
        if (m_limit.m_x >= 0.0f) {
            if ((m_limitFlags & Limit_PlusX) && m_speed.m_x > m_limit.m_x) {
                m_speed.m_x = m_limit.m_x;
            } else if ((m_limitFlags & Limit_MinusX) && m_speed.m_x < -m_limit.m_x) {
                m_speed.m_x = -m_limit.m_x;
            }
        }
        if (m_limit.m_y >= 0.0f) {
            if ((m_limitFlags & Limit_PlusY) && m_speed.m_y > m_limit.m_y) {
                m_speed.m_y = m_limit.m_y;
            } else if ((m_limitFlags & Limit_MinusY) && m_speed.m_y < -m_limit.m_y) {
                m_speed.m_y = -m_limit.m_y;
            }
        }
    }
}

void itKineticEnergy::updateEnergy(soModuleAccesser* moduleAccesser, float rate) {
    if (rate != 1.0f) {
        Vec2f accel = m_accel;
        Vec2f brake = m_brake;
        m_accel.m_x *= rate;
        m_accel.m_y *= rate;
        m_brake.m_x *= rate;
        m_brake.m_y *= rate;
        updateEnergy(moduleAccesser);
        m_accel = accel;
        m_brake = brake;
    } else {
        updateEnergy(moduleAccesser);
    }
}

void itKineticEnergy::mulSpeed(Vec3f* speed) {
    Vec2f mul(*speed);
    m_speed.m_x *= mul.m_x;
    m_speed.m_y *= mul.m_y;
}

void itKineticEnergy::mulAccel(Vec3f* accel) {
    Vec2f mul(*accel);
    m_accel.m_x *= mul.m_x;
    m_accel.m_y *= mul.m_y;
}

static inline void reflectVec(Vec2f& out, const Vec2f& v, const Vec2f& n) {
    out = v - n * 2.0f * (n.m_x * v.m_x + n.m_y * v.m_y);
}

void itKineticEnergy::reflectSpeed(const Vec3f& normal) {
    Vec2f n(normal);
    reflectVec(m_speed, m_speed, n);
}

void itKineticEnergy::reflectAccel(const Vec3f& normal) {
    Vec2f n(normal);
    reflectVec(m_accel, m_accel, n);
}

// TODO: clean up -- MATCH-ONLY paired-single add with the original register assignment (copied from
// so_kinetic_energy_3f.cpp); should be Vec3f::operator+= once the shared Vec3fAdd helper has this variant.
static inline void Vec3fAddLocal(register Vec3f* pOut, register const Vec3f* lhs, register const Vec3f* rhs) {
    register f32 rxy, lxy, sxy, rz, lz, sz;
    // clang-format off
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
    // clang-format on
}

// HYPOTHESIS: draft of the 3D energy (the original scales, adds and multiplies the vectors with paired-single
// helpers); itKineticEnergy with a third axis.
void itKineticEnergy3f::updateEnergy(soModuleAccesser* moduleAccesser) {
    Vec3f accel = m_accel;
    Vec3f brake = m_brake;
    if (m_considerGroundFriction || m_isFollowGroundNormal) {
        if (moduleAccesser->getSituationModule().getKind() == Situation_Ground) {
            if (moduleAccesser->getGroundModule().isTouch(0xFF, 0)) {
                if (m_considerGroundFriction == true) {
                    float friction = moduleAccesser->getGroundModule().getDownFriction(0);
                    if (friction != 1.0f) {
                        Vec3fScale(&accel, &accel, friction);
                        Vec3fScale(&brake, &brake, friction);
                    }
                }
                if (m_isFollowGroundNormal == true) {
                    Vec2f normal = moduleAccesser->getGroundModule().getTouchNormal(8, 0);
                    Vec2f follow = soKineticUtility::projectionNormalFollow(Vec2f(accel), normal);
                    accel.m_x = follow.m_x;
                    accel.m_y = follow.m_y;
                }
            }
        }
    }
    Vec3fAddLocal(&m_speed, &m_speed, &accel);
    Vec3f stableSpeed = m_stableSpeed;
    if (stableSpeed.m_x < 0.0f) {
        stableSpeed.m_x = 0.0f;
    }
    if (stableSpeed.m_y < 0.0f) {
        stableSpeed.m_y = 0.0f;
    }
    if (m_speed.m_x > stableSpeed.m_x) {
        m_speed.m_x -= brake.m_x;
        if (m_speed.m_x < stableSpeed.m_x) {
            m_speed.m_x = stableSpeed.m_x;
        }
    } else {
        m_speed.m_x += brake.m_x;
        if (m_speed.m_x > stableSpeed.m_x) {
            m_speed.m_x = stableSpeed.m_x;
        }
    }
    if (m_speed.m_y > stableSpeed.m_y) {
        m_speed.m_y -= brake.m_y;
        if (m_speed.m_y < stableSpeed.m_y) {
            m_speed.m_y = stableSpeed.m_y;
        }
    } else {
        m_speed.m_y += brake.m_y;
        if (m_speed.m_y > stableSpeed.m_y) {
            m_speed.m_y = stableSpeed.m_y;
        }
    }
    if (m_speed.m_z > stableSpeed.m_z) {
        m_speed.m_z -= brake.m_z;
        if (m_speed.m_z < stableSpeed.m_z) {
            m_speed.m_z = stableSpeed.m_z;
        }
    } else {
        m_speed.m_z += brake.m_z;
        if (m_speed.m_z > stableSpeed.m_z) {
            m_speed.m_z = stableSpeed.m_z;
        }
    }
    if (m_limitFlags & itKineticEnergy::Limit_Length) {
        float limit = 0.0f;
        if (m_limit.m_x >= limit) {
            limit += m_limit.m_x;
        }
        if (m_limit.m_y >= limit) {
            limit += m_limit.m_y;
        }
        if (m_limit.m_z >= limit) {
            limit += m_limit.m_z;
        }
        if (limit >= 0.0f) {
            float lengthSquared = m_speed.m_x * m_speed.m_x + m_speed.m_y * m_speed.m_y + m_speed.m_z * m_speed.m_z;
            double absolute = __fabs(lengthSquared);
            float magnitude = absolute;
            float length = magnitude <= 1.17549435e-38f ? 0.0f : lengthSquared * rsqrtf(lengthSquared);
            if (length > limit) {
                if (limit <= 0.0f) {
                    m_speed.m_x = m_speed.m_y = m_speed.m_z = 0.0f;
                } else {
                    m_speed.normalize();
                    Vec3fScale(&m_speed, &m_speed, limit);
                }
            }
        }
    } else {
        if (m_limit.m_x >= 0.0f) {
            if ((m_limitFlags & itKineticEnergy::Limit_PlusX) && m_speed.m_x > m_limit.m_x) {
                m_speed.m_x = m_limit.m_x;
            } else if ((m_limitFlags & itKineticEnergy::Limit_MinusX) && m_speed.m_x < -m_limit.m_x) {
                m_speed.m_x = -m_limit.m_x;
            }
        }
        if (m_limit.m_y >= 0.0f) {
            if ((m_limitFlags & itKineticEnergy::Limit_PlusY) && m_speed.m_y > m_limit.m_y) {
                m_speed.m_y = m_limit.m_y;
            } else if ((m_limitFlags & itKineticEnergy::Limit_MinusY) && m_speed.m_y < -m_limit.m_y) {
                m_speed.m_y = -m_limit.m_y;
            }
        }
        if (m_limit.m_z >= 0.0f) {
            if ((m_limitFlags & itKineticEnergy::Limit_PlusZ) && m_speed.m_z > m_limit.m_z) {
                m_speed.m_z = m_limit.m_z;
            } else if ((m_limitFlags & itKineticEnergy::Limit_MinusZ) && m_speed.m_z < -m_limit.m_z) {
                m_speed.m_z = -m_limit.m_z;
            }
        }
    }
}

void itKineticEnergy3f::updateEnergy(soModuleAccesser* moduleAccesser, float rate) {
    if (rate != 1.0f) {
        Vec3f accel = m_accel;
        Vec3f brake = m_brake;
        Vec3fScale(&m_accel, &m_accel, rate);
        Vec3fScale(&m_brake, &m_brake, rate);
        updateEnergy(moduleAccesser);
        m_accel = accel;
        m_brake = brake;
    } else {
        updateEnergy(moduleAccesser);
    }
}

// TODO: clean up -- hand-written paired-single multiply (as in so_kinetic_energy_3f.cpp); should be a shared Vec3f
// helper.
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

void itKineticEnergy3f::mulSpeed(Vec3f* speed) {
    Vec3fMul(&m_speed, &m_speed, speed);
}

void itKineticEnergy3f::mulAccel(Vec3f* accel) {
    Vec3fMul(&m_accel, &m_accel, accel);
}

static inline void reflectVec3(Vec3f& out, const Vec3f& v, const Vec3f& n) {
    out = out - n * 2.0f * (n.m_x * v.m_x + n.m_y * v.m_y + n.m_z * v.m_z);
}

void itKineticEnergy3f::reflectSpeed(const Vec3f& normal) {
    reflectVec3(m_speed, m_speed, normal);
}

void itKineticEnergy3f::reflectAccel(const Vec3f& normal) {
    reflectVec3(m_accel, m_accel, normal);
}

void itKineticEnergyGravity::updateEnergy(soModuleAccesser*) {
    m_speed += m_accel * g_soWorld->m_gravityUp;
    float stableSpeed = m_stableSpeed * g_soWorld->m_gravityDown;
    if (stableSpeed < 0.0f) {
        stableSpeed = 0.0f;
    }
    float brake = m_brake;
    if (m_speed > stableSpeed) {
        m_speed -= brake;
        if (m_speed < stableSpeed) {
            m_speed = 0.0f;
        }
    } else {
        m_speed += brake;
        if (m_speed > stableSpeed) {
            m_speed = stableSpeed;
        }
    }
    if (m_limit >= 0.0f) {
        if ((m_limitFlags & itKineticEnergy::Limit_PlusY) && m_speed > m_limit) {
            m_speed = m_limit;
        } else if ((m_limitFlags & itKineticEnergy::Limit_MinusY) && m_speed < -m_limit) {
            m_speed = -m_limit;
        }
    }
}

void itKineticEnergyGravity::updateEnergy(soModuleAccesser* moduleAccesser, float rate) {
    if (rate != 1.0f) {
        float accel = m_accel;
        float brake = m_brake;
        m_accel *= rate;
        m_brake *= rate;
        updateEnergy(moduleAccesser);
        m_accel = accel;
        m_brake = brake;
    } else {
        updateEnergy(moduleAccesser);
    }
}

void itKineticEnergyGravity::mulSpeed(Vec3f* speed) {
    m_speed *= speed->m_y;
}

void itKineticEnergyGravity::mulAccel(Vec3f* accel) {
    m_accel *= accel->m_y;
}

void itKineticEnergyGravity::reflectSpeed(const Vec3f& normal) {
    Vec2f v(0.0f, m_speed);
    Vec2f n(normal);
    reflectVec(v, v, n);
    m_speed = v.m_y;
}

void itKineticEnergyGravity::reflectAccel(const Vec3f& normal) {
    Vec2f v(0.0f, m_accel);
    Vec2f n(normal);
    reflectVec(v, v, n);
    m_accel = v.m_y;
}

void itKineticEnergy3f::resetEnergy(int, const Vec2f& speed, const Vec3f&, soModuleAccesser*) {
    float y;
    float x = speed.m_x;
    y = speed.m_y;
    m_speed.m_x = x;
    m_speed.m_y = y;
}

