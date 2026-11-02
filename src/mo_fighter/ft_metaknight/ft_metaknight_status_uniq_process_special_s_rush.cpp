#include <ft/metaknight/ft_metaknight_status_views.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftMetaknightStatusUniqProcessSpecialSRush : public soStatusUniqProcess {
public:
    virtual ~ftMetaknightStatusUniqProcessSpecialSRush() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

// The native steering table is zero, minimum stick, maximum stick.
static const float kRushStickTable[3] = { 0.0f, -1.0f, 1.0f };

// BYTES: the native steering loop keeps one address for its clamp table.
#pragma opt_propagation off
void ftMetaknightStatusUniqProcessSpecialSRush::execStatus(soModuleAccesser* moduleAccesser) {
    const float* table = &kRushStickTable[0];
    if (moduleAccesser->getControllerModule().getStickY() != table[0]) {
        float stick = moduleAccesser->getControllerModule().getStickY();
        float clamped = stick;
        if (stick < table[1]) {
            clamped = table[1];
        } else if (stick > table[2]) {
            clamped = table[2];
        }
        Vec3f rotation = moduleAccesser->getPostureModule().getRot(0);
        rotation.m_x += -clamped * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb4, 0);
        moduleAccesser->getPostureModule().setRot(&rotation, 0);
    }
}

#pragma opt_propagation reset
void ftMetaknightStatusUniqProcessSpecialSRush::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x11a) {
        Vec3f zero(kRushStickTable[0], kRushStickTable[0], kRushStickTable[0]);
        moduleAccesser->getPostureModule().setRot(&zero, 0);
        moduleAccesser->getPostureModule().updateRotYLr();
    } else if (moduleAccesser->getGroundModule().attachGround(0) == true) {
        moduleAccesser->getSituationModule().setKind(Situation_Ground, false);
    }
}

ftMetaknightStatusUniqProcessSpecialSRush g_ftMetaknightStatusUniqProcessSpecialSRush;
