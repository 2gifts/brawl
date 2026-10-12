#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: base status class (FinalSwim unit defines its methods; see final_walk).
class ftGameWatchStatusUniqProcessFinalSwim : public soStatusUniqProcess {
public:
    virtual ~ftGameWatchStatusUniqProcessFinalSwim() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

class ftGameWatchStatusUniqProcessFinalDash : public ftGameWatchStatusUniqProcessFinalSwim {
public:
    virtual ~ftGameWatchStatusUniqProcessFinalDash() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftGameWatchStatusUniqProcessFinalDash g_ftGameWatchStatusUniqProcessFinalDash;

void ftGameWatchStatusUniqProcessFinalDash::exitStatus(soModuleAccesser* moduleAccesser, int kind) {
    switch (kind) {
    case 0x120:
    case 0x121:
    case 0x122:
    case 0x123:
    case 0x124:
    case 0x125:
    case 0x126:
    case 0x127:
    case 0x128:
    case 0x129:
    case 0x12A:
    case 0x12B:
    case 0x12C:
    case 0x12D:
    case 0x12E:
    case 0x12F:
    case 0x130:
    case 0x131:
    case 0x132:
    case 0x133:
    case 0x134:
    case 0x135:
    case 0x136:
    case 0x137:
        break;
    default: {
        soKineticEnergyNormal& energy = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(2));
        Vec2f speed = energy.getSpeed();
        speed.m_x *= soValueAccesser::getConstantFloat(moduleAccesser, 0xc35, 0);
        energy.m_speed.m_x = speed.m_x;
        energy.m_speed.m_y = speed.m_y;
        break;
    }
    }
    ftGameWatchStatusUniqProcessFinalSwim::exitStatus(moduleAccesser, kind);
}
