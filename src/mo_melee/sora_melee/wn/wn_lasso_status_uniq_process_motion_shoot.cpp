#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <wn/wn_lasso_status_view.h>
#include <types.h>

class wnLassoStatusUniqProcessMotionShoot : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessMotionShoot() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnLassoStatusUniqProcessMotionShoot g_wnLassoStatusUniqProcessMotionShoot;

void wnLassoStatusUniqProcessMotionShoot::initStatus(soModuleAccesser* moduleAccesser) {
    wnLassoGetAltStatus(moduleAccesser)->unkBCInt(5);
    moduleAccesser->getWorkManageModule().onFlag(0x12000003);
}

void wnLassoStatusUniqProcessMotionShoot::execFixPos(soModuleAccesser* moduleAccesser) {
    if (wnLassoGetAltStatus(moduleAccesser)->unkA8Int() == 2) {
        moduleAccesser->getStatusModule().changeStatusForce(2, moduleAccesser);
        moduleAccesser->getMotionModule().setRate(0.0f);
    }
}
