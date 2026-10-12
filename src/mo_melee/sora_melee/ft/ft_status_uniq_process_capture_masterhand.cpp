#include <ft/fighter.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Local view of the capture module (no header in this tree). Slot order taken from the call sites:
// (dtor slot count checked against the diff) slot 12 is called with two ints on exit, slot 17 with no arguments on the fix-position path.
class ftCaptureModuleView {
public:
    virtual ~ftCaptureModuleView() { }
    virtual void unk3();
    virtual void unk4();
    virtual void unk5();
    virtual void unk6();
    virtual void unk7();
    virtual void unk8();
    virtual void unk9();
    virtual void unk10();
    virtual void unk11();
    virtual void unk12(int, int);
    virtual void unk13();
    virtual void unk14();
    virtual void unk15();
    virtual void unk16();
    virtual void unk17();
};

class ftStatusUniqProcessCaptureMasterhand : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptureMasterhand() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* unk);
};

ftStatusUniqProcessCaptureMasterhand g_ftStatusUniqProcessCaptureMasterhand;

// Clatter values from module rodata (lbl_27_rodata_2638 and lbl_27_rodata_263C).
static const float s_captureClatterX = 1.0f;
static const float s_captureClatterY = 0.0f;

void ftStatusUniqProcessCaptureMasterhand::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getStatusKind() == Fighter::Status::Capture_Masterhand) {
        moduleAccesser->getControllerModule().startClatter(s_captureClatterX, s_captureClatterY, s_captureClatterY,
                                                           0, -1, 0, false);
    }
}

void ftStatusUniqProcessCaptureMasterhand::execFixPos(soModuleAccesser* moduleAccesser) {
    static_cast<ftCaptureModuleView*>(moduleAccesser->m_enumerationStart->m_captureModule)->unk17();
}

void ftStatusUniqProcessCaptureMasterhand::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    if (nextStatusKind == Fighter::Status::Capture_Masterhand) {
        return;
    }
    if (nextStatusKind < Fighter::Status::Capture_Masterhand && nextStatusKind == Fighter::Status::Capture_Thrown) {
        return;
    }
    static_cast<ftCaptureModuleView*>(moduleAccesser->m_enumerationStart->m_captureModule)->unk12(0, 0);
}

bool ftStatusUniqProcessCaptureMasterhand::checkDamage(soModuleAccesser* moduleAccesser, void* unk) {
    return true;
}
