#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessBury : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessBury() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessBury g_ftStatusUniqProcessBury;

void ftStatusUniqProcessBury::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessBury::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessBury::execStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessBury::execStop(soModuleAccesser* moduleAccesser) { }
