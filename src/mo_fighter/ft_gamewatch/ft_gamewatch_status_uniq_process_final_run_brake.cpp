#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <math.h>
#include <types.h>

// HYPOTHESIS: run parameter block pointer stored at ftData+0xB8; float at +0x20 is the brake start value.
struct ftGameWatchRunBrakeParam {
    u8 unk0[0x20];
    float m_20;
};

// HYPOTHESIS: base status class of the final statuses (its methods are defined in the FinalSwim unit).
class ftGameWatchStatusUniqProcessFinalSwim : public soStatusUniqProcess {
public:
    virtual ~ftGameWatchStatusUniqProcessFinalSwim() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftGameWatchStatusUniqProcessFinalRunBrake : public ftGameWatchStatusUniqProcessFinalSwim {
public:
    virtual ~ftGameWatchStatusUniqProcessFinalRunBrake() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftGameWatchStatusUniqProcessFinalRunBrake g_ftGameWatchStatusUniqProcessFinalRunBrake;

// HYPOTHESIS: constants from lbl_107_rodata_90 and lbl_107_rodata_94 (values not verified).
static const float s_brakeRateHigh = 1.0f;
static const float s_brakeRateLow = 2.0f;

void ftGameWatchStatusUniqProcessFinalRunBrake::initStatus(soModuleAccesser* moduleAccesser) {
    ftGameWatchRunBrakeParam* param = *reinterpret_cast<ftGameWatchRunBrakeParam**>(
        reinterpret_cast<u8*>(g_ftCommonDataAccesser.getData(Fighter_GameWatch)) + 0xB8);
    moduleAccesser->getWorkManageModule().setFloat(param->m_20, 0x21000006);
    moduleAccesser->getWorkManageModule().offFlag(0x22000010);
    moduleAccesser->getWorkManageModule().offFlag(0x22000011);
    moduleAccesser->getWorkManageModule().onFlag(0x22000012);
    ftGameWatchStatusUniqProcessFinalSwim::initStatus(moduleAccesser);
}

void ftGameWatchStatusUniqProcessFinalRunBrake::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010) == 1) {
        float limit = soValueAccesser::getConstantFloat(moduleAccesser, 0xC39, 0);
        float speedX = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)).m_x;
        if (!moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
            if (fabs(speedX) >= limit) {
                moduleAccesser->getMotionModule().setRate(s_brakeRateHigh);
                moduleAccesser->getWorkManageModule().onFlag(0x22000011);
            }
        } else {
            if (fabs(speedX) <= limit) {
                moduleAccesser->getMotionModule().setRate(s_brakeRateLow);
                moduleAccesser->getWorkManageModule().offFlag(0x22000010);
            }
        }
    }
    if (moduleAccesser->getWorkManageModule().getFloat(0x21000006) > s_brakeRateHigh) {
        moduleAccesser->getWorkManageModule().subFloat(s_brakeRateLow, 0x21000006);
    }
    ftGameWatchStatusUniqProcessFinalSwim::execStatus(moduleAccesser);
}
