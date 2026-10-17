#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/stageobject.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

#include <so/damage/so_damage_util_actor.h>
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
    virtual int unk2C(int);
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
    virtual int unkA4(int);
    virtual float unkA8(int);
    virtual void unkAC();
    virtual u32 unkB0(int);
    virtual void unkB4();
    virtual void unkB8();
    virtual float unkBC(int);
    virtual void unkC0();
    virtual void unkC4();
    virtual void unkC8();
    virtual void unkCC();
    virtual void unkD0();
    virtual void unkD4();
    virtual void unkD8();
    virtual void unkDC();
    virtual void unkE0();
    virtual void unkE4();
    virtual void unkE8();
    virtual void unkEC();
    virtual void unkF0();
    virtual void unkF4();
    virtual void unkF8();
    virtual void unkFC();
    virtual void unk100();
    virtual void unk104();
    virtual void unk108();
    virtual void unk10C();
    virtual void unk110();
    virtual void unk114();
    virtual void unk118();
    virtual void unk11C();
    virtual void unk120();
    virtual void unk124();
    virtual void unk128();
    virtual void unk12C();
    virtual void unk130();
    virtual void unk134();
    virtual void unk138();
    virtual void unk13C();
    virtual void unk140();
    virtual void unk144();
    virtual void unk148();
    virtual void unk14C();
    virtual void unk150();
    virtual void unk154();
    virtual void unk158();
    virtual void unk15C();
    virtual void unk160();
    virtual void unk164();
    virtual void unk168();
    virtual void unk16C();
    virtual void unk170();
    virtual void unk174();
    virtual void unk178();
    virtual void unk17C();
    virtual void unk180();
    virtual void unk184();
    virtual void unk188();
    virtual void unk18C();
    virtual void unk190();
    virtual void unk194(int, int, int);
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
    virtual void unk30(int, int);
    virtual void unk34();
    virtual bool unk38(soModuleAccesser* moduleAccesser, void* hit);
    virtual void unk3C();
    virtual void unk40();
    virtual void unk44(soModuleAccesser* moduleAccesser);
    virtual void unk48();
    virtual void unk4C(int);
};
// HYPOTHESIS: status module (soModuleEnumeration::m_statusModule, +0x70); slot 0x48 returns the status kind
class ftStatusModuleView {
public:
    virtual ~ftStatusModuleView() { }
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
    virtual int unk48();
};
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

// Scale constants from module rodata: lbl_27_rodata_24C0 (0.0) and lbl_27_rodata_24C4 (1.0).

// HYPOTHESIS: damage module (soModuleEnumeration::m_damageModule); a second vtable sits at +0x8 and slots 0x50 is used
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

class ftStatusUniqProcessShoulderedDonkey : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessShoulderedDonkey() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int status);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* hit);
};

ftStatusUniqProcessShoulderedDonkey g_ftStatusUniqProcessShoulderedDonkey;

void ftStatusUniqProcessShoulderedDonkey::initStatus(soModuleAccesser* moduleAccesser) {
    int status = reinterpret_cast<ftStatusModuleView*>(moduleAccesser->m_enumerationStart->m_statusModule)->unk48();
    if (status == 0xd9) {
        float a = soValueAccesser::getConstantFloat(moduleAccesser, 0xd48, 0);
        float b = soValueAccesser::getConstantFloat(moduleAccesser, 0xd49, 0);
        float c = soValueAccesser::getConstantFloat(moduleAccesser, 0xd4a, 0);
        void* damage = moduleAccesser->m_enumerationStart->m_damageModule;
        float damageValue = reinterpret_cast<ftDamageSecondaryView*>(damage)->unk50(0);
        reinterpret_cast<ftControllerModuleView*>(moduleAccesser->m_enumerationStart->m_controllerModule)
            ->unkC8(0, -1, 0, 0, a + b * damageValue, 0.0f, c);
        reinterpret_cast<ftCaptureModuleView*>(moduleAccesser->m_enumerationStart->m_captureModule)->unk3C();
        reinterpret_cast<ftLinkModuleView*>(moduleAccesser->m_enumerationStart->m_linkModule)->unk194(0, 3, 0);
    } else {
        if (reinterpret_cast<ftStatusModuleView*>(moduleAccesser->m_enumerationStart->m_statusModule)->unk48() == 0xda) {
            int kind = 0;
            switch (soValueAccesser::getConstantInt(moduleAccesser, 0x5a98, 0)) {
            case 1:
                kind = 1;
                break;
            case 3:
                kind = 2;
                break;
            }
            // HYPOTHESIS: motion module slot 0x194 is setKindOffset (argument from constant 0x5a98)
            moduleAccesser->getMotionModule().setKindOffset(kind);
        }
    }
}

void ftStatusUniqProcessShoulderedDonkey::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleView*>(moduleAccesser->m_enumerationStart->m_captureModule)->unk44(moduleAccesser);
    int status = reinterpret_cast<ftStatusModuleView*>(moduleAccesser->m_enumerationStart->m_statusModule)->unk48();
    if (status == 0xda) {
        float x = reinterpret_cast<ftControllerModuleView*>(moduleAccesser->m_enumerationStart->m_controllerModule)->unkD4(0);
        float c;
        if (reinterpret_cast<ftLinkModuleView*>(moduleAccesser->m_enumerationStart->m_linkModule)->unkA4(0) == 0) {
            c = soValueAccesser::getConstantFloat(moduleAccesser, 0xd4a, 0);
        } else {
            c = soValueAccesser::getConstantFloat(moduleAccesser, 0xd4b, 0);
        }
        reinterpret_cast<ftControllerModuleView*>(moduleAccesser->m_enumerationStart->m_controllerModule)
            ->unkC8(0, -1, 0, 0, x, 1.0f, c);
    }
}

void ftStatusUniqProcessShoulderedDonkey::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status == 0xda) {
        return;
    }
    if (status == 0xdb) {
        return;
    }
    reinterpret_cast<ftCaptureModuleView*>(moduleAccesser->m_enumerationStart->m_captureModule)->unk30(1, 0);
}

bool ftStatusUniqProcessShoulderedDonkey::checkDamage(soModuleAccesser* moduleAccesser, void* hit) {
    if (reinterpret_cast<ftLinkModuleView*>(moduleAccesser->m_enumerationStart->m_linkModule)->unk2C(0) == 0) {
        return false;
    }
    if (reinterpret_cast<ftLinkModuleView*>(moduleAccesser->m_enumerationStart->m_linkModule)->unkB0(0) == *(u32*)((u8*)hit + 0x98)) {
        return false;
    }
    float reaction = reinterpret_cast<ftLinkModuleView*>(moduleAccesser->m_enumerationStart->m_linkModule)->unkBC(0);
    reaction *= soValueAccesser::getConstantFloat(moduleAccesser, 0x7d2, 0);
    return soDamageUtilActor::getDamageLevel(moduleAccesser, reaction) < 3;
}

