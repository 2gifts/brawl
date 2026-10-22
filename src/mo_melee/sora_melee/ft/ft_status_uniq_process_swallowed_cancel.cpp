#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/stageobject.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_energy.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>

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
    virtual Vec3f unk98(int, int, int);
    virtual void unk9C();
    virtual void unkA0();
    virtual void unkA4();
    virtual float unkA8(int);
    virtual void unkAC();
    virtual void unkB0();
};
// HYPOTHESIS: posture module (soModuleEnumeration::m_postureModule, +0x0C)
class ftPostureModuleView {
public:
    virtual ~ftPostureModuleView() { }
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14(Vec3f* pos);
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
// HYPOTHESIS: visibility module (soModuleEnumeration::m_visibilityModule, +0x58)
class ftVisibilityModuleView {
public:
    virtual ~ftVisibilityModuleView() { }
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual void unk24(int);
};
// HYPOTHESIS: effect module (soModuleEnumeration::m_effectModule, +0x88)
class ftEffectModuleView {
public:
    virtual ~ftEffectModuleView() { }
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
    virtual void unk8C(int);
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

class ftStatusUniqProcessSwallowedCancel : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessSwallowedCancel() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int status);
};

ftStatusUniqProcessSwallowedCancel g_ftStatusUniqProcessSwallowedCancel;

void ftStatusUniqProcessSwallowedCancel::initStatus(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftVisibilityModuleView*>(moduleAccesser->m_enumerationStart->m_visibilityModule)->unk24(1);
    reinterpret_cast<ftEffectModuleView*>(moduleAccesser->m_enumerationStart->m_effectModule)->unk8C(1);
    float linkValue = reinterpret_cast<ftLinkModuleView*>(moduleAccesser->m_enumerationStart->m_linkModule)->unkA8(0);
    reinterpret_cast<ftPostureModuleView*>(moduleAccesser->m_enumerationStart->m_postureModule)->unk30(linkValue);
    reinterpret_cast<ftPostureModuleView*>(moduleAccesser->m_enumerationStart->m_postureModule)->unk54();
    reinterpret_cast<ftStageSecondaryView*>(moduleAccesser->m_stageObject)->unkB4();

    Vec3f linkPos = reinterpret_cast<ftLinkModuleView*>(moduleAccesser->m_enumerationStart->m_linkModule)->unk98(0, 0, 1);
    reinterpret_cast<ftCaptureModuleView*>(moduleAccesser->m_enumerationStart->m_captureModule)->unk30(1, 0);
    Vec3f pos = linkPos;
    pos.m_x = pos.m_x + linkValue * soValueAccesser::getConstantFloat(moduleAccesser, 0xd5a, 0);
    pos.m_y = pos.m_y + soValueAccesser::getConstantFloat(moduleAccesser, 0xd5b, 0);
    reinterpret_cast<ftPostureModuleView*>(moduleAccesser->m_enumerationStart->m_postureModule)->unk14(&pos);

    // HYPOTHESIS: angle and radius come from the constants 0xd5d and 0xd5c (scaled by the link value)
    float angle = soValueAccesser::getConstantFloat(moduleAccesser, 0xd5d, 0);
    float radius = soValueAccesser::getConstantFloat(moduleAccesser, 0xd5c, 0);
    // MATCH-ONLY: a Vec3f (z unused) reproduces the original 0xc-byte stack slot for the launch speed
    Vec3f speed;
    speed.m_x = -linkValue * (radius * cosf(angle));
    speed.m_y = radius * sinf(angle);
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    stop.resetEnergy(6, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    stop.enable();

    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    gravity.resetEnergy(0, &Vec2f(0.0f, speed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    gravity.enable();

    float a = soValueAccesser::getConstantFloat(moduleAccesser, 0xd5e, 0);
    float b = soValueAccesser::getConstantFloat(moduleAccesser, 0xd55, 0);
    float c = soValueAccesser::getConstantFloat(moduleAccesser, 0xd54, 0);
    reinterpret_cast<ftControllerModuleView*>(moduleAccesser->m_enumerationStart->m_controllerModule)
        ->unkC8(0, -1, 0, 0, a, b, c);
}

void ftStatusUniqProcessSwallowedCancel::execStatus(soModuleAccesser* moduleAccesser) {
    if (reinterpret_cast<ftControllerModuleView*>(moduleAccesser->m_enumerationStart->m_controllerModule)->unkD4(0) <= 0.0f) {
        moduleAccesser->getStatusModule().changeStatusRequest(0xe, moduleAccesser);
    }
}

void ftStatusUniqProcessSwallowedCancel::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftStatusUniqProcessSwallowedCancel::exitStatus(soModuleAccesser* moduleAccesser, int status) {
}

