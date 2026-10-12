#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessRun : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessRun() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual bool onChangeLr(soModuleAccesser* moduleAccesser, float, float);
};

ftStatusUniqProcessRun g_ftStatusUniqProcessRun;

void ftStatusUniqProcessRun::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessRun::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessRun::execStatus(soModuleAccesser* moduleAccesser) { }
bool ftStatusUniqProcessRun::onChangeLr(soModuleAccesser* moduleAccesser, float, float) { return false; }
