#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/camera/so_camera_module_impl.h>
#include <so/so_kinetic_utility.h>
#include <ft/ft_kinetic_energy.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessWalkBrake : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessWalkBrake() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftStatusUniqProcessWalkBrake g_ftStatusUniqProcessWalkBrake;

// HYPOTHESIS: work flag 0x22000011 is the brake "rate applied" flag; its name is unknown.
enum { WalkBrake_Flag_RateSet = 0x22000011 };

// HYPOTHESIS: the rate constants come from lbl_27_rodata_2028 (threshold, rate on the first turn, rate later); values not verified.
void ftStatusUniqProcessWalkBrake::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getCameraModule().setRun(0);
}

void ftStatusUniqProcessWalkBrake::execStatus(soModuleAccesser* moduleAccesser) {
    float threshold = 0.0f; // HYPOTHESIS: pool constant lbl_27_rodata_2028+0
    float speedX = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)).m_x;
    if (!moduleAccesser->getWorkManageModule().isFlag(WalkBrake_Flag_RateSet)) {
        if ((float)__fabs(speedX) > threshold) {
            moduleAccesser->getMotionModule().setRate(1.0f);
            moduleAccesser->getWorkManageModule().onFlag(WalkBrake_Flag_RateSet);
        }
    } else {
        if ((float)__fabs(speedX) <= threshold) {
            moduleAccesser->getMotionModule().setRate(0.5f);
        }
    }
}

void ftStatusUniqProcessWalkBrake::exitStatus(soModuleAccesser* moduleAccesser, int) {
    moduleAccesser->getCameraModule().exitRun(0);
}
