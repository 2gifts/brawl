#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessAttackAirInheritJumpAerialMotion : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessAttackAirInheritJumpAerialMotion() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessAttackAirInheritJumpAerialMotion g_ftStatusUniqProcessAttackAirInheritJumpAerialMotion;

void ftStatusUniqProcessAttackAirInheritJumpAerialMotion::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessAttackAirInheritJumpAerialMotion::execFixPos(soModuleAccesser* moduleAccesser) { }
