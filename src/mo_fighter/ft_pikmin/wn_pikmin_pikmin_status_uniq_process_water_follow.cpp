#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

class wnPikminPikminStatusUniqProcessWaterFollow : public soStatusUniqProcess {
public:
    virtual ~wnPikminPikminStatusUniqProcessWaterFollow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

// HYPOTHESIS: the FollowCommon base methods live in the sibling follow TU; called directly by their native symbols.
extern "C" void fn_113_263DC(wnPikminPikminStatusUniqProcessWaterFollow* self, soModuleAccesser* moduleAccesser);
extern "C" void fn_113_26470(wnPikminPikminStatusUniqProcessWaterFollow* self, soModuleAccesser* moduleAccesser);

wnPikminPikminStatusUniqProcessWaterFollow g_wnPikminPikminStatusUniqProcessWaterFollow;

void wnPikminPikminStatusUniqProcessWaterFollow::initStatus(soModuleAccesser* a) {
    fn_113_263DC(this, a);
    a->getWorkManageModule().setInt(0x5a, 0x20000001);
    a->getWorkManageModule().setInt(0x5a, 0x20000000);
    a->getWorkManageModule().onFlag(0x22000000);
}

void wnPikminPikminStatusUniqProcessWaterFollow::execFixPosCounter(soModuleAccesser* a) {
    u32 touched = a->getGroundModule().isTouch(grCollStatus::TouchMask(8), 0);
    if (touched == 1) {
        a->getStatusModule().changeStatusRequest(6, a);
        return;
    }
    fn_113_26470(this, a);
    u32 flagged = a->getWorkManageModule().isFlag(0x22000001);
    if (flagged == 1) {
        a->getWorkManageModule().offFlag(0x22000001);
        a->getStatusModule().changeStatusRequest(0xc, a);
    }
    float speedX = a->getKineticModule().getEnergy(0)->getSpeed().m_x;
    float product = speedX * a->getPostureModule().getLr();
    // HYPOTHESIS: the rodata compare constant is 0.0f.
    if (product < 0.0f) {
        int dir;
        if (speedX < 0.0f) {
            dir = -1;
        } else {
            dir = 1;
        }
        float lr = static_cast<float>(dir);
        a->getPostureModule().setLr(lr);
        a->getPostureModule().updateRotYLr();
    }
}
