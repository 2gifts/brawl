#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessClungCaptain : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessClungCaptain() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessClungCaptain g_ftStatusUniqProcessClungCaptain;

void ftStatusUniqProcessClungCaptain::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessClungCaptain::exitStatus(soModuleAccesser* moduleAccesser, int) { }
bool ftStatusUniqProcessClungCaptain::checkDamage(soModuleAccesser* moduleAccesser, void*) { return false; }
