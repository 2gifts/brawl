#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/motion/so_motion_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Base process defined in src/mo_melee/sora_melee/ft/ft_status_uniq_process_escape.cpp. The derived
// process calls its initStatus directly, so only the declaration is needed here.
class ftStatusUniqProcessEscape : public soStatusUniqProcess {
public:
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

class ftStatusUniqProcessEscapeAirInheritJumpAerialMotion : public ftStatusUniqProcessEscape {
public:
    virtual ~ftStatusUniqProcessEscapeAirInheritJumpAerialMotion() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessEscapeAirInheritJumpAerialMotion g_ftStatusUniqProcessEscapeAirInheritJumpAerialMotion;

void ftStatusUniqProcessEscapeAirInheritJumpAerialMotion::initStatus(soModuleAccesser* moduleAccesser) {
    ftStatusUniqProcessEscape::initStatus(moduleAccesser);

    soMotionModule& motion = moduleAccesser->getMotionModule();
    int kind = motion.getKind();
    float frame = motion.getFrame();

    // HYPOTHESIS: motion kind 0x46 is entered with the constants from lbl_27_rodata_26A0 (frame) and
    // lbl_27_rodata_26A4 (rate); the values are unverified.
    soMotionChangeParam motionParam;
    motionParam.m_kind = 0x46;
    motionParam.m_frame = 0.0f;
    motionParam.m_rate = 1.0f;
    motionParam._12 = 0;
    motionParam._13 = 0;
    motionParam._14 = 0;
    motionParam._15 = 0;
    motion.changeMotionRequest(&motionParam);

    if ((unsigned)(kind - 0x19) <= 1) {
        motion.add2ndAnimChr(frame, 1.0f, kind, false);
        motion.setWeight(1.0f);
        moduleAccesser->getKineticModule().changeKinetic(0x4f, moduleAccesser);
    } else {
        moduleAccesser->getKineticModule().changeKinetic(0x13, moduleAccesser);
    }
}

void ftStatusUniqProcessEscapeAirInheritJumpAerialMotion::execFixPos(soModuleAccesser* moduleAccesser) {
    soMotionModule& motion = moduleAccesser->getMotionModule();
    if (motion.is2ndEnd()) {
        moduleAccesser->getKineticModule().changeKinetic(0x13, moduleAccesser);
        motion.remove2ndAnimChr();
    }
}
