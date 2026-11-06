#define IP_INPUT_BUTTON_RAW // BYTES: IpNull::getButton returns the raw button mask (8 bytes, not a struct copy)
#define WN_STATUS_PROC_VECTOR2_DEFAULT_CTOR // the two-entry status process vector is default-constructed
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#include <wn/wn_module_accesser_builder.h>
// The status process vector and the attack part vector are built by their sora_melee constructors.
WN_STATUS_PROC_VECTOR_ARG_CTOR(2);
FT_DOL_ARRAY_VECTOR(soCollisionAttackPart, 4);
#include <wn/wn_activate_desc.h>
#include <so/damage/so_damage.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/camera/so_camera_module_impl.h>
#include <so/visibility/so_visibility_module_impl.h>
#include <ft/ft_common_data_accesser.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/stop/so_stop_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>
#include <wn/ganon/wn_ganon_beast.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

extern wnGanonBeastStatusUniqProcessStart g_wnGanonBeastStatusUniqProcessStart;  // HYPOTHESIS: the statuses' processes
extern wnGanonBeastStatusUniqProcessAttack g_wnGanonBeastStatusUniqProcessAttack;

inline soDamageEventObserver::~soDamageEventObserver() { }
inline void soDamageEventObserver::notifyEventOnDamage(soDamage*, bool, soModuleAccesser*) { }
inline void soDamageEventObserver::notifyEventAddDamage(soDamage*, soModuleAccesser*) { }

wnGanonBeast::wnGanonBeast(s32 articleId, const wnArticleBuildInfo& info, const wnGanonBeastData* data) :
    wnWeaponBuilder<wnGanonBeastModuleAccesserBuildConfig>(
        "GanonBeast", articleId, info,
        wnModuleAccesserBuildData(0x80, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  reinterpret_cast<s32>(data->unk38), 0, 0, 0, 0, 0, 0)),
    soDamageEventObserver(static_cast<soEventManager&>(getModuleAccesser()->getEventManageModule()).getManageId(), -1),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[2] = {};
    uniqProcs[0] = &g_wnGanonBeastStatusUniqProcessStart;
    uniqProcs[1] = &g_wnGanonBeastStatusUniqProcessAttack;
    getModuleAccesser()->getStatusModule().addRangeUniqProc(uniqProcs, 2);
    getModuleAccesser()->getCollisionHitModule().add(reinterpret_cast<soSet<soCollisionHitData>*>(const_cast<u8*>(data->unk40)), 0);
    Weapon::deactivate(false);
}

bool wnGanonBeast::notifyEventCollisionAttackCheck(u32) { return false; }

// HYPOTHESIS: a hit that sends the beast flying while it is not already stopped adds a hit stop and raises its work flag.
void wnGanonBeast::notifyEventOnDamage(soDamage* damage, bool, soModuleAccesser* moduleAccesser) {
    soStopModule* stop = &moduleAccesser->getStopModule();
    if (damage->m_reaction > 0.0f && !stop->isStop()) {
        int frames = soStopModuleImpl::calcHitStopFrame(damage->m_powerMax, 0.3846154f, 5.0f, 1.0f, 30.0f);
        moduleAccesser->getStopModule().setHitStopFrame(frames, true);
        moduleAccesser->getWorkManageModule().onFlag(0x12000005);
    }
}

void wnGanonBeast::activate(s32 founderTaskId, s32 resourceId, s32 team, const Vec3f* pos, float lr) {
    soModuleAccesser* accesser = m_moduleAccesser;
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0, *pos, lr, team, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    soVisibilityData* visData = *reinterpret_cast<soVisibilityData**>(static_cast<u8*>(g_ftCommonDataAccesser.getGanonBeastData()) + 0x24);
    accesser->getStatusModule().changeStatusForce(0, accesser);
    soVisibilityModuleImpl& visibility = dynamic_cast<soVisibilityModuleImpl&>(accesser->getVisibilityModule());
    visibility.setData(visData);
    for (int count = 0; count < 2; ++count) {
        visibility.setStatusDefault(0, 0, false);
    }
    visibility.setDefaultAll();
}

wnGanonBeastStatusUniqProcessStart::wnGanonBeastStatusUniqProcessStart() { }

wnGanonBeastStatusUniqProcessStart::~wnGanonBeastStatusUniqProcessStart() { }

wnGanonBeastStatusUniqProcessAttack::~wnGanonBeastStatusUniqProcessAttack() { }

void wnGanonBeastStatusUniqProcessStart::initStatus(soModuleAccesser* moduleAccesser) {
    soCameraModule* camera = moduleAccesser->m_enumerationStart->m_cameraModule;
    float lr = moduleAccesser->m_enumerationStart->m_postureModule->getLr();
    Vec3f offset;
    offset.m_x = lr * soValueAccesser::getConstantFloat(moduleAccesser, 4000, 0);
    offset.m_y = soValueAccesser::getConstantFloat(moduleAccesser, 4001, 0);
    offset.m_z = 0.0f;
    camera->addCameraRangeOffset(&offset, 0);
}

void wnGanonBeastStatusUniqProcessAttack::execStatus(soModuleAccesser* moduleAccesser) {
    soPostureModule* posture = moduleAccesser->m_enumerationStart->m_postureModule;
    Vec3f current = posture->getPos();
    Vec3f pos;
    pos = current;
    float lr = posture->getLr();
    float speed = soValueAccesser::getConstantFloat(moduleAccesser, 4002, 0);
    pos.m_x += speed * lr;
    posture->setPos(&pos);
}
