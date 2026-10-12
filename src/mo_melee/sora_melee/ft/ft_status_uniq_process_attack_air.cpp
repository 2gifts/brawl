#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE

#include <ft/fighter.h>
#include <ft/ft_util.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftStatusUniqProcessAttackAir : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessAttackAir() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

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
        if (nextStatusKind < 0x19) {
            if (nextStatusKind >= 0x16) {
                moduleAccesser->getKineticModule().clearSpeedAll();
            }
        }
    }
}

ftStatusUniqProcessAttackAir g_ftStatusUniqProcessAttackAir;
