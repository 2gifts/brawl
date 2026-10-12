#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_energy.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Local view of the visibility module: its vtable starts with a virtual destructor, so setWhole is slot 9.
class ftZeldaVisibilityModuleView {
public:
    virtual void activate();
    virtual void setData(void*);
    virtual void set(u32, int);
    virtual void setStatusDefault(u32, u8, bool);
    virtual void setDefaultAll();
    virtual void update();
    virtual void hideAll();
    virtual void setWhole(u8);
};

class ftZeldaStatusUniqProcessSpecialHi3 : public soStatusUniqProcess {
public:
    virtual ~ftZeldaStatusUniqProcessSpecialHi3() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

void ftZeldaStatusUniqProcessSpecialHi3::initStatus(soModuleAccesser* moduleAccesser) {
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soSituationModule& situation = moduleAccesser->getSituationModule();
    // HYPOTHESIS: extendParam[2] of the Zelda data points to this status' float parameters.
    float* params = static_cast<float*>(g_ftCommonDataAccesser.getData(Fighter_Zelda)->extendParam[2]);
    Vec2f sumSpeed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(-1));
    kinetic.clearSpeedAll();
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(3));
    soKineticEnergyNormal& normal = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(1));
    if (situation.getKind() == 0) {
        kinetic.changeKinetic(6, moduleAccesser);
        sumSpeed.m_x = sumSpeed.m_x * params[11];
        sumSpeed.m_y = 0.0f; // HYPOTHESIS: constant from lbl_104_rodata_50
        stop.m_speed = sumSpeed;
        stop.enable();
    } else {
        kinetic.changeKinetic(0xb, moduleAccesser);
        sumSpeed.m_x = sumSpeed.m_x * params[11];
        sumSpeed.m_y = sumSpeed.m_y * params[11];
        stop.m_speed = sumSpeed;
        stop.enable();
        normal.disable();
    }
    reinterpret_cast<ftZeldaVisibilityModuleView*>(&moduleAccesser->getVisibilityModule())->setWhole(1);
}

void ftZeldaStatusUniqProcessSpecialHi3::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soSituationModule& situation = moduleAccesser->getSituationModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    if (situation.getKind() != 0) {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(3));
        soKineticEnergyNormal& normal = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(1));
        Vec2f sumSpeed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1));
        if (workManage.isFlag(0x22000011)) {
            // HYPOTHESIS: extendParam[2] holds the float parameters; ftParam+0x7C is the speed limit base.
            float* params = static_cast<float*>(g_ftCommonDataAccesser.getData(Fighter_Zelda)->extendParam[2]);
            ftParam* param = g_ftCommonDataAccesser.getParam(Fighter_Zelda);
            normal.m_speed.m_y = sumSpeed.m_y;
            normal.enable();
            float limit = *reinterpret_cast<float*>(reinterpret_cast<u8*>(param) + 0x7C) * params[9];
            if (sumSpeed.m_x < -limit) {
                sumSpeed.m_x = -limit;
            } else if (sumSpeed.m_x > limit) {
                sumSpeed.m_x = limit;
            }
            sumSpeed.m_y = 0.0f; // HYPOTHESIS: constant from lbl_104_rodata_50
            stop.m_speed.m_x = sumSpeed.m_x;
            stop.m_speed.m_y = sumSpeed.m_y;
        } else {
            Vec2f speed = stop.getSpeed();
            const float divisor = 1.5f; // HYPOTHESIS: value of lbl_104_rodata_54 (not verified)
            Vec2f adjusted;
            adjusted.m_x = speed.m_x;
            adjusted.m_y = speed.m_y - speed.m_y / divisor;
            stop.m_speed = adjusted;
        }
    }
}

ftZeldaStatusUniqProcessSpecialHi3 g_ftZeldaStatusUniqProcessSpecialHi3;
