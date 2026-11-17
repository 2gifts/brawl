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
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_STATUS_NO_TAIL
#define SO_OTHER_ANIM_CTOR // BYTES: the sora_melee motion builder constructs and destroys its other-animation slot
#define WN_TRANSITION_CTOR_INLINE
#define WN_ARTICLE_ANIMCMD_INT // BYTES: the builder vtable takes Weapon's int-index notifyEventAnimCmd thunk, as in the original
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#include <wn/wn_module_accesser_builder.h>
#include <wn/samus/wn_samus_cshot.h>
#include <wn/wn_class_info_impl.h>
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

// Registers the Samus charge shot article class (kind 83) in the class info table during static initialization.
wnClassInfoImpl<83, wnSamusCShot, wnSamusCShotData> g_wnSamusCShotClassInfo;

extern soStatusUniqProcess g_wnSamusCShotStatusUniqProcess;

// BYTES: the constructor addresses each constant by its own address (no pooled anchor register).
#pragma push
#pragma pool_data off
wnSamusCShot::wnSamusCShot(s32 articleId, const wnArticleBuildInfo& info, const wnSamusCShotData* data) :
    wnWeaponBuilder<wnSamusCShotModuleAccesserBuildConfig>(
        "SamusCShot", articleId, info,
        wnModuleAccesserBuildData(0x53, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), 0,
                                  &g_soParamAccesserNull, 0, 0, 0, 0, 0, 0, 0)) {
    // Every status runs the same process.
    soStatusUniqProcess* uniqProcs[9];
    uniqProcs[0] = &g_wnSamusCShotStatusUniqProcess;
    uniqProcs[1] = &g_wnSamusCShotStatusUniqProcess;
    uniqProcs[2] = &g_wnSamusCShotStatusUniqProcess;
    uniqProcs[3] = &g_wnSamusCShotStatusUniqProcess;
    uniqProcs[4] = &g_wnSamusCShotStatusUniqProcess;
    uniqProcs[5] = &g_wnSamusCShotStatusUniqProcess;
    uniqProcs[6] = &g_wnSamusCShotStatusUniqProcess;
    uniqProcs[7] = &g_wnSamusCShotStatusUniqProcess;
    uniqProcs[8] = &g_wnSamusCShotStatusUniqProcess;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 9);
    Weapon::deactivate(false);
}
#pragma pop

// The weak builders this unit owns in the link order (the other units with the same configurations reuse them).
typedef wnSamusCShotModuleAccesserBuildConfig ScBC;
// BYTES: the original addresses each singleton's destructor-chain record by its own address (no pooled anchor).
#pragma push
#pragma pool_data off
template soMotionModuleBuilder<ScBC::MotionModuleBuildConfig>::soMotionModuleBuilder(soModuleAccesser*, void*);
#pragma pop
template soStatusModuleBuilder<ScBC::StatusModuleBuildConfig>::soStatusModuleBuilder(soModuleAccesser*, const soStatusData*, void*);

#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnSamusCShotModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>

// Starts the shot on the arm cannon: kind (work int 0x10000007) is the owner's sub kind (Samus or Kirby), the charge
// (work float 0x11000001) starts at 0.
void wnSamusCShot::activate(int founderTaskId, int etcResourceId, int team, int kind) {
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
    m_moduleAccesser->getWorkManageModule().setInt(kind, 0x10000007);
    m_moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x11000001);
    m_moduleAccesser->getWorkManageModule().setInt(0, 0x10000006);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}

// The charge ratio (0 to 1) the fighter hands over when the shot is fired.
void wnSamusCShot::setCharge(float charge) {
    m_moduleAccesser->getWorkManageModule().setFloat(charge, 0x11000001);
}

// The shot spawns at the model origin, 8 units back along x for Samus's own shot (kind 3 in work int 0x10000007), then
// enters the status for its charge level.
void wnSamusCShot::toShoot(float lr, int charge) {
    float offsetX = m_moduleAccesser->getWorkManageModule().getInt(0x10000007) == 3 ? -8.0f : 0.0f;
    Vec3f offset(offsetX, 0.0f, 0.0f);
    Vec3f position = m_moduleAccesser->getModelModule().getNodeGlobalPosition(0, &offset, false, false);
    position.m_z = m_moduleAccesser->getGroundModule().getZ();
    m_moduleAccesser->getPostureModule().init(&position, lr);
    removeConstraint();
    switch (charge) {
    case 0:
        m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
        break;
    case 1:
        m_moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
        break;
    case 2:
        m_moduleAccesser->getStatusModule().changeStatusRequest(3, m_moduleAccesser);
        break;
    case 3:
        m_moduleAccesser->getStatusModule().changeStatusRequest(4, m_moduleAccesser);
        break;
    case 4:
        m_moduleAccesser->getStatusModule().changeStatusRequest(5, m_moduleAccesser);
        break;
    case 5:
        m_moduleAccesser->getStatusModule().changeStatusRequest(6, m_moduleAccesser);
        break;
    case 6:
        m_moduleAccesser->getStatusModule().changeStatusRequest(7, m_moduleAccesser);
        break;
    default:
        m_moduleAccesser->getStatusModule().changeStatusRequest(8, m_moduleAccesser);
        break;
    }
    updateNodeSRT();
}

const wnSamusCShotData* wnSamusCShot::getData() {
    return static_cast<const wnSamusCShotData*>(m_data);
}
