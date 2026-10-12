#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessBuryJump : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessBuryJump() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessBuryJump g_ftStatusUniqProcessBuryJump;

void ftStatusUniqProcessBuryJump::initStatus(soModuleAccesser* moduleAccesser) { }
