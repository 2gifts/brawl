#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define WN_PIKACHU_CLOUD_UNIT
#define FT_POOL_ROOT_NONPOLYMORPHIC // BYTES: the hierarchy destructor expands the pool level (the root has no vptr of its own)
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/pikachu/wn_pikachu_cloud.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/collision/so_collision_manager.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <ft/pikachu/ft_pikachu.h>


// The status process of the cloud: it drops a bolt every few frames while any are left.
class wnPikachuCloudStatusUniqProcessRegular : public soStatusUniqProcess {
public:
    virtual ~wnPikachuCloudStatusUniqProcessRegular() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnPikachuCloudStatusUniqProcessRegular g_wnPikachuCloudStatusUniqProcessRegular;


wnPikachuCloud::wnPikachuCloud(s32 articleId, const wnArticleBuildInfo& info, const wnPikachuCloudData* data) :
    wnWeaponBuilder<wnPikachuCloudModuleAccesserBuildConfig>(
        "wnPikachuCloud", articleId, info,
        wnModuleAccesserBuildData(0xC, data, info.getHeapModule()->getInstanceHeap(),
                                  info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(),
                                  static_cast<u8>(data->m_resGroupNo), -1, &g_soParamAccesserNull, 0, 0, 0, 0, 0, 0, 0)) {
    soStatusUniqProcess* uniqProcs[1];
    uniqProcs[0] = &g_wnPikachuCloudStatusUniqProcessRegular;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 1);
    Weapon::deactivate(false);
}

#pragma auto_inline off

// The cloud is activated at the position of Pikachu raised by the thunder parameter; its work area keeps the team and the etc
// resource id for the bolts, the number of bolts, and the owner.
void wnPikachuCloud::activate(int founderTaskId, int team, const Vec3f* position, int etcResId, ftPikachu* owner) {
    float* offset = static_cast<float*>(ftPikachu::getThunderParam());
    Vec3f pos;
    pos = *position;
    pos.m_y += *offset;
    Vec3f posCopy;
    posCopy = pos;
    wnActivateDesc desc(founderTaskId, 0xFFFF, 0xFFFF, 0xFFFF, -1, -1, 0, 0, posCopy, 0.0f, team, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.flag4DBit7 = 0;
    desc.flag4DBit6 = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    soWorkManageModule& workManage = m_moduleAccesser->getWorkManageModule();
    soGenerateArticleManageModule& articles = m_moduleAccesser->getGenerateArticleManageModule();
    workManage.setFloat(pos.m_x, 0x11000001);
    workManage.setFloat(pos.m_y, 0x11000002);
    workManage.setFloat(pos.m_z, 0x11000003);
    workManage.setInt(team, 0x1000000C);
    workManage.setInt(etcResId, 0x1000000E);
    alloc_log_group();
    rec_pikachu(owner);
    workManage.onFlag(0x12000000);
    workManage.offFlag(0x12000004);
    int num = articles.getGenerateMaxNum(0);
    workManage.setInt(num, 0x10000007);
    workManage.setInt(num, 0x10000008);
    workManage.setInt(0, 0x10000009);
    workManage.setInt(100, 0x1000000B);
}

void wnPikachuCloud::onDeactivate() {
    wnPikachuThunder* thunder = get_thunder();
    if (thunder != NULL) {
        thunder->remove_chain();
    }
    thunder = get_thunder();
    if (thunder != NULL) {
        thunder->deactive_cloud();
    }
    free_log_group();
}

bool wnPikachuCloud::activeArticle(wnPikachuThunder* thunder, soModuleAccesser* acc) {
    soWorkManageModule& workManage = acc->getWorkManageModule();
    wnPikachuCloud& cloud = dynamic_cast<wnPikachuCloud&>(acc->getStageObject());
    wnPikachuThunder* head = cloud.get_thunder();
    bool first = !head;
    Vec3f pos = cloud.get_thunder_generate_pos();
    int team = workManage.getInt(0x1000000C);
    int etcResId = workManage.getInt(0x1000000E);
    workManage.decInt(0x10000007);
    thunder->activate(cloud.m_taskId, etcResId, team, &pos, &cloud, first);
    if (first) {
        cloud.rec_thunder(thunder);
    } else {
        head->chain_last(thunder);
    }
    if (workManage.isFlag(0x12000005)) {
        int newTeam = workManage.getInt(0x1000000C);
        int newEtcResId = workManage.getInt(0x1000000D);
        thunder->change_team(newTeam, newEtcResId);
    }
    return false;
}

void wnPikachuCloud::rec_pikachu(ftPikachu* pikachu) {
    m_moduleAccesser->getWorkManageModule().setInt(reinterpret_cast<int>(pikachu), 0x10000006);
}

ftPikachu* wnPikachuCloud::get_pikachu() {
    return reinterpret_cast<ftPikachu*>(m_moduleAccesser->getWorkManageModule().getInt(0x10000006));
}

void wnPikachuCloud::rec_thunder(wnPikachuThunder* thunder) {
    m_moduleAccesser->getWorkManageModule().setInt(reinterpret_cast<int>(thunder), 0x10000005);
}

wnPikachuThunder* wnPikachuCloud::get_thunder() {
    return reinterpret_cast<wnPikachuThunder*>(m_moduleAccesser->getWorkManageModule().getInt(0x10000005));
}

// The bolts start where the cloud is.
Vec3f wnPikachuCloud::get_thunder_generate_pos() {
    soWorkManageModule& workManage = m_moduleAccesser->getWorkManageModule();
    Vec3f pos;
    pos.m_x = workManage.getFloat(0x11000001);
    pos.m_y = workManage.getFloat(0x11000002);
    pos.m_z = workManage.getFloat(0x11000003);
    return pos;
}

// Whether the position is under the cloud, within the width of a bolt, and the head of the chain is hit there.
bool wnPikachuCloud::thunder_hitcheck(Vec3f* position) {
    soWorkManageModule& workManage = m_moduleAccesser->getWorkManageModule();
    Vec3f cloudPos = m_moduleAccesser->getPostureModule().getPos();
    const float* width = *reinterpret_cast<float**>(static_cast<u8*>(g_ftCommonDataAccesser.getPikachuThunderData()) + 0x34);
    float half = 0.5f * *width;
    float minX = cloudPos.m_x - half;
    float maxX = cloudPos.m_x + half;
    if (workManage.isFlag(0x12000005)) {
        return false;
    }
    if (minX <= position->m_x && position->m_x <= maxX) {
        wnPikachuThunder* thunder = get_thunder();
        if (thunder != NULL && thunder->hitcheck(position)) {
            workManage.incInt(0x10000008);
            return true;
        }
    }
    return false;
}

void wnPikachuCloud::change_head_thunder(wnPikachuThunder* thunder) {
    rec_thunder(thunder);
}

void wnPikachuCloud::deactive_thunder(int thunder) {
    soWorkManageModule& workManage = m_moduleAccesser->getWorkManageModule();
    workManage.incInt(0x10000009);
    int left = workManage.getInt(0x10000007);
    int total = workManage.getInt(0x10000008);
    int dropped = workManage.getInt(0x10000009);
    if (left <= 0 && dropped >= total) {
        deactivate(false);
    }
}

void wnPikachuCloud::notify_reflect(int team, int etcResId) {
    soWorkManageModule& workManage = m_moduleAccesser->getWorkManageModule();
    workManage.onFlag(0x12000005);
    workManage.setInt(team, 0x1000000C);
    workManage.setInt(etcResId, 0x1000000D);
    workManage.onFlag(0x12000006);
}

void wnPikachuCloud::set_change_team() {
    soWorkManageModule& workManage = m_moduleAccesser->getWorkManageModule();
    if (workManage.isFlag(0x12000006)) {
        wnPikachuThunder* thunder;
        int team = workManage.getInt(0x1000000C);
        int etcResId = workManage.getInt(0x1000000D);
        thunder = get_thunder();
        if (thunder != NULL) {
            thunder->unref_log_group(true);
        }
        ftPikachu* pikachu = get_pikachu();
        if (pikachu != NULL) {
            pikachu->notify_thunder_reflect();
        }
        free_log_group();
        alloc_log_group();
        if (thunder != NULL) {
            thunder->change_team(team, etcResId);
            thunder->ref_log_group(true);
        }
        workManage.offFlag(0x12000006);
    }
}

void wnPikachuCloud::alloc_log_group() {
    soWorkManageModule& workManage = m_moduleAccesser->getWorkManageModule();
    int logGroup = soCollisionManager::getInstance()->allocateLogGroup();
    workManage.setInt(logGroup, 0x1000000A);
    if (logGroup != -1) {
        workManage.onFlag(0x12000003);
    }
}

// MATCH-ONLY: the REL carries its own copy of the lazy singleton getter.
soCollisionManager* soCollisionManager::getInstance() {
    if (::g_soCollisionManager == NULL) {
        ::g_soCollisionManager = new (Heaps::System) soCollisionManager;
    }
    return ::g_soCollisionManager;
}

void wnPikachuCloud::free_log_group() {
    soWorkManageModule& workManage = m_moduleAccesser->getWorkManageModule();
    if (workManage.isFlag(0x12000003)) {
        int logGroup = workManage.getInt(0x1000000A);
        soCollisionManager::getInstance()->leaveFreeRefLogGroup(logGroup);
        soCollisionManager::getInstance()->freeLogGroup(logGroup);
        workManage.offFlag(0x12000003);
    }
}

void wnPikachuCloud::ref_log_group(soCollisionAttackModule* attack) {
    soWorkManageModule& workManage = m_moduleAccesser->getWorkManageModule();
    if (workManage.isFlag(0x12000003)) {
        attack->refLogGroup(0, workManage.getInt(0x1000000A));
    }
}

void wnPikachuCloud::unref_log_group(soCollisionAttackModule* attack) {
    attack->unRefLogGroup(0);
}

bool wnPikachuCloud::isDropGround() {
    return m_moduleAccesser->getWorkManageModule().isFlag(0x12000004);
}

// HYPOTHESIS: the hit effect starts at the position of Pikachu (the first parent of the cloud).
void wnPikachuCloud::call_pikachu_hit_effect() {
    Vec3f posCopy;
    Vec3f pos = m_moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(3, static_cast<u32>(0), true);
    posCopy = pos;
    call_effect(&posCopy, static_cast<EfID>(0x80006));
}

void wnPikachuCloud::call_ground_hit_effect(Vec3f* position) {
    call_effect(position, static_cast<EfID>(0x80007));
}

// One effect per drop, until the drop is reset.
void wnPikachuCloud::call_effect(Vec3f* position, EfID effectId) {
    soModuleAccesser* accesser = m_moduleAccesser;
    soWorkManageModule* workManage = &accesser->getWorkManageModule();
    if (!workManage->isFlag(0x12000004)) {
        accesser->getEffectModule().req(effectId, position, &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
        workManage->onFlag(0x12000004);
    }
}

void wnPikachuCloud::set_pikachu_pos(float y) {
    wnPikachuThunder* thunder = get_thunder();
    if (thunder != NULL) {
        thunder->rec_end_pos(y);
    }
}

void wnPikachuCloudStatusUniqProcessRegular::initStatus(soModuleAccesser* moduleAccesser) {
    dynamic_cast<wnPikachuCloud&>(moduleAccesser->getStageObject());
}

// Drops the next bolt once the interval of the bolts is over.
void wnPikachuCloudStatusUniqProcessRegular::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    workManage.incInt(0x1000000B);
    int count = workManage.getInt(0x1000000B);
    if (count >= wnPikachuThunder::getThunderGenerateInterval()) {
        if (workManage.getInt(0x10000007) >= 1) {
            moduleAccesser->getGenerateArticleManageModule().generate(0, 0, false);
        }
        workManage.setInt(0, 0x1000000B);
    }
}

void wnPikachuCloudStatusUniqProcessRegular::execFixPos(soModuleAccesser* moduleAccesser) {
    dynamic_cast<wnPikachuCloud&>(moduleAccesser->getStageObject()).set_change_team();
}

// ---- the thunder pool ---------------------------------------------------------------------------------------------------
// The mediator hands out the first inactive bolt (newest first), else deactivates the oldest active one and reuses it. Slot 0
// is the only article id; the 16 other slots answer with the neutral value.
// HYPOTHESIS: the cloud activates the bolt itself (its activator is a static member taking the bolt and the accesser).
template <class PoolTL, class AssignTL, class KindInfo, class Extra>
template <int S, int L>
inline soArticle* soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::generateSlot(soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
    soArticleDeactivateChecker checker;
    wnPikachuThunder* thunder = m_pools.getSub().find(checker, soIntToType<0>());
    if (thunder == NULL) {
        thunder = static_cast<wnPikachuThunder*>(checker.getCandidate());
        if (thunder == NULL) {
            return wnGetNullArticle();
        }
        thunder->deactivateArticle();
    }
    if (wnPikachuCloud::activeArticle(thunder, acc) == true) {
        return &static_cast<soArticle&>(*thunder);
    }
    return wnGetNullArticle();
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
template <int S, int L>
inline bool soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::isGeneratableSlot(const soIntToType<S>&, const soIntToType<L>&) {
    bool allActive;
    s32 i;
    for (i = 0; i < 4; ++i) {
        if (static_cast<soArticle&>(*m_pools.getSub().getInstanceAt(i)).isActiveArticle() == false) {
            allActive = false;
            goto done;
        }
    }
    allActive = true;
done:
    return allActive == false;
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
template <int S, int L>
inline s32 soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::getActiveNumSlot(const soIntToType<S>&, const soIntToType<L>&) {
    wnArticleSlotCounter counter;
    counter.m_isActive = wnArticleIsActive;
    counter.m_activeNum = 0;
    counter.m_inactiveNum = 0;
    for (s32 i = 0; i < 4; ++i) {
        wnArticleSlotCounter* c = &counter;
        if (c->m_isActive(&static_cast<soArticle&>(*m_pools.getSub().getInstanceAt(i))) == true) {
            ++c->m_activeNum;
        } else {
            ++c->m_inactiveNum;
        }
    }
    return counter.m_activeNum;
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
soArticle* soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return generateSlot(acc, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
bool soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return isGeneratableSlot(WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(false)
    default: return false;
    }
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
s32 soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return getActiveNumSlot(WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(0)
    default: return 0;
    }
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
s32 soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(4, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(0)
    default: return 0;
    }
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
bool soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::shoot(soModuleAccesser*, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnPikachuThunder>(article, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(true)
    default: return false;
    }
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
void soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::deactivate() {
    for (s32 i = 0; i < 4; ++i) {
        wnPikachuThunder* thunder = m_pools.getSub().getInstanceAt(i);
        if (!static_cast<soArticle&>(*thunder).setDeactivateDescendant()) {
            return;
        }
    }
}

// The original names the identical one-line capacity of this array for the mediator's getMediateNum (both return one).
template s32 soArrayVector<soInstanceUnitFullProperty<soCollisionCatchEventObserver*>, 1>::capacity() const;
