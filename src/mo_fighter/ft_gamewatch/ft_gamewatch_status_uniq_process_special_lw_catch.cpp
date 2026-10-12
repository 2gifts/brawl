#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/collision/so_collision_shield_module_impl.h>
#include <types.h>

class ftGamewatchStatusUniqProcessSpecialLwCatch : public soStatusUniqProcess {
public:
    virtual ~ftGamewatchStatusUniqProcessSpecialLwCatch() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

ftGamewatchStatusUniqProcessSpecialLwCatch g_ftGamewatchStatusUniqProcessSpecialLwCatch;

void ftGamewatchStatusUniqProcessSpecialLwCatch::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
        moduleAccesser->getCollisionAbsorberModule().setStatus(0, 1, 0);
    }
}
