#include <so/status/so_status_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

// HYPOTHESIS: capture module (soModuleEnumeration::m_captureModule, +0x40); slots 0x38, 0x44 and 0x4C are used
class ftCaptureModuleView {
public:
    virtual ~ftCaptureModuleView() { }
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual void unk24();
    virtual void unk28();
    virtual void unk2C();
    virtual void unk30();
    virtual void unk34();
    virtual bool unk38(soModuleAccesser* moduleAccesser, void* hit);
    virtual void unk3C();
    virtual void unk40();
    virtual void unk44(soModuleAccesser* moduleAccesser);
    virtual void unk48();
    virtual void unk4C(int);
};

class ftStatusUniqProcessShoulderedDonkeyThrown : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessShoulderedDonkeyThrown() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* hit);
};

ftStatusUniqProcessShoulderedDonkeyThrown g_ftStatusUniqProcessShoulderedDonkeyThrown;

void ftStatusUniqProcessShoulderedDonkeyThrown::initStatus(soModuleAccesser* moduleAccesser) {
    if (soValueAccesser::getConstantInt(moduleAccesser, 0x5a98, 0) == 1) {
        // HYPOTHESIS: motion module slot 0x194 is setKindOffset (argument from constant 0x5a98)
        moduleAccesser->getMotionModule().setKindOffset(4);
    }
}

void ftStatusUniqProcessShoulderedDonkeyThrown::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleView*>(moduleAccesser->m_enumerationStart->m_captureModule)->unk44(moduleAccesser);
}

void ftStatusUniqProcessShoulderedDonkeyThrown::exitStatus(soModuleAccesser* moduleAccesser, int) {
    reinterpret_cast<ftCaptureModuleView*>(moduleAccesser->m_enumerationStart->m_captureModule)->unk4C(0);
}

bool ftStatusUniqProcessShoulderedDonkeyThrown::checkDamage(soModuleAccesser* moduleAccesser, void* hit) {
    return reinterpret_cast<ftCaptureModuleView*>(moduleAccesser->m_enumerationStart->m_captureModule)->unk38(moduleAccesser, hit);
}
