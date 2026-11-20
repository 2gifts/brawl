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

    damage.resetEnergy(2, &Vec2f(sumSpeed.m_x, sumSpeed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    damage.enable();

    gravity.resetEnergy(0, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    gravity.enable();
}

// TODO(W7): 98%. Only the head differs: the target tests `cmpwi 0x127; beq; bge; b end; cmpwi 0x129; bge end` (a
// compare tree whose low side has no case), we fold it into one `cmpwi 0x128`. Tried: if/else cascades, `default`,
// `return` vs `break` cases, an extra case 0x126 or 0x129. The remaining stack slots differ too: the target keeps the
// getSumSpeed result in a hidden slot plus a named copy and the Vec2f below them (we elide the copy).
// MATCH-ONLY: copy propagation off plus the `zero` local keep the int 0 (resetEnergy mode) in a callee-saved register
// next to the shared 1 (flag), as the target does; the target's source for that 0 is not known.
void ftDiddyStatusUniqProcessSpecialBarrelJetDamage::execStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x126: // HYPOTHESIS: the status kinds 0x126 and 0x127 do nothing per frame (0x126 is handled in initStatus)
    case 0x127:
        break;
    case 0x128:
        if (moduleAccesser->getWorkManageModule().getFloat(0x2100000b) > 0.0f) {
            moduleAccesser->getWorkManageModule().subFloat(1.0f, 0x2100000b);
            if (moduleAccesser->getWorkManageModule().getFloat(0x2100000b) <= 0.0f) {
                ftKineticEnergyController& controller =
                    dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
                ftKineticEnergyController::ResetType zero = ftKineticEnergyController::ResetType_0;
                Vec2f sumSpeed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
                Vec2f speed(sumSpeed.m_x, 0.0f);
                controller.resetEnergy(zero, &speed, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                controller.enable();
                ftKineticEnergyDisableAndClear(4, moduleAccesser);
            }
        }
        break;
    }
}

void ftDiddyStatusUniqProcessSpecialBarrelJetDamage::execStop(soModuleAccesser* moduleAccesser) { }
void ftDiddyStatusUniqProcessSpecialBarrelJetDamage::exitStatus(soModuleAccesser* moduleAccesser, int) { }

ftDiddyStatusUniqProcessSpecialBarrelJetDamage g_ftDiddyStatusUniqProcessSpecialBarrelJetDamage;
