#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnLassoStatusUniqProcessShoot : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessShoot() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnLassoStatusUniqProcessShoot g_wnLassoStatusUniqProcessShoot;

void wnLassoStatusUniqProcessShoot::initStatus(soModuleAccesser* moduleAccesser) { }
void wnLassoStatusUniqProcessShoot::execFixPos(soModuleAccesser* moduleAccesser) { }
