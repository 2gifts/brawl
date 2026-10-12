#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_status_posture_view.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Helper for the falcon punch status (fn_27_32907C in the target, map name soModuleAccesser__process_air); called from
// execStatus while the situation is air. Not reconstructed yet.
extern "C" void fn_27_32907C(soModuleAccesser* moduleAccesser);

class ftCaptainStatusUniqProcessFalconPunch : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFalconPunch() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftCaptainStatusUniqProcessFalconPunchTurn : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFalconPunchTurn() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftCaptainStatusUniqProcessFalconPunch g_ftCaptainStatusUniqProcessFalconPunch;
ftCaptainStatusUniqProcessFalconPunchTurn g_ftCaptainStatusUniqProcessFalconPunchTurn;

void ftCaptainStatusUniqProcessFalconPunch::initStatus(soModuleAccesser* moduleAccesser) {
    // Work float 0x21000004 receives the current posture value.
    float posture = reinterpret_cast<ftStatusPostureView&>(moduleAccesser->getPostureModule()).unk2c();
    moduleAccesser->getWorkManageModule().setFloat(posture, 0x21000004);
}

void ftCaptainStatusUniqProcessFalconPunch::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        fn_27_32907C(moduleAccesser);
    }
}

// HYPOTHESIS: the motion energy keeps a float at +0x38 (the shared header models it as unk38) and a float at +0x44
// that is negated when the stored value changes. Accessed by offset because the shared header types them differently.
static float& ftCaptainFalconPunchEnergyFloat(ftKineticEnergyMotion& energy, int offset) {
    return *reinterpret_cast<float*>(reinterpret_cast<char*>(&energy) + offset);
}

void ftCaptainStatusUniqProcessFalconPunchTurn::initStatus(soModuleAccesser* moduleAccesser) { }

void ftCaptainStatusUniqProcessFalconPunchTurn::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    int kind = moduleAccesser->getSituationModule().getKind();
    int prevKind = workManage.getInt(0x20000002);
    workManage.setInt(kind, 0x20000002);
    if (kind != prevKind) {
        ftKineticEnergyMotion& energy =
            dynamic_cast<ftKineticEnergyMotion&>(*moduleAccesser->getKineticModule().getEnergy(0));
        float posture = workManage.getFloat(0x21000004);
        if (posture != ftCaptainFalconPunchEnergyFloat(energy, 0x38)) {
            ftCaptainFalconPunchEnergyFloat(energy, 0x44) = -ftCaptainFalconPunchEnergyFloat(energy, 0x44);
            ftCaptainFalconPunchEnergyFloat(energy, 0x38) = posture;
        }
    }
    if (kind == 2) {
        fn_27_32907C(moduleAccesser);
    }
}
