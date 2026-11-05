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
    switch (g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(
        0, moduleAccesser->getStatusModule().getStatusKind(), moduleAccesser, true)) {
    case 0x117:
        if (moduleAccesser->getWorkManageModule().getInt(0x20000003) == 0) {
            int charge = moduleAccesser->getWorkManageModule().getInt(0x10000040);
            if (charge >= moduleAccesser->getConstantIntKirby(0x5dc0)) {
                int effect;
                if (moduleAccesser->getWorkManageModule().isFlag(0x22000013) == true) {
                    int node = moduleAccesser->getModelModule().getRealNodeId(0x19c);
                    Vec3f position;
                    Vec3f rotation(0.0f, 0.0f, 0.0f);
                    position.set(0.0f, 0.0f, 0.0f);
                    effect = moduleAccesser->getEffectModule().reqFollow(
                        static_cast<EfID>(0x1f000c), node, &position, &rotation, 1.0f, false, 0, 0, -1);
                } else {
                    Vec3f position;
                    Vec3f rotation(0.0f, 0.0f, 0.0f);
                    position.set(0.0f, 0.0f, 0.0f);
                    effect = moduleAccesser->getEffectModule().reqFollow(
                        static_cast<EfID>(0x1f000c), 8, &position, &rotation, 1.0f, false, 0, 0, -1);
                }
                moduleAccesser->getWorkManageModule().setInt(effect, 0x20000003);
            }
        }
        break;
    case 0x118:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000011) == true) {
            if (!moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
                float rotation;
                if (moduleAccesser->getWorkManageModule().isFlag(0x22000013) == true) {
                    int node = moduleAccesser->getModelModule().getRealNodeId(0x19c);
                    rotation = moduleAccesser->getMotionModule().getNodeRotate(node).m_x;
                } else {
                    rotation = moduleAccesser->getMotionModule().getNodeRotate(7).m_x;
                }
                moduleAccesser->getWorkManageModule().setFloat(rotation, 0x21000005);
                moduleAccesser->getWorkManageModule().onFlag(0x22000012);
            }
            float neutralAngle;
            float angle = moduleAccesser->getControllerModule().getStickY();
            neutralAngle = moduleAccesser->getConstantFloatKirby(0xfa0);
            if (angle >= 0.0f) {
                angle = neutralAngle + angle * (moduleAccesser->getConstantFloatKirby(0xfa1) - neutralAngle);
            } else {
                float minAngle = -moduleAccesser->getConstantFloatKirby(0xfa2);
                angle = neutralAngle + -angle * (minAngle - neutralAngle);
            }
            float previousAngle = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
            float maxStep = moduleAccesser->getConstantFloatKirby(0xfa3);
            float step = angle - previousAngle;
            if (step > maxStep) {
                step = maxStep;
            } else if (step < -maxStep) {
                step = -maxStep;
            }
            angle = previousAngle + step;
            moduleAccesser->getWorkManageModule().setFloat(angle, 0x21000004);
            if (!moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
                float rotation = moduleAccesser->getWorkManageModule().getFloat(0x21000005);
                moduleAccesser->getModelModule().setNodeRotateX(7, rotation - angle);
            }
        }
        break;
    }
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
