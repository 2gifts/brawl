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
#include <wn/samus/wn_samus_laser.h>
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

extern soStatusUniqProcess g_wnSamusLaserStatusUniqProcess;

// BYTES: the constructor addresses each constant by its own address (no pooled anchor register).
#pragma push
#pragma pool_data off
wnSamusLaser::wnSamusLaser(s32 articleId, const wnArticleBuildInfo& info, const wnSamusLaserData* data) :
    wnWeaponBuilder<wnSamusLaserModuleAccesserBuildConfig>(
        "SamusLaser", articleId, info,
        wnModuleAccesserBuildData(0x56, data, info.getHeapModule()->getInstanceHeap(), static_cast<Heaps::HeapType>(0x1f),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), 0,
                                  &g_soParamAccesserNull, 0, 0, 0, 0, 0, 0, 0)) {
    // Both statuses run the same process.
    soStatusUniqProcess* uniqProcs[2];
    uniqProcs[0] = &g_wnSamusLaserStatusUniqProcess;
    uniqProcs[1] = &g_wnSamusLaserStatusUniqProcess;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 2);
    Weapon::deactivate(false);
}
#pragma pop

#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnSamusLaserModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>

// HYPOTHESIS: the laser has no etc resource of its own (0xFFFF) and starts in status 0 with a fresh work int 0x10000006.
void wnSamusLaser::activate(int founderTaskId, int etcResourceId, int team) {
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
    m_moduleAccesser->getWorkManageModule().setInt(0, 0x10000006);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000000);
}

// HYPOTHESIS: the laser leaves the model origin, drops its model constraint and enters status 1.
void wnSamusLaser::toShoot(float lr) {
    Vec3f offset(0.0f, 0.0f, 0.0f);
    Vec3f pos = m_moduleAccesser->getModelModule().getNodeGlobalPosition(0, &offset, false, false);
    pos.m_z = m_moduleAccesser->getGroundModule().getZ();
    m_moduleAccesser->getPostureModule().init(&pos, lr);
    m_moduleAccesser->getLinkModule().removeModelConstraint(true);
    m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
    updateNodeSRT();
}

// HYPOTHESIS: the laser passes through everything while it is out (statuses 0 and 1).
bool wnSamusLaser::notifyEventCollisionAttackCheck(u32 flags) {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 0:
    case 1:
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

// HYPOTHESIS: the article data block of the build data is a wnSamusLaserData.
const wnSamusLaserData* wnSamusLaser::getData() {
    return static_cast<const wnSamusLaserData*>(m_data);
}
