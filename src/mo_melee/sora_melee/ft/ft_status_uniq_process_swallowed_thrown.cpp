#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/stageobject.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <so/status/so_status_module_impl.h>

// HYPOTHESIS: controller module (soModuleEnumeration::m_controllerModule, +0x5C)
class ftControllerModuleView {
public:
    virtual ~ftControllerModuleView() { }
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
    virtual void unk38();
    virtual void unk3C();
    virtual void unk40();
    virtual void unk44();
    virtual void unk48();
    virtual void unk4C();
    virtual void unk50();
    virtual void unk54();
    virtual void unk58();
    virtual void unk5C();
    virtual void unk60();
    virtual void unk64();
    virtual void unk68();
    virtual void unk6C();
    virtual void unk70();
    virtual void unk74();
    virtual void unk78();
    virtual void unk7C();
    virtual void unk80();
    virtual void unk84();
    virtual void unk88();
    virtual void unk8C();
    virtual void unk90();
    virtual void unk94();
    virtual void unk98();
    virtual void unk9C();
    virtual void unkA0();
    virtual void unkA4();
    virtual void unkA8();
    virtual void unkAC();
    virtual void unkB0();
    virtual void unkB4();
    virtual void unkB8();
    virtual void unkBC();
    virtual void unkC0();
    virtual void unkC4();
    virtual void unkC8(int, int, int, int, float, float, float);
    virtual void unkCC();
    virtual void unkD0();
    virtual float unkD4(int);
};

// HYPOTHESIS: four work words (0xFF, 0, 0xFF, 1) stored ahead of the status vtable (+0x1C)
class ftStatusUniqProcessSwallowedThrownData {
public:
    s32 m_unk00;
    s32 m_unk04;
    s32 m_unk08;
    s32 m_unk0C;
    u8 m_unk10[0xC];
};

class ftStatusUniqProcessSwallowedThrown : public ftStatusUniqProcessSwallowedThrownData, public soStatusUniqProcess {
public:
    ftStatusUniqProcessSwallowedThrown() {
        m_unk00 = 0xff;
        m_unk04 = 0;
        m_unk08 = 0xff;
        m_unk0C = 1;
    }
    virtual ~ftStatusUniqProcessSwallowedThrown() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int status);
};

ftStatusUniqProcessSwallowedThrown g_ftStatusUniqProcessSwallowedThrown;

void ftStatusUniqProcessSwallowedThrown::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftStatusUniqProcessSwallowedThrown::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010) == false) {
        // HYPOTHESIS: threshold constant lbl_27_rodata_24A0 (value taken as 0.0f)
        if (reinterpret_cast<ftControllerModuleView*>(moduleAccesser->m_enumerationStart->m_controllerModule)->unkD4(0) <= 0.0f) {
            moduleAccesser->getStatusModule().changeStatusRequest(0xe, moduleAccesser);
        }
    } else {
        moduleAccesser->getStatusModule().changeStatusRequest(0xe, moduleAccesser);
    }
}

void ftStatusUniqProcessSwallowedThrown::exitStatus(soModuleAccesser* moduleAccesser, int status) {
}

