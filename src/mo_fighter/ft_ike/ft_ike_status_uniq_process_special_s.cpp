#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: status process names and bodies are not yet reconstructed; only the class shape is present.
class ftIkeStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftIkeStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftIkeStatusUniqProcessSpecialS::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: body not yet reconstructed; left as a stub.
}

void ftIkeStatusUniqProcessSpecialS::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftIkeStatusUniqProcessSpecialS::execFixPos(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: body not yet reconstructed; left as a stub.
}

void ftIkeStatusUniqProcessSpecialS::exitStatus(soModuleAccesser* moduleAccesser, int) {
    // HYPOTHESIS: body not yet reconstructed; left as a stub.
}

ftIkeStatusUniqProcessSpecialS g_ftIkeStatusUniqProcessSpecialS;
