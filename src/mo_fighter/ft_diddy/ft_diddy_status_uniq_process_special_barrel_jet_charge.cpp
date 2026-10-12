#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

// Skeleton: methods without bodies are stubs (not yet reconstructed).

class ftDiddyStatusUniqProcessSpecialBarrelJetCharge : public soStatusUniqProcess {
public:
    virtual ~ftDiddyStatusUniqProcessSpecialBarrelJetCharge() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDiddyStatusUniqProcessSpecialBarrelJetCharge::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftDiddyStatusUniqProcessSpecialBarrelJetCharge::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftDiddyStatusUniqProcessSpecialBarrelJetCharge::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftDiddyStatusUniqProcessSpecialBarrelJetCharge::execStop(soModuleAccesser* moduleAccesser) {
}

void ftDiddyStatusUniqProcessSpecialBarrelJetCharge::exitStatus(soModuleAccesser* moduleAccesser, int) {
}

ftDiddyStatusUniqProcessSpecialBarrelJetCharge g_ftDiddyStatusUniqProcessSpecialBarrelJetCharge;
