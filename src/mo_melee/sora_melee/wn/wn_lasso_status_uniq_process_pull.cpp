#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnLassoStatusUniqProcessPull : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessPull() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnLassoStatusUniqProcessPull g_wnLassoStatusUniqProcessPull;

void wnLassoStatusUniqProcessPull::initStatus(soModuleAccesser* moduleAccesser) { }
void wnLassoStatusUniqProcessPull::execFixPos(soModuleAccesser* moduleAccesser) { }
