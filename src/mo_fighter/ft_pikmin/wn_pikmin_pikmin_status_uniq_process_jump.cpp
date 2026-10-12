#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

// wnPikminPikminUtility helpers (ft_pikmin map: getTargetDist, isOwnerConditionGround, tryAttachGround); no shared
// declaration exists yet, so they are called by their REL symbol names.
extern "C" {
float fn_113_23CA0(soModuleAccesser* moduleAccesser, int index);
u32 fn_113_23B54(int flag);
void fn_113_23B84(soModuleAccesser* moduleAccesser);
}

class wnPikminPikminStatusUniqProcessJump : public soStatusUniqProcess {
public:
    virtual ~wnPikminPikminStatusUniqProcessJump() { }
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnPikminPikminStatusUniqProcessJump g_wnPikminPikminStatusUniqProcessJump;

void wnPikminPikminStatusUniqProcessJump::execFixPos(soModuleAccesser* a) {
    float threshold = soValueAccesser::getConstantFloat(a, 0xfa4, 0);
    float dist = fn_113_23CA0(a, 3);
    if (dist >= threshold) {
        if (fn_113_23B54(a->getWorkManageModule().getInt(0x1000000f)) == 1) {
            fn_113_23B84(a);
        } else {
            a->getStatusModule().changeStatusRequest(8, a);
        }
    }
}
