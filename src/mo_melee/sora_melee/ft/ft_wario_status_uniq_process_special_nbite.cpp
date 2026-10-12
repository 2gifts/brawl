#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <so/controller/so_controller_impl.h>
#include <so/so_module_accesser.h>
#include <so/catch/so_catch_module.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/so_value_accesser.h>
#include <types.h>

// Catch module view. The accesser holds it as a void*; slot offsets are native (setCatch at 0x1c,
// catchCut at 0x2c). The remaining slots are placeholders; the last one (0x3c) takes the damage check.
class ftWarioCatchModuleView {
public:
    virtual ~ftWarioCatchModuleView() { }
    virtual void unk0x0c() { }
    virtual void unk0x10() { }
    virtual void unk0x14() { }
    virtual void unk0x18() { }
    virtual void setCatch(int taskId);
    virtual void unk0x20() { }
    virtual void unk0x24() { }
    virtual void unk0x28() { }
    virtual void catchCut(bool, bool);
    virtual void unk0x30() { }
    virtual void unk0x34() { }
    virtual void unk0x38() { }
    virtual bool checkCatchDamage(soModuleAccesser*, void*);
};

// Controller module view: getButton sits at native slot 0xa0 (40 slots after the dtor), which the
// shared soControllerModule declaration does not reach.
class ftWarioControllerView {
public:
    virtual ~ftWarioControllerView() { }
    virtual void unkPad02() { }
    virtual void unkPad03() { }
    virtual void unkPad04() { }
    virtual void unkPad05() { }
    virtual void unkPad06() { }
    virtual void unkPad07() { }
    virtual void unkPad08() { }
    virtual void unkPad09() { }
    virtual void unkPad10() { }
    virtual void unkPad11() { }
    virtual void unkPad12() { }
    virtual void unkPad13() { }
    virtual void unkPad14() { }
    virtual void unkPad15() { }
    virtual void unkPad16() { }
    virtual void unkPad17() { }
    virtual void unkPad18() { }
    virtual void unkPad19() { }
    virtual void unkPad20() { }
    virtual void unkPad21() { }
    virtual void unkPad22() { }
    virtual void unkPad23() { }
    virtual void unkPad24() { }
    virtual void unkPad25() { }
    virtual void unkPad26() { }
    virtual void unkPad27() { }
    virtual void unkPad28() { }
    virtual void unkPad29() { }
    virtual void unkPad30() { }
    virtual void unkPad31() { }
    virtual void unkPad32() { }
    virtual void unkPad33() { }
    virtual void unkPad34() { }
    virtual void unkPad35() { }
    virtual void unkPad36() { }
    virtual void unkPad37() { }
    virtual void unkPad38() { }
    virtual int getButton();
};

class ftWarioStatusUniqProcessSpecialNBite : public soStatusUniqProcess {
public:
    virtual ~ftWarioStatusUniqProcessSpecialNBite() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* damage);
};

ftWarioStatusUniqProcessSpecialNBite g_ftWarioStatusUniqProcessSpecialNBite;

void ftWarioStatusUniqProcessSpecialNBite::initStatus(soModuleAccesser* moduleAccesser) {
    int origKind = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, moduleAccesser->getStatusModule().getStatusKind(), moduleAccesser, true);
    if (origKind == 0x119) {
        reinterpret_cast<ftWarioCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule)->setCatch(-1);
    }
}

void ftWarioStatusUniqProcessSpecialNBite::execStatus(soModuleAccesser* moduleAccesser) {
    int origKind = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, moduleAccesser->getStatusModule().getStatusKind(), moduleAccesser, true);
    if (origKind == 0x119) {
        return;
    }
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    ftWarioControllerView& controller = reinterpret_cast<ftWarioControllerView&>(moduleAccesser->getControllerModule());
    int frames = work.getInt(0x20000004) - 1;
    bool pressed = false;
    if ((controller.getButton() & soController::getButtonMask((soController::PadButton)1)) ||
        (controller.getButton() & soController::getButtonMask((soController::PadButton)0))) {
        pressed = true;
    }
    if (pressed) {
        frames = (int)moduleAccesser->getConstantFloatKirby(0xfa3);
        moduleAccesser->getMotionModule().setRate(moduleAccesser->getConstantFloatKirby(0xfa2));
    }
    if (frames <= 0) {
        // HYPOTHESIS: constant from lbl_27_rodata_79A0 (value unverified).
        moduleAccesser->getMotionModule().setRate(0.0f);
    }
    work.setInt(frames, 0x20000004);
}

void ftWarioStatusUniqProcessSpecialNBite::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    int origKind = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, nextStatus, moduleAccesser, true);
    if (origKind < 0x11a) {
        if (origKind == 0x3b) {
            return;
        }
    } else if (origKind < 0x11c) {
        return;
    }
    reinterpret_cast<ftWarioCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule)->catchCut(false, false);
}

bool ftWarioStatusUniqProcessSpecialNBite::checkDamage(soModuleAccesser* moduleAccesser, void* damage) {
    return reinterpret_cast<ftWarioCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule)->checkCatchDamage(moduleAccesser, damage);
}
