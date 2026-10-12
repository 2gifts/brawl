#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: Kirby's Special N (inhale) status process; names are unknown. Only the class shape is reconstructed.
class ftKirbyStatusUniqProcessSpecialN : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessSpecialN() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

void ftKirbyStatusUniqProcessSpecialN::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: body not yet reconstructed; left as a stub.
}

void ftKirbyStatusUniqProcessSpecialN::exitStatus(soModuleAccesser* moduleAccesser, int) {
    // HYPOTHESIS: body not yet reconstructed; left as a stub.
}

void ftKirbyStatusUniqProcessSpecialN::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftKirbyStatusUniqProcessSpecialN::execStop(soModuleAccesser* moduleAccesser) {
}

void ftKirbyStatusUniqProcessSpecialN::execFixPos(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: body not yet reconstructed; left as a stub.
}

ftKirbyStatusUniqProcessSpecialN g_ftKirbyStatusUniqProcessSpecialN;
