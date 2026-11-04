#include <types.h>
#include <ft/ft_status_uniq_process_gimmick.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/stageobject.h>

void ftStatusUniqProcessGimmickLadder::initLadder(soModuleAccesser* moduleAccesser, soGimmickLadderEventArgs* info) {
    correctX(moduleAccesser, info->m_upperNodePos.m_x, 0);
    if (moduleAccesser->getPostureModule().getLr() == -1.0f) {
        Vec3f rotation = moduleAccesser->getPostureModule().getRot(0);
        rotation.m_y = 180.0f;
        moduleAccesser->getPostureModule().setRot(&rotation, 0);
    }
    moduleAccesser->getStageObject().updateNodeSRT();
}

void ftStatusUniqProcessGimmickLadder::exitLadder(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010)) {
        float lr = moduleAccesser->getPostureModule().getLr();
        float speedX = soValueAccesser::getConstantFloat(moduleAccesser, 0xBD5, 0);
        speedX *= lr;
        Vec2f speed(speedX, 0.0f);
        soKineticEnergyNormal& energy = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(2));
        energy.m_speed = speed;
    }
}
