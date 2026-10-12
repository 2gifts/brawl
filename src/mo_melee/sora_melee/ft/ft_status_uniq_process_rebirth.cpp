#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessRebirth : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessRebirth() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessRebirth g_ftStatusUniqProcessRebirth;

void ftStatusUniqProcessRebirth::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessRebirth::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessRebirth::execStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessRebirth::execFixPos(soModuleAccesser* moduleAccesser) { }
