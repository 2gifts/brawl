#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

// Skeleton: methods without bodies are stubs (not yet reconstructed).

class ftDiddyStatusUniqProcessSpecialBarrelJet : public soStatusUniqProcess {
public:
    virtual ~ftDiddyStatusUniqProcessSpecialBarrelJet() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDiddyStatusUniqProcessSpecialBarrelJet::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftDiddyStatusUniqProcessSpecialBarrelJet::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftDiddyStatusUniqProcessSpecialBarrelJet::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftDiddyStatusUniqProcessSpecialBarrelJet::execStop(soModuleAccesser* moduleAccesser) {
}

void ftDiddyStatusUniqProcessSpecialBarrelJet::exitStatus(soModuleAccesser* moduleAccesser, int) {
}

ftDiddyStatusUniqProcessSpecialBarrelJet g_ftDiddyStatusUniqProcessSpecialBarrelJet;
