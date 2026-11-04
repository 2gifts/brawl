#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <so/so_module_accesser.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnNessPKFlashStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnNessPKFlashStatusUniqProcessMove() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnNessPKFlashStatusUniqProcessMove g_wnNessPKFlashStatusUniqProcessMove;

void wnNessPKFlashStatusUniqProcessMove::initStatus(soModuleAccesser* moduleAccesser) { }
void wnNessPKFlashStatusUniqProcessMove::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    kinetic.changeKinetic(2, moduleAccesser);
    soKineticEnergy* gravity = kinetic.getEnergy(2);
    gravity->clearSpeed();
    gravity->disable();
    if (nextStatus == 1) {
        int effectHandle = work.getInt(0x20000000);
        moduleAccesser->getEffectModule().kill(effectHandle, true, true);
        soEffectModule& effect = moduleAccesser->getEffectModule();
        effect.reqFollow(static_cast<EfID>(0xB000A), 0,
            &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, false, 0, 0, -1);
    }
}
void wnNessPKFlashStatusUniqProcessMove::execStatus(soModuleAccesser* moduleAccesser) { }
