#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <so/so_module_accesser.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnNessPKFlashStatusUniqProcessBang : public soStatusUniqProcess {
public:
    virtual ~wnNessPKFlashStatusUniqProcessBang() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnNessPKFlashStatusUniqProcessBang g_wnNessPKFlashStatusUniqProcessBang;

void wnNessPKFlashStatusUniqProcessBang::initStatus(soModuleAccesser* moduleAccesser) { }
void wnNessPKFlashStatusUniqProcessBang::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    soCollisionAttackModule& attack = moduleAccesser->getCollisionAttackModule();
    if (attack.isAttack(0, false)) {
        attack.setPower(0, moduleAccesser->getWorkManageModule().getInt(0x20000001), false);
    }
}
