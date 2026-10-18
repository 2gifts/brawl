#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/yoshi/ft_yoshi_status_uniq_process.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/yoshi/ft_yoshi_special_hi_param.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>

void ftYoshiStatusUniqProcessSpecialHi::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soSituationModule& situation = moduleAccesser->getSituationModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soGroundModule& ground = moduleAccesser->getGroundModule();
    ftYoshiSpecialHiParam* param = static_cast<ftYoshiSpecialHiParam*>(
        g_ftCommonDataAccesser.getData(Fighter_Yoshi)->extendParam[1]);
    if (situation.getKind() == Situation_Ground) {
        ground.setCorrect(soGroundShapeImpl::Correct_Ground, 0);
        kinetic.changeKinetic(6, moduleAccesser);
    } else {
        // HYPOTHESIS: 0x10000040 counts aerial Egg Throw uses; 0x11000013 carries boost speed.
        float speed;
        if (work.getInt(0x10000040) >= param->aerialUseLimit) {
            speed = 0.0f;
        } else {
            speed = work.getInt(0x10000040) == 0 ? param->speedY : work.getFloat(0x11000013);
        }
        ground.setCorrect(soGroundShapeImpl::Correct_Air, 0);
        kinetic.changeKinetic(10, moduleAccesser);
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
        float limit = param->fallSpeedMax;
        if (speed != 0.0f) {
            gravity.m_speedY = speed;
            if (limit < speed) limit = speed;
        }
        gravity.m_gravity = -param->gravity;
        gravity.m_fallSpeedMax = param->fallSpeedMax;
        gravity.unk1C = limit;
        speed *= param->repeatSpeedMul;
        work.setFloat(speed, 0x11000013);
        work.incInt(0x10000040);
    }
}

ftYoshiStatusUniqProcessSpecialHi g_ftYoshiStatusUniqProcessSpecialHi;
