#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <so/stageobject.h>

class ftPokeZenigameStatusUniqProcessSpecialN : public soStatusUniqProcess {
public:
    virtual ~ftPokeZenigameStatusUniqProcessSpecialN() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftPokeZenigameStatusUniqProcessSpecialN g_ftPokeZenigameStatusUniqProcessSpecialN;

void ftPokeZenigameStatusUniqProcessSpecialN::initStatus(soModuleAccesser* moduleAccesser) {
    switch (g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(
        0, moduleAccesser->getStatusModule().getStatusKind(), moduleAccesser, true)) {
    case 0x117:
        moduleAccesser->getWorkManageModule().setInt(0, 0x20000003);
        break;
    case 0x118: {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        float speedScale = moduleAccesser->getConstantFloatKirby(0xfa6);
        float charge = work.getInt(0x10000040);
        Vec3f speed(-(charge * speedScale), 0.0f, 0.0f);
        moduleAccesser->getKineticModule().addSpeed(&speed, moduleAccesser);
        break;
    }
    }
    if (moduleAccesser->getStageObject().soGetSubKind() == 5) {
        moduleAccesser->getWorkManageModule().onFlag(0x22000013);
    }
}

void ftPokeZenigameStatusUniqProcessSpecialN::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftPokeZenigameStatusUniqProcessSpecialN::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    int effect = moduleAccesser->getWorkManageModule().getInt(0x20000003);
    if (effect != 0) {
        moduleAccesser->getEffectModule().remove(effect);
        moduleAccesser->getWorkManageModule().setInt(0, 0x20000003);
    }
    switch (g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, nextStatus, moduleAccesser, true)) {
    case 0:
    case 0xe:
        if (g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(
                0, moduleAccesser->getStatusModule().getStatusKind(), moduleAccesser, true) == 0x117) {
            soWorkManageModule& work = moduleAccesser->getWorkManageModule();
            int chargeMax = moduleAccesser->getConstantIntKirby(0x5dc0);
            if (work.getInt(0x10000040) >= chargeMax) {
                moduleAccesser->getEffectModule().reqCommon(0x1a, 0.0f);
            }
        }
        break;
    case 0x1f:
    case 0x20:
    case 0x117:
        break;
    default:
        moduleAccesser->getEffectModule().removeCommon(0x1a);
        break;
    }
}
