#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessItemSwing : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessItemSwing() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void checkAttack(soModuleAccesser* moduleAccesser, void*, float);
};

ftStatusUniqProcessItemSwing g_ftStatusUniqProcessItemSwing;

void ftStatusUniqProcessItemSwing::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessItemSwing::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessItemSwing::checkAttack(soModuleAccesser* moduleAccesser, void*, float) { }
