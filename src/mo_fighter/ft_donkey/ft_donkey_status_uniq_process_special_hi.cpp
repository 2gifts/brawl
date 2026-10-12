#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftDonkeyStatusUniqProcessSpecialHi : public soStatusUniqProcess {
public:
    virtual ~ftDonkeyStatusUniqProcessSpecialHi() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDonkeyStatusUniqProcessSpecialHi::initStatus(soModuleAccesser* moduleAccesser) {}
void ftDonkeyStatusUniqProcessSpecialHi::execStatus(soModuleAccesser* moduleAccesser) {}
void ftDonkeyStatusUniqProcessSpecialHi::execFixPos(soModuleAccesser* moduleAccesser) {}
void ftDonkeyStatusUniqProcessSpecialHi::exitStatus(soModuleAccesser* moduleAccesser, int) {}

ftDonkeyStatusUniqProcessSpecialHi g_ftDonkeyStatusUniqProcessSpecialHi;
