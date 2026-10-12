#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

// Skeleton: methods without bodies are stubs (not yet reconstructed).

class ftDededeStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDededeStatusUniqProcessFinal::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessFinal::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessFinal::execStop(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessFinal::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessFinal::exitStatus(soModuleAccesser* moduleAccesser, int) {
}

ftDededeStatusUniqProcessFinal g_ftDededeStatusUniqProcessFinal;
