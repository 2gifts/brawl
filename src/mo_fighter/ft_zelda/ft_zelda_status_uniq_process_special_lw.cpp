#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_energy.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftZeldaStatusUniqProcessSpecialLw : public soStatusUniqProcess {
public:
    virtual ~ftZeldaStatusUniqProcessSpecialLw() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

void ftZeldaStatusUniqProcessSpecialLw::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: 0x115 is the status kind this transition is entered from.
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x115) {
        soSituationModule& situation = moduleAccesser->getSituationModule();
        soKineticModule& kinetic = moduleAccesser->getKineticModule();
        // HYPOTHESIS: extendParam[3] of the Zelda data points to two float divisors (x, y).
        float* divisor = static_cast<float*>(g_ftCommonDataAccesser.getData(Fighter_Zelda)->extendParam[3]);
        soKineticEnergyNormal& energy1 = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(1));
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(3));
        Vec2f sumSpeed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(-1));
        kinetic.clearSpeedAll();
        sumSpeed.m_x = sumSpeed.m_x / divisor[0];
        sumSpeed.m_y = sumSpeed.m_y / divisor[1];
        Vec2f speed(sumSpeed.m_x, 0.0f); // HYPOTHESIS: zero constant from lbl_104_rodata_58
        Vec2f::copy(stop.m_speed, speed);
        stop.enable();
        if (situation.getKind() != 0) {
            energy1.m_speed.m_y = sumSpeed.m_y;
            energy1.enable();
        }
    }
}

ftZeldaStatusUniqProcessSpecialLw g_ftZeldaStatusUniqProcessSpecialLw;
