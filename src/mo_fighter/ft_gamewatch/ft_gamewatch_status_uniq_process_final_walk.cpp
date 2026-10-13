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

// HYPOTHESIS: helper parameter names and formal source types/order; native GP/FP argument values are verified.
// The function map identifies these shared helpers as initWalkSpeed,
// initWalkMotion, calcWalkSpeed, and setWalkMotion, respectively.
extern "C" void fn_27_1652A8(soModuleAccesser* moduleAccesser, int motionKind,
                             int speedWork, int rateWork, int flags,
                             float parameter, float speedScale);
extern "C" void fn_27_164BF0(soModuleAccesser* moduleAccesser, const int* motionKinds,
                             int speedWork, int rateWork, float speedScale);
extern "C" void fn_27_165458(soModuleAccesser* moduleAccesser, int speedWork, float current,
                             float target, float acceleration, float deceleration,
                             float speedLimit, float speedScale);
extern "C" void fn_27_164E48(soModuleAccesser* moduleAccesser, const int* motionKinds,
                             int speedWork, int rateWork, float speedScale);

static ftGameWatchWalkParam* getWalkParam() {
    ftData* data = g_ftCommonDataAccesser.getData(Fighter_GameWatch);
    return *reinterpret_cast<ftGameWatchWalkParam**>(reinterpret_cast<u8*>(data) + 0xB8);
}

void ftGameWatchStatusUniqProcessFinalWalk::initStatus(soModuleAccesser* moduleAccesser) {
    ftGameWatchWalkParam* param = getWalkParam();
    float speedScale = ftUtil::getWalkSpeedMul(moduleAccesser);

    float motionSpeed = param->unk8;
    fn_27_1652A8(moduleAccesser, 0x1f4, 0x21000004, 0x21000005,
                 0, motionSpeed, speedScale);
    fn_27_164BF0(moduleAccesser, s_walkMotionKinds, 0x21000004,
                 0x21000005, speedScale);
    ftGameWatchStatusUniqProcessFinalSwim::initStatus(moduleAccesser);
}

void ftGameWatchStatusUniqProcessFinalWalk::execStatus(soModuleAccesser* moduleAccesser) {
    ftGameWatchWalkParam* param = getWalkParam();
    float speedScale = ftUtil::getWalkSpeedMul(moduleAccesser);
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    float speedLimit = soValueAccesser::getConstantFloat(moduleAccesser, 0xc2e, 0);
    float current = workManage.getFloat(0x21000004);

    fn_27_165458(moduleAccesser, 0x21000004, current, param->unk8,
                 param->unk0, param->unk4, speedLimit, speedScale);
    fn_27_164E48(moduleAccesser, s_walkMotionKinds, 0x21000004,
                 0x21000005, speedScale);
    ftGameWatchStatusUniqProcessFinalSwim::execStatus(moduleAccesser);
}
