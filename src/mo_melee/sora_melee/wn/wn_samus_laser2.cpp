#define WN_ANIMCMD_RESET_MODE_BOX // BYTES: word-aligned reset-mode temporaries (frame size of the anim cmd sub builder)
#define WN_ANIMCMD_STACK_PTR // BYTES: the interpreter stacks stay in a saved register across getManageId (ft_builder_animcmd.h)
#define WN_ANIMCMD_PRESENTER_LOCAL_STORE // BYTES: the interpreters' presenters keep the looked-up list in a local, as in the Mario fireball
#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: activate copies the Vec3f position as an aggregate (word moves), see mt_vector.h
#define WN_EVENT_UNIT_DTOR_INLINE // BYTES: the inside event builder destructor expands the event unit destructors
#define WN_STATUS_MODULE_KIND_COUNT_ZERO // BYTES: the status module is built with a status kind count of 0 (ft_builder_status.h)
#define SO_BUILD_DATA_SLOPE_LIMIT_EXTERNAL // the build data's default slope limit is not emitted here (no 20.0 in the unit's pool)
#define WN_ANIMCMD_SUB_BUILDER_EXTERN // the anim cmd sub builder constructor is the Samus bomb unit's weak copy
#define WN_WEAPON_NO_COMMON_ENERGIES // no kinetic module, so the weapon builder adds no energies
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
#include <wn/samus/wn_samus_laser2.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <gf/gf_task_scheduler.h>
#include <memory.h>

// BYTES: the empty base destructors that sora_melee always expands (they have no out-of-line copy there).
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }

extern soStatusUniqProcess g_wnSamusLaser2StatusUniqProcess;

// BYTES: the constructor addresses each constant by its own address (no pooled anchor register).
#pragma push
#pragma pool_data off
wnSamusLaser2::wnSamusLaser2(s32 articleId, const wnArticleBuildInfo& info, const wnSamusLaser2Data* data) :
    wnWeaponBuilder<wnSamusLaser2ModuleAccesserBuildConfig>(
        "SamusLaser2", articleId, info,
        wnModuleAccesserBuildData(0x57, data, info.getHeapModule()->getInstanceHeap(), static_cast<Heaps::HeapType>(0x1f),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), 0,
                                  &g_soParamAccesserNull, 0, 0, 0, 0, 0, 0, 0)) {
    // Both statuses run the same process.
    soStatusUniqProcess* uniqProcs[2];
    uniqProcs[0] = &g_wnSamusLaser2StatusUniqProcess;
    uniqProcs[1] = &g_wnSamusLaser2StatusUniqProcess;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 2);
    Weapon::deactivate(false);
}
#pragma pop

#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnSamusLaser2ModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>

void wnSamusLaser2::activate(int founderTaskId, int etcResourceId, int team, float height) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = 0xFFFF;
    desc.unk8 = 0xFFFF;
    desc.unkC = 0xFFFF;
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
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    work.setFloat(height, 0x11000001);
    work.onFlag(0x12000000);
}

// HYPOTHESIS: the second laser leaves the model origin raised by its height; a negative height starts it in status 1.
// The two statuses of the beam. HYPOTHESIS: names; a negative height offset selects the second one.
enum wnSamusLaser2StatusKind {
    wnSamusLaser2Status_High = 0,
    wnSamusLaser2Status_Low = 1,
};

void wnSamusLaser2::toShoot(float lr) {
    soModuleAccesser* acc = m_moduleAccesser;
    Vec3f offset(0.0f, 0.0f, 0.0f);
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(0, &offset, false, false);
    float height = acc->getWorkManageModule().getFloat(0x11000001);
    pos.m_z = acc->getGroundModule().getZ();
    pos.m_y += height;
    acc->getPostureModule().init(&pos, lr);
    acc->getLinkModule().removeModelConstraint(true);
    acc->getStatusModule().changeStatusRequest(height < 0.0f ? wnSamusLaser2Status_Low : wnSamusLaser2Status_High, acc);
    updateNodeSRT();
}

// HYPOTHESIS: the laser passes through everything while it is out (statuses 0 and 1).
bool wnSamusLaser2::notifyEventCollisionAttackCheck(u32 flags) {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 0:
    case 1:
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

const wnSamusLaser2Data* wnSamusLaser2::getData() {
    return static_cast<const wnSamusLaser2Data*>(m_data);
}
