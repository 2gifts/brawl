#define WN_ANIMCMD_RESET_MODE_BOX // BYTES: word-aligned reset-mode temporaries (frame size of the anim cmd sub builder)
#define WN_ANIMCMD_STACK_PTR // BYTES: the interpreter stacks stay in a saved register across getManageId (ft_builder_animcmd.h)
#define WN_ANIMCMD_PRESENTER_LOCAL_STORE // BYTES: the interpreters' presenters keep the looked-up list in a local, as in the Mario fireball
#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: activate copies the Vec3f position as an aggregate (word moves), see mt_vector.h
#define WN_EVENT_UNIT_DTOR_INLINE // BYTES: the inside event builder destructor expands the event unit destructors
#define WN_STATUS_MODULE_KIND_COUNT_ZERO // BYTES: the status module is built with a status kind count of 0 (ft_builder_status.h)
#define SO_BUILD_DATA_SLOPE_LIMIT_EXTERNAL // the build data's default slope limit is not emitted here (no 20.0 in the unit's pool)
#define FT_MODULE_BUILDER
#define WN_MARIO_FIREBALL_UNIT // the sora_melee article unit mode shared with the Mario fireball (builders out of line)
#define WN_ZELDA_LIGHTING_BOW_UNIT // the member layout and constructor of the module builder of this unit mode (wn/zelda/wn_zelda_lighting_bow.h)
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_STATUS_NO_TAIL
#define SO_OTHER_ANIM_CTOR // BYTES: the sora_melee motion builder constructs and destroys its other-animation slot
#define WN_TRANSITION_CTOR_INLINE
#define WN_ARTICLE_ANIMCMD_INT // BYTES: the builder vtable takes Weapon's int-index notifyEventAnimCmd thunk, as in the original
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#include <wn/wn_module_accesser_builder.h>
#include <wn/zelda/wn_zelda_lighting_bow.h>
#include <wn/zelda/wn_szerosuit_paralyzer_bullet.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <gf/gf_task_scheduler.h>
#include <memory.h>

// BYTES: the empty base destructors that sora_melee always expands (they have no out-of-line copy there).
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }

// Registers the Zero Suit paralyzer bullet article class (kind 64) in the class info table during static initialization.
wnClassInfoImpl<64, wnSZerosuitParalyzerBullet, wnSZerosuitParalyzerBulletData> g_wnSZerosuitParalyzerBulletClassInfo;

wnSZerosuitParalyzerBullet::wnSZerosuitParalyzerBullet(s32 articleId, const wnArticleBuildInfo& info,
                                                       const wnSZerosuitParalyzerBulletData* data) :
    wnWeaponBuilder<wnSZerosuitParalyzerBulletModuleAccesserBuildConfig>(
        "SZerosuitParalyzerBullet", articleId, info,
        wnModuleAccesserBuildData(64, data, info.getHeapModule()->getInstanceHeap(),
                                  info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1,
                                  &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

// The unit keeps the weak constructor of its status builder.
template soStatusModuleBuilder<wnSZerosuitParalyzerBulletModuleAccesserBuildConfig::StatusModuleBuildConfig>::soStatusModuleBuilder(
    soModuleAccesser*, const soStatusData*, void*);

inline wnSZerosuitParalyzerBulletParamAccesser::~wnSZerosuitParalyzerBulletParamAccesser() { }

#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnSZerosuitParalyzerBulletModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>

void wnSZerosuitParalyzerBullet::activate(int founderTaskId, int team, const Vec3f& position, bool fullCharge, float lr) {
    wnActivateDesc desc(founderTaskId, 0xFFFF, 0xFFFF, 0xFFFF, -1, -1, 0, 0, position, lr, team, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);

    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    Vec2f speed;
    Vec2f accel(0.0f, 0.0f);
    speed.m_x = 0.0f;
    speed.m_y = 0.0f;
    if (fullCharge) {
        work.onFlag(0x22000000);
        speed.m_x = m_param->m_fullChargeSpeed * lr;
    } else {
        work.offFlag(0x22000000);
        speed.m_x = m_param->m_speed * lr;
    }
    soKineticModule& kinetic = m_moduleAccesser->getKineticModule();
    kinetic.changeKinetic(2, m_moduleAccesser);
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*kinetic.getEnergy(0));
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    energy.resetEnergy(0, &speed, &rotation, moduleAccesser);
    energy.m_accel = accel;
    energy.m_speedLimit = speed;
    energy.m_speedTarget = speed;
    energy.enable();
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}

wnSZerosuitParalyzerBullet::~wnSZerosuitParalyzerBullet() { }
