#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Capture (cut Gokulin) status process: switches the kinetic module to kinetic type 0x49 on entry.
class ftStatusUniqProcessCaptureCutGokulin : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptureCutGokulin() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessCaptureCutGokulin g_ftStatusUniqProcessCaptureCutGokulin;

void ftStatusUniqProcessCaptureCutGokulin::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getKineticModule().changeKinetic(0x49, moduleAccesser);
}
void ftStatusUniqProcessCaptureCutGokulin::execFixPos(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessCaptureCutGokulin::exitStatus(soModuleAccesser* moduleAccesser, int) { }
