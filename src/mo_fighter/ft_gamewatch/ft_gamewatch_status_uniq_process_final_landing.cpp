#include <so/motion/so_motion_change_param.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftGameWatchStatusUniqProcessFinalLanding : public soStatusUniqProcess {
public:
    virtual ~ftGameWatchStatusUniqProcessFinalLanding() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftGameWatchStatusUniqProcessFinalLanding g_ftGameWatchStatusUniqProcessFinalLanding;

static const float s_landingDefaultRate = 1.0f;
static const float s_landingFrame = 0.0f;
static const float s_landingThreshold = -1.0f;

void ftGameWatchStatusUniqProcessFinalLanding::initStatus(soModuleAccesser* moduleAccesser) {
    soMotionModule& motion = moduleAccesser->getMotionModule();
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x12B) {
        float speed = moduleAccesser->getWorkManageModule().getFloat(0x11000000);
        float rate = s_landingDefaultRate;
        if (s_landingFrame != speed) {
            rate = motion.getEndFrame(0x201) / speed;
        }
        soMotionChangeParam param;
        param.m_kind = 0x201;
        param.m_frame = s_landingFrame;
        param.m_rate = rate;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        motion.changeMotionRequest(&param);
    }
}

void ftGameWatchStatusUniqProcessFinalLanding::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000003)) {
        return;
    }
    soMotionModule& motion = moduleAccesser->getMotionModule();
    int kind = moduleAccesser->getStatusModule().getStatusKind();
    float threshold = s_landingThreshold;
    if (kind == 0x12B) {
        threshold = soValueAccesser::getConstantFloat(moduleAccesser, 0xC02, 0);
    } else if (kind == 0x12C) {
        threshold = soValueAccesser::getConstantFloat(moduleAccesser, 0xC01, 0);
    }
    if (s_landingThreshold != threshold) {
        if (motion.getFrame() >= threshold) {
            moduleAccesser->getWorkManageModule().onFlag(0x22000010);
        }
    }
}

void ftGameWatchStatusUniqProcessFinalLanding::exitStatus(soModuleAccesser* moduleAccesser, int) {
    moduleAccesser->getWorkManageModule().setFloat(s_landingFrame, 0x11000000);
    moduleAccesser->getWorkManageModule().offFlag(0x12000003);
}
