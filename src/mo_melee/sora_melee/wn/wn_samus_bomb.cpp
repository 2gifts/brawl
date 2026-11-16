#define WN_ANIMCMD_RESET_MODE_BOX // BYTES: word-aligned reset-mode temporaries (frame size of the anim cmd sub builder)
#define WN_ANIMCMD_STACK_PTR // BYTES: the interpreter stacks stay in a saved register across getManageId (ft_builder_animcmd.h)
#define WN_ANIMCMD_PRESENTER_LOCAL_STORE // BYTES: the interpreters' presenters keep the looked-up list in a local, as in the Mario fireball
#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: activate copies the Vec3f position as an aggregate (word moves), see mt_vector.h
#define WN_EVENT_UNIT_DTOR_INLINE // BYTES: the inside event builder destructor expands the event unit destructors
#define WN_STATUS_MODULE_KIND_COUNT_ZERO // BYTES: the status module is built with a status kind count of 0 (ft_builder_status.h)
#define SO_EVENT_OBSERVER_MANAGE_S16 // the observer registration keeps the manage id as the s16 the article returns (so_event_presenter.h)
#define WN_EVENT_UNIT_NULL_CTOR_OWNER // this unit owns the weak constructors of the disabled event units (so_event_system.h)
#define SO_COLLISION_GROUP_ALIGNED // the collision group records are word-aligned (so_collision_group.h)
#define WN_KINETIC_MEDIATOR_INLINE_DTOR // the kinetic builder destructor this unit owns expands the empty mediator destructor
#define WN_KINETIC_GRAVITY_INLINE_DTOR // the article destructor expands the empty gravity energy destructor
#define SO_COLLISION_SEARCH_OBSERVER_INLINE_CTOR // the article constructor expands the observer's registering constructor
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
#include <wn/samus/wn_samus_bomb.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <ac/ac_anim_cmd_impl.h>
#include <math.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <gf/gf_task_scheduler.h>
#include <memory.h>

// BYTES: the empty base destructors that sora_melee always expands (they have no out-of-line copy there).
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }

extern soStatusUniqProcess g_wnSamusBombStatusUniqProcess;

// BYTES: the constructor addresses each constant by its own address (no pooled anchor register).
#pragma push
#pragma pool_data off
wnSamusBomb::wnSamusBomb(s32 articleId, const wnArticleBuildInfo& info, const wnSamusBombData* data) :
    wnWeaponBuilder<wnSamusBombModuleAccesserBuildConfig>(
        "SamusBomb", articleId, info,
        wnModuleAccesserBuildData(0x54, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1,
                                  &g_soParamAccesserNull, 0, 0, 0, 0, 0, 0, 0)),
    soCollisionSearchEventObserver(-1, -1), m_param(data->m_param),
    m_energyGravity(0.0f, -data->m_param->m_gravity, data->m_param->m_speedLimit) {
    m_moduleAccesser->getKineticModule().addEnergy(&m_energyGravity, 2, soKineticEnergy::AttributeFlag(1), -1);
    m_moduleAccesser->getKineticModule().unableEnergy(2);
    // Both statuses run the same process.
    soStatusUniqProcess* uniqProcs[2];
    uniqProcs[0] = &g_wnSamusBombStatusUniqProcess;
    uniqProcs[1] = &g_wnSamusBombStatusUniqProcess;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 2);
    // The bomb watches its own search collision.
    soEventObserver<soCollisionSearchEventObserver>::addObserverSubS16(getArticleEventManageId(), &static_cast<soCollisionSearchEventObserver&>(*this), -1);
    Weapon::deactivate(false);
}
#pragma pop

// The weak builders this unit owns in the link order (the other units with the same configurations reuse them).
typedef wnSamusBombModuleAccesserBuildConfig SbBC;
template soInsideEventManageModuleBuilder<SbBC::InsideEventManageModuleBuildConfig, SbBC::InsideEventManageModuleTypes>::soInsideEventManageModuleBuilder();
template soModelModuleBuilder<SbBC::ModelModuleBuildConfig>::soModelModuleBuilder(soModuleAccesser*, float, void*, soEventObserverRegistrationDesc*);
// BYTES: the original addresses each singleton's destructor-chain record by its own address (no pooled anchor).
#pragma push
#pragma pool_data off
template soMotionModuleBuilder<SbBC::MotionModuleBuildConfig>::soMotionModuleBuilder(soModuleAccesser*, void*);
#pragma pop
template soStatusModuleBuilder<SbBC::StatusModuleBuildConfig>::soStatusModuleBuilder(soModuleAccesser*, const soStatusData*, void*);
template soKineticModuleBuilder<SbBC::KineticModuleBuildConfig>::soKineticModuleBuilder(soModuleAccesser*);

#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnSamusBombModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>

// Starts the bomb at rest: the movement energy is cleared and the gravity energy starts at the parameter's fall speed.
void wnSamusBomb::activate(int founderTaskId, int etcResourceId, int team) {
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
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    soModuleAccesser* acc = m_moduleAccesser;
    Vec3f rot(0.0f, 0.0f, 0.0f);
    Vec2f speed(0.0f, 0.0f);
    normal.resetEnergy(0, &speed, &rot, acc);
    normal.enable();
    wnKineticEnergyGravity& gravity = dynamic_cast<wnKineticEnergyGravity&>(*m_moduleAccesser->getKineticModule().getEnergy(2));
    gravity.m_speedY = m_param->m_fallSpeed;
    gravity.enable();
}

const wnSamusBombData* wnSamusBomb::getData() {
    return static_cast<const wnSamusBombData*>(m_data);
}

// Drops the bomb at the model origin moved by (x * lr, y, z), on the owner's ground depth.
void wnSamusBomb::toShoot(float lr, float x, float y, float z) {
    Vec3f pos = m_moduleAccesser->getModelModule().getNodeGlobalPosition(0, false);
    pos.m_z = m_moduleAccesser->getGroundModule().getZ();
    pos.m_x += x * lr;
    pos.m_y += y;
    pos.m_z += z;
    m_moduleAccesser->getPostureModule().init(&pos, lr);
    m_moduleAccesser->getLinkModule().removeModelConstraint(true);
    updateNodeSRT();
}

void wnSamusBomb::updateNodeSRT() {
    float scale = m_moduleAccesser->getPostureModule().getScale();
    Vec3f position = m_moduleAccesser->getPostureModule().getPos();
    m_moduleAccesser->getPostureModule().getLr();
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    Vec3f scaling(scale, scale, scale);
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scaling, &rotation, &position);
}

static inline float wnSamusBombDot(const Vec2f& a, const Vec2f& b) {
    return a.m_x * b.m_x + a.m_y * b.m_y;
}

// Reflects the velocity about the surface normal in place.
static inline void wnSamusBombReflectImpl(Vec2f& speed, const Vec2f& normal) {
    speed = speed - normal * 2.0f * wnSamusBombDot(normal, speed);
}
static inline void wnSamusBombReflect(Vec2f& speed, const Vec2f& normal) {
    wnSamusBombReflectImpl(speed, normal);
}

// Anim cmd type 6 is the ground check: a bomb that is too slow comes to rest (its gravity stops unless it is in the air);
// otherwise it bounces off the surface, unless it only slides along it.
bool wnSamusBomb::notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* moduleAccesser, int index) {
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
        wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
        wnKineticEnergyGravity& gravity = dynamic_cast<wnKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(2));
        Vec2f normal = moduleAccesser->getGroundModule().getTouchNormal(0xff, 0);
        Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
        int situation = moduleAccesser->getSituationModule().getKind();
        if (speed.length() < m_param->m_minBounceSpeed) {
            if (situation != 2) {
                gravity.disable();
            }
        } else {
            wnSamusBombReflect(speed, normal);
            Vec2f surface = normal;
            Vec2f direction = speed;
            surface.normalize();
            direction.normalize();
            if (__fabsf(wnSamusBombDot(surface, direction)) > 0.0001f) {
                speed = speed * m_param->m_bounceScale;
                energy.m_speed = Vec2f(speed.m_x, 0.0f);
                gravity.m_speedY = speed.m_y;
            }
        }
        return true;
    }
    }
    return false;
}

// The bomb always takes its search collision events.
bool wnSamusBomb::notifyEventCollisionSearchCheck() {
    return true;
}

// The link event the bomb sends to its owner (kind 0x838): the plain event, nothing added.
struct wnSamusBombLinkEventTouch : soLinkEventArgs {
    wnSamusBombLinkEventTouch(int kind) : soLinkEventArgs(kind) { }
};

// The owner's task id is kept in work int 0x10000000: touching the owner sends link event 0x838 up to it.
void wnSamusBomb::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    u32 ownerTaskId = moduleAccesser->getWorkManageModule().getInt(0x10000000);
    if (collisionLog->m_taskId == ownerTaskId) {
        linkOwner(1, ownerTaskId);
        wnSamusBombLinkEventTouch eventArgs(0x838);
        moduleAccesser->getLinkModule().sendEventParents(1, eventArgs);
        unlinkOwner(1);
    }
}

inline void wnSamusBomb::gatherSpeed() {
    soModuleAccesser* acc = m_moduleAccesser;
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*acc->getKineticModule().getEnergy(0));
    wnKineticEnergyGravity& gravity = dynamic_cast<wnKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(2));
    Vec2f speed = acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    energy.m_speed = speed;
    gravity.m_speedY = 0.0f;
}

inline void wnSamusBomb::splitBounceSpeed() {
    soModuleAccesser* acc = m_moduleAccesser;
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*acc->getKineticModule().getEnergy(0));
    wnKineticEnergyGravity& gravity = dynamic_cast<wnKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(2));
    Vec2f speed = acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    speed = speed * m_param->m_bounceScale;
    energy.m_speed = Vec2f(speed.m_x, 0.0f);
    gravity.m_speedY = speed.m_y;
}

// A reflector turns the bomb around by its speed multiplier.
bool wnSamusBomb::reflect() {
    gatherSpeed();
    float mulY = -m_moduleAccesser->getReflectModule().getSpeedMul();
    float mulX = -m_moduleAccesser->getReflectModule().getSpeedMul();
    Vec3f mul(mulX, mulY, 1.0f);
    m_moduleAccesser->getKineticModule().mulSpeed(&mul, soKineticEnergy::AttributeFlag(1));
    splitBounceSpeed();
    return false;
}

bool wnSamusBomb::hop() {
    gatherSpeed();
    bool result = Weapon::hop();
    splitBounceSpeed();
    return result;
}

// A resting bomb (status 0) that hits something or is clashed goes off (status 1) instead of the usual handling.
bool wnSamusBomb::notifyEventCollisionAttackCheck(u32 flags) {
    soModuleAccesser* acc = m_moduleAccesser;
    if (acc->getStatusModule().getStatusKind() == 0 && (flags & 3) != 0) {
        acc->getStatusModule().changeStatusRequest(1, acc);
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}
