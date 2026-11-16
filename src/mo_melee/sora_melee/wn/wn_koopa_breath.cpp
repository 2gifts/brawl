#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define FT_MODULE_BUILDER
#define WN_MARIO_FIREBALL_UNIT // the sora_melee article unit mode shared with the Mario fireball (builders out of line)
#define WN_ZELDA_LIGHTING_BOW_UNIT // the member layout and constructor of the module builder of this unit mode (wn/zelda/wn_zelda_lighting_bow.h)
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_STATUS_NO_TAIL
#define WN_ARTICLE_ANIMCMD_INT // BYTES: the builder vtable takes Weapon's int-index notifyEventAnimCmd thunk (@72@), as in the original
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#include <wn/wn_module_accesser_builder.h>
#include <wn/zelda/wn_zelda_lighting_bow.h>
#include <wn/koopa/wn_koopa_breath.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <gf/gf_task_scheduler.h>
#include <math.h>
#include <mt/mt_prng.h>

// BYTES: the empty base destructors that sora_melee always expands (they have no out-of-line copy there).
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }

// Registers the Bowser breath article class (kind 0x73) in the class info table during static initialization.
wnClassInfoImpl<0x73, wnKoopaBreath, wnKoopaBreathData> g_wnKoopaBreathClassInfo;

wnKoopaBreath::wnKoopaBreath(s32 articleId, const wnArticleBuildInfo& info, const wnKoopaBreathData* data) :
    wnWeaponBuilder<wnKoopaBreathModuleAccesserBuildConfig>(
        "KoopaBreath", articleId, info,
        wnModuleAccesserBuildData(0x73, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &g_soParamAccesserNull,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param) {
    Weapon::deactivate(false);
}

#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnKoopaBreathModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>

// The energy's speed setter is an inline member in the original (expanded here, a weak copy in the fighter modules).
inline void soKineticEnergyNormal::setSpeed(Vec2f* speed) {
    m_speed.m_x = speed->m_x;
    m_speed.m_y = speed->m_y;
}

void wnKoopaBreath::activate(int founderTaskId, int team, const Vec3f* position, float lr, int kind, float speed, float scale,
                              float speedMul, float angle) {
    wnActivateDesc desc(founderTaskId, 0xFFFF, 0xFFFF, 0xFFFF, -1, -1, 0, 0, *position, lr, team, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getWorkManageModule().setInt(kind, 0x10000007);
    m_moduleAccesser->getWorkManageModule().setInt(m_param->m_work5, 0x10000005);
    m_moduleAccesser->getWorkManageModule().setInt(m_param->m_work6, 0x10000006);
    m_moduleAccesser->getWorkManageModule().setFloat(scale, 0x11000001);
    m_moduleAccesser->getGroundModule().setOffsetY(m_param->m_groundOffsetY, 0);
    m_moduleAccesser->getPostureModule().setScale(scale);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    float magnitude = speedMul * (speed * (m_param->m_speedRandMin + (m_param->m_speedRandMax - m_param->m_speedRandMin) * randf()));
    energy.setSpeed(&Vec2f(lr * (magnitude * cosf(angle)), magnitude * sinf(angle)));
    updateNodeSRT();
}

void wnKoopaBreath::updateNodeSRT() {
    Vec3f scaling;
    float scale = m_moduleAccesser->getPostureModule().getScale();
    scaling.m_z = scale;
    scaling.m_y = scale;
    scaling.m_x = scale;
    Vec3f position = m_moduleAccesser->getPostureModule().getPos();
    Vec2f speed = m_moduleAccesser->getKineticModule().getSumSpeed(1);
    Vec3f rotation;
    float lr = m_moduleAccesser->getPostureModule().getLr();
    rotation.m_x = 57.29578f * -atan2f(speed.m_y, speed.m_x * lr);
    rotation.m_y = lr == 1.0f ? 90.0f : -90.0f;
    rotation.m_z = 0.0f;
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scaling, &rotation, &position);
}

bool wnKoopaBreath::notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* moduleAccesser, int index) {
    if (Weapon::notifyEventAnimCmd(cmd, moduleAccesser, index)) {
        return true;
    }
    s8 group = cmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    // Only command types 0 to 10 belong to the article.
    if (cmd->getType() <= -1 || cmd->getType() >= 11) {
        return false;
    }
    switch (cmd->getType()) {
    case 6: {
        // The flame turns toward the slope it touched, a little per command.
        wnKineticEnergyNormal& energy =
            dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
        Vec2f speed = energy.getSpeed();
        float magnitude = speed.length();
        Vec2f normal = moduleAccesser->getGroundModule().getTouchNormal(static_cast<grCollStatus::TouchMask>(0xff), 0);
        float diff = atan2f(normal.m_x, normal.m_y);
        float speedAngle = atan2f(speed.m_x, speed.m_y);
        diff = speedAngle - diff;
        moduleAccesser->getPostureModule().getLr();
        while (diff > 3.1415927f) {
            diff -= 6.2831855f;
        }
        while (diff < -3.1415927f) {
            diff += 6.2831855f;
        }
        float turn = 0.0f;
        if (diff != 0.0f) {
            float between = normal.angle(&speed);
            // BYTES: the right angle is also kept in a local that the compare does not read (the compare takes the
            // literal): that local's initialisation is what places PI/2 ahead of the two rates in the constant pool.
            // HYPOTHESIS: the original compared against a macro constant next to its three local constants.
            float half = 1.5707964f;
            double wide = fabs(between);
            float rate = wide;
            const float near = 0.02f;
            const float far = 0.5f;
            if (rate < 1.5707964f) {
                rate = near * (rate / 3.1415927f);
            } else {
                rate = far * (rate / 3.1415927f);
            }
            if (diff < 0.0f) {
                rate = -rate;
            }
            turn = between * rate;
        }
        speedAngle -= turn;
        while (speedAngle < -3.1415927f) {
            speedAngle += 6.2831855f;
        }
        while (speedAngle > 3.1415927f) {
            speedAngle -= 6.2831855f;
        }
        energy.setSpeed(&Vec2f(magnitude * sinf(speedAngle), magnitude * cosf(speedAngle)));
        return true;
    }
    }
    return false;
}

bool wnKoopaBreath::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}


