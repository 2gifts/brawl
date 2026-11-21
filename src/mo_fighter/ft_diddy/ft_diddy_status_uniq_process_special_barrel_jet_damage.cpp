#include <ft/ft_kinetic_energy.h>
#include <ft/ft_kinetic_energy_controller.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftDiddyStatusUniqProcessSpecialBarrelJetDamage : public soStatusUniqProcess {
public:
    virtual ~ftDiddyStatusUniqProcessSpecialBarrelJetDamage() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDiddyStatusUniqProcessSpecialBarrelJetDamage::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().onFlag(0x1200003d);
    ftKineticEnergyDisableAndClear(3, moduleAccesser);
    ftKineticEnergyDisableAndClear(1, moduleAccesser);
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    ftKineticEnergyDamage& damage = dynamic_cast<ftKineticEnergyDamage&>(*moduleAccesser->getKineticModule().getEnergy(4));

    Vec2f sumSpeed;
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x128:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
            sumSpeed.m_x = moduleAccesser->getWorkManageModule().getFloat(0x21000009);
            sumSpeed.m_y = moduleAccesser->getWorkManageModule().getFloat(0x2100000a);
        } else {
            sumSpeed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
        }
        break;
    case 0x126:
        sumSpeed.m_x = moduleAccesser->getWorkManageModule().getFloat(0x21000009);
        sumSpeed.m_y = moduleAccesser->getWorkManageModule().getFloat(0x2100000a);
        break;
    case 0x127:
        break;
    }

    damage.resetEnergy(2, Vec2f(sumSpeed.m_x, sumSpeed.m_y), Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    damage.enable();

    gravity.resetEnergy(0, Vec2f(0.0f, 0.0f), Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    gravity.enable();
}

// Case 0x127 returns: an empty `break` case is dropped before the compare tree is built, and the original tree still
// has 0x127 as its root (`cmpwi 0x127; beq end; bge +; b end`).
void ftDiddyStatusUniqProcessSpecialBarrelJetDamage::execStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x128:
        if (moduleAccesser->getWorkManageModule().getFloat(0x2100000b) > 0.0f) {
            moduleAccesser->getWorkManageModule().subFloat(1.0f, 0x2100000b);
            if (moduleAccesser->getWorkManageModule().getFloat(0x2100000b) <= 0.0f) {
                ftKineticEnergyController& controller =
                    dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
                ftKineticEnergyController::ResetType resetType = ftKineticEnergyController::ResetType_0;
                Vec2f sumSpeed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
                controller.resetEnergy(resetType, Vec2f(sumSpeed.m_x, 0.0f), Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                controller.enable();
                ftKineticEnergyDisableAndClear(4, moduleAccesser);
            }
        }
        break;
    // HYPOTHESIS: nothing to do per frame in the status kinds 0x126 (handled in initStatus) and 0x127.
    case 0x126:
        break;
    case 0x127:
        return;
    }
}

void ftDiddyStatusUniqProcessSpecialBarrelJetDamage::execStop(soModuleAccesser* moduleAccesser) { }
void ftDiddyStatusUniqProcessSpecialBarrelJetDamage::exitStatus(soModuleAccesser* moduleAccesser, int) { }

ftDiddyStatusUniqProcessSpecialBarrelJetDamage g_ftDiddyStatusUniqProcessSpecialBarrelJetDamage;
