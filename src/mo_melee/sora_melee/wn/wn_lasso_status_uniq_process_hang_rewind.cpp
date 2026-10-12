#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnLassoStatusUniqProcessHangRewind : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessHangRewind() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnLassoStatusUniqProcessHangRewind g_wnLassoStatusUniqProcessHangRewind;

void wnLassoStatusUniqProcessHangRewind::initStatus(soModuleAccesser* moduleAccesser) { }
void wnLassoStatusUniqProcessHangRewind::execFixPos(soModuleAccesser* moduleAccesser) { }
