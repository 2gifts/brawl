#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_damage.h>
#include <so/damage/so_damage.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

// Hitstun while airborne without launch (the weak air damage motions).
class ftStatusUniqProcessDamageAir : public ftStatusUniqProcessDamage {
public:
    virtual ~ftStatusUniqProcessDamageAir() { }
    virtual void initNormalDamage(soModuleAccesser* moduleAccesser);
    virtual void execNormalDamage(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessDamageAir g_ftStatusUniqProcessDamageAir;

void ftStatusUniqProcessDamageAir::initNormalDamage(soModuleAccesser* moduleAccesser) {
    soDamageLog* damageLog = moduleAccesser->getDamageModule().getDamageLog();
    // HYPOTHESIS: rows are the damage level (clamped to 2), columns the hit height; the fourth row only pads the table.
    int motionKinds[4][3] = { { 0xA1, 0xA1, 0xA1 }, { 0xA2, 0xA2, 0xA2 }, { 0xA3, 0xA3, 0xA3 }, { 0, 0, 0 } };
    int level = damageLog->m_level;
    int height = damageLog->m_height;
    if (level >= 3) {
        level = 2;
    }
    soMotionChangeParam param;
    param.m_kind = motionKinds[level][height];
    param.m_frame = 0.0f;
    param.m_rate = 1.0f;
    param._12 = 0;
    param._13 = 0;
    param._14 = 0;
    param._15 = 0;
    moduleAccesser->getMotionModule().changeMotionRequest(&param);
    ftStatusUniqProcessDamage::initNormalDamageCommon(moduleAccesser);
}

void ftStatusUniqProcessDamageAir::execNormalDamage(soModuleAccesser* moduleAccesser) {
    ftStatusUniqProcessDamage::execNormalDamageCommon(moduleAccesser);
}
