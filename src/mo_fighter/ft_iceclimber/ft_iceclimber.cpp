// TODO: clean up - raw offset accesses into fighter/module structs; add named fields to the fighter class (or a local struct) instead of *(T*)(p + 0x..)
#define FT_POOL_ROOT_NONPOLYMORPHIC
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_TEAM_TYPED_INTERFACE
#define FT_SOUND_GENERATOR_DTOR_NOINLINE
#define FT_FIGHTER_ANIMCMD_LONG // MATCH-ONLY: Fighter::notifyEventAnimCmd(long) overrides the StageObject slot (as in ft_marth.cpp)
#define WN_WEAPON_ANIMCMD_LONG // Weapon::notifyEventAnimCmd(long) overrides the StageObject slot, so deactivateArticle is at 0x1F8
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_POPO_ARTICLE_POOLS
#define MT_VEC3F_OPERATORS_NOINLINE
#define MT_VEC3F_SUB_NATIVE_ORDER
#define MT_VEC3F_LENGTH_SQ_NOINLINE
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
// soKineticEnergy(bool) is only declared if this policy is set the first time the header is parsed.
// Leave the policy off for the headers before ft_iceclimber.h: with it on, the shared posture constructor
// is instantiated before the fighter specialization in ft_module_builders.h.
#define WN_KINETIC_GENERIC_BUILDER
#include <so/kinetic/so_kinetic_energy.h>
#undef WN_KINETIC_GENERIC_BUILDER
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_manager.h>
#include <ft/ft_class_info_impl.h>
#include <ft/iceclimber/ft_iceclimber.h>
#include <ft/iceclimber/ft_iceclimber_extend_param_accesser.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>
#include <ft/iceclimber/ft_iceclimber_transactor.h>

#include <so/glow/so_glow_module_simple.h>

// The fighter owns the shared out-of-line kinetic teardown.
#undef WN_ARTICLE_NATIVE_BUILDERS
#define FT_OBSERVER_VTABLE_ANCHOR_WEAK
#define FT_BC ftPopoBuildConfig
#include <ft/builder/ft_builder_noinline.h>
#include <ft/ft_owner.h>
#include <so/so_external_value_accesser.h>
#include <ai/ai_mgr.h>

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt); the repeated ones are the null placeholders of the
// unused status ranges.
extern "C" {
extern soStatusUniqProcess lbl_105_bss_28C;
extern soStatusUniqProcess lbl_105_bss_2AC;
extern soStatusUniqProcess lbl_105_bss_2FC;
extern soStatusUniqProcess lbl_105_bss_2DC;
extern soStatusUniqProcess lbl_105_bss_29C;
extern soStatusUniqProcess lbl_105_bss_2BC;
extern soStatusUniqProcess lbl_105_bss_2CC;
extern soStatusUniqProcess lbl_105_bss_2EC;
extern soStatusUniqProcess lbl_105_bss_27C;
extern soStatusUniqProcess lbl_105_bss_30C;
extern soStatusUniqProcess lbl_105_bss_31C;
}

ftPopoExtendParamAccesser g_ftPopoExtendParamAccesser;

ftClassInfoImpl<Fighter_Popo, ftPopo> g_ftClassInfoPopo;

ftPopo::ftPopo(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftPopoBuildConfig>(entryId,
                                         Fighter_Popo,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Popo)),
    m_isPartner(false) {
    // Register the status processes in action order.
    soStatusUniqProcess* processes[15] = {0};
    processes[1] = &lbl_105_bss_28C;
    processes[2] = &lbl_105_bss_2AC;
    processes[3] = &lbl_105_bss_2FC;
    processes[4] = &lbl_105_bss_2DC;
    processes[5] = &lbl_105_bss_29C;
    processes[8] = &lbl_105_bss_2BC;
    processes[12] = &lbl_105_bss_2CC;
    processes[14] = &lbl_105_bss_2EC;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 15);
    m_moduleAccesser->getStatusModule().setUniqProc(0x74, &lbl_105_bss_27C);
    m_moduleAccesser->getStatusModule().setUniqProc(0x10E, &lbl_105_bss_30C);
    m_moduleAccesser->getStatusModule().setUniqProc(0xBE, &lbl_105_bss_31C);
}

// BYTES: the native function addresses the local static's guard, object and destructor registration separately
// instead of from one pooled BSS base. The setting is per function: unit-wide it breaks the pooled constructor and
// __sinit, and an in-class inline definition makes the static weak and moves it to the end of .bss.
#pragma pool_data off
ftPopoTransactor* ftPopoTransactor::getInstance() {
    static ftPopoTransactor instance;
    return &instance;
}
#pragma pool_data reset

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Popo, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftPopoArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftPopoArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftPopoArticleKind : wnArticleKindInfo {
    ftPopoArticleKind() {
        kind = 0;
        subKind = Fighter_Popo;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftPopoArticleBuildInfo : wnArticleBuildInfo {
    ftPopoArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_1491C4(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1491DC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_14925C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_149274(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_14928C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <class W> struct ftPopoArticleTraits;
#define FT_POPO_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftPopoArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftPopoArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_Popo, &tag.m_resourceGroup)); \
        } \
    };

FT_POPO_TRAITS(wnPopoBlizzard, wnPopoBlizzardData, 0, fn_27_1491C4)
FT_POPO_TRAITS(wnPopoIceShot, wnPopoIceShotData, 1, fn_27_1491DC)
FT_POPO_TRAITS(wnPopoIceBerg, wnPopoIceBergData, 2, fn_27_14925C)
FT_POPO_TRAITS(wnPopoRubber, wnPopoRubberData, 3, fn_27_149274)
FT_POPO_TRAITS(wnPopoIceBergWind, wnPopoIceBergWindData, 4, fn_27_14928C)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftPopoArticleTraits<W>::ArticleId, ftPopoArticleBuildInfo(ftPopoArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftPopoArticleTraits<W>::getData()) { }

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnPopoBlizzard>::~wnArticleHolder();
template wnArticleHolder<wnPopoIceShot>::~wnArticleHolder();
template wnArticleHolder<wnPopoIceBerg>::~wnArticleHolder();
template wnArticleHolder<wnPopoRubber>::~wnArticleHolder();
template wnArticleHolder<wnPopoIceBergWind>::~wnArticleHolder();
template wnArticleHolder<wnPopoBlizzard>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnPopoIceShot>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftPopoArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftPopoArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftPopoArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftPopoArticleMediator::~ftPopoArticleMediator() { }
#pragma dont_inline off

s32 ftPopoArticleMediator::getMediateNum() { return 5; }
void ftPopoArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Blizzard, 1 IceShot, 2 IceBerg, 3 Rubber, 4 IceBergWind (the rest are unused).

#define FT_POPO_POOL_BLIZZARD_BASE static_cast<wnArticlePool<wnPopoBlizzard, 3, ftPopoIceShotPool>&>(m_pools)
#define FT_POPO_POOL_BLIZZARD FT_POPO_POOL_BLIZZARD_BASE.getSub()
#define FT_POPO_POOL_ICESHOT_BASE static_cast<wnArticlePool<wnPopoIceShot, 5, ftPopoIceBergPool>&>(m_pools)
#define FT_POPO_POOL_ICESHOT FT_POPO_POOL_ICESHOT_BASE.getSub()
#define FT_POPO_POOL_ICEBERG_BASE static_cast<wnArticlePool<wnPopoIceBerg, 1, ftPopoRubberPool>&>(m_pools)
#define FT_POPO_POOL_ICEBERG FT_POPO_POOL_ICEBERG_BASE.getSub()
#define FT_POPO_POOL_RUBBER_BASE static_cast<wnArticlePool<wnPopoRubber, 1, ftPopoIceBergWindPool>&>(m_pools)
#define FT_POPO_POOL_RUBBER FT_POPO_POOL_RUBBER_BASE.getSub()
#define FT_POPO_POOL_ICEBERGWIND_BASE static_cast<wnArticlePool<wnPopoIceBergWind, 1, soInstancePoolRoot>&>(m_pools)
#define FT_POPO_POOL_ICEBERGWIND FT_POPO_POOL_ICEBERGWIND_BASE.getSub()

#define FT_POPO_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) WN_ARTICLE_UNUSED_SLOT(7, value) \
    WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) WN_ARTICLE_UNUSED_SLOT(10, value) \
    WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) WN_ARTICLE_UNUSED_SLOT(13, value) \
    WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) WN_ARTICLE_UNUSED_SLOT(16, value)

void ftPopoArticleMediator::deactivate() {
    for (s32 i = 0; i < 3; ++i) {
        wnPopoBlizzard* weapon = FT_POPO_POOL_BLIZZARD.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 5; ++i) {
        wnPopoIceShot* weapon = FT_POPO_POOL_ICESHOT.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnPopoIceBerg* weapon = FT_POPO_POOL_ICEBERG.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnPopoRubber* weapon = FT_POPO_POOL_RUBBER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnPopoIceBergWind* weapon = FT_POPO_POOL_ICEBERGWIND.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftPopoArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(3, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerateMaxNum(5, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    FT_POPO_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftPopoActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

s32 ftPopoArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnPopoBlizzard, 3, ftPopoIceShotPool>, ftPopoActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotActiveNum<wnArticlePool<wnPopoIceShot, 5, ftPopoIceBergPool>, ftPopoActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotActiveNum<wnArticlePool<wnPopoIceBerg, 1, ftPopoRubberPool>, ftPopoActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotActiveNum<wnArticlePool<wnPopoRubber, 1, ftPopoIceBergWindPool>, ftPopoActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotActiveNum<wnArticlePool<wnPopoIceBergWind, 1, soInstancePoolRoot>, ftPopoActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    FT_POPO_UNUSED_SLOTS(0)
    default: return 0;
    }
}

bool ftPopoArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnPopoBlizzard, 3, ftPopoIceShotPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotIsGeneratable<wnArticlePool<wnPopoIceShot, 5, ftPopoIceBergPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotIsGeneratable<wnArticlePool<wnPopoIceBerg, 1, ftPopoRubberPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotIsGeneratable<wnArticlePool<wnPopoRubber, 1, ftPopoIceBergWindPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotIsGeneratable<wnArticlePool<wnPopoIceBergWind, 1, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
    FT_POPO_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftPopoArticleActivator;
template <> struct ftPopoArticleActivator<wnPopoBlizzard> {
    static bool activate(wnPopoBlizzard* weapon, soModuleAccesser* acc) { return ftPopo::activeArticle(weapon, acc); }
};
template <> struct ftPopoArticleActivator<wnPopoIceShot> {
    static bool activate(wnPopoIceShot* weapon, soModuleAccesser* acc) {
        u32 etcResId = acc->getResourceModule().getResourceIdAccesser()->getEtcResId();
        return ftPopoTransactor::getInstance()->activeArticle1(weapon, acc, etcResId);
    }
};
template <> struct ftPopoArticleActivator<wnPopoIceBerg> {
    static bool activate(wnPopoIceBerg* weapon, soModuleAccesser* acc) { return ftPopo::activeArticle1(weapon, acc); }
};
template <> struct ftPopoArticleActivator<wnPopoRubber> {
    static bool activate(wnPopoRubber* weapon, soModuleAccesser* acc) { return ftPopo::activeArticle2(weapon, acc); }
};
template <> struct ftPopoArticleActivator<wnPopoIceBergWind> {
    static bool activate(wnPopoIceBergWind* weapon, soModuleAccesser* acc) { return wnPopoIceBerg::activeArticle(weapon, acc); }
};

soArticle* ftPopoArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<ftPopoArticleActivator<wnPopoBlizzard> >(static_cast<wnArticlePool<wnPopoBlizzard, 3, ftPopoIceShotPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerate<ftPopoArticleActivator<wnPopoIceShot> >(static_cast<wnArticlePool<wnPopoIceShot, 5, ftPopoIceBergPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerate<ftPopoArticleActivator<wnPopoIceBerg> >(static_cast<wnArticlePool<wnPopoIceBerg, 1, ftPopoRubberPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerate<ftPopoArticleActivator<wnPopoRubber> >(static_cast<wnArticlePool<wnPopoRubber, 1, ftPopoIceBergWindPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotGenerate<ftPopoArticleActivator<wnPopoIceBergWind> >(static_cast<wnArticlePool<wnPopoIceBergWind, 1, soInstancePoolRoot>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    FT_POPO_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

template <int S, int L>
inline bool ftPopoShootSlot1(soArticle* article, const soIntToType<S>&, const soIntToType<L>&) {
    dynamic_cast<wnPopoIceShot&>(*article).shoot();
    return false;
}

bool ftPopoArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnPopoBlizzard>(article, WN_ARTICLE_SLOT_TAGS);
    case 1: return ftPopoShootSlot1(article, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotShoot<wnPopoIceBerg>(article, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotShoot<wnPopoRubber>(article, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotShoot<wnPopoIceBergWind>(article, WN_ARTICLE_SLOT_TAGS);
    FT_POPO_UNUSED_SLOTS(true)
    default: return false;
    }
}


// FIXME: Test code present only to emit the shared builder functions; delete once ftPopo is done
__declspec(weak) void testBuilder() {
    soInsideEventManageModuleBuilder<ftPopoInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftPopoInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

__declspec(weak) u8 fn_105_8130(u8* p) { return *(u8*)(p + 0x4); }
__declspec(weak) void fn_105_A0DC() {}
__declspec(weak) void fn_105_AAFC() {}
__declspec(weak) void fn_105_AB14() {}
__declspec(weak) void fn_105_AB2C() {}
__declspec(weak) void fn_105_AB44() {}
void fn_105_AF48() {}
u8 fn_105_AF4C(u8* p) { return *(u8*)(p + 0x44); }
void fn_105_BF14() {}
__declspec(weak) int fn_105_D540() { return 0x0; }
void fn_105_D76C() {}
void fn_105_D8E4() {}
void fn_105_D924() {}
void fn_105_D928() {}
void fn_105_D9E0() {}
void fn_105_D9E4() {}
void fn_105_D9E8() {}
void fn_105_D9EC() {}
void fn_105_D9F0() {}
void fn_105_D9F4() {}
void fn_105_D9F8() {}
void fn_105_D9FC() {}
void fn_105_DA00() {}
void fn_105_DA04() {}
void fn_105_DA08() {}
void fn_105_DA0C() {}
void fn_105_DA10() {}
void fn_105_DA14() {}
void fn_105_DA18() {}
__declspec(weak) int fn_105_DA1C() { return 0x0; }
void fn_105_DA24() {}
void fn_105_DA28() {}
void fn_105_DA2C() {}
void fn_105_DA30() {}
void fn_105_DA5C() {}
void fn_105_DA60() {}
void fn_105_DA64() {}
void fn_105_DA68() {}
void fn_105_DA6C() {}
int fn_105_DA70(u8* p) { return *(int*)(p + 0x110); }
__declspec(weak) int fn_105_DA88() { return 0x0; }
void fn_105_DA90() {}
void fn_105_DA94() {}
void fn_105_DA98() {}
int fn_105_DA9C() { return 0x0; }
int fn_105_DAA4() { return 0x0; }
void fn_105_DAAC() {}
void fn_105_DAB0() {}
void fn_105_DAB4() {}
void fn_105_DAB8() {}
void fn_105_DABC() {}
void fn_105_DAC0() {}
void fn_105_DAC4() {}
void fn_105_DAC8() {}
void fn_105_DACC() {}
void fn_105_DAD0() {}
int fn_105_DAD4() { return -0x1; }
int fn_105_DAEC(u8* p) { return *(int*)(p + 0xc); }
int fn_105_DAF4() { return 0x0; }
void fn_105_DAFC() {}
void fn_105_DB00(u8* p, int v) { *(int*)(p + 0xc) = v; }
int fn_105_DC18(u8* p) { return *(int*)(p + 0x20); }
int fn_105_DCF4(u8* p) { return *(int*)(p + 0x18); }
int fn_105_DD8C(u8* p) { return *(int*)(p + 0x10); }
int fn_105_DE4C() { return 0x0; }
__declspec(weak) int fn_105_F458() { return 0x5; }
void fn_105_1038C() {}
void fn_105_10D70() {}
int fn_105_10DAC() { return 0x8; }
__declspec(weak) int fn_105_11194() { return 0x0; }
__declspec(weak) int fn_105_11564() { return 0x0; }
__declspec(weak) int fn_105_11930() { return 0x0; }
__declspec(weak) int fn_105_11CF4() { return 0x0; }
__declspec(weak) int fn_105_120D0() { return 0x0; }
__declspec(weak) int fn_105_124AC() { return 0x0; }
__declspec(weak) int fn_105_12888() { return 0x0; }
__declspec(weak) int fn_105_12C64() { return 0x0; }

}

// "Delete when flag > 0" destructors (identical bodies; one per map class name).
extern "C" {
#pragma dont_inline on
void* fn_105_1E2C(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // ftVirtualNodeMatrixPool____dt
void* fn_105_3674(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soParamAccesser____dt
__declspec(weak) void* fn_105_3D5C(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soInstanceManagerFullProperty_P15soKineticEnergy_____dt
void* fn_105_41E8(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soInstanceManagerFullProperty_20soAnimCmdControlUnit_____dt
__declspec(weak) void* fn_105_45E4(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soTeam____dt
__declspec(weak) void* fn_105_48D8(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soResourceIdAccesser____dt
__declspec(weak) void* fn_105_4918(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soParamCustomizeModuleBuilder dt
void* fn_105_628C(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soArticleMediator____dt
void* fn_105_62CC(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soArticleOperator____dt
void* fn_105_630C(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soArticleGenerator____dt
#pragma dont_inline reset
}

// Flag-free register-offset helpers (leaf getters returning sub-object addresses by index).
extern "C" {
__declspec(weak) void fn_105_F460(u8* p, u8 v) { *(u8*)(p + 0x17A10) = v; } // soArticleMediatorImpl setAutoRecycle
__declspec(weak) void* fn_105_F4E8(u8* p, s32 index) { if (index == 0) { return p + 0xc; } return 0; } // soInstancePoolSub getInstanceAt
__declspec(weak) void* fn_105_F500(u8* p, s32 index) { if (index == 0) { return p + 0xc; } return 0; } // soInstancePoolSub getInstanceAt
__declspec(weak) void* fn_105_F518(u8* p, s32 index) { if (index == 0) { return p + 0xc; } return 0; } // soInstancePoolSub getInstanceAt
__declspec(weak) void* fn_105_F58C(u8* p, s32 index) {
    if (index == 2) { return p + 0x3eb4; }
    if (index == 1) { return p + 0x1f64; }
    if (index == 0) { return p + 0x14; }
    return 0;
} // soInstancePoolSub_74...wnPopoBlizzard getInstanceAt
__declspec(weak) void* fn_105_F530(u8* p, s32 index) {
    if (index == 4) { return p + 0x8f6c; }
    if (index == 3) { return p + 0x6b98; }
    if (index == 2) { return p + 0x47c4; }
    if (index == 1) { return p + 0x23f0; }
    if (index == 0) { return p + 0x1c; }
    return 0;
} // soInstancePoolSub_73...wnPopoIceShot getInstanceAt
}

// Fighter-base this-adjusting thunks: subtract the sub-object offset, then tail call the Fighter method.
extern "C" {
void* fn_105_12E68(void* p, s32 flags) { return fn_105_628C((u8*)p - 0x4, flags); } // Fighter-base dtor thunk for soArticleMediator
__declspec(weak) void fn_105_12E88(u8* p, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4) { ((Fighter*)(p - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObject, unk4); } // Fighter::notifyEventLink
__declspec(weak) void fn_105_12E90(u8* p, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); } // Fighter::notifyEventChangeStatus
void fn_105_12E98(u8* p, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0x70))->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser); } // Fighter::notifyEventChangeSituation
void fn_105_12EA0(u8* p, BaseItem* item, u32 nodeIndex, int* unk) { ((Fighter*)(p - 0xd0))->Fighter::notifyUseItem(item, nodeIndex, unk); } // Fighter::notifyUseItem
#pragma dont_inline on
__declspec(weak) bool fn_105_12EB8(u8* p, int unk1) { return ((Fighter*)(p - 0x48))->Fighter::isObserv(unk1); } // Fighter::isObserv (header inline body must not be folded in)
#pragma dont_inline reset
__declspec(weak) void fn_105_12ED8(u8* p, u32 flags) { ((Fighter*)(p - 0x7c))->Fighter::notifyEventCollisionAttackCheck(flags); } // Fighter::notifyEventCollisionAttackCheck
__declspec(weak) void fn_105_12EE0(u8* p, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0x7c))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); } // Fighter::notifyEventCollisionAttack
__declspec(weak) void fn_105_12EE8(u8* p, int index, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0x88))->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser); } // Fighter::notifyEventChangeCollisionHit
__declspec(weak) void fn_105_12EF0(u8* p) { ((Fighter*)(p - 0x94))->Fighter::notifyEventCollisionShieldCheck(); } // Fighter::notifyEventCollisionShieldCheck
__declspec(weak) void fn_105_12EF8(u8* p, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); } // Fighter::notifyEventCollisionShieldSearch
__declspec(weak) void fn_105_12F00(u8* p, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); } // Fighter::notifyEventCollisionShield
__declspec(weak) void fn_105_12F08(u8* p) { ((Fighter*)(p - 0xa0))->Fighter::notifyEventCollisionReflectorCheck(); } // Fighter::notifyEventCollisionReflectorCheck
__declspec(weak) void fn_105_12F10(u8* p, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0xa0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); } // Fighter::notifyEventCollisionReflectorSearch
__declspec(weak) void fn_105_12F18(u8* p, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0xa0))->Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); } // Fighter::notifyEventCollisionReflector
__declspec(weak) void fn_105_12F20(u8* p) { ((Fighter*)(p - 0xac))->Fighter::notifyEventCollisionAbsorberCheck(); } // Fighter::notifyEventCollisionAbsorberCheck
__declspec(weak) void fn_105_12F28(u8* p, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0xac))->Fighter::notifyEventCollisionAbsorber(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); } // Fighter::notifyEventCollisionAbsorber
__declspec(weak) void fn_105_12F30(u8* p) { ((Fighter*)(p - 0xb8))->Fighter::notifyEventCollisionSearchCheck(); } // Fighter::notifyEventCollisionSearchCheck
__declspec(weak) void fn_105_12F38(u8* p, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0xb8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); } // Fighter::notifyEventCollisionSearch
__declspec(weak) void fn_105_12F40(u8* p, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) { ((Fighter*)(p - 0xc4))->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3); } // Fighter::notifyEventCaptureStatus
__declspec(weak) void fn_105_12F48(u8* p, BaseItem* item, int index, bool unk2, u8 unk3) { ((Fighter*)(p - 0xd0))->Fighter::notifyVisibilityItem(item, index, unk2, unk3); } // Fighter::notifyVisibilityItem
__declspec(weak) void fn_105_12F50(u8* p, BaseItem* item, u32 index, bool unk2) { ((Fighter*)(p - 0xd0))->Fighter::notifyEjectAttachItem(item, index, unk2); } // Fighter::notifyEjectAttachItem
__declspec(weak) void fn_105_12F58(u8* p, BaseItem* item, u32 index, bool unk2) { ((Fighter*)(p - 0xd0))->Fighter::notifyEjectItem(item, index, unk2); } // Fighter::notifyEjectItem
__declspec(weak) void fn_105_12F60(u8* p, BaseItem* item) { ((Fighter*)(p - 0xd0))->Fighter::notifyShootBulletItem(item); } // Fighter::notifyShootBulletItem
__declspec(weak) void fn_105_12F68(u8* p, BaseItem* item) { ((Fighter*)(p - 0xd0))->Fighter::notifyDropItem(item); } // Fighter::notifyDropItem
__declspec(weak) void fn_105_12F70(u8* p, BaseItem* item, u32 nodeIndex, int* unk) { ((Fighter*)(p - 0xd0))->Fighter::notifyThrowItem(item, nodeIndex, unk); } // Fighter::notifyThrowItem
__declspec(weak) void fn_105_12F80(u8* p, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk1, bool unk2) { ((Fighter*)(p - 0xd0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk1, unk2); } // Fighter::notifyAttachItem
__declspec(weak) void fn_105_12F88(u8* p, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk) { ((Fighter*)(p - 0xd0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk); } // Fighter::notifyHaveItem
__declspec(weak) void fn_105_12F90(u8* p, BaseItem* item, bool* unk) { ((Fighter*)(p - 0xd0))->Fighter::notifyHaveItemPreCheck(item, unk); } // Fighter::notifyHaveItemPreCheck
__declspec(weak) void fn_105_12FA0(u8* p, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0xe8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); } // Fighter::notifyEventAddDamage
__declspec(weak) void fn_105_12FA8(u8* p, soDamage* damage, bool unk, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0xe8))->Fighter::notifyEventOnDamage(damage, unk, moduleAccesser); } // Fighter::notifyEventOnDamage
__declspec(weak) void fn_105_12FB0(u8* p, float power, int unk) { ((Fighter*)(p - 0xf4))->Fighter::notifyEventPikminFinalAttack(power, unk); } // Fighter::notifyEventPikminFinalAttack
__declspec(weak) void fn_105_12FB8(u8* p) { ((Fighter*)(p - 0xf4))->Fighter::notifyEventChangeAdvUnit(); } // Fighter::notifyEventChangeAdvUnit
__declspec(weak) void fn_105_12FC0(u8* p) { ((Fighter*)(p - 0xf4))->Fighter::notifyEventBeat(); } // Fighter::notifyEventBeat
__declspec(weak) void fn_105_12FC8(u8* p, float damage) { ((Fighter*)(p - 0xf4))->Fighter::notifyEventSetDamage(damage); } // Fighter::notifyEventSetDamage
__declspec(weak) void fn_105_12FD0(u8* p, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0x100))->Fighter::notifyEventTurn(posX, posY, moduleAccesser); } // Fighter::notifyEventTurn
}

extern "C" {
__declspec(weak) bool fn_105_12EB0(u8* p, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, long unk3) { return ((Fighter*)(p - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); } // Fighter::notifyEventAnimCmd
}

// Sub-object destructor wrappers: destroy one base sub-object (flag -1 or 0), then delete when flags > 0.
extern "C" {
void* fn_27_63EBC(void* p, s16 flags);
void* fn_27_5C674(void* p, s16 flags);
void* fn_27_5AE44(void* p, s16 flags);
void* fn_27_6880C(void* p, s16 flags);
void* fn_27_D3B50(void* p, s16 flags);
void __dt__15soKineticEnergyFv(void* p, s16 flags);
void __dt__16soStopModuleImplFv(void* p, s16 flags);
void __dt__17soSoundModuleImplFv(void* p, s16 flags);
void __dt__19soGeneralWorkSimpleFv(void* p, s16 flags);
void* fn_105_327C(void* p, s16 flags);
void* fn_105_3348(void* p, s16 flags);
void* fn_105_3490(void* p, s16 flags);
void* fn_105_4CF4(void* p, s16 flags);

#pragma dont_inline on
__declspec(weak) void* fn_105_3220(void* p, s32 flags) { if (p != 0) { fn_105_327C(p, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soSelectInstanceHolder dt
__declspec(weak) void* fn_105_32EC(void* p, s32 flags) { if (p != 0) { fn_105_3348(p, 0); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soLineHierarchy dt
__declspec(weak) void* fn_105_3430(void* p, s16 flags) { if (p != 0) { fn_105_3490((u8*)p + 0x4, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // wnInstanceHolder dt
void* fn_105_36B4(void* p, s32 flags) { if (p != 0) { __dt__15soKineticEnergyFv(p, 0); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soKineticEnergyRotNormal dt
void* fn_105_3A24(void* p, s32 flags) { if (p != 0) { static_cast<soGlowModuleSimple*>(p)->soGlowModuleSimple::~soGlowModuleSimple(); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soGlowModuleBuilder dt
void* fn_105_3A80(void* p, s32 flags) { if (p != 0) { fn_27_63EBC(p, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soReflectModuleBuilder dt
void* fn_105_3ADC(void* p, s32 flags) { if (p != 0) { fn_27_5C674(p, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soSlowModuleBuilder dt
void* fn_105_3BC4(void* p, s32 flags) { if (p != 0) { __dt__19soGeneralWorkSimpleFv((u8*)p + 0x38, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soGeneralWorkBuilder dt
void* fn_105_4100(void* p, s32 flags) { if (p != 0) { __dt__19soGeneralWorkSimpleFv((u8*)p + 0x14, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soGeneralWorkBuilder dt
void* fn_105_42F0(void* p, s32 flags) { if (p != 0) { fn_27_5AE44(p, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soVisibilityModuleBuilder dt
void* fn_105_4418(void* p, s32 flags) { if (p != 0) { fn_27_6880C((u8*)p + 0x4, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soShakeModuleBuilder dt
void* fn_105_43B8(void* p, s32 flags) { if (p != 0) { __dt__17soSoundModuleImplFv((u8*)p + 0xc, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soSoundModuleBuilder dt
void* fn_105_4478(void* p, s32 flags) { if (p != 0) { __dt__16soStopModuleImplFv(p, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soStopModuleBuilder dt
__declspec(weak) void* fn_105_4C94(void* p, s16 flags) { if (p != 0) { fn_105_4CF4((u8*)p + 0x4, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // wnInstanceHolder dt
void* fn_105_51C4(void* p, s32 flags) { if (p != 0) { fn_27_D3B50(p, 0); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soDamageEventObserver dt
__declspec(weak) void* fn_105_78F8(void* p, s32 flags) { if (p != 0) { fn_105_45E4(p, 0); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // ftTeam dt
#pragma dont_inline reset
}

// ---- the fighter hooks -----------------------------------------------------------------------------------------------
// The link events the Ice Climbers send each other (HYPOTHESIS: the field meanings follow the uses below).
struct ftPopoLinkEventArgs : soLinkEventArgs {
    int m_value;
    bool m_flag;
    ftPopoLinkEventArgs(int eventKind) : soLinkEventArgs(eventKind), m_value(0), m_flag(false) { }
    ftPopoLinkEventArgs(int eventKind, int value) : soLinkEventArgs(eventKind), m_value(value), m_flag(false) { }
};

// The kinetic transactor singleton of the pair; its slot 6 takes the situation change (the shared header names the slot after the
// robot's use of it).
struct ftPopoTransactorSlots {
    virtual ~ftPopoTransactorSlots();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser);
};

void ftPopo::setCursor(bool enable) {
    if (m_isPartner == false) {
        Fighter::setCursor(enable);
    }
}

void ftPopo::setNameCursor(bool enable) {
    if (m_isPartner == false) {
        Fighter::setNameCursor(enable);
    }
}

void ftPopo::setLoupe(bool enable) {
    if (m_isPartner == false) {
        Fighter::setLoupe(enable);
    }
}

void ftPopo::setLoupeDamage(bool enable) {
    if (m_isPartner == false) {
        Fighter::setLoupeDamage(enable);
    }
}

// The second fighter of the pair borrows the sub owner and sub input of the entry.
ftOwner* ftPopo::getOwner() {
    if (m_isPartner == true) {
        return g_ftManager->getSubOwner(m_entryId);
    }
    return g_ftManager->getOwner(m_entryId);
}

Input* ftPopo::getInput() {
    if (m_isPartner == true) {
        return static_cast<Input*>(g_ftManager->getSubInput(m_entryId));
    }
    return static_cast<Input*>(g_ftManager->getInput(m_entryId));
}

// The other fighter of the entry (none when the entry has just one).
Fighter* ftPopo::getPartner() {
    ftManager* manager = g_ftManager;
    if (static_cast<u8>(manager->getFighterCount(m_entryId)) > 1) {
        int index = 1;
        if (m_moduleAccesser->getWorkManageModule().isFlag(0x1200003e)) {
            index = 0;
        }
        return manager->getFighter(m_entryId, index);
    }
    return NULL;
}

void ftPopo::processUpdate() {
    Fighter::processUpdate();
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x117 || m_moduleAccesser->getStatusModule().getStatusKind() == 0x120) {
        soLinkModule& link = m_moduleAccesser->getLinkModule();
        soPostureModule& posture = m_moduleAccesser->getPostureModule();
        Vec3f position = link.getModelConstraintTargetNodePosition();
        posture.setPos(&position);
    }
}

bool ftPopo::notifyUseItem(BaseItem* item, u32 nodeIndex, int* unk) {
    if (m_isPartner == true && item->soGetSubKind() == 0x14) {
        return false;
    }
    return Fighter::notifyUseItem(item, nodeIndex, unk);
}

void ftPopo::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftPopoTransactorSlots*>(ftPopoTransactor::getInstance())->notifyEventChangeSituation(kind, prevKind, moduleAccesser);
    Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser);
}

// Link event without a payload.
struct ftPopoLinkKindEventArgs : soLinkEventArgs {
    ftPopoLinkKindEventArgs(int eventKind) : soLinkEventArgs(eventKind) { }
};

// HYPOTHESIS: entering a few statuses from the Ice Climbers' own statuses (0x111-0x11d) sets the initial speed of the move from a
// constant (work floats 0x11000000/0x11000001, flag 0x12000003) and tells the partner about it through link events 0x45a/0x45d.
void ftPopo::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    switch (statusKind) {
    case 0x19:
        switch (prevStatusKind) {
        case 0x112:
            m_moduleAccesser->getWorkManageModule().setFloat(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA1, 0), 0x11000000);
            m_moduleAccesser->getWorkManageModule().onFlag(0x12000003);
            break;
        case 0x113:
            m_moduleAccesser->getWorkManageModule().setFloat(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA9, 0), 0x11000000);
            m_moduleAccesser->getWorkManageModule().onFlag(0x12000003);
            break;
        case 0x11C:
            m_moduleAccesser->getWorkManageModule().setFloat(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFBD, 0), 0x11000000);
            m_moduleAccesser->getWorkManageModule().onFlag(0x12000003);
            m_moduleAccesser->getWorkManageModule().setFloat(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFBC, 0), 0x11000001);
            break;
        case 0x11B:
            m_moduleAccesser->getWorkManageModule().setFloat(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFD2, 0), 0x11000000);
            m_moduleAccesser->getWorkManageModule().onFlag(0x12000003);
            m_moduleAccesser->getWorkManageModule().setFloat(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFD1, 0), 0x11000001);
            break;
        }
        break;
    case 0x10:
        if (prevStatusKind == 0x11B && moduleAccesser->getLinkModule().isLink(6) == 1) {
            ftPopoLinkKindEventArgs event(0x45D);
            moduleAccesser->getLinkModule().sendEventNodes(6, event, 0);
        }
        break;
    case 0x73:
        if (prevStatusKind == 0x11D && m_moduleAccesser->getWorkManageModule().isFlag(0x1200003E) == true
            && moduleAccesser->getLinkModule().isLink(6) == 1) {
            ftPopoLinkKindEventArgs event(0x45A);
            moduleAccesser->getLinkModule().sendEventNodes(6, event, 0);
        }
        break;
    case 0x120:
        startFinalCommon();
        break;
    }
}

// Link event carrying one flag right after the base fields (HYPOTHESIS: kind 0x462 tells the partner whether to stay in the move).
struct ftPopoLinkFlagEventArgs : soLinkEventArgs {
    bool m_flag;
    ftPopoLinkFlagEventArgs(int eventKind, bool flag) : soLinkEventArgs(eventKind), m_flag(flag) { }
};

// HYPOTHESIS: while linked to the partner (and not in the partner's own state) a few motions are mirrored to it: 0x462 for the
// two movement motion groups, 0x456 with the partner's matching motion for the rest.
void ftPopo::notifyEventChangeMotion(int motionKind, int, void*, soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().isLink(6) == 1 && m_moduleAccesser->getWorkManageModule().isFlag(0x1200003e) == false) {
        int partnerMotion = -1;
        switch (motionKind) {
        case 0x1C2:
            partnerMotion = 0x1EA;
            break;
        case 0x1C3:
            partnerMotion = 0x1EB;
            break;
        case 0x1C5:
            partnerMotion = 0x1EC;
            break;
        case 0x1C7:
            partnerMotion = 0x1EE;
            break;
        case 0x1C9:
            partnerMotion = 0x1F0;
            break;
        case 0x1D3:
        case 0x1D8:
        case 0x1E4: {
            ftPopoLinkFlagEventArgs event(0x462, true);
            moduleAccesser->getLinkModule().sendEventNodes(6, event, 0);
            return;
        }
        case 0x1D5:
        case 0x1D9:
        case 0x1E8: {
            ftPopoLinkFlagEventArgs event(0x462, false);
            moduleAccesser->getLinkModule().sendEventNodes(6, event, 0);
            return;
        }
        }
        if (partnerMotion != -1) {
            ftPopoLinkEventArgs event(0x456, partnerMotion);
            moduleAccesser->getLinkModule().sendEventNodes(6, event, 0);
        }
    }
}

bool ftPopo::isModelSwapColor(int color) {
    switch (color) {
    case 1:
    case 4:
    case 5:
        return true;
    }
    return false;
}

// HYPOTHESIS: the statuses the partner may be in for a couple technique (the Ice Climbers' synchronized specials check it).
bool ftPopo::isEnableCoupleTech(int status) {
    switch (status) {
    case 0:
    case 1:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 10:
    case 11:
    case 12:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 30:
    case 31:
    case 32:
    case 33:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
    case 48:
    case 49:
    case 50:
    case 51:
    case 101:
    case 102:
    case 103:
    case 104:
    case 105:
    case 106:
    case 107:
    case 108:
    case 109:
    case 113:
    case 114:
    case 124:
    case 125:
    case 126:
    case 142:
    case 143:
    case 144:
    case 145:
    case 149:
    case 201:
    case 203:
    case 221:
    case 246:
    case 276:
    case 279:
    case 288:
        return true;
    }
    return false;
}

// Shared value variation getter (the Marth translation unit owns the C++ member; this is the REL copy).
extern int g_soValueVariation;
extern "C" {
int fn_105_E114() { return g_soValueVariation; } // soValueAccesser getValueVariation
}

// Forwarders to ftPopo methods that are not written yet (the target's `this` is the adjusted pointer).
extern "C" {
void* fn_105_F0C8(void* p, s32 flags);
__declspec(weak) void* fn_105_12E70(void* p, s32 flags) { return fn_105_F0C8((u8*)p - 0x4, flags); } // soArticleMediatorImpl shoot thunk
__declspec(weak) void* fn_105_12E78(void* p, s32 flags) { return fn_105_327C((u8*)p - 0x4, flags); } // soArticleMediatorImpl thunk
void fn_105_B138(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4);
void fn_105_12EC0(u8* p, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4) { fn_105_B138(p - 0x54, eventInfo, moduleAccesser, stageObject, unk4); } // ftPopo notifyEventLink thunk
}

// Chained sub-object destructor wrappers (same shape as above): destroy the sub-object, then delete when flags > 0.
extern "C" {
void* fn_105_5C34(void* p, s16 flags);
void* fn_105_56B8(void* p, s16 flags);
void* fn_105_6090(void* p, s16 flags);
__declspec(weak) void* fn_105_5BD4(void* p, s16 flags);
__declspec(weak) void* fn_105_5FD0(void* p, s16 flags);
__declspec(weak) void* fn_105_5658(void* p, s16 flags);
__declspec(weak) void* fn_105_6030(void* p, s16 flags);

#pragma dont_inline on
__declspec(weak) void* fn_105_4B4C(void* p, s32 flags) { if (p != 0) { fn_105_3430((u8*)p + 0x8, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soInstancePoolSub dt
__declspec(weak) void* fn_105_5520(void* p, s32 flags) { if (p != 0) { fn_105_4C94((u8*)p + 0x8, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soInstancePoolSub dt
__declspec(weak) void* fn_105_55F8(void* p, s32 flags) { if (p != 0) { fn_105_5658((u8*)p + 0x8, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soInstancePoolSub dt
__declspec(weak) void* fn_105_5658(void* p, s16 flags) { if (p != 0) { fn_105_56B8((u8*)p + 0x4, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // wnInstanceHolder dt
__declspec(weak) void* fn_105_5B74(void* p, s32 flags) { if (p != 0) { fn_105_5BD4((u8*)p + 0x8, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soInstancePoolSub dt
__declspec(weak) void* fn_105_5BD4(void* p, s16 flags) { if (p != 0) { fn_105_5C34((u8*)p + 0x4, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // wnInstanceHolder dt
__declspec(weak) void* fn_105_5F70(void* p, s32 flags) { if (p != 0) { fn_105_5FD0((u8*)p + 0x4, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soInstancePoolSub dt
__declspec(weak) void* fn_105_5FD0(void* p, s16 flags) { if (p != 0) { fn_105_6030((u8*)p + 0x8, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soInstancePoolSub dt
__declspec(weak) void* fn_105_6030(void* p, s16 flags) { if (p != 0) { fn_105_6090((u8*)p + 0x4, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // wnInstanceHolder dt
#pragma dont_inline reset
}

// Virtual slot forwarder: the object's vtable slot at offset 0x58 (slot 20) is tail called with the same arguments.
extern "C" {
struct ftPopoVSlotsArticle {
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual int slot20();
};
__declspec(weak) int fn_105_F4D8(ftPopoVSlotsArticle* p) { return p->slot20(); } // soArticle checkActivate (virtual tail call)

}

void ftPopo::analyzeSeal(void* seal) {
    ftOwnerSeal* record = static_cast<ftOwnerSeal*>(seal);
    if (record->m_kind == 0x3c) {
        int count = static_cast<int>(record->m_value / 2.0f);
        m_moduleAccesser->getWorkManageModule().setInt(count, 0x10000041);
    }
}

int ftPopo::getHaveItemKind(StageObject* partner) {
    soItemManageModule* items = soExternalValueAccesser::getItemManageModule(partner);
    if (items->isHaveItem(0) == false) {
        return -1;
    }
    return items->getHaveItemKind(0);
}

void ftPopo::reqSummonsEffect() {
    Vec3f position = m_moduleAccesser->getPostureModule().getPos();
    m_moduleAccesser->getEffectModule().req(static_cast<EfID>(0x36), &position, &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
}

void ftPopo::toKnockOut() {
    Fighter::toKnockOut();
    if (!m_moduleAccesser->getWorkManageModule().isFlag(0x1200003e)
        && m_moduleAccesser->getLinkModule().isLink(6) == 1) {
        ftPopoLinkKindEventArgs event(0x461);
        m_moduleAccesser->getLinkModule().sendEventNodes(6, event, 0);
    }
}

void ftPopo::adjustParentGroundCollision(int, float* bounds) {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x117:
    case 0x11a: {
        float up, down, left, right;
        m_moduleAccesser->getGroundModule().calcRhombusModify(&up, &down, &left, &right, 0);
        if (left < bounds[2]) {
            bounds[2] = left;
        }
        if (right > bounds[3]) {
            bounds[3] = right;
        }
        break;
    }
    }
}

void ftPopo::onDeadEnd() {
    Fighter::onDeadEnd();
    Fighter* partner = getPartner();
    if (partner && !m_isPartner && soExternalValueAccesser::getStatusKind(partner) != 0x10b
        && getOwner()->getStockCount() == 0 && m_moduleAccesser->getLinkModule().isLink(6) == 1) {
        ftPopoLinkKindEventArgs event(0x45e);
        m_moduleAccesser->getLinkModule().sendEventNodes(6, event, 0);
    }
}

// HYPOTHESIS: the rubber starts with the task id of the fighter, the effect resource, the team and the task id of the link 6 parent.
bool ftPopo::activeArticle2(wnPopoRubber* weapon, soModuleAccesser* acc) {
    int founderTaskId;
    int parentTaskId = acc->getLinkModule().getParentTaskId(6);
    soResourceModule& resources = acc->getResourceModule();
    founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), acc->getTeamModule().getTeam()->getNo(), parentTaskId);
    return true;
}

// HYPOTHESIS: the blizzard starts at model node 0x27 moved by the parameter offsets (x follows the facing).
bool ftPopo::activeArticle(wnPopoBlizzard* weapon, soModuleAccesser* acc) {
    Vec3f position = acc->getModelModule().getNodeGlobalPosition(0x27, false);
    float lr = acc->getPostureModule().getLr() < 0.0f ? -1 : 1;
    float offsetX = soValueAccesser::getConstantFloat(acc, 0xfa4, 0);
    position.m_x += offsetX * lr;
    position.m_y += soValueAccesser::getConstantFloat(acc, 0xfa5, 0);
    position.m_z = 0.0f;
    soTeamModule& teamModule = acc->getTeamModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, teamModule.getTeam()->getNo(), &position, acc->getPostureModule().getLr());
    return false;
}

// HYPOTHESIS: whether the partner is within the AI follow distance (the first float of the AI system parameters).
bool ftPopo::isApproachPartner(StageObject* partner) {
    Vec3f partnerPos = soExternalValueAccesser::getPos(partner);
    Vec3f diff = m_moduleAccesser->getPostureModule().getPos() - partnerPos;
    float dist = *static_cast<float*>(aiMgr::getAiSystemPar());
    return diff.lengthSq() <= dist * dist;
}

bool ftPopo::checkTransitionStatus(u32 statusKind) {
    if (!Fighter::checkTransitionStatus(statusKind)) {
        return false;
    }
    switch (static_cast<int>(statusKind)) {
    case 0x26:
    case 0x2a:
        getPartner();
        if (m_isPartner == true) {
            Fighter* partner = getPartner();
            if (partner && getHaveItemKind(partner) == 0x21 && isApproachPartner(partner)
                && soExternalValueAccesser::getStatusKind(partner) == 0xa1) {
                return false;
            }
        }
        break;
    }
    return true;
}

void ftPopo::onStart(int startKind) {
    Fighter::onStart(startKind);
    switch (startKind) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 8:
        if (m_isPartner == true) {
            Fighter* partner = getPartner();
            Vec3f position = soExternalValueAccesser::getPos(partner);
            float facing = soExternalValueAccesser::getLr(partner);
            soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
            work.setInt(0, 0x10000040);
            work.setFloat(position.m_x, 0x11000013);
            work.setFloat(position.m_x, 0x11000014);
            work.setFloat(position.m_x, 0x11000015);
            work.setFloat(position.m_x, 0x11000016);
            work.setFloat(position.m_x, 0x11000017);
            work.setFloat(position.m_y, 0x11000018);
            work.setFloat(position.m_y, 0x11000019);
            work.setFloat(position.m_y, 0x1100001A);
            work.setFloat(position.m_y, 0x1100001B);
            work.setFloat(position.m_y, 0x1100001C);
            work.setFloat(facing, 0x1100001D);
            work.setFloat(facing, 0x1100001E);
            work.setFloat(facing, 0x1100001F);
            work.setFloat(facing, 0x11000020);
            work.setFloat(facing, 0x11000021);
        }
        break;
    }
    switch (startKind) {
    case 8: {
        Fighter* partner = getPartner();
        if (partner != NULL && m_isPartner == false && soExternalValueAccesser::getStatusKind(partner) != 0x10B
            && m_moduleAccesser->getLinkModule().isLink(6) == true) {
            ftPopoLinkKindEventArgs event(0x45E);
            m_moduleAccesser->getLinkModule().sendEventNodes(6, event, 0);
        }
        break;
    }
    case 4:
        if (m_isPartner == true) {
            Fighter* partner = getPartner();
            if (partner != NULL) {
                int value = soExternalValueAccesser::getWorkInt(partner, 0x20000000);
                m_moduleAccesser->getWorkManageModule().setInt(value, 0x20000000);
            }
        }
        break;
    }
}

#include <ft/fighter.h>
#include <so/so_module_accesser.h>
#include <so/so_array.h>
// TODO: clean up, give variables real names
namespace jj_A5A4 {

class JostleModule : public soNull, public soNullable {
public:
    virtual void slot14();
    virtual void slot18();
    virtual void slot1C();
    virtual void slot20();
    virtual void slot24();
    virtual void slot28();
    virtual void slot2C();
    virtual void slot30();
    virtual void slot34();
    virtual void slot38();
    virtual void slot3C();
    virtual void slot40();
    virtual void slot44();
    virtual void setData(void*);
    virtual void setEnabled(bool);
};

class ResourceIds : public soNullable {
public:
    virtual ~ResourceIds();
    virtual void setResIdPacked(u32);
    virtual void setAnmResId(u32);
    virtual void setMdlResId(u32);
    virtual void setTexResId(u32);
    virtual void setMdlResIndex(u32);
};

extern void* g_ftManager;
extern "C" {
unsigned char getFighterNo__9ftManagerCFii(void*, int, int);
int getFighterGmKind__9ftManagerCFi(void*, int);
void fn_105_A7F0(void*, int, int, void*, int);
unsigned char getFighterColor__7ftOwnerFv(ftOwner*);
unsigned int isModelSwapColor__6ftPopoFi(int);
void postCreate__7FighterFv(void*);
}

class IceClimber : public Fighter {
    char padding[0x1FE48 - sizeof(Fighter)];
public:
    char* data;
    unsigned char flag;
};

#define DATA (self->data)
#define MODULES (self->m_moduleAccesser->m_enumerationStart)

extern "C" void fn_105_A5A4(IceClimber* self) {
    if (getFighterNo__9ftManagerCFii(g_ftManager, self->m_entryId, *(int*)((char*)self + 0x28)) == 0) {
        self->flag = 0;
    } else {
        *(unsigned char*)((char*)self + 0x1FE4C) = 1;
    }
    unsigned int enabled = 0;
    if (getFighterNo__9ftManagerCFii(g_ftManager, self->m_entryId, *(int*)((char*)self + 0x28)) == 1 ||
        getFighterGmKind__9ftManagerCFi(g_ftManager, self->m_entryId) == 0x12) {
        fn_105_A7F0(self, 1, 0, *(void**)(DATA + 0x7C), 0x1F2);
        fn_105_A7F0(self, 2, 0, *(void**)(DATA + 0x80), 0x1F2);
        fn_105_A7F0(self, 4, 0, *(void**)(DATA + 0x88), 0x1F2);
        if (isModelSwapColor__6ftPopoFi(getFighterColor__7ftOwnerFv(self->getOwner())) == 0) {
            enabled = 1;
        }
        ((JostleModule*)MODULES->m_jostleModule)->setData(*(void**)(DATA + 0xC0));
        ((JostleModule*)MODULES->m_jostleModule)->setEnabled(true);
    } else {
        if (isModelSwapColor__6ftPopoFi(getFighterColor__7ftOwnerFv(self->getOwner())) == 1) {
            enabled = 1;
        }
    }
    if (enabled == 1) {
        fn_105_A7F0(self, 3, 0, *(void**)(DATA + 0x84), 0x1F2);
        MODULES->m_soundModule->setSoundIdData((soSoundModule::SoundIdData*)(DATA + 0xA4));
    }
    ((ResourceIds*)MODULES->m_resourceModule->getResourceIdAccesser())->setMdlResIndex(enabled);
    MODULES->m_groundModule->setIgnoreFriction(true);
    postCreate__7FighterFv(self);
}
}

// TODO: clean up, give variables real names
namespace jj_A9F4 {
extern "C" {
extern signed char lbl_105_bss_260;
extern char lbl_105_bss_264[];
extern char lbl_105_bss_240[];
void fn_27_196C84(void* p);
void* fn_27_82644(void* ptr, int size);
void* __register_global_object(void* object, void* destructor, void* chain);

void* fn_105_A9F4() {
    if (!lbl_105_bss_260) {
        fn_27_196C84(lbl_105_bss_264);
        __register_global_object(lbl_105_bss_264, (void*)fn_27_82644, lbl_105_bss_240);
        lbl_105_bss_260 = 1;
    }
    return lbl_105_bss_264;
}
}
}

// TODO: clean up, give variables real names
namespace jj_B138 {

#define V(n) virtual void f##n();
struct Event {
    s32 kind;
    u8 pad04;
    u8 flag05;
    u8 pad06[2];
    union { float x; s32 value; };
    union { float y; struct { u8 flagC; u8 padD[3]; }; };
    u8 pad10[0x10];
};
class SlotBase3 {
public:
     V(1) V(2) V(3)
};
class SlotBase4 : public SlotBase3 {
public:
    V(4)
};
class SlotBase13 : public SlotBase4 {
public:
    V(5) V(6) V(7) V(8) V(9) V(10) V(11) V(12)
    virtual float f13();
};
struct ModelBase8 { char pad00[8]; };
class ModelModule : public ModelBase8 {
public:
     V(1) V(2) V(3) V(4) V(5) V(6) V(7) V(8) V(9)
    V(10) V(11) V(12) V(13) V(14) V(15) V(16) V(17) V(18) V(19)
    V(20) V(21) V(22) V(23) V(24) V(25) V(26) V(27) V(28) V(29)
    V(30) V(31) V(32) V(33) V(34) V(35) V(36)
    virtual Vec3f f37(int, int);
};
struct U64Ret { s32 a; u32 b; };
class StatusModule : public SlotBase4 {
public:
    virtual int f5(int, int);
    V(6) V(7) V(8) V(9)
    virtual unsigned int f10(int);
    V(11) V(12) V(13) V(14) V(15) V(16) V(17) V(18) V(19) V(20)
    V(21) V(22) V(23) V(24) V(25) V(26) V(27) V(28) V(29) V(30)
    V(31) V(32) V(33) V(34) V(35) V(36) V(37) V(38) V(39) V(40)
    V(41) V(42) V(43) V(44) V(45) V(46) V(47) V(48) V(49) V(50)
    V(51) V(52)
    virtual unsigned int f53(int);
    virtual U64Ret f54(int);
    V(55) V(56) V(57) V(58) V(59) V(60) V(61) V(62) V(63) V(64)
    V(65) V(66) V(67) V(68) V(69) V(70) V(71) V(72) V(73) V(74)
    V(75) V(76) V(77) V(78) V(79) V(80) V(81) V(82) V(83) V(84)
    V(85) V(86) V(87)
    virtual void f88(int);
    virtual unsigned int f89();
    V(90) V(91) V(92) V(93) V(94) V(95) V(96) V(97) V(98) V(99)
    virtual void f100(int, int, int);
};
class MotionModule : public SlotBase3 {
public:
    virtual void f4(int, void*);
    V(5) V(6) V(7) V(8) V(9) V(10) V(11) V(12) V(13)
    V(14) V(15) V(16)
    virtual int f17();
};
class WorkModule : public SlotBase13 {
public:
    virtual void f14(float, unsigned long);
    V(15) V(16) V(17)
    virtual unsigned int f18(unsigned long);
    virtual void f19(unsigned long);
    virtual void f20(unsigned long);
};
class ActionModule : public SlotBase13 {
public:
    virtual float f14();
    V(15) V(16) V(17) V(18) V(19) V(20) V(21)
    virtual int f22();
    V(23) V(24) V(25) V(26) V(27) V(28) V(29) V(30)
    virtual void f31(void*);
};
class GroundModule : public SlotBase4 {
public:
    V(5)
    virtual void f6(void*);
    V(7) V(8) V(9)
    virtual float f10();
    V(11) V(12) V(13) V(14) V(15) V(16) V(17) V(18) V(19) V(20)
    V(21) V(22)
    virtual float f23();
};
class MiscModule : public SlotBase4 {
public:
    V(5) V(6)
    virtual void f7(int, u8);
};
class SpecialModule : public SlotBase4 {
public:
    V(5) V(6) V(7) V(8) V(9)
    virtual int f10(int, int, int);
    V(11) V(12) V(13) V(14) V(15) V(16) V(17) V(18)
    virtual unsigned int f19(int);
};
struct AccesserBase3c { char pad00[0x3c]; };
class AccesserModule : public AccesserBase3c {
public:
     V(1) V(2) V(3) V(4) V(5) V(6) V(7) V(8) V(9)
    V(10) V(11) V(12) V(13) V(14) V(15) V(16) V(17) V(18) V(19)
    V(20) V(21) V(22) V(23) V(24) V(25) V(26) V(27) V(28) V(29)
    V(30) V(31) V(32) V(33) V(34) V(35) V(36) V(37) V(38) V(39)
    V(40) V(41) V(42) V(43)
    V(44)
};
struct TransitionBase8 { char pad00[8]; };
class TransitionModule : public TransitionBase8 {
public:
     V(1) V(2) V(3) V(4) V(5) V(6) V(7) V(8) V(9)
    V(10) V(11) V(12) V(13) V(14) V(15) V(16) V(17) V(18) V(19)
    V(20) V(21) V(22) V(23) V(24) V(25) V(26) V(27) V(28) V(29)
    V(30) V(31) V(32) V(33) V(34) V(35) V(36) V(37) V(38) V(39)
    V(40) V(41) V(42) V(43) V(44) V(45) V(46) V(47) V(48) V(49)
    V(50) V(51) V(52) V(53) V(54) V(55) V(56) V(57) V(58) V(59)
    V(60) V(61) V(62) V(63) V(64) V(65) V(66) V(67) V(68) V(69)
    V(70) V(71) V(72) V(73) V(74) V(75) V(76) V(77) V(78) V(79)
    V(80) V(81) V(82) V(83) V(84) V(85) V(86) V(87) V(88) V(89)
    V(90) V(91) V(92) V(93) V(94) V(95) V(96) V(97) V(98) V(99)
    V(100) V(101) V(102) V(103) V(104) V(105) V(106) V(107)
    virtual void f108(void*, int);
    V(109) V(110) V(111)
    virtual float f112();
    virtual int f113(int);
};
struct ModuleSet {
    char pad00[4];
    ModelModule* model;
    ActionModule* action;
    GroundModule* ground;
    TransitionModule* motion;
    MiscModule* misc;
    char pad18[0x54 - 0x18];
    StatusModule* status;
    char pad58[0x64 - 0x58];
    WorkModule* work;
    char pad68[8];
    MotionModule* state;
    char pad74[0x84 - 0x74];
    SpecialModule* special;
};
struct Accesser {
    char pad00[8];
    AccesserModule* model;
    char pad0c[0xd8 - 0x0c];
    ModuleSet* modules;
};
struct IceFighter {
    char pad00[0xd8];
    ModuleSet* modules;
};
struct SelfBase { char pad00[0x28]; s32 taskId; char pad2c[0x10]; };
struct Self : SelfBase {
     V(1) V(2) V(3) V(4) V(5) V(6) V(7) V(8) V(9)
    V(10) V(11) V(12) V(13) V(14) V(15) V(16) V(17) V(18) V(19)
    V(20) V(21) V(22) V(23) V(24) V(25) V(26) V(27) V(28) V(29)
    V(30) V(31) V(32) V(33) V(34) V(35) V(36) V(37) V(38) V(39)
    virtual int f40();
    virtual int f41();
    virtual unsigned int f42();
    V(43) V(44) V(45) V(46) V(47) V(48) V(49) V(50) V(51) V(52)
    V(53) V(54) V(55) V(56) V(57) V(58) V(59) V(60) V(61) V(62)
    V(63) V(64) V(65) V(66) V(67) V(68) V(69) V(70) V(71) V(72)
    V(73) V(74) V(75) V(76) V(77) V(78) V(79) V(80) V(81) V(82)
    V(83) V(84) V(85) V(86) V(87) V(88) V(89) V(90) V(91) V(92)
    V(93) V(94) V(95) V(96) V(97) V(98) V(99) V(100) V(101) V(102)
    V(103) V(104) V(105) V(106) V(107) V(108) V(109) V(110) V(111)
    V(112) V(113) V(114) V(115) V(116) V(117) V(118) V(119) V(120)
    V(121) V(122) V(123) V(124) V(125) V(126) V(127) V(128) V(129)
    V(130) V(131) V(132) V(133) V(134) V(135) V(136) V(137) V(138)
    V(139) V(140) V(141) V(142) V(143) V(144) V(145) V(146) V(147)
    V(148) V(149) V(150) V(151) V(152)
    virtual int f153();
    V(154) V(155) V(156) V(157) V(158) V(159) V(160) V(161)
    V(162)
    char pad40[0x20];
    IceFighter* fighter;
    char pad64[0x1fe4c - 0x64];
    u8 partnerFlag;
};
struct IceBase3c { char pad00[0x28]; s32 taskId; char pad2c[0x10]; };
struct IceStageObject : IceBase3c {
     V(1) V(2) V(3) V(4) V(5) V(6) V(7) V(8) V(9)
    V(10) V(11) V(12) V(13) V(14) V(15) V(16) V(17) V(18) V(19)
    V(20) V(21) V(22) V(23) V(24) V(25) V(26) V(27) V(28) V(29)
    V(30) V(31) V(32) V(33) V(34) V(35) V(36) V(37) V(38) V(39)
    virtual int f40();
    virtual int f41();
    V(42)
};
#undef V
extern "C" void notifyEventLink__7FighterFP15soLinkEventArgsP16soModuleAccesserP11StageObjecti(Self*, Event*, Accesser*, IceStageObject*, s32);
extern "C" void standby__7FighterFi(Self*, u32);
extern "C" void endFinal__7FighterFbbb(Self*, bool, bool, int);
extern "C" void detachConstraint__38ftPopoStatusUniqProcessSpecialSPartnerFP16soModuleAccesser(IceFighter*);
extern "C" void __ct__19soMotionChangeParamFiffUcUcbUc(void*, int, float, float, unsigned char, unsigned char, int, unsigned char);
extern "C" void __ct__5Vec2fFff(float*, float, float);
extern "C" void reqSummonsEffect__6ftPopoFv();
extern "C" const float lbl_105_rodata_8;
extern "C" const float lbl_105_rodata_10;
extern ftManager* g_ftManager;

inline int getSelfEntry(int otherTask, const s32& selfTask, int& entry,
                        int* taskCopy = 0) {
    if (taskCopy)
        *taskCopy = otherTask;
    ftManager* manager = g_ftManager;
    int ownTask = selfTask;
    entry = manager->getEntryIdFromTaskId(otherTask, 0);
    return manager->getEntryIdFromTaskId(ownTask, 0);
}

enum WorkFlagId {
    WorkFlag10 = 0x22000010,
    WorkFlag11 = 0x22000011,
    WorkFlag12 = 0x22000012,
    WorkFlag14 = 0x22000014,
    WorkFlag15 = 0x22000015
};

struct BD50Data {
    int value;
    float first;
    float second;
    unsigned char flags[4];
};

extern "C" void fn_105_B138(Self* self, Event* event, Accesser* accesser,
                            IceStageObject* other, s32 value) {
    ftManager* manager;
    int selfTask;
    int id;
    int motion;
    switch (event->kind) {
    case 0x455: {
        U64Ret result;
        motion = event->value;
        if (accesser->modules->state->f17() == 0x10e) {
            if (accesser->modules->status->f89() == 1U) {
                Vec3f position = accesser->modules->model->f37(2, 0);
                accesser->modules->status->f88(1);
                accesser->model->f44();
                position.m_z = accesser->modules->motion->f112();
                accesser->modules->ground->f6(&position);
                accesser->modules->status->f100(6, 0xd, 1);
            }
            if (motion == 0) {
                if (accesser->modules->motion->f113(0) == 0) {
                    accesser->modules->misc->f7(2, 0);
                    motion = 0xe;
                }
            }
            if (self->f153() != 0) {
                if (accesser->modules->status->f10(6) == 1U) {
                    if (self->fighter->modules->status->f53(6) == 1U) {
                        result = self->fighter->modules->status->f54(6);
                        self->fighter->modules->motion->f108(&result, 0);
                    }
                }
            }
        }
        if (motion == 0x10b) {
            standby__7FighterFi(self, 1);
        } else {
            self->fighter->modules->state->f4(motion, self->fighter);
        }
        break;
    }
    case 0x456: {
        if (self->fighter->modules->work->f18(0x1200003e) == 0)
            return;
        float frame = lbl_105_rodata_10;
        if (event->flagC == 1)
            frame = self->fighter->modules->action->f13();
        BD50Data data;
        __ct__19soMotionChangeParamFiffUcUcbUc(&data, event->value, frame, lbl_105_rodata_8, 0, 0,
                    event->flagC, 0);
        self->fighter->modules->action->f31(&data);
        break;
    }
    case 0x457: {
        if (self->fighter->modules->work->f18(0x1200003e) == 0)
            return;
        if (self->fighter->modules->state->f17() == 0x117) {
            id = 0x1d6;
            if (event->value != 0)
                id = 0x1d7;
            float frame = self->fighter->modules->action->f13();
            BD50Data data;
            __ct__19soMotionChangeParamFiffUcUcbUc(&data, id, frame, lbl_105_rodata_8, 0, 0, 1, 0);
            self->fighter->modules->action->f31(&data);
        } else {
            self->fighter->modules->misc->f7(event->value, event->flagC);
        }
        break;
    }
    case 0x458: {
        if (self->fighter->modules->state->f17() == 0x117 ||
            self->fighter->modules->state->f17() == 0x113) {
            if (self->fighter->modules->work->f18(0x1200003e) == 1U)
                detachConstraint__38ftPopoStatusUniqProcessSpecialSPartnerFP16soModuleAccesser(self->fighter);
            if ((self->fighter->modules->action->f22() == 0x1d3 ||
                 self->fighter->modules->action->f22() == 0x1d6) &&
                self->fighter->modules->motion->f113(0) == 1U) {
                int zero = 0;
                self->fighter->modules->misc->f7(zero, zero);
                self->fighter->modules->state->f4(0, self->fighter);
            } else {
                self->fighter->modules->misc->f7(2, 0);
                self->fighter->modules->state->f4(0x10, self->fighter);
            }
        }
        break;
    }
    case 0x459: {
        int sign = self->fighter->modules->ground->f10() < lbl_105_rodata_10 ? -1 : 1;
        float vertical;
        float direction = (float)sign;
        vertical = soValueAccesser::getConstantFloat(
            reinterpret_cast<soModuleAccesser*>(accesser), 0xfcd, 0);
        float horizontal = soValueAccesser::getConstantFloat(
            reinterpret_cast<soModuleAccesser*>(accesser), 0xfcc, 0);
        horizontal *= direction;
        float velocity[2];
        __ct__5Vec2fFff(velocity, horizontal, vertical);
        float scale = self->fighter->modules->ground->f23();
        if (scale < lbl_105_rodata_8) {
            velocity[0] *= scale;
            velocity[1] *= scale;
        }
        if (self->fighter->modules->work->f18(0x1200003e) == 1U &&
            self->fighter->modules->state->f17() == 0x11d) {
            self->fighter->modules->work->f14(velocity[0] + event->x, 0x21000004);
            self->fighter->modules->work->f14(velocity[1] + event->y, 0x21000005);
        } else if (self->fighter->modules->work->f18(0x1200003e) == 0 &&
                   self->fighter->modules->state->f17() == 0x11f) {
            self->fighter->modules->work->f14(velocity[0] + event->x, 0x21000004);
            self->fighter->modules->work->f14(velocity[1] + event->y, 0x21000005);
        }
        break;
    }
    case 0x3b: {
        if (value != 6 || other->f40() != 0 || other->f41() != 0xf ||
            self->partnerFlag != 0)
            break;
        int entry;
        int selfEntry = getSelfEntry(other->taskId, self->taskId, entry);
        if (selfEntry == entry &&
            self->fighter->modules->status->f5(6, other->taskId) == 0)
            return;
        break;
    }
    case 0x45a:
        if (self->partnerFlag == 0 &&
            self->fighter->modules->state->f17() == 0x119)
            self->fighter->modules->work->f19(0x22000012);
        break;
    case 0x45b:
        if (self->partnerFlag == 1 &&
            self->fighter->modules->state->f17() == 0x120)
            self->fighter->modules->work->f19(0x22000010);
        break;
    case 0x45c:
        self->fighter->modules->work->f20(0x1200003f);
        endFinal__7FighterFbbb(self, true, true, 0);
        break;
    case 0x45d:
        if (self->fighter->modules->special->f19(3) == 1)
            self->fighter->modules->special->f10(3, 3, 0);
        break;
    case 0x45e:
        reqSummonsEffect__6ftPopoFv();
        standby__7FighterFi(self, 1);
        break;
    case 0x462: {
        if (self->fighter->modules->work->f18(0x1200003e) == 0)
            return;
        switch (self->fighter->modules->state->f17()) {
        case 0x117: {
            WorkFlagId flag = event->flag05 ? WorkFlag10 : WorkFlag11;
            self->fighter->modules->work->f19(flag);
            break;
        }
        case 0x120: {
            WorkFlagId flag = event->flag05 ? WorkFlag11 : WorkFlag12;
            self->fighter->modules->work->f19(flag);
            break;
        }
        case 0x11a: {
            WorkFlagId flag = event->flag05 ? WorkFlag14 : WorkFlag15;
            self->fighter->modules->work->f19(flag);
            break;
        }
        }
        break;
    }
    case 0x461:
        if (self->fighter->modules->work->f18(0x1200003e) == 1U &&
            self->f42() == 1U &&
            self->fighter->modules->work->f18(0x12000018) == 0)
            self->f162();
        break;
    }
    notifyEventLink__7FighterFP15soLinkEventArgsP16soModuleAccesserP11StageObjecti(self, event, accesser, other, value);
}
}

// TODO: clean up, give variables real names
namespace jj_BD50 {
struct BD50Data
{
    int value;
    float first;
    float second;
    unsigned char flags[4];
};

extern "C" void __ct__19soMotionChangeParamFiffUcUcbUc(
    BD50Data *data,
    int value,
    float first,
    float second,
    unsigned char flag0,
    unsigned char flag1,
    unsigned char flag2,
    unsigned char flag3)
{
    data->value = value;
    data->first = first;
    data->second = second;
    data->flags[0] = flag0;
    data->flags[1] = flag1;
    data->flags[2] = flag2;
    data->flags[3] = flag3;
}
}

// TODO: clean up, give variables real names
namespace jj_BD70 {
extern "C" void __ct__5Vec2fFff(float* object, float x, float y)
{
    object[0] = x;
    object[1] = y;
}
}

// TODO: clean up, give variables real names
namespace jj_C084 {
extern "C" {
extern const float lbl_105_rodata_10;
extern char __RTTI__24ftResourceIdAccesserImpl[], __RTTI__20soResourceIdAccesser[];
void* __dynamic_cast(void*, long, const void*, const void*, short);
int getFinalResId__24ftResourceIdAccesserImplFv(void*);
void __ct__5Vec3fFfff(void*, float, float, float);
void activate__13wnPopoIceBergFiiiPC5Vec3ffi(void*, int, int, int, void*, float, int);
}
class Object_vptr_0 {
public:
 virtual void slot0();
 virtual void slot1();
 virtual Object_vptr_0* slot2();
 virtual void slot3();
 virtual void slot4();
 virtual void slot5();
 virtual void slot6();
 virtual void slot7();
 virtual void slot8();
 virtual float slot9();
};
class Resource {
public:
 int pad[2];
 virtual void slot0();
 virtual void slot1();
 virtual void slot2();
 virtual void slot3();
 virtual void* slot4();
};
struct Stage {
 char pad[0x3c];
 virtual void s0();
 virtual void s1();
 virtual void s2();
 virtual void s3();
 virtual void s4();
 virtual void s5();
 virtual void s6();
 virtual void s7();
 virtual void s8();
 virtual void s9();
 virtual void s10();
 virtual void s11();
 virtual void s12();
 virtual void s13();
 virtual void s14();
 virtual void s15();
 virtual void s16();
 virtual void s17();
 virtual void s18();
 virtual void s19();
 virtual void s20();
 virtual void s21();
 virtual void s22();
 virtual void s23();
 virtual void s24();
 virtual void s25();
 virtual void s26();
 virtual void s27();
 virtual void s28();
 virtual void s29();
 virtual void s30();
 virtual void s31();
 virtual void s32();
 virtual void s33();
 virtual void s34();
 virtual void s35();
 virtual void s36();
 virtual void s37();
 virtual void s38();
 virtual void s39();
 virtual void s40();
 virtual void s41();
 virtual void s42();
 virtual void s43();
 virtual void s44();
 virtual void s45();
 virtual void s46();
 virtual void s47();
 virtual void s48();
 virtual void s49();
 virtual void s50();
 virtual void s51();
 virtual void s52();
 virtual void s53();
 virtual void s54();
 virtual void s55();
 virtual void s56();
 virtual void s57();
 virtual void s58();
 virtual void s59();
 virtual void get(void*, int);
};
extern "C" Stage* g_Stage;
extern "C" int activeArticle1__6ftPopoFP13wnPopoIceBergP16soModuleAccesser(void* self, char* input) {
 float vec[3];
 __ct__5Vec3fFfff(vec, lbl_105_rodata_10, lbl_105_rodata_10, lbl_105_rodata_10);
 if (!g_Stage) return 0;
 g_Stage->get(vec, 5);
 char* modules = *(char**)(input + 0xd8);
 Object_vptr_0* node = *(Object_vptr_0**)(modules + 0x18);
 Resource* resource = *(Resource**)modules;
 int task = *(int*)(*(char**)(input + 8) + 0x28);
 float value = (*(Object_vptr_0**)(modules + 0xc))->slot9();
 int owner = (int)node->slot2()->slot2();
 void* object = resource->slot4();
 int res = getFinalResId__24ftResourceIdAccesserImplFv(__dynamic_cast(object, 0, __RTTI__24ftResourceIdAccesserImpl, __RTTI__20soResourceIdAccesser, short(1)));
 activate__13wnPopoIceBergFiiiPC5Vec3ffi(self, task, res, owner, vec, value, 0x1f);
 return 1;
}
}

// TODO: clean up, give variables real names
namespace jj_109C8 {

namespace {
// Each special kinetic kind gets a one-byte default-initialized state.
struct KineticEnergyStatePolicy {
    unsigned char initialized;
    KineticEnergyStatePolicy() {
        __memclr(&initialized, sizeof(initialized));
    }
    void update(soKineticEnergy* energy, soModuleAccesser* accesser) {
        energy->updateEnergy(accesser);
    }
};
}

extern "C" void fn_105_109C8(void*,
    soArrayFixed<soInstanceUnitFullProperty<soKineticEnergy*>*>* energies,
    soModuleAccesser* accesser) {
    for (int index = 0; index < energies->size(); ++index) {
        soKineticEnergy* energy = energies->at(index)->m_element;
        if (energy->isEnable() == false)
            continue;
        unsigned int suspended = reinterpret_cast<u8*>(energy)[6];
        if (suspended == 1)
            continue;
        if (energy->isEnable() == true && suspended == 0) {
            int kineticType = accesser->getKineticModule().getKineticType();
            if (kineticType <= 99) {
                if (energy->isEnable() == true && reinterpret_cast<u8*>(energy)[6] == 0)
                    energy->updateEnergy(accesser);
            } else {
                if (energy->isEnable() && reinterpret_cast<u8*>(energy)[6] != 1) {
                    if ((unsigned int)(kineticType - 101) <= 3U) {
                        energy->updateEnergy(accesser);
                    } else {
                        switch (accesser->getKineticModule().getKineticType()) {
                        case 100: { KineticEnergyStatePolicy state; state.update(energy, accesser); break; }
                        case 101: { KineticEnergyStatePolicy state; state.update(energy, accesser); break; }
                        case 102: { KineticEnergyStatePolicy state; state.update(energy, accesser); break; }
                        case 103: { KineticEnergyStatePolicy state; state.update(energy, accesser); break; }
                        case 104: { KineticEnergyStatePolicy state; state.update(energy, accesser); break; }
                        case 105: { KineticEnergyStatePolicy state; state.update(energy, accesser); break; }
                        case 106: { KineticEnergyStatePolicy state; state.update(energy, accesser); break; }
                        case 107: { KineticEnergyStatePolicy state; state.update(energy, accesser); break; }
                        case 108: { KineticEnergyStatePolicy state; state.update(energy, accesser); break; }
                        case 109: { KineticEnergyStatePolicy state; state.update(energy, accesser); break; }
                        case 110: { KineticEnergyStatePolicy state; state.update(energy, accesser); break; }
                        case 111: { KineticEnergyStatePolicy state; state.update(energy, accesser); break; }
                        case 112: { KineticEnergyStatePolicy state; state.update(energy, accesser); break; }
                        case 113: { KineticEnergyStatePolicy state; state.update(energy, accesser); break; }
                        case 114: { KineticEnergyStatePolicy state; state.update(energy, accesser); break; }
                        }
                    }
                }
            }
        }
    }
}
}

void operator delete(void*);
// TODO: clean up, give variables real names
namespace jj_F934 {

extern "C" void* __dt__15soKineticEnergyFv(void*, short);

extern "C" void* __dt__25ftKineticEnergyControllerFv(void* object, short deleting) {
    if (object != 0) {
        if (object != 0) {
            short baseDeleting = 0;
            __dt__15soKineticEnergyFv(object, baseDeleting);
        }
        if (deleting > 0) {
            operator delete(object);
        }
    }
    return object;
}
}

