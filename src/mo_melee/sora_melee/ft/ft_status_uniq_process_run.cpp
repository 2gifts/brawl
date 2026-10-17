#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_util.h>
#include <math.h>
#include <so/camera/so_camera_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftStatusUniqProcessRun : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessRun() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual bool onChangeLr(soModuleAccesser* moduleAccesser, float, float);
};

ftStatusUniqProcessRun g_ftStatusUniqProcessRun;

// Run helpers shared with the walk status (ftStatusUniqProcessWalkUtil); declared by their module-map names.
// HYPOTHESIS: argument meanings and the work ids 0x21000004 (brake counter), 0x21000005 (run speed) and 0x21000007 (run rate).
extern "C" {
void fn_27_1652A8(soModuleAccesser* moduleAccesser, int kind, int workA, int workB, int flag, float a, float b);
void fn_27_165458(soModuleAccesser* moduleAccesser, int work, float f1, float f2, float f3, float f4, float f5, float f6);
}

// HYPOTHESIS: work flag 0x1200000A marks a dash-dance style run entry (the run starts from the dash instead of a standing start).
void ftStatusUniqProcessRun::initStatus(soModuleAccesser* moduleAccesser) {
    float constSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xBC0, 0);
    if (moduleAccesser->getWorkManageModule().isFlag(0x1200000A) == true) {
        moduleAccesser->getKineticModule().changeKinetic(0x45, moduleAccesser);
    } else {
        moduleAccesser->getKineticModule().changeKinetic(0xE, moduleAccesser);
    }
    fn_27_1652A8(moduleAccesser, 0xF, 0x21000005, 0x21000007, 0, constSpeed, ftUtil::getRunSpeedMul(moduleAccesser));
    if (moduleAccesser->getWorkManageModule().isFlag(0x1200000A) == true) {
        if ((float) __fabs(moduleAccesser->getWorkManageModule().getFloat(0x21000005)) < 1.0f) {
            float entrySpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xD8A, 0);
            moduleAccesser->getWorkManageModule().setFloat(entrySpeed * reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).getLr(), 0x21000005);
        }
    }
    float speed = moduleAccesser->getWorkManageModule().getFloat(0x21000005);
    float rate = moduleAccesser->getWorkManageModule().getFloat(0x21000007);
    float motionRate;
    if (moduleAccesser->getWorkManageModule().isFlag(0x1200000A) == true) {
        float minRate = soValueAccesser::getConstantFloat(moduleAccesser, 0xD8A, 0);
        motionRate = rate * minRate;
        if (motionRate < minRate) {
            motionRate = minRate;
        }
    } else {
        motionRate = rate * (float) __fabs(speed);
    }
    float maxRate = soValueAccesser::getConstantFloat(moduleAccesser, 0xC3C, 0);
    if (motionRate > maxRate) {
        motionRate = maxRate;
    }
    moduleAccesser->getMotionModule().setRate(motionRate);
    moduleAccesser->getCameraModule().setRun(0);
}

void ftStatusUniqProcessRun::execStatus(soModuleAccesser* moduleAccesser) {
    float current = moduleAccesser->getWorkManageModule().getFloat(0x21000005);
    fn_27_165458(moduleAccesser, 0x21000005, current, soValueAccesser::getConstantFloat(moduleAccesser, 0xBC0, 0),
                 soValueAccesser::getConstantFloat(moduleAccesser, 0xBBE, 0), soValueAccesser::getConstantFloat(moduleAccesser, 0xBBF, 0),
                 soValueAccesser::getConstantFloat(moduleAccesser, 0xC37, 0), ftUtil::getRunSpeedMul(moduleAccesser));
    float speed = moduleAccesser->getWorkManageModule().getFloat(0x21000005);
    float rate = moduleAccesser->getWorkManageModule().getFloat(0x21000007);
    float motionRate = 0.0f;
    if (moduleAccesser->getWorkManageModule().isFlag(0x1200000A) == true) {
        float minRate = soValueAccesser::getConstantFloat(moduleAccesser, 0xD8A, 0);
        motionRate = rate * minRate;
        if (motionRate < minRate) {
            motionRate = minRate;
        }
    } else {
        motionRate = rate * (float) __fabs(speed);
    }
    float maxRate = soValueAccesser::getConstantFloat(moduleAccesser, 0xC3C, 0);
    if (motionRate > maxRate) {
        motionRate = maxRate;
    }
    moduleAccesser->getMotionModule().setRate(motionRate);
    if (moduleAccesser->getWorkManageModule().getFloat(0x21000004) > 0.0f) {
        moduleAccesser->getWorkManageModule().subFloat(1.0f, 0x21000004);
    }
}

bool ftStatusUniqProcessRun::onChangeLr(soModuleAccesser* moduleAccesser, float, float) {
    float speed = moduleAccesser->getWorkManageModule().getFloat(0x21000005);
    moduleAccesser->getWorkManageModule().setFloat(-speed, 0x21000005);
    return false;
}

void ftStatusUniqProcessRun::exitStatus(soModuleAccesser* moduleAccesser, int) {
    moduleAccesser->getCameraModule().exitRun(0);
}
