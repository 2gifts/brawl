#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

#include <wn/gamewatch/wn_game_watch_octopus_status_uniq_process_common.h>

wnGameWatchOctopusStatusUniqProcessCommon g_wnGameWatchOctopusStatusUniqProcessCommon;

void wnGameWatchOctopusStatusUniqProcessCommon::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000001)) {
        int value = (int)moduleAccesser->getWorkManageModule().getFloat(0x21000000);
        moduleAccesser->getVisibilityModule().setWhole((value >> 1) & 1);
    }
}
