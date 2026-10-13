#include <ft/ft_status_uniq_process_catched_air_ganon.h>
#include <ft/ft_status_uniq_process_views.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/stageobject.h>
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


// Globals are registered in this order by the static initializer.
ftStatusUniqProcessCatchedAirGanon g_ftStatusUniqProcessCatchedAirGanon;
ftStatusUniqProcessCatchedAirFallGanon g_ftStatusUniqProcessCatchedAirFallGanon;
ftStatusUniqProcessCatchedAirEndGanon g_ftStatusUniqProcessCatchedAirEndGanon;

void ftStatusUniqProcessCatchedAirGanon::initStatus(soModuleAccesser* moduleAccesser) {
    // Same opening as ClungGanon::initStatus: face away from the attacker, then pick the motion kind offset.
    const soModuleEnumeration* modules = moduleAccesser->m_enumerationStart;
    void* posture = modules->m_postureModule;
    float linkValue = reinterpret_cast<ftLinkModuleView*>(modules->m_linkModule)->unkA8(0);
    reinterpret_cast<ftPostureModuleView*>(posture)->unk30(-linkValue);
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
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot3c();
}

void ftStatusUniqProcessCatchedAirGanon::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot44();
}

void ftStatusUniqProcessCatchedAirGanon::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    // HYPOTHESIS: the capture is released for every status outside 0xE9..0xEA (exact order still unverified).
    if (status < 0xEB) {
        if (status >= 0xE9) {
            return;
        }
    }
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot30(0, 0);
}

bool ftStatusUniqProcessCatchedAirGanon::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    return true;
}

void ftStatusUniqProcessCatchedAirFallGanon::initStatus(soModuleAccesser* moduleAccesser) {
    soMotionModule* motion = moduleAccesser->m_enumerationStart->m_motionModule;
    int flag = 0;
    switch (soValueAccesser::getConstantInt(moduleAccesser, 0x5a98, 0)) {
    case 1:
        flag = 4;
        break;
    }
    // HYPOTHESIS: motion module slot 0x194 is setKindOffset (argument from constant 0x5a98)
    motion->setKindOffset(flag);
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot3c();
}

void ftStatusUniqProcessCatchedAirFallGanon::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot44();
}

void ftStatusUniqProcessCatchedAirFallGanon::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status == 0xEA) {
        return;
    }
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot30(0, 0);
}

bool ftStatusUniqProcessCatchedAirFallGanon::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    return true;
}

void ftStatusUniqProcessCatchedAirEndGanon::initStatus(soModuleAccesser* moduleAccesser) {
    soMotionModule* motion = moduleAccesser->m_enumerationStart->m_motionModule;
    int flag = 0;
    switch (soValueAccesser::getConstantInt(moduleAccesser, 0x5a98, 0)) {
    case 1:
        flag = 4;
        break;
    }
    // HYPOTHESIS: motion module slot 0x194 is setKindOffset (argument from constant 0x5a98)
    motion->setKindOffset(flag);
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot3c();
}

void ftStatusUniqProcessCatchedAirEndGanon::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot44();
}

void ftStatusUniqProcessCatchedAirEndGanon::exitStatus(soModuleAccesser* moduleAccesser, int) {
    // HYPOTHESIS: the two range tests on the same value cannot both hold, so the first branch never runs; the
    // original spelling is unknown and this form is only kept because it compiles to the same branches.
    int value = soValueAccesser::getVariableInt(moduleAccesser, 0x4e23, 0);
    if (value == 0xe8 && value == 0xe9) {
        reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot54(0);
        reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot30(0, 0);
    } else {
        // HYPOTHESIS: motion module slot 0x48 is isEnd().
        if (moduleAccesser->m_enumerationStart->m_motionModule->isEnd()) {
            reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot54(0);
        }
        reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot30(0, 0);
    }
}

bool ftStatusUniqProcessCatchedAirEndGanon::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot54(0);
    return true;
}
