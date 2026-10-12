#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessEscapeAirInheritJumpAerialMotion : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessEscapeAirInheritJumpAerialMotion() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessEscapeAirInheritJumpAerialMotion g_ftStatusUniqProcessEscapeAirInheritJumpAerialMotion;

void ftStatusUniqProcessEscapeAirInheritJumpAerialMotion::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessEscapeAirInheritJumpAerialMotion::execFixPos(soModuleAccesser* moduleAccesser) { }
