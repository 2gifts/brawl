#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessCaptureGokulin : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptureGokulin() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessCaptureGokulin g_ftStatusUniqProcessCaptureGokulin;

void ftStatusUniqProcessCaptureGokulin::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessCaptureGokulin::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessCaptureGokulin::execFixPos(soModuleAccesser* moduleAccesser) { }
bool ftStatusUniqProcessCaptureGokulin::checkDamage(soModuleAccesser* moduleAccesser, void*) { return false; }
