#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_gimmick.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/stop/so_stop_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <types.h>

void ftStatusUniqProcessGimmickBarrel::exitStatus(soModuleAccesser* moduleAccesser, int next) {
    ftStatusUniqProcessGimmick::exitStatus(moduleAccesser, next);
    moduleAccesser->getVisibilityModule().setWhole(true);
}

void ftStatusUniqProcessGimmickBarrel::execStop(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().getInt(0x20000000);
}

void ftStatusUniqProcessGimmickBarrel::execFixPos(soModuleAccesser* moduleAccesser) {
    bool breakA;
    bool breakB;
    bool breakC;
    bool breakD;
    moduleAccesser->getGroundModule().getAttackPointBreak(&breakA, &breakB, &breakC, &breakD, 0);
    if (breakA == true || breakB == true || breakC == true || breakD == true) {
        int hitStopFrame = moduleAccesser->getWorkManageModule().getInt(0x20000002);
        moduleAccesser->getStopModule().setHitStopFrame(hitStopFrame, 0);
    }
}
