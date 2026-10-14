#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>
#include <wn/pikmin/wn_pikmin_pikmin_utility.h>

class wnPikminPikminStatusUniqProcessJump : public soStatusUniqProcess {
public:
    virtual ~wnPikminPikminStatusUniqProcessJump() { }
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnPikminPikminStatusUniqProcessJump g_wnPikminPikminStatusUniqProcessJump;

void wnPikminPikminStatusUniqProcessJump::execFixPos(soModuleAccesser* a) {
    float threshold = soValueAccesser::getConstantFloat(a, 0xfa4, 0);
    float dist = wnPikminPikminUtility::getTargetDist(a, 3);
    if (dist >= threshold) {
        if (wnPikminPikminUtility::isOwnerConditionGround(a->getWorkManageModule().getInt(0x1000000f)) == 1) {
            wnPikminPikminUtility::tryAttachGround(a);
        } else {
            a->getStatusModule().changeStatusRequest(8, a);
        }
    }
}
