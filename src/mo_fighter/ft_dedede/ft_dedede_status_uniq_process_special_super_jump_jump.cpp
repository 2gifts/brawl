#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

// Skeleton: methods without bodies are stubs (not yet reconstructed).

class ftDededeStatusUniqProcessSpecialSuperJumpJump : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialSuperJumpJump() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDededeStatusUniqProcessSpecialSuperJumpJump::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialSuperJumpJump::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialSuperJumpJump::execStop(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialSuperJumpJump::exitStatus(soModuleAccesser* moduleAccesser, int) {
}

ftDededeStatusUniqProcessSpecialSuperJumpJump g_ftDededeStatusUniqProcessSpecialSuperJumpJump;
