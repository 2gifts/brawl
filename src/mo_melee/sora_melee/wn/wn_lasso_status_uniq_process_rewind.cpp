#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnLassoStatusUniqProcessRewind : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessRewind() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnLassoStatusUniqProcessRewind g_wnLassoStatusUniqProcessRewind;

void wnLassoStatusUniqProcessRewind::initStatus(soModuleAccesser* moduleAccesser) { }
void wnLassoStatusUniqProcessRewind::execFixPos(soModuleAccesser* moduleAccesser) { }
