#include <ft/ft_common_data_accesser.h>
#include <ft/ft_util.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <math.h>
#include <types.h>

// HYPOTHESIS: run parameter block pointer stored at ftData+0xB8; floats at +0x14, +0x18 and +0x1C.
struct ftGameWatchRunParam {
    u8 unk0[0x14];
    float unk14;
    float unk18;
    float unk1C;
};

// HYPOTHESIS: the GameWatch data continues past extendParam; +0xB8 points at the final-smash movement block
// (ft_gamewatch_kinetic_transactor.cpp reads the same pointer).
struct ftGameWatchRunData : public ftData {
    u8 unk94[0x24];
    ftGameWatchRunParam* m_runParam; // 0xB8
};

#include <ft/gamewatch/ft_gamewatch_status_uniq_process_final_swim.h>

class ftGameWatchStatusUniqProcessFinalRun : public ftGameWatchStatusUniqProcessFinalSwim {
public:
    virtual ~ftGameWatchStatusUniqProcessFinalRun() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    void setMotionRate(soModuleAccesser* moduleAccesser);
};

ftGameWatchStatusUniqProcessFinalRun g_ftGameWatchStatusUniqProcessFinalRun;

// Native movement multiplier and countdown threshold.
static const float s_runSpeedMul = 1.0f;
static const float s_runCountdownThreshold = 0.0f;

// The shared walk utility (map names initWalkSpeed and calcWalkSpeed) initializes and updates the final movement
// speed; see src/mo_melee/sora_melee/ft/ft_status_uniq_process_walk_util.cpp.
// HYPOTHESIS: these C-linkage declarations order the parameters as this unit's native calls evaluate their
// arguments (the register assignment is the same under any order); the callee's own source order is unknown.
extern "C" void fn_27_1652A8(soModuleAccesser* moduleAccesser, float speedMax, int motionKind, int speedWork,
                             float speedMul, int rateWork, bool isBack);
extern "C" void fn_27_165458(soModuleAccesser* moduleAccesser, float speed, float speedMax, float accel,
                             float accelAdd, float accelDecay, int speedWork, float speedMul);

void ftGameWatchStatusUniqProcessFinalRun::initStatus(soModuleAccesser* moduleAccesser) {
    ftData* data = g_ftCommonDataAccesser.getData(Fighter_GameWatch);
    fn_27_1652A8(moduleAccesser, static_cast<ftGameWatchRunData*>(data)->m_runParam->unk1C, 0x1f7, 0x21000005,
                 s_runSpeedMul, 0x21000007, false);
    setMotionRate(moduleAccesser);
    ftGameWatchStatusUniqProcessFinalSwim::initStatus(moduleAccesser);
}

void ftGameWatchStatusUniqProcessFinalRun::execStatus(soModuleAccesser* moduleAccesser) {
    ftGameWatchRunParam* param =
        static_cast<ftGameWatchRunData*>(g_ftCommonDataAccesser.getData(Fighter_GameWatch))->m_runParam;
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    float speedScale = soValueAccesser::getConstantFloat(moduleAccesser, 0xc37, 0);
    float current = workManage.getFloat(0x21000005);

    fn_27_165458(moduleAccesser, current, param->unk1C, param->unk14, param->unk18, speedScale, 0x21000005,
                 s_runSpeedMul);
    setMotionRate(moduleAccesser);

    // Advance the countdown only while it is positive.
    if (moduleAccesser->getWorkManageModule().getFloat(0x21000004) > s_runCountdownThreshold) {
        moduleAccesser->getWorkManageModule().subFloat(s_runSpeedMul, 0x21000004);
    }
    ftGameWatchStatusUniqProcessFinalSwim::execStatus(moduleAccesser);
}

void ftGameWatchStatusUniqProcessFinalRun::setMotionRate(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    double magnitude = fabs(workManage.getFloat(0x21000005)); // MATCH-ONLY: the double result is narrowed afterwards
    float speed = (float)magnitude;
    float rate = moduleAccesser->getWorkManageModule().getFloat(0x21000007);
    rate = speed * rate;
    moduleAccesser->getMotionModule().setRate(rate);
}
