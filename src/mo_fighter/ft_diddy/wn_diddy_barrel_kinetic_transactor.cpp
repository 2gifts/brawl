#define WN_KINETIC_GENERIC_BUILDER
#define WN_WEAPON_BUILDER
#include <wn/diddy/wn_diddy_barrel_kinetic_transactor.h>
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <wn/wn_kinetic_transactor.h>

void wnDiddyBarrelKineticTransactor::addSpeed(
    void*, void*, soModuleAccesser*) {
}

void wnDiddyBarrelKineticTransactor::addSpeedOutside(
    int type, void* speedData, void* pools, soModuleAccesser* moduleAccesser) {
    Vec2f* speed = static_cast<Vec2f*>(speedData);
    u8* poolBytes = static_cast<u8*>(pools);
    soParamAccesser* paramAccesser = static_cast<soParamAccesser*>(
        moduleAccesser->getWorkManageModule().getParamAccesser());

    if (type == soKineticEnergy::Outside_Wind) {
        soKineticEnergyWindNormal* wind =
            reinterpret_cast<soKineticEnergyWindNormal*>(poolBytes + 0xEC);
        Vec2f delta(0.0f, 0.0f);
        delta.m_x = speed->m_x;
        delta.m_y += speed->m_y;
        Vec2f brake(0.0f, 0.0f);
        brake.m_x = paramAccesser->getParamFloat(moduleAccesser, 0xFA9, 0);
        if (delta.m_x != 0.0f || delta.m_y != 0.0f) {
            // HYPOTHESIS: native treats these unknown pool words as floats.
            float* unk120 = reinterpret_cast<float*>(poolBytes + 0x120);
            float* unk124 = reinterpret_cast<float*>(poolBytes + 0x124);
            float* unk128 = reinterpret_cast<float*>(poolBytes + 0x128);
            *unk120 += delta.m_x;
            *unk124 += delta.m_y;
            *unk128 = paramAccesser->getParamFloat(moduleAccesser, 0xFA8, 0);
            wind->m_brake = brake;
            wind->enable();
        }
        return;
    }

    if (type != soKineticEnergy::Outside_Attack) return;

    Vec2f speedToAdd(0.0f, 0.0f);
    int kineticType = moduleAccesser->getKineticModule().getKineticType();
    switch (kineticType) {
    case 0:
        speedToAdd.m_x = speed->m_x;
        break;
    case 2:
    case 3:
        speedToAdd = *speed;
        break;
    }

    if (speedToAdd.m_x != 0.0f) {
        soKineticEnergyNormal* normal =
            reinterpret_cast<soKineticEnergyNormal*>(poolBytes + 0x10);
        Vec2f current = normal->getSpeed();
        current.m_x += speedToAdd.m_x;
        normal->m_speed = current;
        normal->enable();
    }

    if (speedToAdd.m_y != 0.0f) {
        wnKineticEnergyGravity* gravity =
            reinterpret_cast<wnKineticEnergyGravity*>(poolBytes + 0x50);
        Vec2f current = gravity->getSpeed();
        current.m_y += speedToAdd.m_y;
        gravity->m_speedY = current.m_y;
        gravity->enable();
    }
}

void wnDiddyBarrelKineticTransactor::changeKinetic(
    int mode, void* pools, soModuleAccesser* moduleAccesser) {
    if (mode <= 0x1F) {
        wnKineticTransactor::changeKinetic(mode, pools, moduleAccesser);
        return;
    }

    switch (mode) {
    case 0x20: {
        volatile u8 unk9 = 0; // MATCH-ONLY: native initializes this unused local byte.
        moduleAccesser->getKineticModule().unableEnergyAll();
        break;
    }
    case 0x21: {
        wnDiddyBarrelKineticTransactor empty = {};
        empty.changeKineticSub(pools, moduleAccesser);
        break;
    }
    }
}

void wnDiddyBarrelKineticTransactor::changeKineticSub(
    void* pools, soModuleAccesser* moduleAccesser) {
    u8* poolBytes = static_cast<u8*>(pools);
    soKineticEnergyNormal* normal =
        reinterpret_cast<soKineticEnergyNormal*>(poolBytes + 0x10);
    wnKineticEnergyGravity* gravity =
        reinterpret_cast<wnKineticEnergyGravity*>(poolBytes + 0x50);

    moduleAccesser->getKineticModule().unableEnergyAll();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soParamAccesser* paramAccesser = static_cast<soParamAccesser*>(
        work.getParamAccesser());
    Vec2f initialSpeed(work.getFloat(0x21000004), 0.0f);
    normal->m_speed = initialSpeed;
    normal->enable();
    gravity->m_speedY = work.getFloat(0x21000005);
    gravity->m_gravity = -paramAccesser->getParamFloat(moduleAccesser, 0xFA4, 0);
    gravity->m_speedLimit = paramAccesser->getParamFloat(moduleAccesser, 0xFA5, 0);
    gravity->enable();
}
