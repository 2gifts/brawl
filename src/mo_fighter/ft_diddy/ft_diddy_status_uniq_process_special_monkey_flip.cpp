#define MT_VEC3F_CTOR_NOINLINE // BYTES: the original calls the out-of-line Vec3f constructor.

class soModuleAccesser;
#include <so/status/so_status_uniq_process.h>

// The class is defined ahead of the module headers: class definition order decides the order of the RTTI the unit emits.
class ftDiddyStatusUniqProcessSpecialMonkeyFlip : public soStatusUniqProcess {
public:
    ftDiddyStatusUniqProcessSpecialMonkeyFlip() { }
    virtual ~ftDiddyStatusUniqProcessSpecialMonkeyFlip() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

void ftDiddyStatusUniqProcessSpecialMonkeyFlip::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x11b: {
        float airSpeedY = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa9, 0);
        float airSpeedX = soValueAccesser::getConstantFloat(moduleAccesser, 0xfaa, 0);
        float lr = moduleAccesser->getPostureModule().getLr();
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
            airSpeedY = soValueAccesser::getConstantFloat(moduleAccesser, 0xfab, 0);
        }
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        Vec2f gravitySpeed(0.0f, airSpeedX);
        gravity.resetEnergy(0, gravitySpeed, Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.enable();

        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
        Vec2f stopSpeed(airSpeedY * lr, 0.0f);
        stop.resetEnergy(6, stopSpeed, Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop.enable();
    }
    }
}

void ftDiddyStatusUniqProcessSpecialMonkeyFlip::execStatus(soModuleAccesser* moduleAccesser) { }
void ftDiddyStatusUniqProcessSpecialMonkeyFlip::execStop(soModuleAccesser* moduleAccesser) { }
void ftDiddyStatusUniqProcessSpecialMonkeyFlip::exitStatus(soModuleAccesser* moduleAccesser, int) { }

ftDiddyStatusUniqProcessSpecialMonkeyFlip g_ftDiddyStatusUniqProcessSpecialMonkeyFlip;
