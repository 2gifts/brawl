#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class wnLucasPKFlashStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnLucasPKFlashStatusUniqProcessMove() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

wnLucasPKFlashStatusUniqProcessMove g_wnLucasPKFlashStatusUniqProcessMove;

void wnLucasPKFlashStatusUniqProcessMove::initStatus(soModuleAccesser* moduleAccesser) {
}

void wnLucasPKFlashStatusUniqProcessMove::execStatus(soModuleAccesser* moduleAccesser) {
}

void wnLucasPKFlashStatusUniqProcessMove::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    kinetic.changeKinetic(2, moduleAccesser);
    soKineticEnergy* energy = kinetic.getEnergy(2);
    energy->clearSpeed();
    energy->disable();
    if (nextStatus == 1) {
        int handle = work.getInt(0x20000000);
        moduleAccesser->getEffectModule().kill(handle, true, true);
        soEffectModule& effect = moduleAccesser->getEffectModule();
        effect.reqFollow(static_cast<EfID>(0x1B000D), 0,
            &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, false, 0, 0, -1);
    }
}
