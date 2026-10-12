#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnNessPKFireStatusUniqProcessShoot : public soStatusUniqProcess {
public:
    virtual ~wnNessPKFireStatusUniqProcessShoot() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

wnNessPKFireStatusUniqProcessShoot g_wnNessPKFireStatusUniqProcessShoot;

void wnNessPKFireStatusUniqProcessShoot::initStatus(soModuleAccesser* moduleAccesser) { }
void wnNessPKFireStatusUniqProcessShoot::exitStatus(soModuleAccesser* moduleAccesser, int) { }
