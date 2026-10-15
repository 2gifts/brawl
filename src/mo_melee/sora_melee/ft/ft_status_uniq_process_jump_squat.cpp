#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/motion/so_motion_change_param.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessJumpSquat : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessJumpSquat() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftStatusUniqProcessJumpSquat g_ftStatusUniqProcessJumpSquat;

// HYPOTHESIS: the change frame is the float at lbl_27_rodata_20A8+8; its value is not verified (0.5f here).
// The wrap-around threshold (0.0f) and step (1.0f) come from the same literal pool.
void ftStatusUniqProcessJumpSquat::initStatus(soModuleAccesser* moduleAccesser) {
    float frames = (float)soValueAccesser::getConstantInt(moduleAccesser, 0x59da, 0);
    float rate = moduleAccesser->getMotionModule().getEndFrame(0x14) / frames;
    if (rate < 0.0f) {
        rate += 1.0f;
    }
    soMotionChangeParam param;
    param.m_kind = 0x14;
    param.m_frame = 0.5f;
    param.m_rate = rate;
    param._12 = 0;
    param._13 = 0;
    param._14 = 0;
    param._15 = 0;
    moduleAccesser->getMotionModule().changeMotionRequest(&param);
}

void ftStatusUniqProcessJumpSquat::execStatus(soModuleAccesser*) {}

void ftStatusUniqProcessJumpSquat::exitStatus(soModuleAccesser*, int) {}
