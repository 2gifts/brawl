#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessDead : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessDead() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixCamera(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessDead g_ftStatusUniqProcessDead;

void ftStatusUniqProcessDead::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessDead::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessDead::execStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessDead::execFixCamera(soModuleAccesser* moduleAccesser) { }
