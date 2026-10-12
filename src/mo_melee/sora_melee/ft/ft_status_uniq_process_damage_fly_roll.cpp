#include <ft/ft_status_uniq_process_damage_fly.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

// Angle helpers for the rolling knockback (shared ftUtil, declared locally).
class ftUtil {
public:
    static void setDamageFlyRollAngle(soModuleAccesser* moduleAccesser);
    static void setXAngle(soModuleAccesser* moduleAccesser, float angle);
};

// Knockback variant used while rolling (DamageFlyRoll, status 0xA8).
class ftStatusUniqProcessDamageFlyRoll : public ftStatusUniqProcessDamageFly {
public:
    ftStatusUniqProcessDamageFlyRoll() { }
    virtual ~ftStatusUniqProcessDamageFlyRoll() { }
    virtual void initNormalDamage(soModuleAccesser* moduleAccesser);
    virtual void execNormalDamage(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};

ftStatusUniqProcessDamageFlyRoll g_ftStatusUniqProcessDamageFlyRoll;

void ftStatusUniqProcessDamageFlyRoll::initNormalDamage(soModuleAccesser* moduleAccesser) {
    soMotionChangeParam motionParam;
    motionParam.m_kind = 0xa8;
    motionParam.m_frame = 0.0f; // HYPOTHESIS: constant from lbl_27_rodata_21C8
    motionParam.m_rate = 1.0f;  // HYPOTHESIS: constant from lbl_27_rodata_21CC
    motionParam._12 = 0;
    motionParam._13 = 0;
    motionParam._14 = 0;
    motionParam._15 = 0;
    moduleAccesser->getMotionModule().changeMotionRequest(&motionParam);
    ftStatusUniqProcessDamageFly::initNormalDamageCommon(moduleAccesser);
    ftUtil::setDamageFlyRollAngle(moduleAccesser);
    ftStatusUniqProcessDamageFly::setDamageCamera(moduleAccesser);
}

void ftStatusUniqProcessDamageFlyRoll::execNormalDamage(soModuleAccesser* moduleAccesser) {
    ftStatusUniqProcessDamageFly::execNormalDamageCommon(moduleAccesser);
    ftUtil::setDamageFlyRollAngle(moduleAccesser);
}

void ftStatusUniqProcessDamageFlyRoll::execStop(soModuleAccesser* moduleAccesser) {
    ftStatusUniqProcessDamage::execStop(moduleAccesser);
    ftUtil::setDamageFlyRollAngle(moduleAccesser);
}

void ftStatusUniqProcessDamageFlyRoll::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    ftStatusUniqProcessDamageFly::exitStatus(moduleAccesser, nextStatusKind);
    ftUtil::setXAngle(moduleAccesser, 0.0f); // HYPOTHESIS: constant from lbl_27_rodata_21C8
}
