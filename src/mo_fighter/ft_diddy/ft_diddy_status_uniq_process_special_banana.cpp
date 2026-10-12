#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

// Skeleton: methods without bodies are stubs (not yet reconstructed).

class ftDiddyStatusUniqProcessSpecialBanana : public soStatusUniqProcess {
public:
    virtual ~ftDiddyStatusUniqProcessSpecialBanana() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDiddyStatusUniqProcessSpecialBanana::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftDiddyStatusUniqProcessSpecialBanana::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftDiddyStatusUniqProcessSpecialBanana::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftDiddyStatusUniqProcessSpecialBanana::exitStatus(soModuleAccesser* moduleAccesser, int) {
}

ftDiddyStatusUniqProcessSpecialBanana g_ftDiddyStatusUniqProcessSpecialBanana;
