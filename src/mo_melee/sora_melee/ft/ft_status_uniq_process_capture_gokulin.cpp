#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <ft/ft_status_uniq_process_views.h>
#include <types.h>

// HYPOTHESIS: slot names are unverified; argument types inferred from register use. Collision hit module view (local). Destructor entries, then 12 pads, put slotHit3c at vtable offset 0x3C.
class ftCollisionHitModuleSlotView {
public:
    virtual ~ftCollisionHitModuleSlotView() { }
    virtual void pad0();
    virtual void pad1();
    virtual void pad2();
    virtual void pad3();
    virtual void pad4();
    virtual void pad5();
    virtual void pad6();
    virtual void pad7();
    virtual void pad8();
    virtual void pad9();
    virtual void pad10();
    virtual void pad11();
    virtual void slotHit3c(void* value, u32 group);
};

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessCaptureGokulin : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptureGokulin() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessCaptureGokulin g_ftStatusUniqProcessCaptureGokulin;

void ftStatusUniqProcessCaptureGokulin::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessCaptureGokulin::exitStatus(soModuleAccesser* moduleAccesser, int) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot30(0, 0);
    ftCollisionHitModuleSlotView* hitModule =
        reinterpret_cast<ftCollisionHitModuleSlotView*>(moduleAccesser->m_enumerationStart->m_collisionHitModule);
    hitModule->slotHit3c(soValueAccesser::getConstantIndefinite(moduleAccesser, 0xA803, 0), 0);
}
void ftStatusUniqProcessCaptureGokulin::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot44();
}
bool ftStatusUniqProcessCaptureGokulin::checkDamage(soModuleAccesser* moduleAccesser, void*) { return true; }
