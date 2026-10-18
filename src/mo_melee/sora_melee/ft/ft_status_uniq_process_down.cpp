#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_down.h>
#include <ft/ft_util.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

void ftStatusUniqProcessDown::initStatus(soModuleAccesser* moduleAccesser) {
    int kind;
    if (ftUtil::isBodyUp(moduleAccesser) == 1) {
        kind = 0xaa;
        moduleAccesser->getWorkManageModule().onFlag(0x22000019);
    } else {
        kind = 0xb3;
        moduleAccesser->getWorkManageModule().offFlag(0x22000019);
    }
    moduleAccesser->getWorkManageModule().setInt(kind, 0x20000008);
}

void ftStatusUniqProcessDown::execStatus(soModuleAccesser*) {}
void ftStatusUniqProcessDown::exitStatus(soModuleAccesser*, int) {}

ftStatusUniqProcessDown g_ftStatusUniqProcessDown;
