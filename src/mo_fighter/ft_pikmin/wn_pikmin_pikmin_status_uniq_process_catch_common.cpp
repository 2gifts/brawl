#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnPikminPikminStatusUniqProcessCatchCommon : public soStatusUniqProcess {
public:
    virtual ~wnPikminPikminStatusUniqProcessCatchCommon() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

wnPikminPikminStatusUniqProcessCatchCommon g_wnPikminPikminStatusUniqProcessCatchCommon;

void wnPikminPikminStatusUniqProcessCatchCommon::exitStatus(soModuleAccesser* moduleAccesser, int) { }
