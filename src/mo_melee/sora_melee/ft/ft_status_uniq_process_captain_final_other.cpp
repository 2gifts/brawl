#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessCaptainFinalOther : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptainFinalOther() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftStatusUniqProcessCaptainFinalOther g_ftStatusUniqProcessCaptainFinalOther;

void ftStatusUniqProcessCaptainFinalOther::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* workManage = moduleAccesser->m_enumerationStart->m_workManageModule;
    workManage->onFlag(0x22000011);
    workManage->onFlag(0x22000012);
    moduleAccesser->getKineticModule().clearSpeedAll();
}
void ftStatusUniqProcessCaptainFinalOther::exitStatus(soModuleAccesser* moduleAccesser, int) { }
