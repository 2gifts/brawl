#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: Kirby's Hammer (down-B charge swing) status process; class and constant names are unknown.
class ftKirbyStatusUniqProcessHammer : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessHammer() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftKirbyStatusUniqProcessHammer::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getSituationModule().getKind() != 2) {
        return;
    }
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(2));
    soKineticEnergyNormal& motion = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(3));
    kinetic.changeKinetic(0, moduleAccesser);
    float speedY = soValueAccesser::getConstantFloat(moduleAccesser, 0xfac, 0);
    gravity.enable();
    Vec2f speed(0.0f, speedY);
    gravity.resetEnergy(0, &speed, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    if (soValueAccesser::getConstantInt(moduleAccesser, 0x5dc2, 0) == 1) {
        Vec2f speed2 = stop.getSpeed();
        stop.enable();
        motion.enable();
        motion.resetEnergy(9, &speed2, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    }
}

void ftKirbyStatusUniqProcessHammer::exitStatus(soModuleAccesser* moduleAccesser, int) {
    // HYPOTHESIS: exit path not yet reconstructed; left as a stub.
}

void ftKirbyStatusUniqProcessHammer::execStatus(soModuleAccesser* moduleAccesser) {
}

ftKirbyStatusUniqProcessHammer g_ftKirbyStatusUniqProcessHammer;
