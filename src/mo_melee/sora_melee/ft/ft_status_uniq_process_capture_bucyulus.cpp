#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <ft/ft_status_uniq_process_views.h>
#include <types.h>

// Capture (Bucyulus) status process. Forwards to the fighter's capture module (m_enumerationStart->m_captureModule).
class ftStatusUniqProcessCaptureBucyulus : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptureBucyulus() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessCaptureBucyulus g_ftStatusUniqProcessCaptureBucyulus;

void ftStatusUniqProcessCaptureBucyulus::initStatus(soModuleAccesser* moduleAccesser) { }

bool ftStatusUniqProcessCaptureBucyulus::checkDamage(soModuleAccesser* moduleAccesser, void*) { return true; }

void ftStatusUniqProcessCaptureBucyulus::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status == 0x105) {
        return;
    }
    if (status == 0x106) {
        return;
    }
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot30(0, 0);
}
