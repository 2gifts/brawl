#define WN_PIT_IKARUS_FULL_BUILDER // the complete Ikarus builder (see the header)
#define WN_WEAPON_BUILDER
#define WN_DAMAGE_ACTOR_INTERFACE
#define SO_KINETIC_ENERGY_ABSTRACT // the base vtable (empty pure slots) and its friction switches are weak copies here
#define SO_DAMAGE_EFFECTOR_NULL_IMPLICIT_DTOR
#define WN_DAMAGE_TRANSACTOR_NULL_ACCESSOR
#define SO_DAMAGE_EFFECTOR_PRIVATE_NULL_BASE
#define IP_INPUT_BUTTON_RAW // BYTES: IpNull::getButton returns the raw button mask (8 bytes, not a struct copy)
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#ifdef WN_PIT_IKARUS_FULL_BUILDER
#define SO_KINETIC_ENERGY_3F_FULL // BYTES: the builder holds the full 3f energy (size 0x48); its dtor is then expanded in this unit
#endif
#define WN_KINETIC_GENERIC_BUILDER
#define WN_BUILD_DATA_INLINE_DTOR // BYTES: the build data temporary's weak destructor comes right after getHeapModule
#define SO_KINETIC_ENERGY_3F_DEFAULT_CTOR // BYTES: the Ikarus pool constructs its 3f energy here (five Vec3f defaults)
// BYTES: link order. MWCC emits the weak vtables (and with them their inline virtual functions) in reverse order of the
// class definitions; these headers come first, in the order the original defines the classes (as in the G&W rescue).
#define SO_PARAM_CUSTOMIZE_IMPL_LATE
#define SO_ARTICLE_LOG_HELPER_LATE
#include <ft/builder/ft_dol_array_list.h>
#include <ip/ip_null.h>
#include <so/damage/so_damage_event_presenter.h>
#include <so/team/so_team.h>
#include <so/resource/so_resource_id_accesser.h>
#include <so/team/so_team_impl.h>
#include <so/damage/so_damage_transactor_null.h>
#include <so/damage/so_damage_effector_null.h>
#include <so/kinetic/so_kinetic_energy.h>
#include <so/article/so_article_mediator_null.h>
#include <so/param/so_param_customize_module_impl.h>
#include <wn/wn_module_accesser_builder.h>
// The status unique-process vector and the hit part vector are built by their sora_melee constructors.
WN_STATUS_PROC_VECTOR_ARG_CTOR(5);
FT_DOL_ARRAY_VECTOR(soCollisionHitPart, 1);
#include <so/kinetic/so_kinetic_energy_3f.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/kinetic/so_kinetic_transactor.h>
#include <so/so_kinetic_energy_rot_normal.h>
#include <so/so_module_accesser.h>
#include <so/so_type_list.h>
#include <so/so_value_accesser.h>
#include <mt/mt_prng.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/pit/wn_pit_ikarus.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <wn/wn_kinetic_transactor.h>

#include <so/collision/so_collision_hit_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/stop/so_stop_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/damage/so_damage.h>

// The Ikarus emits the damage observer's weak teardown and its empty event handlers (the vtable it stores).
inline soDamageEventObserver::~soDamageEventObserver() { }
inline void soDamageEventObserver::notifyEventOnDamage(soDamage*, bool, soModuleAccesser*) { }
inline void soDamageEventObserver::notifyEventAddDamage(soDamage*, soModuleAccesser*) { }
// The gravity energy's weak teardown is emitted here too (its pool lives in the Ikarus builder).
inline wnKineticEnergyGravity::~wnKineticEnergyGravity() { }

// The status processes are defined in their own units; this unit only hands out their instances.
class wnPitIkarusStatusUniqProcessEntry : public soStatusUniqProcess {
public:
    static wnPitIkarusStatusUniqProcessEntry* getInstance();
    virtual ~wnPitIkarusStatusUniqProcessEntry() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};
class wnPitIkarusStatusUniqProcessTurn : public soStatusUniqProcess {
public:
    static wnPitIkarusStatusUniqProcessTurn* getInstance();
    virtual ~wnPitIkarusStatusUniqProcessTurn() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};
class wnPitIkarusStatusUniqProcessAttack : public soStatusUniqProcess {
public:
    static wnPitIkarusStatusUniqProcessAttack* getInstance();
    virtual ~wnPitIkarusStatusUniqProcessAttack() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};
extern wnPitIkarusStatusUniqProcessEntry g_wnPitIkarusStatusUniqProcessEntry;
extern wnPitIkarusStatusUniqProcessTurn g_wnPitIkarusStatusUniqProcessTurn;
extern wnPitIkarusStatusUniqProcessAttack g_wnPitIkarusStatusUniqProcessAttack;

inline wnPitIkarusStatusUniqProcessEntry* wnPitIkarusStatusUniqProcessEntry::getInstance() {
    return &g_wnPitIkarusStatusUniqProcessEntry;
}
inline wnPitIkarusStatusUniqProcessTurn* wnPitIkarusStatusUniqProcessTurn::getInstance() {
    return &g_wnPitIkarusStatusUniqProcessTurn;
}
inline wnPitIkarusStatusUniqProcessAttack* wnPitIkarusStatusUniqProcessAttack::getInstance() {
    return &g_wnPitIkarusStatusUniqProcessAttack;
}

// Hit-stop constants passed to soStopModuleImpl::calcHitStopFrame (same values and layout as in
// wn_snake_nikita_missile.cpp): MWCC keeps these external-linkage objects at the start of .rodata, ahead of the
// builder constructor's pooled literals.
// HYPOTHESIS: names and the exact original declaration (likely an extern declared in a shared weapon header).
extern const float g_wnHitStopFrameMul = 0.3846154f;
extern const float g_wnHitStopFrameAdd = 5.0f;
extern const float g_wnHitStopFrameMax = 30.0f;

#pragma dont_inline on // BYTES: the native constructor calls every helper (builder, accessors, getInstance) out of line
wnPitIkarus::wnPitIkarus(s32 articleId, const wnArticleBuildInfo& info, const wnPitIkarusData* data) :
    wnWeaponBuilder<wnPitIkarusModuleAccesserBuildConfig>(
        "PitIkarus", articleId, info,
        wnModuleAccesserBuildData(0x7d, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &unk22D8,
                                  0, 0, 0, 0, 0, 0, 0)),
    soDamageEventObserver(static_cast<soEventManager&>(getModuleAccesser()->getEventManageModule()).getManageId(), -1), unk22D4(data->m_param), unk22D8(data->m_param) {
    soStatusUniqProcess* uniqProcs[5] = {};
    uniqProcs[0] = wnPitIkarusStatusUniqProcessEntry::getInstance();
    uniqProcs[1] = wnPitIkarusStatusUniqProcessTurn::getInstance();
    uniqProcs[3] = wnPitIkarusStatusUniqProcessAttack::getInstance();
    getModuleAccesser()->getStatusModule().addRangeUniqProc(uniqProcs, 5);
    getModuleAccesser()->getCollisionHitModule().add(
        reinterpret_cast<soSet<soCollisionHitData>*>(const_cast<u8*>(data->unk38)), 0);
    Weapon::deactivate(false);
}
#pragma dont_inline reset

wnWeaponBuilder<wnPitIkarusModuleAccesserBuildConfig>::wnWeaponBuilder(
    const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd) :
    Weapon(name, bd.m_kind, bd.m_data, articleId, &info, m_modules.getModuleAccesser(), true), m_modules(this, bd) {
    Weapon::initialize();
}

void wnPitIkarus::activate(u32 founderTaskId, u32 resourceId, int team, const Vec3f& pos, float unk1, float unk2) {
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0,
                        pos, 0.0f, team, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagBit7 = true;
    desc.flagBit6 = false;
    desc.flagBit5 = false;
    desc.flagBit4 = false;
    desc.flagBit3 = false;
    desc.flagBit2 = false;
    desc.flagBit1 = false;
    desc.flagBit0 = false;
    desc.flag4DBit7 = false;
    desc.flag4DBit6 = false;
    Weapon::activate(&desc);
    m_moduleAccesser->getTeamModule().setHitTeam(-1);
    m_moduleAccesser->getTeamModule().set2nd(5);
    m_moduleAccesser->getTeamModule().setHit2nd(5);
    m_moduleAccesser->getWorkManageModule().setFloat(unk1, 0x21000001);
    m_moduleAccesser->getPostureModule().setScale(unk2);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}

bool wnPitIkarus::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags != 0) {
        m_moduleAccesser->getStatusModule().changeStatusForce(4, m_moduleAccesser);
        return true;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

void wnPitIkarus::notifyEventOnDamage(soDamage* damage, bool, soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: a hit while the Ikarus is in its fly statuses (2-3) that is strong enough sends it back the way it came.
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 2:
    case 3:
        if (damage->m_damage >= soValueAccesser::getConstantFloat(moduleAccesser, 0xFAA, 0)) {
            m_moduleAccesser->getWorkManageModule().setFloat(-damage->m_lr, 0x11000001);
            moduleAccesser->getPostureModule().setLr(damage->m_lr);
            moduleAccesser->getStatusModule().changeStatusForce(4, moduleAccesser);
        }
        if (damage->m_reaction > 0.0f) {
            int frames = soStopModuleImpl::calcHitStopFrame(damage->m_powerMax, 0.3846154f, 5.0f, 1.0f, 30.0f);
            moduleAccesser->getStopModule().setHitStopFrame(frames, true);
        }
        break;
    }
}

#ifndef WN_PIT_IKARUS_FULL_BUILDER
soKineticEnergy3f::~soKineticEnergy3f() { }
#endif


template <typename E>
void soKineticTransactor::updateEnergy(E* energy, soModuleAccesser* moduleAccesser) {
    if (energy->isEnable() == true && !energy->isSuspend())
        energy->updateEnergy(moduleAccesser);
}
template void soKineticTransactor::updateEnergy<soKineticEnergy3f>(soKineticEnergy3f*, soModuleAccesser*);
template void soKineticTransactor::updateEnergy<soKineticEnergyRotNormal>(soKineticEnergyRotNormal*, soModuleAccesser*);
template void soKineticTransactor::updateEnergy<wnKineticEnergyNormal>(wnKineticEnergyNormal*, soModuleAccesser*);
template void soKineticTransactor::updateEnergy<wnKineticEnergyGravity>(wnKineticEnergyGravity*, soModuleAccesser*);
