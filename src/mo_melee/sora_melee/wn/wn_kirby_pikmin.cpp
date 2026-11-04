#include <so/so_value_accesser.h>
#include <wn/pikmin/wn_kirby_pikmin.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_kinetic_energy_normal.h>

// Registers the Kirby's Pikmin article class (kind 150) in the class info table during static initialization.
wnClassInfoImpl<150, wnKirbyPikmin, wnKirbyPikminData> g_wnKirbyPikminClassInfo;

bool wnKirbyPikmin::notifyEventCollisionAttackCheck(u32 flags) {
    if ((flags & 7) != 0) {
        return false;
    }
    int reflected = 0;
    if ((flags & 8) != 0) {
        if (m_moduleAccesser->getReflectModule().isReflect() == 1) {
            reflected = 1;
        }
    }
    if (reflected != 0) {
        Vec2f accel(1.0f, 1.0f);
        float speedY = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA0, 0);
        Vec2f speed(-m_moduleAccesser->getReflectModule().getSpeedMul(), speedY);
        return reflectRequest(&speed, &accel);
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

void wnKirbyPikmin::shoot(Vec3f* position, float lr) {
    if (m_moduleAccesser->getLinkModule().isModelConstraint() == true) {
        m_moduleAccesser->getLinkModule().removeModelConstraint(true);
    }
    m_moduleAccesser->getStatusModule().changeStatusForce(1, m_moduleAccesser);
    m_moduleAccesser->getPostureModule().setPos(position);
    m_moduleAccesser->getPostureModule().setLr(lr);
    updateNodeSRT();
    wnKineticEnergyNormal* movement = dynamic_cast<wnKineticEnergyNormal*>(
        m_moduleAccesser->getKineticModule().getEnergy(0));
    if (movement) {
        const float* parameters = unk2044->unk70[m_moduleAccesser->getWorkManageModule().getInt(0x10000005)];
        movement->m_speed = Vec2f(parameters[2] * (m_moduleAccesser->getPostureModule().getLr() < 0.0f ? -1 : 1), parameters[4]);
        float speedLimitX = parameters[3];
        movement->m_accel = Vec2f(0.0f, -parameters[0]);
        movement->m_speedLimit = Vec2f(speedLimitX, parameters[1]);
        movement->enable();
    }
}
