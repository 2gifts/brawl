#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_kinetic_energy.h>
#include <so/camera/so_camera_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessDash : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessDash() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftStatusUniqProcessDash g_ftStatusUniqProcessDash;

// HYPOTHESIS: work ids. 0x20000000 is the step counter, 0x22000010/11/13 are dash flags.
enum {
    Dash_Int_Count = 0x20000000,
    Dash_Flag_Step1 = 0x22000010,
    Dash_Flag_Step2 = 0x22000011,
    Dash_Flag_Sound = 0x22000013,
};

void ftStatusUniqProcessDash::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000013) == true) {
        moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1fa9), false, false, 0);
    }
    moduleAccesser->getCameraModule().setRun(0);
}

void ftStatusUniqProcessDash::execStatus(soModuleAccesser* moduleAccesser) {
    int count = moduleAccesser->getWorkManageModule().getInt(Dash_Int_Count);
    moduleAccesser->getWorkManageModule().setInt(count + 1, Dash_Int_Count);
    if (!moduleAccesser->getWorkManageModule().isFlag(Dash_Flag_Step1)) {
        if ((float)(count + 1) > soValueAccesser::getConstantFloat(moduleAccesser, 0xc33, 0)) {
            moduleAccesser->getWorkManageModule().onFlag(Dash_Flag_Step1);
        }
    }
    if (!moduleAccesser->getWorkManageModule().isFlag(Dash_Flag_Step2)) {
        if ((float)(count + 1) > soValueAccesser::getConstantFloat(moduleAccesser, 0xc34, 0)) {
            moduleAccesser->getWorkManageModule().onFlag(Dash_Flag_Step2);
        }
    }
    if (!moduleAccesser->getWorkManageModule().isFlag(Dash_Flag_Sound)) {
        moduleAccesser->getWorkManageModule().onFlag(Dash_Flag_Sound);
    }
}

// Leaving the dash on the ground slows the horizontal speed (common constant 0xC35, pulled towards 1.0 on slippery ground)
// unless the next status is one that keeps the dash momentum (4, 0xA, 0x26, 0x89 or 0x8C).
void ftStatusUniqProcessDash::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (nextStatus) {
    case 4:
    case 0xA:
    case 0x26:
    case 0x89:
    case 0x8C:
        break;
    default:
        if (moduleAccesser->getSituationModule().getKind() == Situation_Ground) {
            soKineticEnergy* energy = moduleAccesser->getKineticModule().getEnergy(2);
            energy->getSpeed();
            float slowdown = soValueAccesser::getConstantFloat(moduleAccesser, 0xC35, 0);
            float friction = moduleAccesser->getGroundModule().getDownFriction(0);
            if (friction < 1.0f) {
                slowdown += (1.0f - slowdown) * (1.0f - friction);
            }
            Vec3f factor(slowdown, 1.0f, 1.0f);
            energy->mulSpeed(&factor);
        }
        break;
    }
    moduleAccesser->getCameraModule().exitRun(0);
}
