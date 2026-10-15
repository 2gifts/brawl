#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: link module (soModuleEnumeration::m_linkModule, +0x54); slot 0xA8 returns a float
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
};

// HYPOTHESIS: posture module (soModuleEnumeration::m_postureModule, +0x0C); slots 0x30 and 0x54
class ftPostureModuleView {
public:
    virtual ~ftPostureModuleView() { }
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual void unk24();
    virtual void unk28();
    virtual void unk2C();
    virtual void unk30(float);
    virtual void unk34();
    virtual void unk38();
    virtual void unk3C();
    virtual void unk40();
    virtual void unk44();
    virtual void unk48();
    virtual void unk4C();
    virtual void unk50();
    virtual void unk54();
};

// HYPOTHESIS: capture module (soModuleEnumeration::m_captureModule, +0x40); slots 0x30, 0x38, 0x44, 0x4C and 0x54 are used
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
    virtual void unk50();
    virtual void unk54(int);
};

// HYPOTHESIS: stage object (StageObject*), a second vtable sits at +0x3C and slot 0xB4 is used
struct ftStageSecondaryViewPad { u8 m_pad[0x3C]; };
class ftStageSecondaryView : public ftStageSecondaryViewPad {
public:
    virtual ~ftStageSecondaryView() { }
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
};

class ftStatusUniqProcessCatchedGanon : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCatchedGanon() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int status);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* hit);
};

ftStatusUniqProcessCatchedGanon g_ftStatusUniqProcessCatchedGanon;

void ftStatusUniqProcessCatchedGanon::initStatus(soModuleAccesser* moduleAccesser) {
    const soModuleEnumeration* modules = moduleAccesser->m_enumerationStart;
    void* posture = modules->m_postureModule;
    float linkValue = reinterpret_cast<ftLinkModuleView*>(modules->m_linkModule)->unkA8(0);
    reinterpret_cast<ftPostureModuleView*>(posture)->unk30(-1.0f * linkValue);
    reinterpret_cast<ftPostureModuleView*>(moduleAccesser->m_enumerationStart->m_postureModule)->unk54();

    StageObject* stage = moduleAccesser->m_stageObject;
    reinterpret_cast<ftStageSecondaryView*>(stage)->unkB4();

    soMotionModule* motion = moduleAccesser->m_enumerationStart->m_motionModule;
    int flag = 0;
    switch (soValueAccesser::getConstantInt(moduleAccesser, 0x5a98, 0)) {
    case 1:
        flag = 4;
        break;
    }
    // HYPOTHESIS: motion module slot 0x194 is setKindOffset (argument from constant 0x5a98)
    motion->setKindOffset(flag);

    moduleAccesser->getWorkManageModule().setInt(0x1b1, 0x20000000);
}

void ftStatusUniqProcessCatchedGanon::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftStatusUniqProcessCatchedGanon::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    switch (status) {
    case 0x42:
        break;
    case 0xea:
        break;
    default:
        reinterpret_cast<ftCaptureModuleView*>(moduleAccesser->m_enumerationStart->m_captureModule)->unk54(0);
        reinterpret_cast<ftCaptureModuleView*>(moduleAccesser->m_enumerationStart->m_captureModule)->unk30(0, 0);
        break;
    }
}

bool ftStatusUniqProcessCatchedGanon::checkDamage(soModuleAccesser* moduleAccesser, void* hit) {
    return true;
}
