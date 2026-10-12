#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/damage/so_damage_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftDonkeyStatusUniqProcessSpecialNAttack : public soStatusUniqProcess {
public:
    virtual ~ftDonkeyStatusUniqProcessSpecialNAttack() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftDonkeyStatusUniqProcessSpecialNAttack g_ftDonkeyStatusUniqProcessSpecialNAttack;

// Work int holding the number of charged hits (id 0x10000040); the scaled result is stored in 0x20000003.
void ftDonkeyStatusUniqProcessSpecialNAttack::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* workManage = &moduleAccesser->getWorkManageModule();
    // HYPOTHESIS: the two Kirby-scope integer constants (0x5dc0 = cap, 0x5dc1 = multiplier).
    int cap = moduleAccesser->getConstantIntKirby(0x5dc0);
    int count = workManage->getInt(0x10000040);
    if (count > cap) {
        count = cap;
    }
    workManage->setInt(count * moduleAccesser->getConstantIntKirby(0x5dc1), 0x20000003);
}

void ftDonkeyStatusUniqProcessSpecialNAttack::exitStatus(soModuleAccesser* moduleAccesser, int) {
    moduleAccesser->getWorkManageModule().setInt(0, 0x10000040);
    moduleAccesser->getDamageModule().setNoReactionModeStatus(-1.0f, -1.0f, 0);
}
