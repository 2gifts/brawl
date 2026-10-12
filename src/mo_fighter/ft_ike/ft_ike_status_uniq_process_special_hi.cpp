#include <ft/fighter.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftIkeStatusUniqProcessSpecialHi : public soStatusUniqProcess {
public:
    virtual ~ftIkeStatusUniqProcessSpecialHi() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

// HYPOTHESIS: the init and per-frame bodies (about 1 KB each) are not reconstructed yet; this keeps the class shape.
void ftIkeStatusUniqProcessSpecialHi::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftIkeStatusUniqProcessSpecialHi::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftIkeStatusUniqProcessSpecialHi::execStop(soModuleAccesser* moduleAccesser) {
}

void ftIkeStatusUniqProcessSpecialHi::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftIkeStatusUniqProcessSpecialHi::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
}

ftIkeStatusUniqProcessSpecialHi g_ftIkeStatusUniqProcessSpecialHi;
