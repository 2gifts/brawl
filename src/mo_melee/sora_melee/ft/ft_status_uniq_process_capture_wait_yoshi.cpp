#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessCaptureWaitYoshi : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptureWaitYoshi() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessCaptureWaitYoshi g_ftStatusUniqProcessCaptureWaitYoshi;

void ftStatusUniqProcessCaptureWaitYoshi::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessCaptureWaitYoshi::execFixPos(soModuleAccesser* moduleAccesser) { }
bool ftStatusUniqProcessCaptureWaitYoshi::checkDamage(soModuleAccesser* moduleAccesser, void*) { return false; }
