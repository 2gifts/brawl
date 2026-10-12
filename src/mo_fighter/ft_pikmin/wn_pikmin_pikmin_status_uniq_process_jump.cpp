#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnPikminPikminStatusUniqProcessJump : public soStatusUniqProcess {
public:
    virtual ~wnPikminPikminStatusUniqProcessJump() { }
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnPikminPikminStatusUniqProcessJump g_wnPikminPikminStatusUniqProcessJump;

void wnPikminPikminStatusUniqProcessJump::execFixPos(soModuleAccesser* moduleAccesser) { }
