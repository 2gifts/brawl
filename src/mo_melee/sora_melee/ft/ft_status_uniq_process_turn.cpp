#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_posture_view.h>
#include <so/camera/so_camera_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessTurn : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessTurn() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftStatusUniqProcessTurn g_ftStatusUniqProcessTurn;

// HYPOTHESIS: work ids. 0x21000004 is the turn speed float, 0x21000005 the turn rate float, 0x22000010/11/12
// are turn flags; names are not verified.
enum {
    Turn_Float_Speed = 0x21000004,
    Turn_Float_Rate = 0x21000005,
    Turn_Flag_Brake = 0x22000010,
    Turn_Flag_Rate = 0x22000011,
    Turn_Flag_Init = 0x22000012,
};

// HYPOTHESIS: constants from the turn literal pool; values not verified.
static const float s_turnRateStart = 1.0f;   // lbl_27_rodata_2058
static const float s_turnRateStep = 0.5f;    // lbl_27_rodata_205C

void ftStatusUniqProcessTurn::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getStatusKind() == 7) {
        moduleAccesser->getWorkManageModule().setFloat(s_turnRateStart, Turn_Float_Rate);
        moduleAccesser->getWorkManageModule().onFlag(Turn_Flag_Brake);
    } else {
        float rate = soValueAccesser::getConstantFloat(moduleAccesser, 0xbdf, 0);
        moduleAccesser->getWorkManageModule().setFloat(rate, Turn_Float_Rate);
        moduleAccesser->getWorkManageModule().offFlag(Turn_Flag_Brake);
    }
    moduleAccesser->getWorkManageModule().offFlag(Turn_Flag_Init);
    moduleAccesser->getWorkManageModule().setFloat(reinterpret_cast<ftStatusPostureView&>(moduleAccesser->getPostureModule()).unk2c(), Turn_Float_Speed);
    reinterpret_cast<ftStatusPostureView&>(moduleAccesser->getPostureModule()).unk34();
    moduleAccesser->getCameraModule().setRun(0);
}

void ftStatusUniqProcessTurn::execStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().offFlag(Turn_Flag_Rate);
    float rate = moduleAccesser->getWorkManageModule().getFloat(Turn_Float_Rate);
    if (rate > s_turnRateStart) {
        moduleAccesser->getWorkManageModule().subFloat(s_turnRateStep, Turn_Float_Rate);
    } else if (!moduleAccesser->getWorkManageModule().isFlag(Turn_Flag_Init)) {
        moduleAccesser->getWorkManageModule().onFlag(Turn_Flag_Init);
        moduleAccesser->getWorkManageModule().onFlag(Turn_Flag_Rate);
    }
    float threshold = soValueAccesser::getConstantFloat(moduleAccesser, 0xc32, 0);
    int limit = soValueAccesser::getConstantInt(moduleAccesser, 0x59f7, 0);
    if (moduleAccesser->getControllerModule().getStickX() * -moduleAccesser->getWorkManageModule().getFloat(Turn_Float_Speed) >= threshold) {
        if ((u8)moduleAccesser->getControllerModule().getFlickX() < limit) {
            moduleAccesser->getWorkManageModule().onFlag(Turn_Flag_Brake);
        }
    }
}

void ftStatusUniqProcessTurn::exitStatus(soModuleAccesser* moduleAccesser, int) {
    moduleAccesser->getCameraModule().exitRun(0);
}
