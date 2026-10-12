#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

// Base and helper functions of the Pikmin follow family (ft_pikmin map: FollowCommon, wnPikminPikminUtility), called by
// their REL symbol names until a shared declaration exists.
extern "C" {
void fn_113_263DC(void* self, soModuleAccesser* moduleAccesser);
void fn_113_26470(void* self, soModuleAccesser* moduleAccesser);
float fn_113_23CA0(soModuleAccesser* moduleAccesser, int index);
u32 fn_113_23B54(int flag);
void fn_113_23B84(soModuleAccesser* moduleAccesser);
}

class wnPikminPikminStatusUniqProcessAirFollow : public soStatusUniqProcess {
public:
    virtual ~wnPikminPikminStatusUniqProcessAirFollow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnPikminPikminStatusUniqProcessAirFollow g_wnPikminPikminStatusUniqProcessAirFollow;

void wnPikminPikminStatusUniqProcessAirFollow::initStatus(soModuleAccesser* a) {
    fn_113_263DC(this, a);
    a->getWorkManageModule().setInt(90, 0x20000001);
    a->getWorkManageModule().setInt(90, 0x20000000);
    a->getWorkManageModule().setFloat(10.0f, 0x21000001);
    a->getWorkManageModule().onFlag(0x22000000);
}

void wnPikminPikminStatusUniqProcessAirFollow::execStatus(soModuleAccesser* a) {
    if (a->getWorkManageModule().getInt(0x10000011) & 0x20) {
        a->getGroundModule().setPassableCheck(false, 0);
    } else {
        a->getGroundModule().setPassableCheck(true, 0);
    }
}

void wnPikminPikminStatusUniqProcessAirFollow::execFixPosCounter(soModuleAccesser* a) {
    fn_113_26470(this, a);
    if (a->getWorkManageModule().isFlag(0x22000001) == 1) {
        a->getWorkManageModule().offFlag(0x22000001);
        a->getStatusModule().changeStatusRequest(10, a);
        return;
    }
    float threshold = soValueAccesser::getConstantFloat(a, 0xfa4, 0);
    if (fn_113_23CA0(a, 3) >= threshold) {
        return;
    }
    if (fn_113_23B54(a->getWorkManageModule().getInt(0x10000010)) == 1) {
        a->getStatusModule().changeStatusRequest(10, a);
    }
}
