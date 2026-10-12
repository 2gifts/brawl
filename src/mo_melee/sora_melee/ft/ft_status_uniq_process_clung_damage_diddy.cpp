#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/stageobject.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: link module (soModuleEnumeration::m_linkModule, +0x54)
class ftLinkModuleView {
public:
    virtual ~ftLinkModuleView() { }
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
    virtual float unkA8(int);
    virtual void unkAC();
    virtual void unkB0();
    virtual void unkB4();
    virtual void unkB8();
    virtual void unkBC();
    virtual void unkC0();
    virtual void unkC4();
    virtual u32 unkC8(int);
    virtual u32 unkCC(int);
};
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
    virtual void unk30();
    virtual void unk34();
    virtual bool unk38(soModuleAccesser* moduleAccesser, void* hit);
    virtual void unk3C(soModuleAccesser* moduleAccesser);
    virtual void unk40();
    virtual void unk44(soModuleAccesser* moduleAccesser);
    virtual void unk48();
    virtual void unk4C(int);
};

// HYPOTHESIS: damage module (soModuleEnumeration::m_damageModule); a second vtable sits at +0x8 and slot 0xB4 is used
struct ftDamageSecondaryViewPad { u8 m_pad[0x8]; };
class ftDamageSecondaryView : public ftDamageSecondaryViewPad {
public:
    virtual ~ftDamageSecondaryView() { }
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
    virtual float unk50(int);
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
    virtual bool unkB4();
};

class ftStatusUniqProcessClungDamageDiddy : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessClungDamageDiddy() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int status);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* hit);
};

ftStatusUniqProcessClungDamageDiddy g_ftStatusUniqProcessClungDamageDiddy;

void ftStatusUniqProcessClungDamageDiddy::initStatus(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleView*>(moduleAccesser->m_enumerationStart->m_captureModule)->unk3C(moduleAccesser);
}

void ftStatusUniqProcessClungDamageDiddy::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleView*>(moduleAccesser->m_enumerationStart->m_captureModule)->unk44(moduleAccesser);
}

void ftStatusUniqProcessClungDamageDiddy::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status != 0xd8) {
        reinterpret_cast<ftCaptureModuleView*>(moduleAccesser->m_enumerationStart->m_captureModule)->unk4C(0);
    }
}

bool ftStatusUniqProcessClungDamageDiddy::checkDamage(soModuleAccesser* moduleAccesser, void* hit) {
    int result = 0;
    void* damage = moduleAccesser->m_enumerationStart->m_damageModule;
    if (reinterpret_cast<ftDamageSecondaryView*>(damage)->unkB4() == 0) {
        if (reinterpret_cast<ftLinkModuleView*>(moduleAccesser->m_enumerationStart->m_linkModule)->unkC8(0) == 1) {
            if (reinterpret_cast<ftLinkModuleView*>(moduleAccesser->m_enumerationStart->m_linkModule)->unkCC(0) == 0) {
                result = 1;
            }
        } else {
            result = 1;
        }
    }
    return result == 1;
}
