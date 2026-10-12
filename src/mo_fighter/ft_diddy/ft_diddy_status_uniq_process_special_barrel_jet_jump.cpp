#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftDiddyStatusUniqProcessSpecialBarrelJetJump : public soStatusUniqProcess {
public:
    virtual ~ftDiddyStatusUniqProcessSpecialBarrelJetJump() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDiddyStatusUniqProcessSpecialBarrelJetJump::initStatus(soModuleAccesser* moduleAccesser) {}
void ftDiddyStatusUniqProcessSpecialBarrelJetJump::execStatus(soModuleAccesser* moduleAccesser) {}
void ftDiddyStatusUniqProcessSpecialBarrelJetJump::execFixPos(soModuleAccesser* moduleAccesser) {}
void ftDiddyStatusUniqProcessSpecialBarrelJetJump::execStop(soModuleAccesser* moduleAccesser) {}
void ftDiddyStatusUniqProcessSpecialBarrelJetJump::exitStatus(soModuleAccesser* moduleAccesser, int) {}

ftDiddyStatusUniqProcessSpecialBarrelJetJump g_ftDiddyStatusUniqProcessSpecialBarrelJetJump;
