#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessLanding : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessLanding() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessLanding g_ftStatusUniqProcessLanding;

void ftStatusUniqProcessLanding::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessLanding::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessLanding::execStatus(soModuleAccesser* moduleAccesser) { }
