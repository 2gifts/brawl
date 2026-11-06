#define WN_SIMPLE_AFTER_ARTICLE_BUILDERS
#define FT_POOL_ROOT_NONPOLYMORPHIC
#define WN_TOON_LINK_HOOK_SHOT_FULL
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define WN_KINETIC_GENERIC_BUILDER
#include <wn/wn_module_accesser_builder.h>
WN_STATUS_PROC_VECTOR_ARG_CTOR(9);
#include <wn/toonlink/wn_toon_link_hook_shot.h>
#include <ft/purin/ft_purin_article.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>

// The status unique processes of the hookshot (named Lasso in the object).
extern soStatusUniqProcess g_wnLassoStatusUniqProcessShoot;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessRewind;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessPull;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessHang;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessHangRewind;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessReach;

wnInstanceHolderHookShot::wnInstanceHolderHookShot(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(0, wnSimpleConstructionInfo(wnSimpleKindInfo(Fighter_Purin), acc->m_enumerationStart->m_heapModule),
               g_ftCommonDataAccesser.getSimpleData(static_cast<ftKind>(wnSimpleKindInfo(Fighter_Purin).subKind),
                                                    &ftArticleDataFlags().get().resourceGroup), false) { }

wnToonLinkHookShot::wnToonLinkHookShot(s32 articleId, const wnArticleBuildInfo& info, const wnToonLinkHookShotData* data) :
    wnWeaponBuilder<wnToonLinkHookShotModuleAccesserBuildConfig>(
        "ToonLinkHookShot", articleId, info,
        wnModuleAccesserBuildData(148, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1,
                                  &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[9];
    for (int i = 0; i < 9; ++i) uniqProcs[i] = 0;
    uniqProcs[1] = &g_wnLassoStatusUniqProcessShoot;
    uniqProcs[3] = &g_wnLassoStatusUniqProcessRewind;
    uniqProcs[4] = &g_wnLassoStatusUniqProcessPull;
    uniqProcs[5] = &g_wnLassoStatusUniqProcessHang;
    uniqProcs[6] = &g_wnLassoStatusUniqProcessHangRewind;
    uniqProcs[7] = &g_wnLassoStatusUniqProcessReach;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 9);
    Weapon::deactivate(false);
}

struct wnToonLinkHookShotActivator {
    static bool activate(wnSimple* weapon, soModuleAccesser* acc) { return wnToonLinkHookShot::activeArticle(weapon, acc); }
};

bool wnToonLinkHookShot::activeArticle(wnSimple* article, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    s32 founderTaskId = acc->getStageObject().m_taskId;
    const Vec3f& position = Vec3f(0.0f, 0.0f, 0.0f);
    ftData* data = g_ftCommonDataAccesser.getData(Fighter_ToonLink);
    // HYPOTHESIS: source member name; offset 0xC4 of the fighter data holds the simple-article data of the claw tip.
    wnSimpleData* articleData = *reinterpret_cast<wnSimpleData**>(reinterpret_cast<u8*>(data) + 0xC4);
    article->activate(founderTaskId, resources.getResourceIdAccesser()->getBinResId(), articleData, true, false, position,
                      0xFFFF, 0xFFFF, 0, true, false, -1, 0, false, 2, 0, false);
    return true;
}

// HYPOTHESIS: the module at slot 0x9C of the enumeration, typed only by the vtable slots the hookshot calls.
struct wnToonLinkHookShotSlotModule {
    u32 pad[2];
    virtual void v08();
    virtual void v0C();
    virtual void v10();
    virtual void v14();
    virtual void v18(int, int, int, int, bool, int, int, int);
    virtual void v1C();
    virtual void v20();
    virtual void v24();
    virtual void v28();
    virtual void v2C();
    virtual void v30();
    virtual void v34();
    virtual void v38();
    virtual void v3C();
    virtual void v40();
    virtual void v44();
    virtual void v48();
    virtual void v4C();
    virtual void v50();
    virtual void v54();
    virtual void v58();
    virtual void v5C();
    virtual void v60();
    virtual void v64();
    virtual void v68();
    virtual void v6C();
    virtual void v70();
    virtual void v74();
    virtual void v78();
    virtual void v7C();
    virtual void v80();
    virtual void v84();
    virtual void v88();
    virtual void v8C();
    virtual void v90();
    virtual void v94();
    virtual void v98();
    virtual void v9C();
    virtual void vA0();
    virtual void vA4(int);
    virtual void vA8();
    virtual void vAC();
    virtual void vB0();
    virtual void vB4();
    virtual void vB8();
    virtual void vBC();
    virtual void vC0();
    virtual void vC4();
    virtual void vC8();
    virtual void vCC();
    virtual void vD0();
    virtual void vD4();
    virtual void vD8();
    virtual void vDC();
};

static s32 negOne = -1;

void wnToonLinkHookShot::activate(int founderTaskId, int resourceId, int team, float lr, const Vec3f& position, bool unk) {
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0, position, lr, team, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    const soModuleEnumeration* modules = m_moduleAccesser->m_enumerationStart;
    wnToonLinkHookShotSlotModule* slot = reinterpret_cast<wnToonLinkHookShotSlotModule*>(modules->m_physicsModule);
#define SLOT reinterpret_cast<wnToonLinkHookShotSlotModule*>(m_moduleAccesser->m_enumerationStart->m_physicsModule)
    s32 n = negOne;
    s32 binResId = modules->m_resourceModule->getResourceIdAccesser()->getBinResId();
    slot->v18(binResId, 0, n, n, true, 0, 0, -1);
    m_moduleAccesser->getWorkManageModule().setFlag(unk, 0x22000000);
    m_moduleAccesser->getWorkManageModule().setInt(0x17, 0x10000005);
    soArticle* tip = static_cast<soGenerateArticleManageModule*>(m_moduleAccesser->m_enumerationStart->m_generateArticleManageModule)->generate(0, 0, false);
    if (!reinterpret_cast<soNullable*>(tip)->isNull()) {
        static_cast<soGenerateArticleManageModule*>(m_moduleAccesser->m_enumerationStart->m_generateArticleManageModule)->entry(tip);
    }
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    m_moduleAccesser->getPostureModule().getScale();
    SLOT->vDC();
    SLOT->vA4(6);
}


soArticle* soArticleMediatorImplHookShot::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<wnToonLinkHookShotActivator>(m_pools, acc, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}
bool soArticleMediatorImplHookShot::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<soInstancePoolHookShot >(this, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(false)
    default: return false;
    }
}
s32 soArticleMediatorImplHookShot::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<soInstancePoolHookShot, wnArticleSlotCounter>(this, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(0)
    default: return 0;
    }
}
s32 soArticleMediatorImplHookShot::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(0)
    default: return 0;
    }
}
bool soArticleMediatorImplHookShot::shoot(soModuleAccesser*, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnSimple>(article, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(true)
    default: return false;
    }
}
void soArticleMediatorImplHookShot::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        if (!static_cast<soArticle&>(*m_pools.getSub().getInstanceAt(i)).setDeactivateDescendant()) return;
    }
}
s32 soArticleMediatorImplHookShot::getMediateNum() { return 1; }
void soArticleMediatorImplHookShot::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

#pragma dont_inline on
wnInstanceHolderHookShot::~wnInstanceHolderHookShot() { }
soSelectInstanceHolderHookShot::~soSelectInstanceHolderHookShot() { }
soGenerateArticleManageModuleBuilderHookShot::~soGenerateArticleManageModuleBuilderHookShot() { }
#pragma dont_inline off
