#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class wnLucasPKFlashStatusUniqProcessBang : public soStatusUniqProcess {
public:
    virtual ~wnLucasPKFlashStatusUniqProcessBang() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnLucasPKFlashStatusUniqProcessBang g_wnLucasPKFlashStatusUniqProcessBang;

void wnLucasPKFlashStatusUniqProcessBang::initStatus(soModuleAccesser* moduleAccesser) {
}

void wnLucasPKFlashStatusUniqProcessBang::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    soCollisionAttackModule& attack = moduleAccesser->getCollisionAttackModule();
    if (attack.isAttack(0, false)) {
        // HYPOTHESIS: the work value at 0x20000001 is the attack power for hit 0.
        attack.setPower(0, moduleAccesser->getWorkManageModule().getInt(0x20000001), false);
    }
}
