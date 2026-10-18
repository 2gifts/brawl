#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_kinetic_energy.h>
#include <so/camera/so_camera_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessRunBrake : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessRunBrake() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftStatusUniqProcessRunBrake g_ftStatusUniqProcessRunBrake;

// HYPOTHESIS: work ids. 0x21000006 is a brake speed float, 0x22000010/11/12/13 are brake flags,
// 0x20000000 is an int counter; names are not verified.
enum {
    RunBrake_Float_Speed = 0x21000006,
    RunBrake_Flag_Brake = 0x22000010,
    RunBrake_Flag_RateSet = 0x22000011,
    RunBrake_Flag_Init = 0x22000012,
    RunBrake_Flag_Stop = 0x22000013,
    RunBrake_Int_Count = 0x20000000,
};

// HYPOTHESIS: constants from lbl_27_rodata_2050 / lbl_27_rodata_2054 (values not verified).
static const float s_runBrakeLimit = 0.0f;
static const float s_runBrakeRate = 1.0f;

void ftStatusUniqProcessRunBrake::initStatus(soModuleAccesser* moduleAccesser) {
    float speed = soValueAccesser::getConstantFloat(moduleAccesser, 0xbc1, 0);
    moduleAccesser->getWorkManageModule().setFloat(speed, RunBrake_Float_Speed);
    moduleAccesser->getWorkManageModule().onFlag(RunBrake_Flag_Init);
    moduleAccesser->getCameraModule().setRun(0);
}

void ftStatusUniqProcessRunBrake::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(RunBrake_Flag_Brake) == true) {
        float threshold = soValueAccesser::getConstantFloat(moduleAccesser, 0xc39, 0);
        float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xbc0, 0);
        threshold = scale * threshold;
        float speedX = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)).m_x;
        if (!moduleAccesser->getWorkManageModule().isFlag(RunBrake_Flag_RateSet)) {
            if ((float)__fabs(speedX) > threshold) {
                moduleAccesser->getMotionModule().setRate(s_runBrakeLimit);
                moduleAccesser->getWorkManageModule().onFlag(RunBrake_Flag_RateSet);
            }
        } else {
            if ((float)__fabs(speedX) <= threshold) {
                moduleAccesser->getMotionModule().setRate(s_runBrakeRate);
                moduleAccesser->getWorkManageModule().offFlag(RunBrake_Flag_Brake);
            }
        }
    }
    if (!moduleAccesser->getWorkManageModule().isFlag(RunBrake_Flag_Stop)) {
        int count = moduleAccesser->getWorkManageModule().getInt(RunBrake_Int_Count);
        if (count > soValueAccesser::getConstantInt(moduleAccesser, 0x59fa, 0)) {
            moduleAccesser->getWorkManageModule().onFlag(RunBrake_Flag_Stop);
        }
    }
    if (moduleAccesser->getWorkManageModule().getFloat(RunBrake_Float_Speed) > s_runBrakeLimit) {
        moduleAccesser->getWorkManageModule().subFloat(s_runBrakeRate, RunBrake_Float_Speed);
    }
}

void ftStatusUniqProcessRunBrake::exitStatus(soModuleAccesser* moduleAccesser, int) {
    moduleAccesser->getCameraModule().exitRun(0);
}
