#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE

#include <ft/fighter.h>
#include <ft/ft_status_uniq_process_attack_air.h>
#include <ft/ft_util.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

void ftStatusUniqProcessAttackAir::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftStatusUniqProcessAttackAir::execStatus(soModuleAccesser* moduleAccesser) {
    ftUtil::setCloudThroughCheck(moduleAccesser);
}

void ftStatusUniqProcessAttackAir::execFixPos(soModuleAccesser* moduleAccesser) {
    ftUtil::checkCloudThroughOut(moduleAccesser);
}

void ftStatusUniqProcessAttackAir::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    // HYPOTHESIS: work flag 0x2200001F gates a kinetic reset for transitions into statuses 0x16-0x18.
    if (moduleAccesser->getWorkManageModule().isFlag(0x2200001F) == 1) {
        switch (nextStatusKind) {
        case 0x16:
        case 0x17:
        case 0x18:
            moduleAccesser->getKineticModule().clearSpeedAll();
            break;
        }
    }
}

ftStatusUniqProcessAttackAir g_ftStatusUniqProcessAttackAir;
