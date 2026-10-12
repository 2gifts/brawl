#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessCaptureTaboo : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptureTaboo() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessCaptureTaboo g_ftStatusUniqProcessCaptureTaboo;

void ftStatusUniqProcessCaptureTaboo::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessCaptureTaboo::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessCaptureTaboo::execFixPos(soModuleAccesser* moduleAccesser) { }
bool ftStatusUniqProcessCaptureTaboo::checkDamage(soModuleAccesser* moduleAccesser, void*) { return false; }
