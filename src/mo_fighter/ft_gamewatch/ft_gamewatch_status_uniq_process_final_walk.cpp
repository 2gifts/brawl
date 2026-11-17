#include <ft/ft_common_data_accesser.h>
#include <ft/ft_util.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: the final-walk parameter block pointer is at ftData+0xB8;
// the movement values used below are read at offsets +0, +4, and +8.
struct ftGameWatchWalkParam {
    float unk0;
    float unk4;
    float unk8;
};

// HYPOTHESIS: the GameWatch data continues past extendParam; +0xB8 points at the final-smash movement block
// (ft_gamewatch_kinetic_transactor.cpp reads the same pointer).
struct ftGameWatchWalkData : public ftData {
    u8 unk94[0x24];
    ftGameWatchWalkParam* m_walkParam; // 0xB8
};

#include <ft/gamewatch/ft_gamewatch_status_uniq_process_final_swim.h>

class ftGameWatchStatusUniqProcessFinalWalk : public ftGameWatchStatusUniqProcessFinalSwim {
public:
    virtual ~ftGameWatchStatusUniqProcessFinalWalk() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftGameWatchStatusUniqProcessFinalWalk g_ftGameWatchStatusUniqProcessFinalWalk;

// Native motion kinds selected by the shared walk-motion helpers.
static const int s_walkMotionKinds[4] = { 0x1f4, 0x1f3, 0x1f2, 0 };

// The shared walk utility (map names initWalkSpeed, initWalkMotion, calcWalkSpeed and setWalkMotion); see
// src/mo_melee/sora_melee/ft/ft_status_uniq_process_walk_util.cpp.
// HYPOTHESIS: these C-linkage declarations order the parameters as this unit's native calls evaluate their
// arguments (the register assignment is the same under any order); the callee's own source order is unknown.
extern "C" void fn_27_1652A8(soModuleAccesser* moduleAccesser, float speedMax, int motionKind, int speedWork,
                             float speedMul, int rateWork, bool isBack);
extern "C" void fn_27_164BF0(soModuleAccesser* moduleAccesser, const int* motionKinds, int speedWork, float speedMul,
                             int rateWork);
extern "C" void fn_27_165458(soModuleAccesser* moduleAccesser, float speed, float speedMax, float accel,
                             float accelAdd, float accelDecay, int speedWork, float speedMul);
extern "C" void fn_27_164E48(soModuleAccesser* moduleAccesser, const int* motionKinds, int speedWork, float speedMul,
                             int rateWork);

static ftGameWatchWalkParam* getWalkParam() {
    return static_cast<ftGameWatchWalkData*>(g_ftCommonDataAccesser.getData(Fighter_GameWatch))->m_walkParam;
}

void ftGameWatchStatusUniqProcessFinalWalk::initStatus(soModuleAccesser* moduleAccesser) {
    ftGameWatchWalkParam* param = getWalkParam();
    float speedScale = ftUtil::getWalkSpeedMul(moduleAccesser);

    fn_27_1652A8(moduleAccesser, param->unk8, 0x1f4, 0x21000004, speedScale, 0x21000005, false);
    fn_27_164BF0(moduleAccesser, s_walkMotionKinds, 0x21000004, speedScale, 0x21000005);
    ftGameWatchStatusUniqProcessFinalSwim::initStatus(moduleAccesser);
}

void ftGameWatchStatusUniqProcessFinalWalk::execStatus(soModuleAccesser* moduleAccesser) {
    // BYTES: the speed limit is declared before the walk speed multiplier (that order gives the native f30/f31 pairing).
    float speedLimit;
    ftGameWatchWalkParam* param = getWalkParam();
    float speedScale = ftUtil::getWalkSpeedMul(moduleAccesser);
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    speedLimit = soValueAccesser::getConstantFloat(moduleAccesser, 0xc2e, 0);
    float current = workManage.getFloat(0x21000004);

    fn_27_165458(moduleAccesser, current, param->unk8, param->unk0, param->unk4, speedLimit, 0x21000004, speedScale);
    fn_27_164E48(moduleAccesser, s_walkMotionKinds, 0x21000004, speedScale, 0x21000005);
    ftGameWatchStatusUniqProcessFinalSwim::execStatus(moduleAccesser);
}
