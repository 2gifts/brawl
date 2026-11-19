#include <so/so_kinetic_energy_normal.h>
#include <math.h>
#include <types.h>

// HYPOTHESIS: kinetic energy 5 of the Pikmin article (wn_pikmin_pikmin.h declares only setStopSpeed); it slows the speed down.
class soKineticEnergyStop : public soKineticEnergyNormal {
public:
    soKineticEnergyStop();
    virtual void updateEnergy(soModuleAccesser*);
    virtual ~soKineticEnergyStop();
    void setStopSpeed(float speed);
    float m_stopSpeed;
};

inline bool isNearZeroX(float v) {
    bool result = false;
    float absolute = __fabs(v);
    if (absolute < 0.00001f) {
        result = true;
    }
    return result;
}

inline bool isNearZero(const Vec2f& v) {
    bool result = false;
    if (isNearZeroX(v.m_x) && isNearZeroX(v.m_y)) {
        result = true;
    }
    return result;
}

inline float Vec2f_length(const Vec2f& v) {
    float lengthSquared = v.m_x * v.m_x + v.m_y * v.m_y;
    double absolute = __fabs(lengthSquared);
    float magnitude = absolute;
    if (magnitude <= 1.17549435e-38f) {
        return 0.0f;
    }
    return lengthSquared * rsqrtf(lengthSquared);
}

soKineticEnergyStop::soKineticEnergyStop() : m_stopSpeed(0.0f) {
    m_speedTarget = Vec2f(0.0f, 0.0f);
}

soKineticEnergyStop::~soKineticEnergyStop() { }

void soKineticEnergyStop::setStopSpeed(float speed) {
    m_stopSpeed = speed;
}

void soKineticEnergyStop::updateEnergy(soModuleAccesser* moduleAccesser) {
    if (isNearZero(getSpeed())) {
        m_speed = Vec2f(0.0f, 0.0f);
        m_brake = Vec2f(0.0f, 0.0f);
    } else if (m_stopSpeed > 0.0f) {
        float length = Vec2f_length(getSpeed());
        float remain = length - m_stopSpeed;
        if (remain < 0.0f) {
            m_brake = Vec2f(0.0f, 0.0f);
            m_accel = Vec2f(0.0f, 0.0f);
            m_speed = Vec2f(0.0f, 0.0f);
        } else {
            Vec2f accel = getSpeed() * (remain / length - 1.0f);
            m_accel = accel;
        }
    }
    soKineticEnergyNormal::updateEnergy(moduleAccesser);
}
