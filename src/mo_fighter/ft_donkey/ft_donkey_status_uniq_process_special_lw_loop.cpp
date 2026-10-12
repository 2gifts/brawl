#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftDonkeyStatusUniqProcessSpecialLwLoop : public soStatusUniqProcess {
public:
    virtual ~ftDonkeyStatusUniqProcessSpecialLwLoop() { }
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

void ftDonkeyStatusUniqProcessSpecialLwLoop::execFixPosCounter(soModuleAccesser* moduleAccesser) {}

ftDonkeyStatusUniqProcessSpecialLwLoop g_ftDonkeyStatusUniqProcessSpecialLwLoop;
