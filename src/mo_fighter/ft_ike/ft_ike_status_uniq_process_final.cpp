#include <ft/fighter.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftIkeStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftIkeStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

// HYPOTHESIS: the bodies (the 1.6 KB init path, the per-frame update and the exit path) are not reconstructed
// yet; these stubs keep the class shape and the static instance.
void ftIkeStatusUniqProcessFinal::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftIkeStatusUniqProcessFinal::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftIkeStatusUniqProcessFinal::execStop(soModuleAccesser* moduleAccesser) {
}

void ftIkeStatusUniqProcessFinal::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftIkeStatusUniqProcessFinal::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
}

ftIkeStatusUniqProcessFinal g_ftIkeStatusUniqProcessFinal;
