#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessTread : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessTread() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessTread g_ftStatusUniqProcessTread;

void ftStatusUniqProcessTread::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessTread::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessTread::execStatus(soModuleAccesser* moduleAccesser) { }
