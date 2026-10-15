#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Landing from the air. HYPOTHESIS: status 0x16 is the normal landing, 0x17 the light landing, 0x18 the heavy/aerial
// landing (its motion and landing lag are picked by work int 0x20000000) and 0x19 the special landing. The landing
// motion is played at a rate that makes its length match the landing lag stored in work float 0x11000000.
class ftStatusUniqProcessLanding : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessLanding() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessLanding g_ftStatusUniqProcessLanding;

void ftStatusUniqProcessLanding::initStatus(soModuleAccesser* moduleAccesser) {
    int statusKind = moduleAccesser->getStatusModule().getStatusKind();
    if (statusKind == 0x16) {
        float rate = 1.0f;
        float lag = moduleAccesser->getWorkManageModule().getFloat(0x11000000);
        if (0.0f != lag) {
            rate = moduleAccesser->getMotionModule().getEndFrame(0x32) / lag;
        }
        soMotionChangeParam param;
        param.m_kind = 0x32;
        param.m_frame = 0.0f;
        param.m_rate = rate;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
    } else if (statusKind == 0x17) {
    } else if (statusKind == 0x18) {
        int landingKind = moduleAccesser->getWorkManageModule().getInt(0x20000000);
        float rate = 1.0f;
        int motionKind = landingKind + 5;
        float lag = soValueAccesser::getConstantFloat(moduleAccesser, 0xBA1 + landingKind, 0);
        if (0.0f != lag) {
            rate = moduleAccesser->getMotionModule().getEndFrame(motionKind) /
                   (lag + lag * soValueAccesser::getConstantFloat(moduleAccesser, 0xCA0, 0));
        }
        soMotionChangeParam param;
        param.m_kind = motionKind;
        param.m_frame = 0.0f;
        param.m_rate = rate;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
    } else if (statusKind == 0x19) {
        float rate = 1.0f;
        float lag = moduleAccesser->getWorkManageModule().getFloat(0x11000000);
        if (0.0f != lag) {
            rate = moduleAccesser->getMotionModule().getEndFrame(0x33) / lag;
        }
        soMotionChangeParam param;
        param.m_kind = 0x33;
        param.m_frame = 0.0f;
        param.m_rate = rate;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
    }
}

// Work flag 0x22000010 is raised once the landing lag has run out (the fighter may act again).
void ftStatusUniqProcessLanding::execStatus(soModuleAccesser* moduleAccesser) {
    if (!moduleAccesser->getWorkManageModule().isFlag(0x12000003)) {
        int statusKind = moduleAccesser->getStatusModule().getStatusKind();
        if (statusKind == 0x16) {
            float limit = soValueAccesser::getConstantFloat(moduleAccesser, 0xC02, 0);
            if (-1.0f != limit) {
                if (limit <= moduleAccesser->getMotionModule().getFrame()) {
                    moduleAccesser->getWorkManageModule().onFlag(0x22000010);
                }
            }
        } else if (statusKind == 0x17) {
            float limit = soValueAccesser::getConstantFloat(moduleAccesser, 0xC01, 0);
            if (-1.0f != limit) {
                if (limit <= moduleAccesser->getMotionModule().getFrame()) {
                    moduleAccesser->getWorkManageModule().onFlag(0x22000010);
                }
            }
        } else if (statusKind == 0x18) {
            int landingKind = moduleAccesser->getMotionModule().getKind();
            int frames = moduleAccesser->getWorkManageModule().getInt(0x20000002);
            moduleAccesser->getWorkManageModule().setInt(frames + 1, 0x20000002);
            if ((int) soValueAccesser::getConstantFloat(moduleAccesser, 0xB9C + landingKind, 0) <= frames + 1) {
                moduleAccesser->getWorkManageModule().onFlag(0x22000010);
            }
        }
    }
}

void ftStatusUniqProcessLanding::exitStatus(soModuleAccesser* moduleAccesser, int) {
    moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x11000000);
    moduleAccesser->getWorkManageModule().offFlag(0x12000003);
}
