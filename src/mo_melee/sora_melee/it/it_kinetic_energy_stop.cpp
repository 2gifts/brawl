#include <it/kinetic/it_kinetic_energy.h>
#include <mt/mt_vector.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

itKineticEnergyStop::itKineticEnergyStop() : m_mode(-1) {
    onConsiderGroundFriction();
}

void itKineticEnergyStop::resetEnergy(int mode, const Vec2f& speed, const Vec3f&, soModuleAccesser*) {
    clearSpeed();
    m_mode = -1;
    float x = speed.m_x;
    float y = speed.m_y;
    m_accel = Vec2f(0.0f, 0.0f);
    m_stableSpeed = Vec2f(0.0f, 0.0f);
    m_brake = Vec2f(0.0f, 0.0f);
    m_limit = Vec2f(-1.0f, -1.0f);
    m_speed.m_x = x;
    m_speed.m_y = y;
    m_mode = mode;
}

void itKineticEnergyStop::updateEnergy(soModuleAccesser* moduleAccesser) {
    float length = getSpeed().length();
    if (length <= 0.00001f) {
        m_accel = Vec2f(0.0f, 0.0f);
    } else if (length > 0.00001f) {
        float brake;
        if (moduleAccesser->getSituationModule().getKind() == Situation_Ground) {
            brake = soValueAccesser::getConstantFloat(moduleAccesser, 0xD6C, 0);
            m_brake = Vec2f(brake, m_brake.m_y);
        } else {
            brake = soValueAccesser::getConstantFloat(moduleAccesser, 0xD6B, 0);
            m_brake = Vec2f(brake, m_brake.m_y);
        }
        float rest = length - brake;
        if (rest < 0.0f) {
            if (m_mode == 10) {
                Vec2f accel = getSpeed();
                Vec2f stableSpeed = m_stableSpeed;
                accel.m_x *= -1.0f;
                accel.m_y *= -1.0f;
                if (accel.m_x > 0.0f) {
                    accel.m_x -= stableSpeed.m_x;
                }
                if (accel.m_x < 0.0f) {
                    accel.m_x += stableSpeed.m_x;
                }
                if (accel.m_y > 0.0f) {
                    accel.m_y -= stableSpeed.m_y;
                }
                if (accel.m_y < 0.0f) {
                    accel.m_y += stableSpeed.m_y;
                }
                m_accel = accel;
            } else {
                m_accel = Vec2f(0.0f, 0.0f);
            }
        } else {
            Vec2f accel = getSpeed() * (rest / length - 1.0f);
            m_accel = accel;
        }
    }
    itKineticEnergy::updateEnergy(moduleAccesser);
}

// TODO: clean up, give variables real names
namespace jj_2BF288 {
extern const f32 lbl_27_rodata_5FC0;
extern const f32 lbl_27_rodata_5FC4;

struct Vec2 {
    f32 x, y;

    Vec2() {}
    Vec2(f32 a, f32 b) : x(a), y(b) {}

    void set(f32 a, f32 b) {
        x = a;
        y = b;
    }
};

class Obj {
public:
    virtual void vf0() = 0;
    virtual void vf1() = 0;
    virtual void vf2() = 0;
    virtual void vf3() = 0;
    virtual void vf4() = 0;
    virtual void vf5() = 0;
    virtual void vf6() = 0;

    u32 pad4;
    Vec2 pos, a, b, c, d;
    u32 pad30, pad34;
    s32 i38;

    void reset() {
        Vec2 vd, vc, vb, va;
        va.set(lbl_27_rodata_5FC4, lbl_27_rodata_5FC4);
        vb.set(lbl_27_rodata_5FC4, lbl_27_rodata_5FC4);
        vc.set(lbl_27_rodata_5FC4, lbl_27_rodata_5FC4);
        vd.set(lbl_27_rodata_5FC0, lbl_27_rodata_5FC0);

        i38 = -1;
        a = va;
        b = vb;
        c = vc;
        d = vd;
    }
};

extern "C" void fn_27_2BF288(Obj* self, s32 arg1, const Vec2* arg2) {
    self->vf6();
    self->reset();
    self->pos = *arg2;
    self->i38 = arg1;
}
}

