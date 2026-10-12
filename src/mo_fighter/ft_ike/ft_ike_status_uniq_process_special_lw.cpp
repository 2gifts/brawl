#include <ft/fighter.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftIkeStatusUniqProcessSpecialLw : public soStatusUniqProcess {
public:
    virtual ~ftIkeStatusUniqProcessSpecialLw() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

// HYPOTHESIS: the bodies (about 1 KB of init and per-frame code, plus the fix-position and exit paths) are not
// reconstructed yet; these stubs keep the class shape and the static instance.
void ftIkeStatusUniqProcessSpecialLw::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftIkeStatusUniqProcessSpecialLw::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftIkeStatusUniqProcessSpecialLw::execStop(soModuleAccesser* moduleAccesser) {
}

void ftIkeStatusUniqProcessSpecialLw::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftIkeStatusUniqProcessSpecialLw::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
}

ftIkeStatusUniqProcessSpecialLw g_ftIkeStatusUniqProcessSpecialLw;
