#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// Use the native Fighter observer override before transitive Fighter includes;
// this preserves the verified primary vtable slot of getOwner.
#define FT_MODULE_BUILDER
#include <ft/ft_owner.h>
#include <ft/fighter.h>
#undef FT_MODULE_BUILDER
#include <ft/ft_log_transactor.h>
#include <ft/ft_status_uniq_process_attack_air.h>
#include <math.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessAttackAirInheritJumpAerialMotion : public ftStatusUniqProcessAttackAir {
public:
    virtual ~ftStatusUniqProcessAttackAirInheritJumpAerialMotion() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessAttackAirInheritJumpAerialMotion g_ftStatusUniqProcessAttackAirInheritJumpAerialMotion;

// HYPOTHESIS: an aerial attack started out of a jump picks the neutral / forward / back / up / down aerial motion (0x62..0x66)
// from the stick (a dead zone from the constants 0xc5c and 0xc5d gives the neutral one), and the attack log kind 0xe..0x12
// that goes with it. A jump motion that is still playing (0x19, 0x1a) is kept as a second animation.
void ftStatusUniqProcessAttackAirInheritJumpAerialMotion::initStatus(soModuleAccesser* moduleAccesser) {
    ftStatusUniqProcessAttackAir::initStatus(moduleAccesser);
    soMotionModule& motion = moduleAccesser->getMotionModule();
    soControllerModule& controller = moduleAccesser->getControllerModule();
    int motionKind = motion.getKind();
    float frame = motion.getFrame();
    float stickX = controller.getStickX();
    float stickY = controller.getStickY();
    float lr = reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).getLr();
    int newKind;
    int logKind;
    // MATCH-ONLY: the reused double and the comma expression keep both fabs results in one register ahead of the constant calls.
    double absStick = fabs(stickX);
    if (absStick < soValueAccesser::getConstantFloat(moduleAccesser, 0xc5c, 0)
        && ((absStick = fabs(stickY)), absStick < soValueAccesser::getConstantFloat(moduleAccesser, 0xc5d, 0))) {
        newKind = 0x62;
        logKind = 0xe;
    } else if (controller.isStickSide()) {
        if (stickX * lr >= 0.0f) {
            newKind = 0x63;
            logKind = 0xf;
        } else {
            newKind = 0x64;
            logKind = 0x10;
        }
    } else if (stickY >= 0.0f) {
        newKind = 0x65;
        logKind = 0x11;
    } else {
        newKind = 0x66;
        logKind = 0x12;
    }
    soMotionChangeParam param;
    param.m_kind = newKind;
    param.m_frame = 0.0f;
    param.m_rate = 1.0f;
    param._12 = 0;
    param._13 = 0;
    param._14 = 0;
    param._15 = 0;
    motion.changeMotionRequest(&param);
    if (motionKind == 0x19 || motionKind == 0x1a) {
        motion.add2ndAnimChr(frame, 1.0f, motionKind, false);
        motion.setWeight(1.0f);
        moduleAccesser->getKineticModule().changeKinetic(0x4f, moduleAccesser);
    } else {
        moduleAccesser->getKineticModule().changeKinetic(0x13, moduleAccesser);
    }
    ftLogTransactor::resetLogActionInfo(2, logKind, 0, dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject).getOwner(), moduleAccesser);
}

void ftStatusUniqProcessAttackAirInheritJumpAerialMotion::execFixPos(soModuleAccesser* moduleAccesser) {
    ftStatusUniqProcessAttackAir::execFixPos(moduleAccesser);
    soMotionModule& motion = moduleAccesser->getMotionModule();
    if (motion.is2ndEnd()) {
        moduleAccesser->getKineticModule().changeKinetic(0x13, moduleAccesser);
        motion.remove2ndAnimChr();
    }
}
