#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <math.h>
#include <types.h>

// HYPOTHESIS: run parameter block pointer stored at ftData+0xB8; float at +0x20 initializes brake work.
struct ftGameWatchRunBrakeParam {
    u8 unk0[0x20];
    float unk20;
};

#include <ft/gamewatch/ft_gamewatch_status_uniq_process_final_swim.h>

class ftGameWatchStatusUniqProcessFinalRunBrake : public ftGameWatchStatusUniqProcessFinalSwim {
public:
    virtual ~ftGameWatchStatusUniqProcessFinalRunBrake() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftGameWatchStatusUniqProcessFinalRunBrake g_ftGameWatchStatusUniqProcessFinalRunBrake;

// Native rodata values select the two motion phases and advance the brake timer.
static const float s_brakeMotionStartRate = 0.0f;
static const float s_brakeMotionFinishRate = 1.0f;

void ftGameWatchStatusUniqProcessFinalRunBrake::initStatus(soModuleAccesser* moduleAccesser) {
    ftGameWatchRunBrakeParam* param =
        *reinterpret_cast<ftGameWatchRunBrakeParam**>(
            reinterpret_cast<u8*>(g_ftCommonDataAccesser.getData(Fighter_GameWatch)) + 0xB8);
    float countdown = param->unk20;
    moduleAccesser->getWorkManageModule().setFloat(countdown, 0x21000006);
    moduleAccesser->getWorkManageModule().offFlag(0x22000010);
    moduleAccesser->getWorkManageModule().offFlag(0x22000011);
    moduleAccesser->getWorkManageModule().onFlag(0x22000012);
    ftGameWatchStatusUniqProcessFinalSwim::initStatus(moduleAccesser);
}

void ftGameWatchStatusUniqProcessFinalRunBrake::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010) == 1) {
        float speedLimit = soValueAccesser::getConstantFloat(moduleAccesser, 0xc39, 0);
        float speedX = moduleAccesser->getKineticModule()
                           .getSumSpeed(soKineticEnergy::AttributeFlag(1)).m_x;

        if (!moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
            if (static_cast<float>(fabs(speedX)) >= speedLimit) {
                moduleAccesser->getMotionModule().setRate(s_brakeMotionStartRate);
                moduleAccesser->getWorkManageModule().onFlag(0x22000011);
            }
        } else if (static_cast<float>(fabs(speedX)) <= speedLimit) {
            moduleAccesser->getMotionModule().setRate(s_brakeMotionFinishRate);
            moduleAccesser->getWorkManageModule().offFlag(0x22000010);
        }
    }

    // Advance the countdown only while it is positive.
    if (moduleAccesser->getWorkManageModule().getFloat(0x21000006) > s_brakeMotionStartRate) {
        moduleAccesser->getWorkManageModule().subFloat(s_brakeMotionFinishRate, 0x21000006);
    }
    ftGameWatchStatusUniqProcessFinalSwim::execStatus(moduleAccesser);
}
