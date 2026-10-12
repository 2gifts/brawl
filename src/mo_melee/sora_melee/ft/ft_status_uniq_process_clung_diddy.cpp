#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/stageobject.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: capture module (soModuleEnumeration::m_captureModule, +0x40)
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
    virtual void unk30(int, int);
    virtual void unk34();
    virtual bool unk38(soModuleAccesser* moduleAccesser, void* hit);
    virtual void unk3C();
    virtual void unk40();
    virtual void unk44(soModuleAccesser* moduleAccesser);
    virtual void unk48();
    virtual void unk4C(int);
};

class ftStatusUniqProcessClungDiddy : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessClungDiddy() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int status);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* hit);
};

ftStatusUniqProcessClungDiddy g_ftStatusUniqProcessClungDiddy;

void ftStatusUniqProcessClungDiddy::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftStatusUniqProcessClungDiddy::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftStatusUniqProcessClungDiddy::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftStatusUniqProcessClungDiddy::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status != 0xd7) {
        reinterpret_cast<ftCaptureModuleView*>(moduleAccesser->m_enumerationStart->m_captureModule)->unk30(0, 0);
    }
}

bool ftStatusUniqProcessClungDiddy::checkDamage(soModuleAccesser* moduleAccesser, void* hit) {
    return false;
}

