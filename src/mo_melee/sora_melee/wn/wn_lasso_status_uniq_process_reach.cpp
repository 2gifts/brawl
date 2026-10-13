#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <wn/wn_lasso_status_view.h>
#include <types.h>

class wnLassoStatusUniqProcessReach : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessReach() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnLassoStatusUniqProcessReach g_wnLassoStatusUniqProcessReach;

void wnLassoStatusUniqProcessReach::initStatus(soModuleAccesser* moduleAccesser) { }

void wnLassoStatusUniqProcessReach::exitStatus(soModuleAccesser* moduleAccesser, int kind) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000005)) {
        if (moduleAccesser->getWorkManageModule().getInt(0x20000001) == kind) {
            wnLassoGetLink(moduleAccesser)->unk164Int(1);
            // TODO: the stage object cast and the Weapon constraint call (Weapon::constraint) are not reconstructed yet.
        }
    }
}

void wnLassoStatusUniqProcessReach::execStatus(soModuleAccesser* moduleAccesser) {
    if (wnLassoGetAltStatus(moduleAccesser)->unkD4Int() != 0) {
        moduleAccesser->getWorkManageModule().onFlag(0x22000000);
    }
}

void wnLassoStatusUniqProcessReach::execFixPos(soModuleAccesser* moduleAccesser) { }
