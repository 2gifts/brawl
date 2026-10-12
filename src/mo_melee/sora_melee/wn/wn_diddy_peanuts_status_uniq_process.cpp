#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class wnDiddyPeanutsStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnDiddyPeanutsStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

wnDiddyPeanutsStatusUniqProcess g_wnDiddyPeanutsStatusUniqProcess;

void wnDiddyPeanutsStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) {
}

void wnDiddyPeanutsStatusUniqProcess::execStatus(soModuleAccesser* moduleAccesser) {
}

void wnDiddyPeanutsStatusUniqProcess::execStop(soModuleAccesser* moduleAccesser) {
}

void wnDiddyPeanutsStatusUniqProcess::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
}
