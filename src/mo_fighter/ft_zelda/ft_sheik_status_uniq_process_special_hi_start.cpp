#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_energy.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftSheikStatusUniqProcessSpecialHiStart : public soStatusUniqProcess {
public:
    virtual ~ftSheikStatusUniqProcessSpecialHiStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

void ftSheikStatusUniqProcessSpecialHiStart::initStatus(soModuleAccesser* moduleAccesser) {
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    if (moduleAccesser->getSituationModule().getKind() != 0) {
        // HYPOTHESIS: extendParam[2] of the Sheik data is a pointer to the fall speed constant.
        float* fallSpeed = static_cast<float*>(g_ftCommonDataAccesser.getData(Fighter_Sheik)->extendParam[2]);
        Vec2f sumSpeed;
        Vec2f::copy(sumSpeed, kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(-1)));
        kinetic.clearSpeedAll();
        soKineticEnergyNormal& energy1 = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(1));
        energy1.m_speed.m_y = *fallSpeed;
        energy1.enable();
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(3));
        Vec2f speed(sumSpeed.m_x, 0.0f); // HYPOTHESIS: zero constant from lbl_104_rodata_150
        Vec2f::copy(stop.m_speed, speed);
        stop.enable();
    }
}

ftSheikStatusUniqProcessSpecialHiStart g_ftSheikStatusUniqProcessSpecialHiStart;
