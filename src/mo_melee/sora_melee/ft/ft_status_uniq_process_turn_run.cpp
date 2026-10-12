#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_posture_view.h>
#include <so/camera/so_camera_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <ft/ft_kinetic_energy.h>
#include <types.h>

class ftStatusUniqProcessTurnRun : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessTurnRun() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftStatusUniqProcessTurnRun g_ftStatusUniqProcessTurnRun;

// HYPOTHESIS: work ids. 0x21000004/5 are turn-run floats, 0x22000010/11/12/13 turn-run flags, 0x20000000 a counter.
enum {
    TurnRun_Float_Speed = 0x21000004,
    TurnRun_Float_Rate = 0x21000005,
    TurnRun_Flag_Brake = 0x22000010,
    TurnRun_Flag_Rate = 0x22000011,
};

// MATCH-ONLY: ground module queries at vtable offsets 0x170 and 0xd8 are not declared in the shared header;
// they are called through the vtable slots directly (same call sequence as the target).
typedef float (*ftTurnRunGroundFloatFn)(void* self, u32 arg);
typedef int (*ftTurnRunGroundIntFn)(void* self, u32 arg, float a, float b);
static void* turnRunGround(soModuleAccesser* moduleAccesser) {
    return moduleAccesser->m_enumerationStart->m_groundModule;
}
static float turnRunGroundFloat(void* ground) {
    ftTurnRunGroundFloatFn fn = *reinterpret_cast<ftTurnRunGroundFloatFn*>(reinterpret_cast<char*>(*static_cast<void**>(ground)) + 0x170);
    return fn(ground, 0);
}
static int turnRunGroundInt(void* ground, float a, float b) {
    ftTurnRunGroundIntFn fn = *reinterpret_cast<ftTurnRunGroundIntFn*>(reinterpret_cast<char*>(*static_cast<void**>(ground)) + 0xd8);
    return fn(ground, 0, a, b);
}

// HYPOTHESIS: constants from lbl_27_rodata_2060 (not verified): [0] 0.0f rate threshold, [4] rate, [8] rate limit.
static const float s_turnRunRate[3] = {0.0f, 1.0f, 1.0f};

void ftStatusUniqProcessTurnRun::initStatus(soModuleAccesser* moduleAccesser) {
    float postureValue = reinterpret_cast<ftStatusPostureView&>(moduleAccesser->getPostureModule()).unk2c();
    moduleAccesser->getWorkManageModule().setFloat(postureValue, TurnRun_Float_Rate);
    moduleAccesser->getWorkManageModule().offFlag(TurnRun_Flag_Rate);
    moduleAccesser->getWorkManageModule().setFloat(soValueAccesser::getConstantFloat(moduleAccesser, 0xc3b, 0), TurnRun_Float_Speed);
    moduleAccesser->getWorkManageModule().offFlag(TurnRun_Flag_Brake);
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000013) == true) {
        moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1faa), false, false, 0);
    }
    moduleAccesser->getCameraModule().setRun(0);
}

void ftStatusUniqProcessTurnRun::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(TurnRun_Flag_Brake) == true) {
        soValueAccesser::getConstantFloat(moduleAccesser, 0xc39, 0);
        if (!moduleAccesser->getWorkManageModule().isFlag(TurnRun_Flag_Rate)) {
            moduleAccesser->getMotionModule().setRate(s_turnRunRate[0]);
            moduleAccesser->getWorkManageModule().onFlag(TurnRun_Flag_Rate);
        } else {
            float rate = moduleAccesser->getWorkManageModule().getFloat(TurnRun_Float_Rate);
            float product = rate * moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)).m_x;
            if (product <= s_turnRunRate[0]) {
                moduleAccesser->getMotionModule().setRate(s_turnRunRate[1]);
                moduleAccesser->getWorkManageModule().offFlag(TurnRun_Flag_Brake);
            } else if (moduleAccesser->getWorkManageModule().getInt(0x20000000) < 0) {
                void* ground = turnRunGround(moduleAccesser);
                if (turnRunGroundFloat(ground) < s_turnRunRate[1]) {
                    if (turnRunGroundInt(ground, rate, s_turnRunRate[2]) == 1) {
                        moduleAccesser->getMotionModule().setRate(s_turnRunRate[1]);
                        moduleAccesser->getWorkManageModule().offFlag(TurnRun_Flag_Brake);
                    }
                }
            }
        }
    }
}

void ftStatusUniqProcessTurnRun::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus == 0x38) {
        float rate = moduleAccesser->getWorkManageModule().getFloat(TurnRun_Float_Rate);
        reinterpret_cast<ftStatusPostureView&>(moduleAccesser->getPostureModule()).unk30(rate);
    }
    moduleAccesser->getCameraModule().exitRun(0);
}
