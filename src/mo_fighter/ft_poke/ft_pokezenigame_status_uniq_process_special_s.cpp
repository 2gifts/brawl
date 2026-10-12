#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/controller/so_controller_module_impl.h>
#include <types.h>

// Zenigame's Water Gun (special S). initStatus, execStatus and checkDamage are not reconstructed yet;
// exitStatus ends the controller clatter (rumble) when the status changes to the clatter-free status 0x11c.
class ftPokeZenigameStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftPokeZenigameStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* damageInfo);
};

ftPokeZenigameStatusUniqProcessSpecialS g_ftPokeZenigameStatusUniqProcessSpecialS;

void ftPokeZenigameStatusUniqProcessSpecialS::initStatus(soModuleAccesser*) {}

void ftPokeZenigameStatusUniqProcessSpecialS::execStatus(soModuleAccesser*) {}

void ftPokeZenigameStatusUniqProcessSpecialS::exitStatus(soModuleAccesser* moduleAccesser, int) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x11c:
        moduleAccesser->getControllerModule().endClatter(0);
        break;
    default:
        break;
    }
}

bool ftPokeZenigameStatusUniqProcessSpecialS::checkDamage(soModuleAccesser*, void*) {
    return false;
}
