#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_damage.h>
#include <so/so_module_accesser.h>
#include <types.h>

// HYPOTHESIS: class shape from the shared Down_Damage status (kind 0x54) on top of the damage base.
class ftStatusUniqProcessDownDamage : public ftStatusUniqProcessDamage {
public:
    ftStatusUniqProcessDownDamage() { }
    virtual ~ftStatusUniqProcessDownDamage() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
};

// HYPOTHESIS: lbl_27_rodata_21E8 is 0.0f (the initial and threshold value of the work float 0x21000009).
// HYPOTHESIS: lbl_27_rodata_21EC is the per-frame decrement applied to that work float.
void ftStatusUniqProcessDownDamage::initStatus(soModuleAccesser* moduleAccesser) {
    int prevKind = moduleAccesser->getStatusModule().getPrevStatusKind(0);
    if (prevKind != 0x4d && prevKind != 0x4a && prevKind != 0x54) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000019);
        moduleAccesser->getWorkManageModule().offFlag(0x2200001b);
        float zero = 0.0f;
        moduleAccesser->getWorkManageModule().setFloat(zero, 0x21000009);
    }
    ftStatusUniqProcessDamage::initNormalDamageCommon(moduleAccesser);
    if (moduleAccesser->getSituationModule().getKind() != 0) {
        moduleAccesser->getKineticModule().changeKinetic(0x1f, moduleAccesser);
        moduleAccesser->getWorkManageModule().onFlag(0x2200001c);
    } else {
        moduleAccesser->getKineticModule().changeKinetic(0x1e, moduleAccesser);
    }
}

void ftStatusUniqProcessDownDamage::execStatus(soModuleAccesser* moduleAccesser) {
    ftStatusUniqProcessDamage::execNormalDamageCommon(moduleAccesser);
    float timer = moduleAccesser->getWorkManageModule().getFloat(0x21000009);
    if (timer > 0.0f) {
        moduleAccesser->getWorkManageModule().setFloat(timer - 1.0f, 0x21000009);
    }
}

ftStatusUniqProcessDownDamage g_ftStatusUniqProcessDownDamage;
