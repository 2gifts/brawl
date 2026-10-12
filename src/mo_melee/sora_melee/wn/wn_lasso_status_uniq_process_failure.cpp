#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnLassoStatusUniqProcessFailure : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessFailure() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnLassoStatusUniqProcessFailure g_wnLassoStatusUniqProcessFailure;

void wnLassoStatusUniqProcessFailure::initStatus(soModuleAccesser* moduleAccesser) { }
void wnLassoStatusUniqProcessFailure::execFixPos(soModuleAccesser* moduleAccesser) { }
