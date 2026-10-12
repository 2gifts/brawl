#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnPitIkarusStatusUniqProcessAttack : public soStatusUniqProcess {
public:
    virtual ~wnPitIkarusStatusUniqProcessAttack() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

wnPitIkarusStatusUniqProcessAttack g_wnPitIkarusStatusUniqProcessAttack;

void wnPitIkarusStatusUniqProcessAttack::initStatus(soModuleAccesser* moduleAccesser) { }
