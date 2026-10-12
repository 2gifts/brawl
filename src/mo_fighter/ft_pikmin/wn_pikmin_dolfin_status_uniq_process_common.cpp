#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnPikminDolfinStatusUniqProcessCommon : public soStatusUniqProcess {
public:
    virtual ~wnPikminDolfinStatusUniqProcessCommon() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

wnPikminDolfinStatusUniqProcessCommon g_wnPikminDolfinStatusUniqProcessCommon;

void wnPikminDolfinStatusUniqProcessCommon::exitStatus(soModuleAccesser* moduleAccesser, int) { }
