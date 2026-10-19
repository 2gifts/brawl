#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/catch/so_catch_module.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/so_value_accesser.h>
#include <types.h>

// The shared catch prefix is verified through slot 0x2C. Damage checks use
// the later native slot at 0x3C; the intervening operations remain unnamed.
class ftWarioCatchModuleView : public soCatchModule {
public:
    virtual void unk30();
    virtual void unk34();
    virtual void unk38();
    virtual bool checkCatchDamage(soModuleAccesser*, void*);
};

// Controller module view: the native button-mask query is at 0xA0. The
// current shared module declaration places getButton at 0xAC.
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
    soMotionModule& motion = moduleAccesser->getMotionModule();
    ftWarioControllerView& controller = reinterpret_cast<ftWarioControllerView&>(moduleAccesser->getControllerModule());
    int frames = work.getInt(0x20000004) - 1;
    bool pressed = false;
    if ((controller.getButton() & soController::getButtonMask((soController::PadButton)1)) ||
        (controller.getButton() & soController::getButtonMask((soController::PadButton)0))) {
        pressed = true;
    }
    if (pressed) {
        frames = (int)moduleAccesser->getConstantFloatKirby(0xfa3);
        motion.setRate(moduleAccesser->getConstantFloatKirby(0xfa2));
    }
    if (frames <= 0) {
        // Return to normal motion speed when the accelerated-bite timer ends.
        motion.setRate(1.0f);
    }
    work.setInt(frames, 0x20000004);
}

void ftWarioStatusUniqProcessSpecialNBite::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    int origKind = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, nextStatus, moduleAccesser, true);
    switch (origKind) {
    case 0x3B: case 0x11A: case 0x11B:
        break;
    default:
        static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->catchCut(false, false);
        break;
    }
}

bool ftWarioStatusUniqProcessSpecialNBite::checkDamage(soModuleAccesser* moduleAccesser, void* damage) {
    return reinterpret_cast<ftWarioCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule)->checkCatchDamage(moduleAccesser, damage);
}
