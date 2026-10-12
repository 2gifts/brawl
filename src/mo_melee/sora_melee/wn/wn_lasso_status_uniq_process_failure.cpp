#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <wn/wn_lasso_status_view.h>
#include <types.h>

class wnLassoStatusUniqProcessFailure : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessFailure() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnLassoStatusUniqProcessFailure g_wnLassoStatusUniqProcessFailure;

void wnLassoStatusUniqProcessFailure::initStatus(soModuleAccesser* moduleAccesser) {
    wnLassoGetAltStatus(moduleAccesser)->unkA4Int(5);
}

void wnLassoStatusUniqProcessFailure::execFixPos(soModuleAccesser* moduleAccesser) {
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
    } else {
        moduleAccesser->getWorkManageModule().onFlag(0x22000000);
    }
}
