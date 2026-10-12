#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessPassive : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessPassive() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessPassive g_ftStatusUniqProcessPassive;

void ftStatusUniqProcessPassive::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessPassive::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessPassive::execStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessPassive::execStop(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessPassive::execFixPos(soModuleAccesser* moduleAccesser) { }
