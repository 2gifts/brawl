#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

extern "C" {
void fn_27_1DE288(void* self, soModuleAccesser* moduleAccesser);
}

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnPikminChainStatusUniqProcessReach : public soStatusUniqProcess {
public:
    virtual ~wnPikminChainStatusUniqProcessReach() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

wnPikminChainStatusUniqProcessReach g_wnPikminChainStatusUniqProcessReach;

void wnPikminChainStatusUniqProcessReach::initStatus(soModuleAccesser* a) {
    fn_27_1DE288(this, a);
    int count = a->getWorkManageModule().getInt(0x10000007);
    a->getWorkManageModule().setInt(count, 0x20000002);
    float scaled = 10.0f * (float)count;
    a->getWorkManageModule().setFloat(scaled, 0x21000001);
}
