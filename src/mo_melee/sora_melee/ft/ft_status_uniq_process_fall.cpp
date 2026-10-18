#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_audience_manager.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_kinetic_energy_controller.h>
#include <math.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftStatusUniqProcessFall : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessFall() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessFall g_ftStatusUniqProcessFall;

// HYPOTHESIS: work ints 0x20000000/0x20000001 hold the second-animation kinds used while leaning left/right (set per
// fall status kind 0xE, 0xF and 0x10), 0x20000002 the kind currently blended in and 0x20000003 a frame counter; work
// float 0x21000004 is the smoothed horizontal stick lean.
void ftStatusUniqProcessFall::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0xE:
        moduleAccesser->getWorkManageModule().setInt(0x21, 0x20000000);
        moduleAccesser->getWorkManageModule().setInt(0x22, 0x20000001);
        break;
    case 0xF:
        moduleAccesser->getWorkManageModule().setInt(0x24, 0x20000000);
        moduleAccesser->getWorkManageModule().setInt(0x25, 0x20000001);
        break;
    case 0x10:
        moduleAccesser->getWorkManageModule().setInt(0x27, 0x20000000);
        moduleAccesser->getWorkManageModule().setInt(0x28, 0x20000001);
        int entryId = moduleAccesser->getWorkManageModule().getInt(0x10000000);
        Vec3f pos = moduleAccesser->getPostureModule().getPos();
        g_ftAudienceManager->m_audience->checkTumultFallSpecial(pos.m_y, entryId);
        moduleAccesser->getEffectModule().reqCommon(0x18, 0.0f);
        break;
    }
    moduleAccesser->getWorkManageModule().setInt(-1, 0x20000002);
    moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x21000004);
    float xSpeedMul = moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    if (xSpeedMul != 1.0f) {
        dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2)).mulXSpeedMax(xSpeedMul);
    }
    int prevKind = moduleAccesser->getStatusModule().getPrevStatusKind(0);
    if (prevKind == 1 || prevKind == 3 || prevKind == 4) {
        moduleAccesser->getWorkManageModule().onFlag(0x22000010);
    }
}

void ftStatusUniqProcessFall::execStatus(soModuleAccesser* moduleAccesser) {
    float lean = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
    float delta = moduleAccesser->getControllerModule().getStickX() - lean;
    float newLean = lean + delta * soValueAccesser::getConstantFloat(moduleAccesser, 0xC4A, 0);
    double leanAbsWide = fabs(newLean);
    float leanAbs = leanAbsWide;
    moduleAccesser->getWorkManageModule().setFloat(newLean, 0x21000004);
    if (leanAbs < soValueAccesser::getConstantFloat(moduleAccesser, 0xC4B, 0)) {
        moduleAccesser->getMotionModule().remove2ndAnimChr();
        soMotionModule& motion = moduleAccesser->getMotionModule();
        motion.setWeightRate(1.0f - motion.getPrevWeight());
        moduleAccesser->getWorkManageModule().setInt(-1, 0x20000002);
    } else {
        int current = moduleAccesser->getWorkManageModule().getInt(0x20000002);
        float lr = reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).getLr();
        int next;
        if (newLean * lr > 0.0f) {
            next = moduleAccesser->getWorkManageModule().getInt(0x20000000);
        } else {
            next = moduleAccesser->getWorkManageModule().getInt(0x20000001);
        }
        if (next != current) {
            moduleAccesser->getMotionModule().remove2ndAnimChr();
            soMotionModule& motion = moduleAccesser->getMotionModule();
            motion.add2ndAnimChr(motion.getFrame(), 1.0f, next, true);
            moduleAccesser->getWorkManageModule().setInt(next, 0x20000002);
        }
        soMotionModule& blend = moduleAccesser->getMotionModule();
        blend.setWeightRate(1.0f - leanAbs - blend.getPrevWeight());
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010) == true) {
        int frames = moduleAccesser->getWorkManageModule().getInt(0x20000003);
        moduleAccesser->getWorkManageModule().setInt(frames + 1, 0x20000003);
    }
}

void ftStatusUniqProcessFall::exitStatus(soModuleAccesser* moduleAccesser, int) {
    moduleAccesser->getMotionModule().remove2ndAnimChr();
    moduleAccesser->getWorkManageModule().setFloat(1.0f, 0x11000001);
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x10) {
        moduleAccesser->getEffectModule().removeCommon(0x18);
    }
}
