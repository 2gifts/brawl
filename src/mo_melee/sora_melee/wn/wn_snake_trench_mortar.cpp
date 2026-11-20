#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: the muzzle direction is copied as an aggregate (word moves), see mt_vector.h
#define SO_ARTICLE_EVENT_OBSERVER_FULL // BYTES: the 16-byte article event observer with its out-of-line constructor (so/article/so_article.h)
#define FT_MODULE_BUILDER
#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the article destructor expands the empty soParamAccesser base destructor
#define WN_ANIMCMD_UNIT5_SELECT
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define WN_KINETIC_GENERIC_BUILDER
#define WN_KINETIC_NORMAL_HOLDER_DEFAULT
#define WN_KINETIC_NORMAL_EXTERN
#define WN_INSIDE_EVENT_NULL_TAIL
#define SO_SHAKE_TERM_CTOR
#define SO_EFFECT_CONTINUAL_CTOR
#define SO_OTHER_ANIM_CTOR
#define SO_COLLISION_GROUP_ALIGNED
#define WN_TRANSITION_CTOR_INLINE
#define WN_TRANSITION_INFO_BARE
#define WN_ATTACK_ABSOLUTE_DTOR_INLINE
#define SO_NULLABLE_INLINE_CTOR
#define WN_ANIMCMD_UNIT_CTOR_OUTLINE
#define WN_ANIMCMD_UNIT_FLOAT_ARG
#define WN_ANIMCMD_STACK_PTR
#define WN_ACCESSER_CTOR_INLINE
#define soLineHierarchy soLineHierarchyOfArrayPool
#include <wn/wn_module_accesser_builder.h>
#undef soLineHierarchy
#include <wn/snake/wn_snake_trench_mortar.h>
#include <wn/wn_class_info_impl.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <gf/gf_task_scheduler.h>

inline soArticleMediator::~soArticleMediator() { }
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }

wnSnakeTrenchMortar::wnSnakeTrenchMortar(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeTrenchMortarData* data) :
    wnWeaponBuilder<wnSnakeTrenchMortarModuleAccesserBuildConfig>(
        "SnakeTrenchMortar", articleId, info,
        wnModuleAccesserBuildData(19, data, info.getHeapModule()->getInstanceHeap(),
            info.getHeapModule()->getNWModelInstanceHeap(),
            info.getHeapModule()->getNWMotionInstanceHeap(),
            static_cast<u8>(data->m_resGroupNo), -1, &g_soParamAccesserNull, 0, 0, 0, 0, 0, 0, 0)),
    unk7CE0(0.0f), unk7CE4(0.0f) {
    Weapon::deactivate(false);
}


// The article constructor calls the generate-article builder constructor out of line (a copy of its own in the unit).
template <class Mediator>
soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfig<Mediator> >::soGenerateArticleManageModuleBuilder(
    soModuleAccesser* acc) :
    m_articles(), m_observers(BC::ArticleCount, 0), m_mediator(acc),
    m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }

// HYPOTHESIS: parameter meanings from the fighter's activator call (resource id, team, facing, founder task id).
void wnSnakeTrenchMortar::activate(int etcResourceId, int team, float lr, int founderTaskId) {
    unk7CE0 = lr;
    Vec3f position(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc(founderTaskId, etcResourceId, etcResourceId, etcResourceId, -1, -1, 0, 0, position, lr, team, 0, 2, 0x80,
                        0, 0x35F, 0);
    desc.flagBit7 = 1;
    desc.flagBit6 = 1;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
}

wnSnakeTrenchMortar::~wnSnakeTrenchMortar() { }

// ---- the shell pool -----------------------------------------------------------------------------------------------------
// Hands out the first inactive shell (newest first), else deactivates the oldest active one and reuses it. The generate call
// answers the null article even for a shell it just started (as the Toad's spore pool does).
// HYPOTHESIS: the roles of the model node 2 and the mortar's two stored floats.
inline bool wnSnakeTrenchMortarActivateBullet(wnSnakeTrenchMortarBullet* bullet, soModuleAccesser* acc) {
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(2, false);
    pos.m_z = acc->getGroundModule().getZ();
    Vec3f rot = acc->getModelModule().getNodeGlobalRotation(2, false);
    wnSnakeTrenchMortar* mortar = dynamic_cast<wnSnakeTrenchMortar*>(acc->m_stageObject);
    if (mortar != NULL) {
        float speed = mortar->unk7CE4;
        Vec3f dir;
        dir = Vec3f(1.0f, 0.0f, 0.0f);
        float lr = mortar->unk7CE0;
        bullet->activate(acc->m_stageObject->m_taskId, acc->getResourceModule().getResourceIdAccesser()->getAnmResId(),
                         acc->getTeamModule().getTeam()->getNo(), &pos, &dir, lr, speed);
    }
    return false;
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
template <int S, int L>
inline soArticle* soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::generateSlot(soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
    soArticleDeactivateChecker checker;
    wnSnakeTrenchMortarBullet* bullet = m_pools.find(checker, soIntToType<0>());
    if (bullet == NULL) {
        bullet = static_cast<wnSnakeTrenchMortarBullet*>(checker.getCandidate());
        if (bullet == NULL) {
            return wnGetNullArticle();
        }
        bullet->deactivateArticle();
    }
    if (wnSnakeTrenchMortarActivateBullet(bullet, acc) == true) {
        return &static_cast<soArticle&>(*bullet);
    }
    return wnGetNullArticle();
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
template <int S, int L>
inline bool soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::isGeneratableSlot(const soIntToType<S>&, const soIntToType<L>&) {
    bool allActive;
    s32 i;
    for (i = 0; i < 3; ++i) {
        if (static_cast<soArticle&>(*m_pools.getInstanceAt(i)).isActiveArticle() == false) {
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
    for (s32 i = 0; i < 3; ++i) {
        wnArticleSlotCounter* c = &counter;
        if (c->m_isActive(&static_cast<soArticle&>(*m_pools.getInstanceAt(i))) == true) {
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
    case 0: return wnArticleSlotGenerateMaxNum(3, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(0)
    default: return 0;
    }
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
bool soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::shoot(soModuleAccesser*, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnSnakeTrenchMortarBullet>(article, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(true)
    default: return false;
    }
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
void soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::deactivate() {
    for (s32 i = 0; i < 3; ++i) {
        wnSnakeTrenchMortarBullet* bullet = m_pools.getInstanceAt(i);
        if (!static_cast<soArticle&>(*bullet).setDeactivateDescendant()) {
            return;
        }
    }
}
