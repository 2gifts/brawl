#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnLassoStatusUniqProcessMotionPull : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessMotionPull() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnLassoStatusUniqProcessMotionPull g_wnLassoStatusUniqProcessMotionPull;

void wnLassoStatusUniqProcessMotionPull::initStatus(soModuleAccesser* moduleAccesser) { }
void wnLassoStatusUniqProcessMotionPull::execFixPos(soModuleAccesser* moduleAccesser) { }
