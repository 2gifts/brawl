#include <ft/ft_common_data_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

#include <ft/gamewatch/ft_gamewatch_status_uniq_process_final_swim.h>

// HYPOTHESIS: the FinalTurn parameter block is reached through ftData+0xB8;
// its initial turn countdown is at +0xA8.
struct ftGameWatchFinalTurnParam {
    u8 unk0[0xA8];
    float unkA8;
};

class ftGameWatchStatusUniqProcessFinalTurn : public ftGameWatchStatusUniqProcessFinalSwim {
public:
    virtual ~ftGameWatchStatusUniqProcessFinalTurn() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftGameWatchStatusUniqProcessFinalTurn g_ftGameWatchStatusUniqProcessFinalTurn;

static const float s_finalTurnCountdownThreshold = 0.0f;
static const float s_finalTurnCountdownStep = 1.0f;

static ftGameWatchFinalTurnParam* getFinalTurnParam() {
    ftData* data = g_ftCommonDataAccesser.getData(Fighter_GameWatch);
    return *reinterpret_cast<ftGameWatchFinalTurnParam**>(reinterpret_cast<u8*>(data) + 0xB8);
}

void ftGameWatchStatusUniqProcessFinalTurn::initStatus(soModuleAccesser* moduleAccesser) {
    ftGameWatchFinalTurnParam* param = getFinalTurnParam();
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x123) {
        moduleAccesser->getWorkManageModule().setFloat(s_finalTurnCountdownThreshold, 0x21000005);
        moduleAccesser->getWorkManageModule().onFlag(0x22000010);
    } else {
        float countdown = param->unkA8;
        moduleAccesser->getWorkManageModule().setFloat(countdown, 0x21000005);
        moduleAccesser->getWorkManageModule().offFlag(0x22000010);
    }
    moduleAccesser->getWorkManageModule().offFlag(0x22000012);

    // Retain the previous facing for the input test, then reverse the fighter.
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    float facing = moduleAccesser->getPostureModule().getLr();
    workManage.setFloat(facing, 0x21000004);
    moduleAccesser->getPostureModule().reverseLr();
    moduleAccesser->getKineticModule().clearSpeedAll();
    ftGameWatchStatusUniqProcessFinalSwim::initStatus(moduleAccesser);
}

void ftGameWatchStatusUniqProcessFinalTurn::execStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().offFlag(0x22000011);
    // Advance the turn countdown, then signal its completion once.
    if (moduleAccesser->getWorkManageModule().getFloat(0x21000005) >
        s_finalTurnCountdownThreshold) {
        moduleAccesser->getWorkManageModule().subFloat(s_finalTurnCountdownStep, 0x21000005);
    } else if (!moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
        moduleAccesser->getWorkManageModule().onFlag(0x22000012);
        moduleAccesser->getWorkManageModule().onFlag(0x22000011);
    }

    float inputThreshold = soValueAccesser::getConstantFloat(moduleAccesser, 0xc32, 0);
    int turnWindowFrames = soValueAccesser::getConstantInt(moduleAccesser, 0x59f7, 0);
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soControllerModule& controller = moduleAccesser->getControllerModule();
    float oppositeFacing = -workManage.getFloat(0x21000004);
    float stickX = controller.getStickX();

    if ((stickX * oppositeFacing) >= inputThreshold &&
        moduleAccesser->getControllerModule().getFlickX() < turnWindowFrames) {
        moduleAccesser->getWorkManageModule().onFlag(0x22000010);
    }
    ftGameWatchStatusUniqProcessFinalSwim::execStatus(moduleAccesser);
}
