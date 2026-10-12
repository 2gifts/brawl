#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnLassoStatusUniqProcessMotionShoot : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessMotionShoot() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnLassoStatusUniqProcessMotionShoot g_wnLassoStatusUniqProcessMotionShoot;

void wnLassoStatusUniqProcessMotionShoot::initStatus(soModuleAccesser* moduleAccesser) { }
void wnLassoStatusUniqProcessMotionShoot::execFixPos(soModuleAccesser* moduleAccesser) { }
