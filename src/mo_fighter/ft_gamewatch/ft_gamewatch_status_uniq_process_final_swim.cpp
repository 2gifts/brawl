#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/gamewatch/ft_gamewatch_status_uniq_process_final_swim.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Mr. Game & Watch's Final Smash (Octopus) in water.
// TODO: initStatus, execStatus, execFixPos and the water helpers (getWaterAreaInfo, getWaterSurfaceOffsetPos,
// calcWaterSurfacePos); only exitStatus and the static instance are reconstructed.

// Leaving the Final Smash statuses (0x12d..0x137) clears its work flag 0x12000041.
void ftGameWatchStatusUniqProcessFinalSwim::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (nextStatus) {
    case 0x12d:
    case 0x12e:
    case 0x12f:
    case 0x130:
    case 0x131:
    case 0x132:
    case 0x133:
    case 0x134:
    case 0x135:
    case 0x136:
    case 0x137:
        break;
    default:
        moduleAccesser->getWorkManageModule().offFlag(0x12000041);
        break;
    }
}

ftGameWatchStatusUniqProcessFinalSwim g_ftGameWatchStatusUniqProcessFinalSwim;
