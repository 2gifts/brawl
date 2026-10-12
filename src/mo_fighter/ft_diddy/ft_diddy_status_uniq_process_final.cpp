#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

// Skeleton: methods without bodies are stubs (not yet reconstructed).

class ftDiddyStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftDiddyStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDiddyStatusUniqProcessFinal::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftDiddyStatusUniqProcessFinal::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftDiddyStatusUniqProcessFinal::execStop(soModuleAccesser* moduleAccesser) {
}

void ftDiddyStatusUniqProcessFinal::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftDiddyStatusUniqProcessFinal::exitStatus(soModuleAccesser* moduleAccesser, int) {
}

ftDiddyStatusUniqProcessFinal g_ftDiddyStatusUniqProcessFinal;
