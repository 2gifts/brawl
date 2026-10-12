#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_kinetic_energy.h>
#include <so/camera/so_camera_module_impl.h>
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
    int next = moduleAccesser->getWorkManageModule().getInt(Dash_Int_Count) + 1;
    moduleAccesser->getWorkManageModule().setInt(next, Dash_Int_Count);
    if (!moduleAccesser->getWorkManageModule().isFlag(Dash_Flag_Step1)) {
        if ((float)next > soValueAccesser::getConstantFloat(moduleAccesser, 0xc33, 0)) {
            moduleAccesser->getWorkManageModule().onFlag(Dash_Flag_Step1);
        }
    }
    if (!moduleAccesser->getWorkManageModule().isFlag(Dash_Flag_Step2)) {
        if ((float)next > soValueAccesser::getConstantFloat(moduleAccesser, 0xc34, 0)) {
            moduleAccesser->getWorkManageModule().onFlag(Dash_Flag_Step2);
        }
    }
    if (!moduleAccesser->getWorkManageModule().isFlag(Dash_Flag_Sound)) {
        moduleAccesser->getWorkManageModule().onFlag(Dash_Flag_Sound);
    }
}

// HYPOTHESIS: the exit skips the speed reset when the next status is one of 4, 0xa, 0x26, 0x89, 0x8c.
// The speed reset on kinetic energy 2 (unresolved virtuals on the energy and ground modules) is not written yet.
void ftStatusUniqProcessDash::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x26 && nextStatus != 0xa && nextStatus != 4 && nextStatus != 0x89 && nextStatus != 0x8c) {
        if (moduleAccesser->getSituationModule().getKind() == 0) {
            // TODO: energy 2 speed reset (lbl_27_rodata_2040 constants)
        }
    }
    moduleAccesser->getCameraModule().exitRun(0);
}
