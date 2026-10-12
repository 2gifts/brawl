#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessFall : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessFall() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessFall g_ftStatusUniqProcessFall;

void ftStatusUniqProcessFall::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessFall::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessFall::execStatus(soModuleAccesser* moduleAccesser) { }
