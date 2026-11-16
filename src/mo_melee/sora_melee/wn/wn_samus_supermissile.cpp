#define WN_ANIMCMD_RESET_MODE_BOX // BYTES: word-aligned reset-mode temporaries (frame size of the anim cmd sub builder)
#define WN_ANIMCMD_STACK_PTR // BYTES: the interpreter stacks stay in a saved register across getManageId (ft_builder_animcmd.h)
#define WN_ANIMCMD_PRESENTER_LOCAL_STORE // BYTES: the interpreters' presenters keep the looked-up list in a local, as in the Mario fireball
#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: activate copies the Vec3f position as an aggregate (word moves), see mt_vector.h
#define WN_EVENT_UNIT_DTOR_INLINE // BYTES: the inside event builder destructor expands the event unit destructors
#define WN_EVENT_UNIT_DTOR_INLINE // BYTES: the inside event builder destructor expands the event unit destructors
#define WN_STATUS_MODULE_KIND_COUNT_ZERO // BYTES: the status module is built with a status kind count of 0 (ft_builder_status.h)
#define WN_ANIMCMD_SUB_BUILDER_EXTERN // the anim cmd sub builder constructor is the Samus bomb unit's weak copy
#define FT_MODULE_BUILDER
#define WN_MARIO_FIREBALL_UNIT // the sora_melee article unit mode shared with the Mario fireball (builders out of line)
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_STATUS_NO_TAIL
#define SO_OTHER_ANIM_CTOR // BYTES: the sora_melee motion builder constructs and destroys its other-animation slot
#define WN_TRANSITION_CTOR_INLINE
#define WN_ARTICLE_ANIMCMD_INT // BYTES: the builder vtable takes Weapon's int-index notifyEventAnimCmd thunk, as in the original
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#include <wn/wn_module_accesser_builder.h>
#include <wn/samus/wn_samus_supermissile.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <ac/ac_anim_cmd_impl.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <gf/gf_task_scheduler.h>
#include <math.h>
#include <memory.h>

// BYTES: the empty base destructors that sora_melee always expands (they have no out-of-line copy there).
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }

extern soStatusUniqProcess g_wnSamusSuperMissileStatusUniqProcess;

// HYPOTHESIS: the -1 is the build data's etc resource slot (the laser passes 0), as for the missile.
wnSamusSuperMissile::wnSamusSuperMissile(s32 articleId, const wnArticleBuildInfo& info, const wnSamusSuperMissileData* data) :
    wnWeaponBuilder<wnSamusSuperMissileModuleAccesserBuildConfig>(
        "SamusSuperMissile", articleId, info,
        wnModuleAccesserBuildData(0x59, data, info.getHeapModule()->getInstanceHeap(),
                                  info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1,
                                  &g_soParamAccesserNull, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param) {
    // Both statuses run the same process.
    soStatusUniqProcess* uniqProcs[2];
    uniqProcs[0] = &g_wnSamusSuperMissileStatusUniqProcess;
    uniqProcs[1] = &g_wnSamusSuperMissileStatusUniqProcess;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 2);
    Weapon::deactivate(false);
}

#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnSamusSuperMissileModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>

// HYPOTHESIS: parameter names; the same activation descriptor as the other projectile articles.
void wnSamusSuperMissile::activate(int founderTaskId, int etcResourceId, int team) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = etcResourceId;
    desc.unk8 = etcResourceId;
    desc.unkC = etcResourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = pos;
    desc.lr = 0.0f;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagBit7 = 1;
    desc.flagBit6 = 1;
    desc.flagsMidPair = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    changeMotion(0, false);
}

const wnSamusSuperMissileData* wnSamusSuperMissile::getData() {
    return static_cast<const wnSamusSuperMissileData*>(m_data);
}

// HYPOTHESIS: the super missile leaves the model origin, offset along x by the speed on the facing side, and flies at work angle 0 or pi.
void wnSamusSuperMissile::toShoot(float lr, float speed) {
    soModuleAccesser* acc = m_moduleAccesser;
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(0, false);
    pos.m_z = acc->getGroundModule().getZ();
    pos.m_x += speed * lr;
    acc->getPostureModule().init(&pos, lr);
    removeConstraint();
    acc->getWorkManageModule().setFloat(lr > 0.0f ? 0.0f : 3.1415927f, 0x11000001);
    updateNodeSRT();
    acc->getStatusModule().changeStatusForce(0, acc);
}

// HYPOTHESIS: while flying (status 0) the model points along the work angle 0x11000001.
void wnSamusSuperMissile::updateNodeSRT() {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 0: {
        float scale = m_moduleAccesser->getPostureModule().getScale();
        Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
        m_moduleAccesser->getPostureModule().getLr();
        float angle = m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
        Vec3f rot(57.295776f * angle, 90.0f, 0.0f);
        Vec3f scaleVec(scale, scale, scale);
        m_moduleAccesser->getModelModule().setNodeSRT(0, &scaleVec, &rot, &pos);
        break;
    }
    default:
        StageObject::updateNodeSRT();
        break;
    }
}

bool wnSamusSuperMissile::notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* moduleAccesser, int index) {
    if (Weapon::notifyEventAnimCmd(cmd, moduleAccesser, index)) {
        return true;
    }
    s8 group = cmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    if (cmd->getType() <= -1 || cmd->getType() >= 11) {
        return false;
    }
    return true;
}

// HYPOTHESIS: while flying, a hit or a clash ends the flight (status 1); a shield ends it too unless it asked for a hop.
bool wnSamusSuperMissile::notifyEventCollisionAttackCheck(u32 flags) {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 0:
        if (flags & 0x13) {
            m_moduleAccesser->getStatusModule().changeStatusForce(1, m_moduleAccesser);
            return false;
        }
        if ((flags & 4) && unkA4Bit7 != 1) {
            m_moduleAccesser->getStatusModule().changeStatusForce(1, m_moduleAccesser);
            return false;
        }
        break;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

// The super missile points along its flight direction (work angle 0x11000001).
static inline void setFlyAngle(wnKineticEnergyNormal& energy, soWorkManageModule& work) {
    Vec2f speed = energy.getSpeed();
    if (speed.length() > 0.00001f) {
        work.setFloat(-atan2f(speed.m_y, speed.m_x), 0x11000001);
    }
}

// HYPOTHESIS: after a reflect the work angle follows the new flight direction and the remaining life is capped at the
// parameter life.
bool wnSamusSuperMissile::reflect() {
    bool result = Weapon::reflect();
    soModuleAccesser* acc = getModuleAccesser();
    soWorkManageModule& work = acc->getWorkManageModule();
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*acc->getKineticModule().getEnergy(0));
    setFlyAngle(energy, work);
    wnSamusSuperMissile& missile = dynamic_cast<wnSamusSuperMissile&>(*acc->m_stageObject);
    int life = missile.getData()->m_param->unk8;
    if (work.getInt(0x10000006) > life) {
        work.setInt(life, 0x10000006);
    }
    return result;
}

bool wnSamusSuperMissile::hop() {
    bool result = Weapon::hop();
    soModuleAccesser* acc = getModuleAccesser();
    soWorkManageModule& work = acc->getWorkManageModule();
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*acc->getKineticModule().getEnergy(0));
    setFlyAngle(energy, work);
    wnSamusSuperMissile& missile = dynamic_cast<wnSamusSuperMissile&>(*acc->m_stageObject);
    int life = missile.getData()->m_param->unk8;
    if (work.getInt(0x10000006) > life) {
        work.setInt(life, 0x10000006);
    }
    return result;
}
