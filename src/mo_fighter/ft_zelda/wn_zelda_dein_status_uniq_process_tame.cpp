#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnZeldaDeinStatusUniqProcessTame : public soStatusUniqProcess {
public:
    virtual ~wnZeldaDeinStatusUniqProcessTame() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnZeldaDeinStatusUniqProcessTame g_wnZeldaDeinStatusUniqProcessTame;

void wnZeldaDeinStatusUniqProcessTame::initStatus(soModuleAccesser* moduleAccesser) { }
void wnZeldaDeinStatusUniqProcessTame::execStatus(soModuleAccesser* moduleAccesser) { }
