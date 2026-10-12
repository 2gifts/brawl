#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/lucas/ft_lucas_status_uniq_process.h>
#include <ft/lucas/ft_lucas.h>
#include <so/so_module_accesser.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/situation/so_situation_module_impl.h>

void ftLucasStatusUniqProcessSpecialLw::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getSituationModule().getKind() == Situation_Air) {
        soKineticModule& kinetic = moduleAccesser->getKineticModule();
        ftLucas& lucas = static_cast<ftLucas&>(moduleAccesser->getStageObject());

        // HYPOTHESIS: parameter group 3 +0xC scales the air launch speed (read at the point of use).
        const u8* params = static_cast<const u8*>(lucas.getExtendParam()[3]);

        soKineticEnergyNormal& airEnergy = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(3));
        Vec2f speed = airEnergy.getSpeed();
        airEnergy.m_speed.m_x = speed.m_x * *reinterpret_cast<const float*>(params + 0xC);
        airEnergy.m_speed.m_y = speed.m_y;

        // The inherited soNullable vtable prefix places clearRotSpeed at slot +0x20.
        kinetic.getEnergy(1)->clearSpeed();
        kinetic.getEnergy(0)->clearSpeed();
    }

    moduleAccesser->getWorkManageModule().setInt(-1, 0x20000002);
    moduleAccesser->getWorkManageModule().setInt(-1, 0x20000003);
}

ftLucasStatusUniqProcessSpecialLw::~ftLucasStatusUniqProcessSpecialLw() {}

ftLucasStatusUniqProcessSpecialLw g_ftLucasStatusUniqProcessSpecialLw;
