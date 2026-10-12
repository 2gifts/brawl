#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <ft/ft_status_uniq_process_views.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessCaptureWaitYoshi : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptureWaitYoshi() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessCaptureWaitYoshi g_ftStatusUniqProcessCaptureWaitYoshi;

void ftStatusUniqProcessCaptureWaitYoshi::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessCaptureWaitYoshi::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessCaptureWaitYoshi::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot44();
}
bool ftStatusUniqProcessCaptureWaitYoshi::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    return reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot34(0x109);
}
