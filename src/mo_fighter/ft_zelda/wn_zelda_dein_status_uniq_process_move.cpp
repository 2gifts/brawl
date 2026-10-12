#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnZeldaDeinStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnZeldaDeinStatusUniqProcessMove() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnZeldaDeinStatusUniqProcessMove g_wnZeldaDeinStatusUniqProcessMove;

void wnZeldaDeinStatusUniqProcessMove::execStatus(soModuleAccesser* moduleAccesser) { }
