#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <ft/ft_status_uniq_process_views.h>
#include <types.h>

// Capture (Taboo) status process. Forwards to the fighter's capture module (m_enumerationStart->m_captureModule).
class ftStatusUniqProcessCaptureTaboo : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptureTaboo() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessCaptureTaboo g_ftStatusUniqProcessCaptureTaboo;

void ftStatusUniqProcessCaptureTaboo::initStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessCaptureTaboo::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot44();
}

void ftStatusUniqProcessCaptureTaboo::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status == 0x104) {
        return;
    }
    if (status < 0x104 && status == 0x42) {
        return;
    }
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot30(0, 0);
}

bool ftStatusUniqProcessCaptureTaboo::checkDamage(soModuleAccesser* moduleAccesser, void*) { return true; }
