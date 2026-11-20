#define WN_ANIMCMD_RESET_MODE_BOX // BYTES: word-aligned reset-mode temporaries (frame size of the anim cmd sub builder)
#define WN_ANIMCMD_STACK_PTR // BYTES: the interpreter stacks stay in a saved register across getManageId (ft_builder_animcmd.h)
#define WN_ANIMCMD_PRESENTER_LOCAL_STORE
#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
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
#define WN_SHEIK_NEEDLE_UNIT
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_STATUS_NO_TAIL
#define SO_OTHER_ANIM_CTOR // BYTES: the sora_melee motion builder constructs and destroys its other-animation slot
#define WN_TRANSITION_CTOR_INLINE
#define WN_ARTICLE_ANIMCMD_INT // BYTES: the builder vtable takes Weapon's int-index notifyEventAnimCmd thunk, as in the original
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#include <wn/wn_module_accesser_builder.h>
#include <wn/sheik/wn_sheik_needle.h>
#include <wn/wn_activate_desc.h>
#include <so/model/so_model_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <gf/gf_task_scheduler.h>
#include <memory.h>
#include <wn/wn_class_info_impl.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <mt/mt_prng.h>
#include <math.h>

// Registers the Sheik needle article class (kind 68) in the class info table during static initialization.
wnClassInfoImpl<68, wnSheikNeedle, wnSheikNeedleData> g_wnSheikNeedleClassInfo;

// BYTES: the empty base destructors that sora_melee always expands (they have no out-of-line copy there).
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }

extern soStatusUniqProcess g_wnSheikNeedleStatusUniqProcessMove;
extern soStatusUniqProcess g_wnSheikNeedleStatusUniqProcessDrop;
extern soStatusUniqProcess g_wnSheikNeedleStatusUniqProcessStay;
extern soStatusUniqProcess g_wnSheikNeedleStatusUniqProcessJump;

#pragma push
#pragma pool_data off
wnSheikNeedle::wnSheikNeedle(s32 articleId, const wnArticleBuildInfo& info, const wnSheikNeedleData* data) :
    wnWeaponBuilder<wnSheikNeedleModuleAccesserBuildConfig>(
        "SheikNeedle", articleId, info,
        wnModuleAccesserBuildData(0x44, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(*reinterpret_cast<const s32*>(data)), -1,
                                  &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_param(*reinterpret_cast<const wnSheikNeedleParam* const*>(reinterpret_cast<const u8*>(data) + 0x34)), m_paramAccesser(m_param) {
    soStatusUniqProcess* uniqProcs[4];
    uniqProcs[0] = &g_wnSheikNeedleStatusUniqProcessMove;
    uniqProcs[1] = &g_wnSheikNeedleStatusUniqProcessDrop;
    uniqProcs[2] = &g_wnSheikNeedleStatusUniqProcessStay;
    uniqProcs[3] = &g_wnSheikNeedleStatusUniqProcessJump;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 4);
    Weapon::deactivate(false);
}
#pragma pop

typedef wnSheikNeedleModuleAccesserBuildConfig SnBC;
template soInsideEventManageModuleBuilder<SnBC::InsideEventManageModuleBuildConfig, SnBC::InsideEventManageModuleTypes>::soInsideEventManageModuleBuilder();
#pragma push
#pragma pool_data off
template soMotionModuleBuilder<SnBC::MotionModuleBuildConfig>::soMotionModuleBuilder(soModuleAccesser*, void*);
#pragma pop
template soStatusModuleBuilder<SnBC::StatusModuleBuildConfig>::soStatusModuleBuilder(soModuleAccesser*, const soStatusData*, void*);

#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnSheikNeedleModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>

void wnSheikNeedle::updateNodeSRT() {
    Vec3f scale;
    Vec3f rot;
    Vec3f pos;
    scale.m_x = scale.m_y = scale.m_z = m_moduleAccesser->getPostureModule().getScale();
    pos = m_moduleAccesser->getPostureModule().getPos();
    rot = m_moduleAccesser->getPostureModule().getRot(0);
    rot.m_y += m_moduleAccesser->getPostureModule().getRotYLr();
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rot, &pos);
}

template class wnExtendParamAccesser<3999, 2, 23999, 2, 0, 0>;

bool wnSheikNeedle::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 3) {
        if (randi(3) == 0) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(3, m_moduleAccesser);
            return false;
        }
        deactivate(false);
        return true;
    }
    if (flags & 4) {
        if (randi(3) == 0) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(3, m_moduleAccesser);
            return false;
        }
        deactivate(false);
        return true;
    }
    if (flags & 0x10) {
        deactivate(false);
        return true;
    }
    if ((flags & 8) && m_moduleAccesser->getReflectModule().isReflect() == true) {
        reflectSettingTeam();
        soPostureModule& posture = m_moduleAccesser->getPostureModule();
        soKineticModule& kinetic = m_moduleAccesser->getKineticModule();
        const wnSheikNeedleParam* param = dynamic_cast<wnSheikNeedle&>(m_moduleAccesser->getStageObject()).m_param;
        Vec2f speed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1));
        double speedX = -speed.m_x;
        double speedY = -speed.m_y;
        float angle = (float)atan2(speedY, speedX);
        Vec3f rot(57.29578f * (angle * -posture.getLr()), 0.0f, 0.0f);
        posture.setRot(&rot, 0);
        speed.m_x = param->unkC * (float)cos(angle);
        speed.m_y = param->unkC * (float)sin(angle);
        wnKineticEnergyNormal& movement = dynamic_cast<wnKineticEnergyNormal&>(*kinetic.getEnergy(0));
        movement.m_speed = speed;
        movement.m_accel = Vec2f(0.0f, 0.0f);
        movement.m_speedTarget = speed;
        movement.m_speedLimit = speed;
        movement.enable();
        float life = m_moduleAccesser->getWorkManageModule().getInt(0x10000003);
        life *= param->unk8;
        m_moduleAccesser->getWorkManageModule().setInt((int)life, 0x10000004);
        return false;
    }
    return false;
}

// TODO: clean up, give variables real names
namespace jj_33DEA8 {
extern "C" void __dl__FPv(void*);

extern "C" void* __dt__26wnSheikNeedleParamAccesserFv(void* ptr, int size) {
    if (ptr != 0 && size > 0)
        __dl__FPv(ptr);
    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_33DEE8 {
extern "C" void* fn_27_33DEE8(void* self) {
    return reinterpret_cast<u8*>(self) + 0x4b4;
}
}

// TODO: clean up, give variables real names
namespace jj_33EC7C {
extern "C" void fn_27_33EC7C(void) {}
}

// TODO: clean up, give variables real names
namespace jj_33EC80 {
extern "C" void* fn_27_33EC80(void* self) {
    return reinterpret_cast<u8*>(self) + 0x9c;
}
}

// TODO: clean up, give variables real names
namespace jj_33EC88 {
extern "C" void* fn_27_33EC88(void* self) {
    return reinterpret_cast<u8*>(self) + 0xdc;
}
}

// TODO: clean up, give variables real names
namespace jj_33EC90 {
extern "C" void* fn_27_33EC90(void* self) {
    return reinterpret_cast<u8*>(self) + 0xb8;
}
}

// TODO: clean up, give variables real names
namespace jj_3414F8 {
extern "C" void fn_27_33D590(void*);

extern "C" void fn_27_3414F8(void* self) {
    fn_27_33D590(reinterpret_cast<u8*>(self) - 0x40);
}
}

// TODO: clean up, give variables real names
namespace jj_341500 {
extern "C" void fn_27_33D590(void*);

extern "C" void fn_27_341500(void* self) {
    fn_27_33D590(reinterpret_cast<u8*>(self) - 0x90);
}
}

// TODO: clean up, give variables real names
namespace jj_341508 {
extern "C" void fn_27_33D590(void*);

extern "C" void fn_27_341508(void* self) {
    fn_27_33D590(reinterpret_cast<u8*>(self) - 0x88);
}
}

// TODO: clean up, give variables real names
namespace jj_341510 {
extern "C" void __dt__13wnSheikNeedleFv(void*);

extern "C" void fn_27_341510(void* self) {
    __dt__13wnSheikNeedleFv(reinterpret_cast<u8*>(self) - 0x40);
}
}

// TODO: clean up, give variables real names
namespace jj_341518 {
extern "C" void notifyEventCollisionAttackCheck__13wnSheikNeedleFUl(void*);

extern "C" void fn_27_341518(void* self) {
    notifyEventCollisionAttackCheck__13wnSheikNeedleFUl(reinterpret_cast<u8*>(self) - 0x70);
}
}

// TODO: clean up, give variables real names
namespace jj_341520 {
extern "C" void __dt__13wnSheikNeedleFv(void*);

extern "C" void fn_27_341520(void* self) {
    __dt__13wnSheikNeedleFv(reinterpret_cast<u8*>(self) - 0x90);
}
}

// TODO: clean up, give variables real names
namespace jj_341528 {
extern "C" void fn_27_341280(void*);

extern "C" void fn_27_341528(void* self) {
    fn_27_341280(reinterpret_cast<u8*>(self) - 0x88);
}
}

// TODO: clean up, give variables real names
namespace jj_341530 {
extern "C" void __dt__13wnSheikNeedleFv(void*);

extern "C" void fn_27_341530(void* self) {
    __dt__13wnSheikNeedleFv(reinterpret_cast<u8*>(self) - 0x88);
}
}

