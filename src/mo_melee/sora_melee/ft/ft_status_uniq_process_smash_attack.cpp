#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_smash_attack.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

ftStatusUniqProcessSmashAttack g_ftStatusUniqProcessSmashAttack;

void ftStatusUniqProcessSmashAttack::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x2B:
    case 0x2E:
    case 0x31:
    case 0xA2:
        float reactionMul = soValueAccesser::getConstantFloat(moduleAccesser, 0xC5B, 0);
        moduleAccesser->getDamageModule().setReactionMul2nd(reactionMul);
        moduleAccesser->getEffectModule().reqCommon(0x19, 0.0f);
        break;
    case 0x2C:
    case 0x2F:
    case 0x32:
    case 0xA1:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000003)) {
            float charge = moduleAccesser->getWorkManageModule().getFloat(0x21000000);
            float powerMax = soValueAccesser::getConstantFloat(moduleAccesser, 0xC5A, 0);
            float powerMul = 1.0f + (powerMax - 1.0f) * charge;
            moduleAccesser->getCollisionAttackModule().setPowerMulStatus(powerMul);
        }
        break;
    }
}

void ftStatusUniqProcessSmashAttack::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    int statusKind = moduleAccesser->getStatusModule().getStatusKind();
    bool wasHold = false;
    switch (statusKind) {
    case 0x2B:
    case 0x2E:
    case 0x31:
    case 0xA2:
        wasHold = true;
        break;
    }
    switch (nextStatusKind) {
    case 0x2C:
    case 0x2F:
    case 0x32:
    case 0xA1:
        if (wasHold == true) {
            float frame = moduleAccesser->getMotionModule().getFrame();
            float endFrame = moduleAccesser->getMotionModule().getEndFrame();
            float ratio = endFrame > 0.0f ? frame / endFrame : 0.0f;
            moduleAccesser->getWorkManageModule().setFloat(ratio, 0x21000000);
            moduleAccesser->getWorkManageModule().onFlag(0x22000003);
        } else {
            moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x21000000);
            moduleAccesser->getWorkManageModule().offFlag(0x22000003);
        }
        break;
    }
    if (wasHold == true) {
        moduleAccesser->getEffectModule().removeCommon(0x19);
    }
}
