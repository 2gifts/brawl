#define WN_STATUS_PROC_VECTOR2_DEFAULT_CTOR
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/donkey/wn_donkey_sound_effect.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>

wnDonkeySoundEffect::wnDonkeySoundEffect(s32 articleId, const wnArticleBuildInfo& info, const wnDonkeySoundEffectData* data) :
    wnWeaponBuilder<wnDonkeySoundEffectModuleAccesserBuildConfig>(
        "wnDonkeySoundEffect", articleId, info,
        wnModuleAccesserBuildData(0x72, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

// MATCH-ONLY: isolate the native boolean-to-status conversion before the work
// flag call; the original inline helper boundary is not yet established.
static inline int noteStatus(bool hit) { return hit != false; }

// Notes leave the drum at an angle and use the rhythm bonuses for power,
// speed and initial scale. A successful beat selects the hit status and speed.
void wnDonkeySoundEffect::activate(int founderTaskId, int team, Vec3f* position, float angle,
                                  float powerMul, float speedMul, float scale, bool goodStrike) {
    float facing = angle <= 3.1415927f ? 1.0f : -1.0f;
    wnActivateDesc desc(founderTaskId, 0xFFFF, 0xFFFF, 0xFFFF, -1, -1, 0, 0,
                        *position, facing, team, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    int statusKind = noteStatus(goodStrike);
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000000);
    m_moduleAccesser->getStatusModule().changeStatusForce(statusKind, m_moduleAccesser);
    Vec3f rotation;
    rotation.m_x = 0.0f;
    rotation.m_y = 0.0f;
    rotation.m_z = angle * 57.29578f;
    m_moduleAccesser->getPostureModule().setRot(&rotation, 0);
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    Vec2f zero;
    zero.m_x = 0.0f;
    zero.m_y = 0.0f;
    float speed;
    if (goodStrike) {
        speed = m_param->hitSpeed;
    } else {
        speed = m_param->missSpeed;
    }
    speed *= speedMul;
    Vec2f velocity;
    velocity.m_x = 0.0f;
    velocity.m_y = speed;
    m_moduleAccesser->getCollisionAttackModule().setPowerMul(powerMul);
    m_moduleAccesser->getPostureModule().setScale(1.0f);
    float targetScale = scale * m_param->targetScaleMul;
    // HYPOTHESIS: a nonpositive growth time retains the speed multiplier as
    // the growth step, as observed in the original activation path.
    if (m_param->scaleGrowthTime > 0.0f) {
        speedMul = (targetScale - scale) / m_param->scaleGrowthTime;
    }
    m_moduleAccesser->getPostureModule().setScale(scale);
    m_moduleAccesser->getWorkManageModule().setFloat(targetScale, 0x21000000);
    m_moduleAccesser->getWorkManageModule().setFloat(speedMul, 0x21000001);
    velocity.rot(angle, &velocity);
    energy.m_speed = velocity;
    energy.m_accel = zero;
    energy.m_brake = zero;
    energy.m_speedTarget = velocity;
    energy.m_speedLimit = velocity;
    energy.enable();
    updateNodeSRT();
}

// The note grows toward its target scale and applies its current pose to the
// model's root. Both pose copies are the native aggregate assignment calls.
void wnDonkeySoundEffect::updateNodeSRT() {
    float target = m_moduleAccesser->getWorkManageModule().getFloat(0x21000000);
    float scale = m_moduleAccesser->getWorkManageModule().getFloat(0x21000001);
    float currentScale = m_moduleAccesser->getPostureModule().getScale();
    scale = currentScale + scale;
    if (scale > target) {
        scale = target;
    }
    Vec3f nodeScale;
    nodeScale.m_x = nodeScale.m_y = nodeScale.m_z = scale;
    Vec3f rotation;
    Vec3f position;
    position = m_moduleAccesser->getPostureModule().getPos();
    rotation = m_moduleAccesser->getPostureModule().getRot(0);
    m_moduleAccesser->getPostureModule().setScale(scale);
    m_moduleAccesser->getModelModule().setNodeSRT(0, &nodeScale, &rotation, &position);
}
