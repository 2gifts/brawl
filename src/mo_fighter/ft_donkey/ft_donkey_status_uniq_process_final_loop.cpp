#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftDonkeyStatusUniqProcessFinalLoop : public soStatusUniqProcess {
public:
    virtual ~ftDonkeyStatusUniqProcessFinalLoop() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDonkeyStatusUniqProcessFinalLoop::initStatus(soModuleAccesser* moduleAccesser) {}
void ftDonkeyStatusUniqProcessFinalLoop::execStatus(soModuleAccesser* moduleAccesser) {}
void ftDonkeyStatusUniqProcessFinalLoop::execFixPosCounter(soModuleAccesser* moduleAccesser) {}
void ftDonkeyStatusUniqProcessFinalLoop::exitStatus(soModuleAccesser* moduleAccesser, int) {}

ftDonkeyStatusUniqProcessFinalLoop g_ftDonkeyStatusUniqProcessFinalLoop;
