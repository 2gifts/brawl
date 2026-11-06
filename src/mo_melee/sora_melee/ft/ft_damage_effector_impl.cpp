#include <ft/ft_damage_effector_impl.h>
#include <ft/ft_common_data_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/so_module_hyp.h>
#include <types.h>

// HYPOTHESIS: map name soDamageEffectorImpl::reqCommonEffectParam (static); it stays unnamed in sora_melee.
extern "C" void fn_27_60CDC(float power, float reaction, float lr, float param0, float param1, float param2, float param3, float param4,
                            soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData, soCollisionLog* collisionLog);

// HYPOTHESIS: five common fighter parameters handed to the common damage effect.
struct ftDamageEffectCommonParamView {
    u8 unk0[0x36C];
    float unk36C;
    float unk370;
    float unk374;
    float unk378;
    float unk37C;
};

static inline const ftDamageEffectCommonParamView* getDamageEffectCommonParam() {
    return reinterpret_cast<const ftDamageEffectCommonParamView*>(g_ftCommonDataAccesser.getParamCommon());
}

void ftDamageEffectorImpl::reqCommonEffect(float power, float reaction, float lr, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData, soCollisionLog* collisionLog) {
    // BYTES: declared ahead of the loads, which run in reverse argument order, to get the original float registers.
    float param1;
    float param2;
    float param3;
    float param4;
    param4 = getDamageEffectCommonParam()->unk37C;
    param3 = getDamageEffectCommonParam()->unk378;
    param2 = getDamageEffectCommonParam()->unk374;
    param1 = getDamageEffectCommonParam()->unk370;
    fn_27_60CDC(power, reaction, lr, getDamageEffectCommonParam()->unk36C, param1, param2, param3, param4,
                moduleAccesser, attackData, collisionLog);
}

void ftDamageEffectorImpl::reqUniqEffect(soModuleAccesser* moduleAccesser, int level, soCollisionAttackData* attackData) {
    switch (attackData->m_attribute) {
        case soCollisionAttackData::Attribute_Fire:
            moduleAccesser->getEffectModule().reqCommon(level + 3, 0.0f);
            break;
        case soCollisionAttackData::Attribute_Electric:
            moduleAccesser->getEffectModule().reqCommon(level + 7, 0.0f);
            break;
        case soCollisionAttackData::Attribute_Purple:
            moduleAccesser->getEffectModule().reqCommon(level + 11, 0.0f);
            break;
        case soCollisionAttackData::Attribute_Aura:
            moduleAccesser->getEffectModule().reqCommon(level + 15, 0.0f);
            break;
        default:
            moduleAccesser->getEffectModule().reqCommon(2, 0.0f);
            break;
        case soCollisionAttackData::Attribute_Sleep:
        case soCollisionAttackData::Attribute_None2:
            break;
    }
}

void ftDamageEffectorImpl::reqDamageEffectParam(float damageAdd, float reaction, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData) {
    if (damageAdd > 0.0f && reaction >= soValueAccesser::getConstantFloat(moduleAccesser, 0xCD3, 0)) {
        switch (attackData->m_attribute) {
            case soCollisionAttackData::Attribute_Fire:
                moduleAccesser->getEffectModule().reqScreen(2);
                break;
            case soCollisionAttackData::Attribute_Electric:
                moduleAccesser->getEffectModule().reqScreen(3);
                break;
            case soCollisionAttackData::Attribute_Purple:
                moduleAccesser->getEffectModule().reqScreen(5);
                break;
            case soCollisionAttackData::Attribute_Aura:
                moduleAccesser->getEffectModule().reqScreen(6);
                break;
            default:
                moduleAccesser->getEffectModule().reqScreen(1);
                break;
        }
    }
}

void ftDamageEffectorImpl::reqQuake(float frameReaction, soModuleAccesser* moduleAccesser, int level) {
    if (level == 3 && moduleAccesser->getSituationModule().getKind() != Situation_Ground) {
        soCameraModule::QuakeKind kind;
        if (frameReaction >= soValueAccesser::getConstantFloat(moduleAccesser, 0x7F4, 0)) {
            kind = soCameraModule::Quake_XL;
        } else {
            kind = (soCameraModule::QuakeKind)(frameReaction >= soValueAccesser::getConstantFloat(moduleAccesser, 0x7F3, 0));
        }
        if (kind == soCameraModule::Quake_XL && moduleAccesser->getWorkManageModule().getInt(0x1000002C) > 8) {
            kind = soCameraModule::Quake_L;
        }
        moduleAccesser->getCameraModule().reqQuake(kind, moduleAccesser);
    }
}

void ftDamageEffectorImpl::reqShake(soModuleAccesser* moduleAccesser, int situation, Vec2f* normal, soCollisionAttackData* attackData, int hitStopFrame) {
    // HYPOTHESIS: shake kind 1 on the ground, 2 for electric attacks in status 0x54, none for ice or status 0xCA.
    int kind = -1;
    if (attackData->m_attribute != soCollisionAttackData::Attribute_Ice) {
        int statusKind = moduleAccesser->getStatusModule().getStatusKind();
        if (statusKind != 0xCA) {
            if (attackData->m_attribute == soCollisionAttackData::Attribute_Electric) {
                if (statusKind == 0x54) {
                    kind = 2;
                }
            } else {
                kind = situation == 0;
            }
        }
    }
    if (kind != -1) {
        int frame;
        if (attackData->m_attribute == soCollisionAttackData::Attribute_Paralyze) {
            frame = hitStopFrame;
        } else {
            frame = soValueAccesser::getConstantFloat(moduleAccesser, 0x7E6, 0) + hitStopFrame * soValueAccesser::getConstantFloat(moduleAccesser, 0x7E5, 0);
        }
        getShakeModule(moduleAccesser).req(kind, frame, situation == 0, normal);
    }
}

void ftDamageEffectorImpl::reqStop() { }

ftDamageEffectorImpl::~ftDamageEffectorImpl() { }

ftDamageEffectorImpl g_soDamageModuleNullA;
