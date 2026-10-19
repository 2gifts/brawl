#define FT_SZEROSUIT_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_PARAM_ACCESSER_TYPED_DTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_manager.h>
#include <ft/ft_class_info_impl.h>
#include <ft/szerosuit/ft_szerosuit.h>
#include <so/so_value_accesser.h>

#define FT_BC ftSZerosuitBuildConfig
#include <ft/builder/ft_builder_noinline.h>

extern soStatusUniqProcess g_ftSZerosuitStatusUniqProcessSpecialLw;
extern soStatusUniqProcess g_ftSZerosuitStatusUniqProcessSpecialLwKick;
extern soStatusUniqProcess g_ftSZerosuitStatusUniqProcessSpecialLwStep;
extern soStatusUniqProcess g_ftSZerosuitStatusUniqProcessSpecialLwFlip;
extern soStatusUniqProcess g_ftSZerosuitStatusUniqProcessFinalSamus;

ftSZerosuit::ftSZerosuit(s32 entryId,
                         Heaps::HeapType instHeap,
                         Heaps::HeapType nwModelInstHeap,
                         Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftSZerosuitBuildConfig>(entryId,
                                             Fighter_SZeroSuit,
                                             instHeap,
                                             nwModelInstHeap,
                                             nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_SZeroSuit)) {
    // Register the status processes in action order.
    soStatusUniqProcess* processes[15] = {0};
    processes[3] = &g_ftSZerosuitStatusUniqProcessSpecialLw;
    processes[8] = &g_ftSZerosuitStatusUniqProcessSpecialLw;
    processes[9] = &g_ftSZerosuitStatusUniqProcessSpecialLwKick;
    processes[10] = &g_ftSZerosuitStatusUniqProcessSpecialLwStep;
    processes[11] = &g_ftSZerosuitStatusUniqProcessSpecialLwFlip;
    processes[12] = &g_ftSZerosuitStatusUniqProcessSpecialLw;
    processes[13] = &g_ftSZerosuitStatusUniqProcessSpecialLw;
    processes[14] = &g_ftSZerosuitStatusUniqProcessFinalSamus;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 15);
    // HYPOTHESIS: the area data of the kind sits at +0x9C of the fighter data.
    m_moduleAccesser->getAreaModule().addArea(reinterpret_cast<soSet<soAreaData>*>(reinterpret_cast<u8*>(m_data) + 0x9c));
}

// ---- the article holders --------------------------------------------------------------------------------------------
// HYPOTHESIS: the original builds the descriptors in the holder constructor itself (see ft_samus.cpp).
struct ftSZerosuitArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftSZerosuitArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftSZerosuitArticleKind : wnArticleKindInfo {
    ftSZerosuitArticleKind() {
        kind = 0;
        subKind = Fighter_SZeroSuit;
    }
};

struct ftSZerosuitArticleBuildInfo : wnArticleBuildInfo {
    ftSZerosuitArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_148BBC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148BD0(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148C18(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148C40(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <class W> struct ftSZerosuitArticleTraits;
#define FT_SZEROSUIT_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftSZerosuitArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftSZerosuitArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_SZeroSuit, &tag.m_resourceGroup)); \
        } \
    };

FT_SZEROSUIT_TRAITS(wnSZerosuitParalyzer, wnSZerosuitParalyzerData, 0, fn_27_148BBC)
FT_SZEROSUIT_TRAITS(wnSZerosuitParalyzerBullet, wnSZerosuitParalyzerBulletData, 1, fn_27_148BD0)
FT_SZEROSUIT_TRAITS(wnSZerosuitWhip, wnSZerosuitWhipData, 2, fn_27_148C18)
FT_SZEROSUIT_TRAITS(wnSZerosuitArmor, wnSZerosuitArmorData, 3, fn_27_148C40)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftSZerosuitArticleTraits<W>::ArticleId, ftSZerosuitArticleBuildInfo(ftSZerosuitArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftSZerosuitArticleTraits<W>::getData()) { }

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnSZerosuitParalyzer>::~wnArticleHolder();
template wnArticleHolder<wnSZerosuitParalyzerBullet>::~wnArticleHolder();
template wnArticleHolder<wnSZerosuitWhip>::~wnArticleHolder();
template wnArticleHolder<wnSZerosuitArmor>::~wnArticleHolder();
template wnArticleHolder<wnSZerosuitParalyzerBullet>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnSZerosuitArmor>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftSZerosuitArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftSZerosuitArticleMediator, 10>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftSZerosuitArticleMediator, 10>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftSZerosuitArticleMediator::~ftSZerosuitArticleMediator() { }
#pragma dont_inline off

s32 ftSZerosuitArticleMediator::getMediateNum() { return 4; }
void ftSZerosuitArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Paralyzer, 1 ParalyzerBullet, 2 Whip, 3 Armor (the rest are unused).

#define FT_SZEROSUIT_POOL_PARALYZER_BASE static_cast<wnArticlePool<wnSZerosuitParalyzer, 1, ftSZerosuitBulletPool>&>(m_pools)
#define FT_SZEROSUIT_POOL_PARALYZER FT_SZEROSUIT_POOL_PARALYZER_BASE.getSub()
#define FT_SZEROSUIT_POOL_BULLET_BASE static_cast<wnArticlePool<wnSZerosuitParalyzerBullet, 2, ftSZerosuitWhipPool>&>(m_pools)
#define FT_SZEROSUIT_POOL_BULLET FT_SZEROSUIT_POOL_BULLET_BASE.getSub()
#define FT_SZEROSUIT_POOL_WHIP_BASE static_cast<wnArticlePool<wnSZerosuitWhip, 1, ftSZerosuitArmorPool>&>(m_pools)
#define FT_SZEROSUIT_POOL_WHIP FT_SZEROSUIT_POOL_WHIP_BASE.getSub()
#define FT_SZEROSUIT_POOL_ARMOR_BASE static_cast<wnArticlePool<wnSZerosuitArmor, 8, soInstancePoolRoot>&>(m_pools)
#define FT_SZEROSUIT_POOL_ARMOR FT_SZEROSUIT_POOL_ARMOR_BASE.getSub()

#define FT_SZEROSUIT_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

void ftSZerosuitArticleMediator::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnSZerosuitParalyzer* weapon = FT_SZEROSUIT_POOL_PARALYZER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 2; ++i) {
        wnSZerosuitParalyzerBullet* weapon = FT_SZEROSUIT_POOL_BULLET.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnSZerosuitWhip* weapon = FT_SZEROSUIT_POOL_WHIP.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 8; ++i) {
        wnSZerosuitArmor* weapon = FT_SZEROSUIT_POOL_ARMOR.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftSZerosuitArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 1; }
    case 1: { wnArticleSlotTag tag; return 2; }
    case 2: { wnArticleSlotTag tag; return 1; }
    case 3: { wnArticleSlotTag tag; return 8; }
    FT_SZEROSUIT_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftSZerosuitActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

#define FT_SZEROSUIT_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftSZerosuitActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftSZerosuitActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftSZerosuitArticleMediator::getActiveNum(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: FT_SZEROSUIT_COUNT_ACTIVE(FT_SZEROSUIT_POOL_PARALYZER, 1)
    case 1: FT_SZEROSUIT_COUNT_ACTIVE(FT_SZEROSUIT_POOL_BULLET, 2)
    case 2: FT_SZEROSUIT_COUNT_ACTIVE(FT_SZEROSUIT_POOL_WHIP, 1)
    case 3: FT_SZEROSUIT_COUNT_ACTIVE(FT_SZEROSUIT_POOL_ARMOR, 8)
    FT_SZEROSUIT_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_SZEROSUIT_CAN_GENERATE(POOL, N, ID) \
    { \
        wnArticleSlotTag tag; \
        bool allActive; \
        s32 i; \
        for (i = 0; i < N; ++i) { \
            if (static_cast<soArticle&>(*POOL.getInstanceAt(i)).isActiveArticle() == false) { \
                allActive = false; \
                goto done_##ID; \
            } \
        } \
        allActive = true; \
    done_##ID: \
        return allActive == false; \
    }

bool ftSZerosuitArticleMediator::isGeneratable(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: FT_SZEROSUIT_CAN_GENERATE(FT_SZEROSUIT_POOL_PARALYZER, 1, 0)
    case 1: FT_SZEROSUIT_CAN_GENERATE(FT_SZEROSUIT_POOL_BULLET, 2, 1)
    case 2: FT_SZEROSUIT_CAN_GENERATE(FT_SZEROSUIT_POOL_WHIP, 1, 2)
    case 3: FT_SZEROSUIT_CAN_GENERATE(FT_SZEROSUIT_POOL_ARMOR, 8, 3)
    FT_SZEROSUIT_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftSZerosuitArticleActivator;
template <> struct ftSZerosuitArticleActivator<wnSZerosuitParalyzer> {
    static bool activate(wnSZerosuitParalyzer* weapon, soModuleAccesser* acc) { return ftSZerosuitTransactor::getInstance()->activeArticle1(weapon, acc); }
};
template <> struct ftSZerosuitArticleActivator<wnSZerosuitParalyzerBullet> {
    static bool activate(wnSZerosuitParalyzerBullet* weapon, soModuleAccesser* acc) { return ftSZerosuitTransactor::getInstance()->activeArticle2(weapon, acc); }
};
template <> struct ftSZerosuitArticleActivator<wnSZerosuitWhip> {
    static bool activate(wnSZerosuitWhip* weapon, soModuleAccesser* acc) { return ftSZerosuit::activeArticle(weapon, acc); }
};
template <> struct ftSZerosuitArticleActivator<wnSZerosuitArmor> {
    static bool activate(wnSZerosuitArmor* weapon, soModuleAccesser* acc) { return ftSZerosuit::activeArticle1(weapon, acc); }
};

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_SZEROSUIT_GENERATE(W, POOL) \
    { \
        wnArticleSlotTag tag; \
        soArticleDeactivateChecker checker; \
        volatile u8 findTag = 0; /* MATCH-ONLY: the native traversal keeps a one-byte marker */ \
        W* weapon = POOL##_BASE.find(checker); \
        if (weapon == NULL) { \
            weapon = static_cast<W*>(checker.getCandidate()); \
            if (weapon == NULL) { \
                return wnGetNullArticle(); \
            } \
            weapon->deactivateArticle(); \
        } \
        if (ftSZerosuitArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftSZerosuitArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_SZEROSUIT_GENERATE(wnSZerosuitParalyzer, FT_SZEROSUIT_POOL_PARALYZER)
    case 1: FT_SZEROSUIT_GENERATE(wnSZerosuitParalyzerBullet, FT_SZEROSUIT_POOL_BULLET)
    case 2: FT_SZEROSUIT_GENERATE(wnSZerosuitWhip, FT_SZEROSUIT_POOL_WHIP)
    case 3: FT_SZEROSUIT_GENERATE(wnSZerosuitArmor, FT_SZEROSUIT_POOL_ARMOR)
    FT_SZEROSUIT_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftSZerosuitArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnSZerosuitParalyzer&>(*article);
        return true;
    }
    case 1: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnSZerosuitParalyzerBullet&>(*article);
        return true;
    }
    case 2: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnSZerosuitWhip&>(*article);
        return true;
    }
    case 3: {
        wnArticleSlotTag tag;
        return ftSZerosuit::shootArticle(&dynamic_cast<wnSZerosuitArmor&>(*article), acc);
    }
    FT_SZEROSUIT_UNUSED_SLOTS(true)
    default: return false;
    }
}
