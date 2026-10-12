#include <ft/ft_kinetic_energy.h>
#include <ft/ft_kinetic_energy_controller.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: Kirby's Final Cutter status processes (FinalCutter, Jump, FallLoop, Landing); names are unknown.
class ftKirbyStatusUniqProcessFinalCutter : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessFinalCutter() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftKirbyStatusUniqProcessFinalCutterJump : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessFinalCutterJump() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftKirbyStatusUniqProcessFinalCutterFallLoop : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessFinalCutterFallLoop() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

class ftKirbyStatusUniqProcessFinalCutterLanding : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessFinalCutterLanding() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

void ftKirbyStatusUniqProcessFinalCutter::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    workManage.setInt(moduleAccesser->getSituationModule().getKind(), 0x20000002);
    kinetic.clearSpeedAll();
    kinetic.getEnergy(0)->disable();
    kinetic.getEnergy(1)->disable();
    kinetic.getEnergy(2)->disable();
    kinetic.getEnergy(3)->disable();
    kinetic.getEnergy(2)->disable();
    ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*kinetic.getEnergy(2));
    controller.enable();
    float c = 0.0f; // HYPOTHESIS: constant from lbl_96_rodata_28 (not verified)
    Vec2f speed(c, c);
    Vec3f zero(c, c, c);
    controller.resetEnergy(4, &speed, &zero, moduleAccesser);
    // HYPOTHESIS: the remaining speed setup reads ftCommonDataAccesser params and an owner call; not reconstructed.
}

void ftKirbyStatusUniqProcessFinalCutter::exitStatus(soModuleAccesser* moduleAccesser, int) {
    // HYPOTHESIS: body not yet reconstructed; left as a stub.
}

void ftKirbyStatusUniqProcessFinalCutter::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftKirbyStatusUniqProcessFinalCutterJump::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: body not yet reconstructed; left as a stub.
}

void ftKirbyStatusUniqProcessFinalCutterJump::exitStatus(soModuleAccesser* moduleAccesser, int) {
    // HYPOTHESIS: body not yet reconstructed; left as a stub.
}

void ftKirbyStatusUniqProcessFinalCutterJump::execStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: body not yet reconstructed; left as a stub.
}

void ftKirbyStatusUniqProcessFinalCutterFallLoop::exitStatus(soModuleAccesser* moduleAccesser, int) {
    // HYPOTHESIS: body not yet reconstructed; left as a stub.
}

void ftKirbyStatusUniqProcessFinalCutterLanding::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: body not yet reconstructed; left as a stub.
}

ftKirbyStatusUniqProcessFinalCutter g_ftKirbyStatusUniqProcessFinalCutter;
ftKirbyStatusUniqProcessFinalCutterJump g_ftKirbyStatusUniqProcessFinalCutterJump;
ftKirbyStatusUniqProcessFinalCutterFallLoop g_ftKirbyStatusUniqProcessFinalCutterFallLoop;
ftKirbyStatusUniqProcessFinalCutterLanding g_ftKirbyStatusUniqProcessFinalCutterLanding;
