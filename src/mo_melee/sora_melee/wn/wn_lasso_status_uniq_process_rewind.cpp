#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <wn/wn_lasso_status_view.h>
#include <types.h>

class wnLassoStatusUniqProcessRewind : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessRewind() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnLassoStatusUniqProcessRewind g_wnLassoStatusUniqProcessRewind;

void wnLassoStatusUniqProcessRewind::initStatus(soModuleAccesser* moduleAccesser) {
    wnLassoGetAltStatus(moduleAccesser)->unkA4Int(5);
}

void wnLassoStatusUniqProcessRewind::execFixPos(soModuleAccesser* moduleAccesser) {
    int statusKind = wnLassoGetAltStatus(moduleAccesser)->unk88Int();
    if (statusKind > 0) {
        // HYPOTHESIS: the work value is clamped to the status kind before it is passed on.
        int work = moduleAccesser->getWorkManageModule().getInt(0x20000001);
        int value;
        if (work <= 0) {
            value = 1;
        } else {
            value = work;
            if (work > statusKind) {
                value = statusKind;
            }
        }
        wnLassoGetAltStatus(moduleAccesser)->unkB4Int(value);
        if (wnLassoGetAltStatus(moduleAccesser)->unk88Int() == 0) {
            wnLassoGetAltStatus(moduleAccesser)->unkA4Int(6);
            soMotionChangeParam motionParam;
            // HYPOTHESIS: frame and rate constants from lbl_27_rodata_2B20 / lbl_27_rodata_2B24.
            motionParam.m_kind = moduleAccesser->getWorkManageModule().getInt(0x20000000);
            motionParam.m_frame = 0.0f;
            motionParam.m_rate = 1.0f;
            motionParam._12 = 0;
            motionParam._13 = 0;
            motionParam._14 = 0;
            motionParam._15 = 0;
            moduleAccesser->getMotionModule().changeMotionRequest(&motionParam);
            moduleAccesser->getWorkManageModule().onFlag(0x22000000);
        }
    }
}
