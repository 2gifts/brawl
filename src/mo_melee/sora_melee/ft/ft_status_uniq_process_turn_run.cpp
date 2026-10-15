#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_posture_view.h>
#include <so/camera/so_camera_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
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

void ftStatusUniqProcessTurnRun::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().setFloat(reinterpret_cast<ftStatusPostureView&>(moduleAccesser->getPostureModule()).unk2c(), TurnRun_Float_Rate);
    moduleAccesser->getWorkManageModule().offFlag(TurnRun_Flag_Rate);
    float speed = soValueAccesser::getConstantFloat(moduleAccesser, 0xc3b, 0);
    moduleAccesser->getWorkManageModule().setFloat(speed, TurnRun_Float_Speed);
    moduleAccesser->getWorkManageModule().offFlag(TurnRun_Flag_Brake);
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000013) == true) {
        moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1faa), false, false, 0);
    }
    moduleAccesser->getCameraModule().setRun(0);
}

// HYPOTHESIS: the braking turn releases the motion rate once the signed speed along the turn direction reaches zero, or while
// the ground is not slippery and the fighter is not near a cliff.
void ftStatusUniqProcessTurnRun::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(TurnRun_Flag_Brake) == true) {
        soValueAccesser::getConstantFloat(moduleAccesser, 0xc39, 0);
        if (!moduleAccesser->getWorkManageModule().isFlag(TurnRun_Flag_Rate)) {
            moduleAccesser->getMotionModule().setRate(0.0f);
            moduleAccesser->getWorkManageModule().onFlag(TurnRun_Flag_Rate);
        } else {
            float rate = moduleAccesser->getWorkManageModule().getFloat(TurnRun_Float_Rate);
            float product = rate * moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)).m_x;
            bool released = product <= 0.0f;
            if (released) {
                moduleAccesser->getMotionModule().setRate(1.0f);
                moduleAccesser->getWorkManageModule().offFlag(TurnRun_Flag_Brake);
            } else if (moduleAccesser->getWorkManageModule().getInt(0x20000000) < 0) {
                if (moduleAccesser->getGroundModule().getDownFriction(0) < 1.0f) {
                    if (moduleAccesser->getGroundModule().isNearCliff(rate, 0.01f, 0) == true) {
                        moduleAccesser->getMotionModule().setRate(1.0f);
                        moduleAccesser->getWorkManageModule().offFlag(TurnRun_Flag_Brake);
                    }
                }
            }
        }
    }
}

void ftStatusUniqProcessTurnRun::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus == 0x38) {
        reinterpret_cast<ftStatusPostureView&>(moduleAccesser->getPostureModule()).unk30(moduleAccesser->getWorkManageModule().getFloat(TurnRun_Float_Rate));
    }
    moduleAccesser->getCameraModule().exitRun(0);
}
