#define WN_STATUS_MODULE_KIND_COUNT_ZERO // BYTES: the status module is built with a status kind count of 0 (ft_builder_status.h)
#define WN_ANIM_CMD_UNIQ_STACK_OF_UNIT0 // the uniq animation command thread has the stack capacity of thread 0 (wn_anim_cmd_full_property.h)
#define WN_ZELDA_BUILDER_CTOR_WEAK // the module accesser builder constructor is a weak symbol of this unit
#define WN_EVENT_UNIT_DTOR_INLINE // BYTES: the inside event builder destructor expands the event unit destructors
#define SO_EVENT_OBSERVER_MANAGE_S16 // the observer registration keeps the manage id as the s16 the article returns (so_event_presenter.h)
#define WN_KINETIC_MEDIATOR_INLINE_DTOR // the kinetic builder destructor this unit owns expands the empty mediator destructor
#define SO_OTHER_ANIM_CTOR // BYTES: the sora_melee motion builder constructs and destroys its other-animation slot
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: activate copies the Vec3f position as an aggregate (word moves), see mt_vector.h
#define FT_MODULE_BUILDER
#define WN_POKE_FUSHIGISOU_SEED_UNIT // the article unit mode of this source (the real class and its builders)
#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define WN_MARIO_FIREBALL_UNIT // the sora_melee article unit mode shared with the Mario fireball (builders out of line)
#define WN_ZELDA_LIGHTING_BOW_UNIT // the member layout and constructor of the module builder of this unit mode
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_STATUS_NO_TAIL
#define WN_TRANSITION_CTOR_INLINE
#define FT_POOL_ROOT_NONPOLYMORPHIC // BYTES: without it the hierarchy destructor is never_inline
#define WN_ANIMCMD_RESET_MODE_BOX // BYTES: word-aligned reset-mode temporaries (frame size of the anim cmd sub builder)
#define WN_ANIMCMD_PRESENTER_LOCAL_STORE // the interpreters' presenters take the list from the lookup as a value
#define WN_ANIMCMD_STACK_PTR // BYTES: the interpreter stacks stay in a saved register across getManageId
#define WN_ARTICLE_ANIMCMD_INT // BYTES: the builder vtable takes Weapon's int-index notifyEventAnimCmd thunk
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#include <wn/wn_module_accesser_builder.h>
#include <wn/poke/wn_poke_fushigisou_seed.h>
#include <wn/wn_class_info_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_value_accesser.h>
#include <mt/mt_prng.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/work/so_work_manage_module_impl.h>

// BYTES: the empty base destructors that sora_melee always expands (they have no out-of-line copy there).
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }

wnPokeFushigisouSeed::wnPokeFushigisouSeed(s32 articleId, const wnArticleBuildInfo& info, const wnPokeFushigisouSeedData* data) :
    wnWeaponBuilder<wnPokeFushigisouSeedModuleAccesserBuildConfig>(
        "PokeFushigisouSeed", articleId, info,
        wnModuleAccesserBuildData(0x4a, data, info.getHeapModule()->getInstanceHeap(),
            info.getHeapModule()->getNWModelInstanceHeap(), info.getHeapModule()->getNWMotionInstanceHeap(),
            static_cast<u8>(data->m_resGroupNo), -1, &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

template class wnExtendParamAccesser<3999, 1, 23999, 1, 0, 0>;

// The weak builders this unit owns in the link order (the other units with the same configurations reuse them).
typedef wnPokeFushigisouSeedModuleAccesserBuildConfig SfBC;
template soInsideEventManageModuleBuilder<SfBC::InsideEventManageModuleBuildConfig, SfBC::InsideEventManageModuleTypes>::soInsideEventManageModuleBuilder();
// BYTES: the original addresses each singleton's destructor-chain record by its own address (no pooled anchor).
#pragma push
#pragma pool_data off
template soMotionModuleBuilder<SfBC::MotionModuleBuildConfig>::soMotionModuleBuilder(soModuleAccesser*, void*);
#pragma pop
template soStatusModuleBuilder<SfBC::StatusModuleBuildConfig>::soStatusModuleBuilder(soModuleAccesser*, const soStatusData*, void*);

#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnPokeFushigisouSeedModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>


// Registers the Pokemon seed article class (kind 74) in the class info table during static initialization.
wnClassInfoImpl<74, wnPokeFushigisouSeed, wnPokeFushigisouSeedData> g_wnPokeFushigisouSeedClassInfo;

// Launches the seed straight up at the given speed; the kind flag selects the Kirby variant of the effect.
void wnPokeFushigisouSeed::activate(int founderTaskId, int team, Vec3f* position, int unk, int kind, float lr, float speed) {
    wnActivateDesc desc(founderTaskId, 0xFFFF, 0xFFFF, 0xFFFF, -1, -1, 0, 0, *position, lr, team, 0, 5, 0x80, 0, 0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getWorkManageModule().setFlag(kind, 0x12000003);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    energy.m_speed = Vec2f(0.0f, speed);
    updateNodeSRT();
}

void wnPokeFushigisouSeed::onDeactivate() { }

// A hit on a target bursts the seed (status 2).
bool wnPokeFushigisouSeed::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        m_moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

void wnPokeFushigisouSeed::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    switch (statusKind) {
    case 2: {
        float threshold = soValueAccesser::getConstantFloat(moduleAccesser, 0xFA0, 0);
        if (randf() >= threshold) {
            Vec3f position = moduleAccesser->getPostureModule().getPos();
            if (!moduleAccesser->getWorkManageModule().isFlag(0x12000003)) {
                moduleAccesser->getEffectModule().req(ef_ptc_poke_fushigisou_tane_mghit, &position, &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
            } else {
                moduleAccesser->getEffectModule().req(ef_ptc_kb_poke_fushigisou_tane_mghit, &position, &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
            }
        }
        break;
    }
    case 1: {
        Vec3f position = moduleAccesser->getPostureModule().getPos();
        if (!moduleAccesser->getWorkManageModule().isFlag(0x12000003)) {
            moduleAccesser->getEffectModule().req(ef_ptc_poke_fushigisou_tane_mghit, &position, &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
        } else {
            moduleAccesser->getEffectModule().req(ef_ptc_kb_poke_fushigisou_tane_mghit, &position, &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
        }
        break;
    }
    }
}

