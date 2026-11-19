// TODO: clean up - raw offset accesses into fighter/module structs; add named fields to the fighter class (or a local struct) instead of *(T*)(p + 0x..)
#define FT_POOL_ROOT_NONPOLYMORPHIC
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_NOINLINE_SEARCH_BUILDER
#define FT_DEDEDE_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/dedede/ft_dedede.h>
#include <ft/dedede/ft_dedede_extend_param_accesser.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>
#include <ft/dedede/ft_dedede_transactor.h>
#include <ft/ft_kinetic_energy_controller.h>
#include <so/so_value_accesser.h>

#define FT_BC ftDededeBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt).
extern "C" {
extern soStatusUniqProcess g_ftDededeStatusUniqProcessSpecialN;
extern soStatusUniqProcess g_ftDededeStatusUniqProcessSpecialWaddledeeThrow;
extern soStatusUniqProcess lbl_117_bss_23C;
extern soStatusUniqProcess lbl_117_bss_28C;
extern soStatusUniqProcess lbl_117_bss_2DC;
extern soStatusUniqProcess lbl_27_bss_891C;
extern soStatusUniqProcess lbl_27_bss_890C;
extern soStatusUniqProcess g_ftDededeStatusUniqProcessSpecialNDrinkItem;
extern soStatusUniqProcess lbl_27_bss_88FC;
extern soStatusUniqProcess lbl_117_bss_24C;
}
extern soStatusUniqProcess g_ftDededeStatusUniqProcessSpecialWaddledeeThrowPickup;
extern soStatusUniqProcess g_ftDededeStatusUniqProcessSpecialSuperJumpLanding;
extern soStatusUniqProcess g_ftDededeStatusUniqProcessSpecialSuperJumpHitCeil;
extern soStatusUniqProcess g_ftDededeStatusUniqProcessSpecialSuperJumpTurn;

ftDededeExtendParamAccesser g_ftDededeExtendParamAccesser;

ftClassInfoImpl<Fighter_Dedede, ftDedede> g_ftClassInfoDedede;

void ftDedede::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x113:
    case 0x129:
        if (kind == 0) {
            soLinkEventArgs eventArgs(0x838);
            moduleAccesser->getLinkModule().sendEventNodes(1, eventArgs, 0);
        } else {
            soLinkEventArgs eventArgs(0x839);
            moduleAccesser->getLinkModule().sendEventNodes(1, eventArgs, 0);
        }
        break;
    case 0x115:
    case 0x12F:
    case 0x130:
    case 0x131:
    case 0x132:
    case 0x133:
    case 0x134:
    case 0x135:
    case 0x136:
    case 0x137:
        if (kind == 0) {
            soLinkEventArgs eventArgs(0x838);
            moduleAccesser->getLinkModule().sendEventNodes(1, eventArgs, 0);
        } else {
            soLinkEventArgs eventArgs(0x839);
            moduleAccesser->getLinkModule().sendEventNodes(1, eventArgs, 0);
        }
        break;
    }
    Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser);
}

ftDedede::ftDedede(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftDededeBuildConfig>(entryId,
                                         Fighter_Dedede,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Dedede)) {
    // Register the status processes in action order; entries without a process stay empty.
    soStatusUniqProcess* processes[40] = {0};
    processes[0] = &g_ftDededeStatusUniqProcessSpecialN;
    processes[1] = &g_ftDededeStatusUniqProcessSpecialWaddledeeThrow;
    processes[2] = &lbl_117_bss_23C;
    processes[3] = &lbl_117_bss_28C;
    processes[4] = &lbl_117_bss_2DC;
    processes[5] = &g_ftDededeStatusUniqProcessSpecialN;
    processes[7] = &lbl_27_bss_891C;
    processes[8] = &lbl_27_bss_891C;
    processes[9] = &lbl_27_bss_890C;
    processes[10] = &lbl_27_bss_890C;
    processes[11] = &lbl_27_bss_890C;
    processes[12] = &lbl_27_bss_890C;
    processes[13] = &lbl_27_bss_890C;
    processes[14] = &lbl_27_bss_890C;
    processes[15] = &lbl_27_bss_890C;
    processes[16] = &lbl_27_bss_890C;
    processes[17] = &lbl_27_bss_890C;
    processes[18] = &lbl_27_bss_890C;
    processes[19] = &lbl_27_bss_890C;
    processes[20] = &lbl_27_bss_890C;
    processes[21] = &g_ftDededeStatusUniqProcessSpecialNDrinkItem;
    processes[22] = &lbl_27_bss_88FC;
    processes[23] = &g_ftDededeStatusUniqProcessSpecialWaddledeeThrowPickup;
    processes[24] = &lbl_117_bss_24C;
    processes[25] = &g_ftDededeStatusUniqProcessSpecialSuperJumpLanding;
    processes[26] = &g_ftDededeStatusUniqProcessSpecialSuperJumpHitCeil;
    processes[28] = &g_ftDededeStatusUniqProcessSpecialSuperJumpTurn;
    processes[29] = &lbl_117_bss_28C;
    processes[30] = &lbl_117_bss_28C;
    processes[31] = &lbl_117_bss_28C;
    processes[32] = &lbl_117_bss_28C;
    processes[33] = &lbl_117_bss_28C;
    processes[34] = &lbl_117_bss_28C;
    processes[35] = &lbl_117_bss_28C;
    processes[36] = &lbl_117_bss_28C;
    processes[37] = &lbl_117_bss_28C;
    processes[38] = &lbl_117_bss_2DC;
    processes[39] = &lbl_117_bss_2DC;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 40);
    // HYPOTHESIS: the common data record points at the fighter's area definitions (+0xb8).
    m_moduleAccesser->getAreaModule().addArea(*reinterpret_cast<soSet<soAreaData>**>(reinterpret_cast<u8*>(m_data) + 0xb8));
}

// The Jet Hammer starts with the model resource of the fighter and the facing.
bool ftDedede::activeArticle1(wnDededeJethammer* weapon, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    float lr = acc->getPostureModule().getLr();
    int modelResId = resources.getResourceIdAccesser()->getMdlResId();
    weapon->activate(founderTaskId, modelResId, lr);
    return true;
}

// HYPOTHESIS: the work flags 0x22000013/0x22000014 mark the two throw directions of the Star Spit; the star starts at the
// model node "TopN" moved by the parameter offsets (the second one follows the facing), and the used flag is cleared.
bool ftDedede::activeArticle(wnDededeStar* weapon, soModuleAccesser* acc) {
    float lr = acc->getPostureModule().getLr();
    bool flip;
    Vec3f pos;
    Vec3f offset;
    if (acc->getWorkManageModule().isFlag(0x22000013)) {
        offset.m_x = 0.0f;
        offset.m_y = soValueAccesser::getConstantFloat(acc, 0xfbd, 0);
        offset.m_z = lr * soValueAccesser::getConstantFloat(acc, 0xfbc, 0);
        const char* nodeName = "TopN";
        soModelModule* model = &acc->getModelModule();
        pos = model->getNodeGlobalPosition(model->getNodeId(nodeName), &offset, false, false);
        flip = false;
        acc->getWorkManageModule().offFlag(0x22000013);
    }
    if (acc->getWorkManageModule().isFlag(0x22000014)) {
        offset.m_x = 0.0f;
        offset.m_y = soValueAccesser::getConstantFloat(acc, 0xfbf, 0);
        offset.m_z = lr * soValueAccesser::getConstantFloat(acc, 0xfbe, 0);
        const char* nodeName = "TopN";
        soModelModule* model = &acc->getModelModule();
        pos = model->getNodeGlobalPosition(model->getNodeId(nodeName), &offset, false, false);
        flip = true;
        acc->getWorkManageModule().offFlag(0x22000014);
    }
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    int team = acc->getTeamModule().getTeam()->getNo();
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), team, &pos, flip, 1.0f, 20.0f);
    return false;
}

bool ftDedede::notifyEventCollisionSearchCheck() {
    if (ftDededeTransactor::getInstance()->notifyEventCollisionSearchCheck(m_moduleAccesser) == true) {
        return true;
    }
    return Fighter::notifyEventCollisionSearchCheck();
}

void ftDedede::onStart(int startKind) {
    m_moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
    m_moduleAccesser->getWorkManageModule().setInt(0, 0x10000042);
    m_moduleAccesser->getWorkManageModule().setInt(-1, 0x10000043);
    m_moduleAccesser->getWorkManageModule().setInt(-1, 0x10000044);
    Fighter::onStart(startKind);
}

// BYTES: the native function addresses the local static's guard, object and destructor registration separately
// instead of from one pooled BSS base. The setting is per function: unit-wide it breaks the pooled constructor and
// __sinit, and an in-class inline definition makes the static weak and moves it to the end of .bss.
#pragma pool_data off
ftDededeTransactor* ftDededeTransactor::getInstance() {
    static ftDededeTransactor instance;
    return &instance;
}
#pragma pool_data reset

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Dedede, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftDededeArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftDededeArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftDededeArticleKind : wnArticleKindInfo {
    ftDededeArticleKind() {
        kind = 0;
        subKind = Fighter_Dedede;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftDededeArticleBuildInfo : wnArticleBuildInfo {
    ftDededeArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_148910(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <class W> struct ftDededeArticleTraits;
#define FT_DEDEDE_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftDededeArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftDededeArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_Dedede, &tag.m_resourceGroup)); \
        } \
    };
#define FT_DEDEDE_TRAITS_MEMBER(W, DATA, ID, GETTER) \
    template <> struct ftDededeArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftDededeArticleDataTag tag; \
            return static_cast<const DATA*>(g_ftCommonDataAccesser.GETTER(Fighter_Dedede, &tag.m_resourceGroup)); \
        } \
    };

FT_DEDEDE_TRAITS_MEMBER(wnDededeStar, wnDededeStarData, 0, getWeaponData<wnDededeStarData>)
FT_DEDEDE_TRAITS_MEMBER(wnDededeJethammer, wnDededeJethammerData, 1, getWeaponData<wnDededeJethammerData>)
FT_DEDEDE_TRAITS(wnDededeStarmissile, wnDededeStarmissileData, 2, fn_27_148910)
FT_DEDEDE_TRAITS_MEMBER(wnDededeWaddledooBeam, wnDededeWaddledooBeamData, 3, getWeaponData<wnDededeWaddledooBeamData>)
FT_DEDEDE_TRAITS_MEMBER(wnDededeFollower, wnDededeFollowerData, 4, getWeaponData<wnDededeFollowerData>)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftDededeArticleTraits<W>::ArticleId, ftDededeArticleBuildInfo(ftDededeArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftDededeArticleTraits<W>::getData()) { }

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnDededeStar>::~wnArticleHolder();
template wnArticleHolder<wnDededeJethammer>::~wnArticleHolder();
template wnArticleHolder<wnDededeStarmissile>::~wnArticleHolder();
template wnArticleHolder<wnDededeWaddledooBeam>::~wnArticleHolder();
template wnArticleHolder<wnDededeFollower>::~wnArticleHolder();
template wnArticleHolder<wnDededeStar>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnDededeWaddledooBeam>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnDededeFollower>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftDededeArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftDededeArticleMediator, 25>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftDededeArticleMediator, 25>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftDededeArticleMediator::~ftDededeArticleMediator() { }
#pragma dont_inline off

s32 ftDededeArticleMediator::getMediateNum() { return 5; }
void ftDededeArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Star, 1 Jethammer, 2 Starmissile, 3 WaddledooBeam, 4 Follower (the rest are unused).

#define FT_DEDEDE_POOL_STAR_BASE static_cast<wnArticlePool<wnDededeStar, 2, ftDededeJethammerPool>&>(m_pools)
#define FT_DEDEDE_POOL_STAR FT_DEDEDE_POOL_STAR_BASE.getSub()
#define FT_DEDEDE_POOL_JETHAMMER_BASE static_cast<wnArticlePool<wnDededeJethammer, 1, ftDededeStarmissilePool>&>(m_pools)
#define FT_DEDEDE_POOL_JETHAMMER FT_DEDEDE_POOL_JETHAMMER_BASE.getSub()
#define FT_DEDEDE_POOL_STARMISSILE_BASE static_cast<wnArticlePool<wnDededeStarmissile, 1, ftDededeWaddledooBeamPool>&>(m_pools)
#define FT_DEDEDE_POOL_STARMISSILE FT_DEDEDE_POOL_STARMISSILE_BASE.getSub()
#define FT_DEDEDE_POOL_WADDLEDOOBEAM_BASE static_cast<wnArticlePool<wnDededeWaddledooBeam, 4, ftDededeFollowerPool>&>(m_pools)
#define FT_DEDEDE_POOL_WADDLEDOOBEAM FT_DEDEDE_POOL_WADDLEDOOBEAM_BASE.getSub()
#define FT_DEDEDE_POOL_FOLLOWER_BASE static_cast<wnArticlePool<wnDededeFollower, 19, soInstancePoolRoot>&>(m_pools)
#define FT_DEDEDE_POOL_FOLLOWER FT_DEDEDE_POOL_FOLLOWER_BASE.getSub()

#define FT_DEDEDE_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) WN_ARTICLE_UNUSED_SLOT(7, value) \
    WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) WN_ARTICLE_UNUSED_SLOT(10, value) \
    WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) WN_ARTICLE_UNUSED_SLOT(13, value) \
    WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) WN_ARTICLE_UNUSED_SLOT(16, value)

void ftDededeArticleMediator::deactivate() {
    for (s32 i = 0; i < 2; ++i) {
        wnDededeStar* weapon = FT_DEDEDE_POOL_STAR.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnDededeJethammer* weapon = FT_DEDEDE_POOL_JETHAMMER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnDededeStarmissile* weapon = FT_DEDEDE_POOL_STARMISSILE.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 4; ++i) {
        wnDededeWaddledooBeam* weapon = FT_DEDEDE_POOL_WADDLEDOOBEAM.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 19; ++i) {
        wnDededeFollower* weapon = FT_DEDEDE_POOL_FOLLOWER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftDededeArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(2, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerateMaxNum(4, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotGenerateMaxNum(19, WN_ARTICLE_SLOT_TAGS);
    FT_DEDEDE_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftDededeActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

s32 ftDededeArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnDededeStar, 2, ftDededeJethammerPool>, ftDededeActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotActiveNum<wnArticlePool<wnDededeJethammer, 1, ftDededeStarmissilePool>, ftDededeActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotActiveNum<wnArticlePool<wnDededeStarmissile, 1, ftDededeWaddledooBeamPool>, ftDededeActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotActiveNum<wnArticlePool<wnDededeWaddledooBeam, 4, ftDededeFollowerPool>, ftDededeActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotActiveNum<wnArticlePool<wnDededeFollower, 19, soInstancePoolRoot>, ftDededeActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    FT_DEDEDE_UNUSED_SLOTS(0)
    default: return 0;
    }
}

bool ftDededeArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnDededeStar, 2, ftDededeJethammerPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotIsGeneratable<wnArticlePool<wnDededeJethammer, 1, ftDededeStarmissilePool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotIsGeneratable<wnArticlePool<wnDededeStarmissile, 1, ftDededeWaddledooBeamPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotIsGeneratable<wnArticlePool<wnDededeWaddledooBeam, 4, ftDededeFollowerPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotIsGeneratable<wnArticlePool<wnDededeFollower, 19, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
    FT_DEDEDE_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftDededeArticleActivator;
template <> struct ftDededeArticleActivator<wnDededeStar> {
    static bool activate(wnDededeStar* weapon, soModuleAccesser* acc) { return ftDedede::activeArticle(weapon, acc); }
};
template <> struct ftDededeArticleActivator<wnDededeJethammer> {
    static bool activate(wnDededeJethammer* weapon, soModuleAccesser* acc) { return ftDedede::activeArticle1(weapon, acc); }
};
template <> struct ftDededeArticleActivator<wnDededeStarmissile> {
    static bool activate(wnDededeStarmissile* weapon, soModuleAccesser* acc) {
        u32 etcResId = acc->getResourceModule().getResourceIdAccesser()->getEtcResId();
        return ftDededeTransactor::getInstance()->activeArticle(weapon, acc, etcResId);
    }
};
template <> struct ftDededeArticleActivator<wnDededeWaddledooBeam> {
    static bool activate(wnDededeWaddledooBeam* weapon, soModuleAccesser* acc) { return ftDedede::activeArticle3(weapon, acc); }
};
template <> struct ftDededeArticleActivator<wnDededeFollower> {
    static bool activate(wnDededeFollower* weapon, soModuleAccesser* acc) { return ftDedede::activeArticle2(weapon, acc); }
};

soArticle* ftDededeArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<ftDededeArticleActivator<wnDededeStar> >(static_cast<wnArticlePool<wnDededeStar, 2, ftDededeJethammerPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerate<ftDededeArticleActivator<wnDededeJethammer> >(static_cast<wnArticlePool<wnDededeJethammer, 1, ftDededeStarmissilePool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerate<ftDededeArticleActivator<wnDededeStarmissile> >(static_cast<wnArticlePool<wnDededeStarmissile, 1, ftDededeWaddledooBeamPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerate<ftDededeArticleActivator<wnDededeWaddledooBeam> >(static_cast<wnArticlePool<wnDededeWaddledooBeam, 4, ftDededeFollowerPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotGenerate<ftDededeArticleActivator<wnDededeFollower> >(static_cast<wnArticlePool<wnDededeFollower, 19, soInstancePoolRoot>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    FT_DEDEDE_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

template <int S, int L>
inline bool ftDededeShootSlot1(soArticle* article, const soIntToType<S>&, const soIntToType<L>&) {
    (void)dynamic_cast<wnDededeJethammer&>(*article);
    return false;
}

template <int S, int L>
inline bool ftDededeShootSlot4(soModuleAccesser* acc, soArticle* article, const soIntToType<S>&, const soIntToType<L>&) {
    return ftDedede::shootArticle(&dynamic_cast<wnDededeFollower&>(*article), acc);
}

bool ftDededeArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnDededeStar>(article, WN_ARTICLE_SLOT_TAGS);
    case 1: return ftDededeShootSlot1(article, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotShoot<wnDededeStarmissile>(article, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotShoot<wnDededeWaddledooBeam>(article, WN_ARTICLE_SLOT_TAGS);
    case 4: return ftDededeShootSlot4(acc, article, WN_ARTICLE_SLOT_TAGS);
    FT_DEDEDE_UNUSED_SLOTS(true)
    default: return false;
    }
}


// FIXME: Test code present only to emit the shared builder functions; delete once ftDedede is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftDededeInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftDededeInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Destructors whose bodies are empty; the compiler emits the deleting variants.
#pragma dont_inline on
soResourceIdAccesser::~soResourceIdAccesser() { }
#pragma dont_inline off
ftKineticEnergyController::~ftKineticEnergyController() { }
#pragma dont_inline on
ftVirtualNodeMatrixPool::~ftVirtualNodeMatrixPool() { }
#pragma dont_inline off

// ftDedede::notifyArticleEventRemove forwards to the StageObject version (tail call).
extern "C" void fn_117_99E0(StageObject* self, int unk1, int* unk2) { self->StageObject::notifyArticleEventRemove(unk1, unk2); }

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_117_67D4(u8* p) { return *(u8*)(p + 0x4); }
void fn_117_8A60() {}
int soGetSubKind__6WeaponFv(u8* p) { return *(int*)(p + 0xb8); }
int fn_117_9F40() { return 0x0; }
void fn_117_A13C() {}
void adjustParentGroundCollision__11StageObjectFiPf() {}
u8 fn_117_A2B8(u8* p) { return *(u8*)(p + 0x44); }
void processGameProc__11StageObjectFv() {}
void fn_117_A300() {}
void fn_117_A304() {}
void fn_117_A394() {}
void fn_117_A3C0() {}
void fn_117_A3C4() {}
void fn_117_A3C8() {}
void fn_117_A3CC() {}
void fn_117_A3D0() {}
void fn_117_A3D4() {}
void fn_117_A3D8() {}
void fn_117_A3DC() {}
void fn_117_A3E0() {}
void fn_117_A3E4() {}
void fn_117_A3E8() {}
void fn_117_A3EC() {}
void fn_117_A3F0() {}
void fn_117_A3F4() {}
void fn_117_A3F8() {}
int fn_117_A3FC() { return 0x0; }
void fn_117_A404() {}
void fn_117_A408() {}
void fn_117_A40C() {}
void fn_117_A410() {}
void fn_117_A43C() {}
void fn_117_A440() {}
void fn_117_A444() {}
void fn_117_A448() {}
void fn_117_A44C() {}
int fn_117_A450(u8* p) { return *(int*)(p + 0x110); }
int fn_117_A468() { return 0x0; }
void fn_117_A470() {}
void fn_117_A474() {}
void fn_117_A478() {}
void fn_117_A47C() {}
int fn_117_A480() { return 0x0; }
int fn_117_A488() { return 0x0; }
void fn_117_A490() {}
void fn_117_A494() {}
void fn_117_A498() {}
void fn_117_A49C() {}
void fn_117_A4A0() {}
int fn_117_A654(u8* p) { return *(int*)(p + 0x20); }
int fn_117_A730(u8* p) { return *(int*)(p + 0x18); }
int fn_117_A7C8(u8* p) { return *(int*)(p + 0x10); }
int fn_117_A888() { return 0x0; }
int fn_117_C104() { return 0x5; }
void fn_117_D0F8() {}
void fn_117_D804() {}
int fn_117_D840() { return 0x8; }
void fn_117_D888() {}
int fn_117_D934() { return 0x0; }
void fn_117_D970() {}
int fn_117_DA1C() { return 0x0; }
void fn_117_DA58() {}
int fn_117_DB04() { return 0x0; }
void fn_117_DB40() {}
int fn_117_DBEC() { return 0x0; }
void fn_117_DC28() {}
int fn_117_DCD4() { return 0x0; }
void fn_117_DD10() {}
int fn_117_DDBC() { return 0x0; }
void fn_117_DDF8() {}
int fn_117_DEA4() { return 0x0; }
void fn_117_DEE0() {}
int fn_117_DF8C() { return 0x0; }

}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_117_E1B0(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
void fn_117_E1B8(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
void fn_117_E1C0(u8* self, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x70))->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser); }
bool fn_117_E1C8(u8* self) { return ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_117_E1D0(u8* self, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); }
bool fn_117_E208(u8* self, u32 flags) { return ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttackCheck(flags); }
void fn_117_E210(u8* self, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); }
void fn_117_E218(u8* self, int index, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x88))->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser); }
bool fn_117_E220(u8* self) { return ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
bool fn_117_E238(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_117_E250(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
bool fn_117_E270(u8* self, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) { return ((Fighter*)(self - 0xC4))->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3); }
void fn_117_E278(u8* self, BaseItem* item, int unk2, bool unk3, u8 unk4) { ((Fighter*)(self - 0xD0))->Fighter::notifyVisibilityItem(item, unk2, unk3, unk4); }
void fn_117_E280(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectAttachItem(item, index, unk3); }
void fn_117_E288(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectItem(item, index, unk3); }
void fn_117_E290(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyShootBulletItem(item); }
void fn_117_E298(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyDropItem(item); }
void fn_117_E2A0(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyThrowItem(item, nodeIndex, unk3); }
void fn_117_E2A8(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyUseItem(item, nodeIndex, unk3); }
void fn_117_E2B0(u8* self, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk4, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk4, unk5); }
void fn_117_E2B8(u8* self, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk5); }
bool fn_117_E2C0(u8* self, BaseItem* item, bool* unk2) { return ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItemPreCheck(item, unk2); }
void fn_117_E2C8(u8* self, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); }
void fn_117_E2D0(u8* self, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser); }
void fn_117_E2D8(u8* self, float unk1, int unk2) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventPikminFinalAttack(unk1, unk2); }
void fn_117_E2E0(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_117_E2E8(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventBeat(); }
void fn_117_E2F0(u8* self, float unk1) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventSetDamage(unk1); }
void fn_117_E2F8(u8* self, float unk1, float unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x100))->Fighter::notifyEventTurn(unk1, unk2, moduleAccesser); }
bool fn_117_E1E0(u8* self, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) { return ((Fighter*)(self - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); }
void fn_117_E228(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_117_E230(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_117_E240(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_117_E248(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_117_E258(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_117_43A8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* __dt__17soArticleOperatorFv(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* __dt__18soArticleGeneratorFv(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
} // extern "C"

void ftDedede::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    switch (statusKind) {
    case 0x115: {
        soLinkEventArgs eventArgs(0x83a);
        moduleAccesser->getLinkModule().sendEventNodes(1, eventArgs, 0);
        break;
    }
    case 0x12f: {
        soLinkEventArgs eventArgs(0x83b);
        moduleAccesser->getLinkModule().sendEventNodes(1, eventArgs, 0);
        break;
    }
    case 0x130: {
        soLinkEventArgs eventArgs(0x83c);
        moduleAccesser->getLinkModule().sendEventNodes(1, eventArgs, 0);
        break;
    }
    case 0x131: {
        soLinkEventArgs eventArgs(0x83d);
        moduleAccesser->getLinkModule().sendEventNodes(1, eventArgs, 0);
        break;
    }
    case 0x132: {
        soLinkEventArgs eventArgs(0x83e);
        moduleAccesser->getLinkModule().sendEventNodes(1, eventArgs, 0);
        break;
    }
    case 0x133: {
        soLinkEventArgs eventArgs(0x83f);
        moduleAccesser->getLinkModule().sendEventNodes(1, eventArgs, 0);
        break;
    }
    case 0x134: {
        soLinkEventArgs eventArgs(0x840);
        moduleAccesser->getLinkModule().sendEventNodes(1, eventArgs, 0);
        break;
    }
    case 0x135: {
        soLinkEventArgs eventArgs(0x841);
        moduleAccesser->getLinkModule().sendEventNodes(1, eventArgs, 0);
        break;
    }
    case 0x136: {
        soLinkEventArgs eventArgs(0x842);
        moduleAccesser->getLinkModule().sendEventNodes(1, eventArgs, 0);
        break;
    }
    case 0x137: {
        soLinkEventArgs eventArgs(0x843);
        moduleAccesser->getLinkModule().sendEventNodes(1, eventArgs, 0);
        break;
    }
    }
    switch (prevStatusKind) {
    case 0x116:
    case 0x138:
    case 0x139:
        if (statusKind != 0x138 && statusKind != 0x139) {
            soLinkEventArgs eventArgs(0x83c);
            moduleAccesser->getLinkModule().sendEventNodes(-1, eventArgs, 0);
        }
        break;
    }
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
}

// TODO: clean up, give variables real names
namespace jj_9E84 {
extern "C" void* getInstance__18ftDededeTransactorFv();
extern "C" void notifyEventCollisionSearch__7FighterFP14soCollisionLogP16soModuleAccesser(void*, void*, void*);

namespace {
struct DededeDispatch {
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
    virtual void slot10(void*, void*);
};
}

extern "C" void fn_117_9E84(void* self, void* a, void* b) {
    DededeDispatch* obj = static_cast<DededeDispatch*>(getInstance__18ftDededeTransactorFv());
    obj->slot10(a, b);
    notifyEventCollisionSearch__7FighterFP14soCollisionLogP16soModuleAccesser(self, a, b);
}
}

// TODO: clean up, give variables real names
namespace jj_E1F0 {
// The interface pointer for this entry points 0x54 bytes past the complete
// object (the secondary vtable header stores an offset-to-top of -0x54
// immediately before this slot); the entry re-derives the complete object
// pointer and tail-calls fn_117_94DC, which forwards the event to the
// DededeEventHandler::handle member function.
struct DededeEvent;
struct DededeEventHandler;
class soModuleAccesser;

extern "C" void fn_117_94DC(DededeEventHandler*, DededeEvent*,
                            soModuleAccesser*, void*, int);

extern "C" void fn_117_E1F0(void* object, DededeEvent* event,
                            soModuleAccesser* accesser, void* param, int kind) {
    fn_117_94DC(reinterpret_cast<DededeEventHandler*>(
                    static_cast<unsigned char*>(object) - 0x54),
                event, accesser, param, kind);
}
}

// TODO: clean up, give variables real names
namespace jj_E268 {
// The interface pointer for this entry points 0xB8 bytes past the complete
// object; this entry slot re-derives the complete object pointer and
// tail-calls the in-module handler fn_117_9E84.
//
// Target evidence (original ft_dedede.rel): .text[0xE268,0xE270) is the raw
// word pair 3863ff48 4bffbc18 = addi r3,r3,-0xB8 ; b 0x9E84. No relocation
// of any import stream patches the branch word, so the displacement is the
// link-time-resolved intra-module branch, unlike the sibling thunks E208 /
// E210 / E218 whose branches carry module-27 REL24 relocations.
//
// Referencing-slot context (original ft_dedede.rel, self-stream records):
// exactly one record references 0xE268, the ADDR32 slot at .data 0x330. The
// recorded physical slot values around it: .data 0x32C points to .text
// 0xA358, whose bytes disassemble as the five-instruction sequence
// extsh r4,r4 ; mr r0,r5 ; mr r5,r3 ; extsb r6,r0 ; b <link-patched>
// (the same instruction sequence recurs at .text 0xA330/0xA344/0xA36C);
// .data 0x334 points to E260; .data 0x338 points to .data 0x698; raw words
// -0xB8 and -0xC4 sit at .data 0x328 and 0x33C, and the negative words
// across this table step by 0xC (-0xA0/-0xAC/-0xB8/-0xC4); the same slot
// values recur in a second copy (.data 0x814/0x818/0x81C -> .text
// 0xA358/E1D0/E1C8 and 0x820 -> .data 0xB94), which has no E268 entry.
// Because the bytes at .text 0xA358 are executable code, the initial
// draft's "typeinfo" label for slot 0x32C is withdrawn. Whether this region
// is a C++ vtable or some other structure is not resolved by this evidence;
// no structural classification of the surrounding records is asserted, and
// the reconstruction relies only on the single E268 reference and the 8
// target bytes.
//
// fn_117_9E84, from the disassembly of its preserved 0x64 bytes
// (evidence/fn_117_9E84.disasm.txt): it saves the incoming r3/r4/r5 into
// r29/r30/r31, calls the local function at 0x946C, restores r4/r5 and makes
// a virtual slot-12 call, then restores r3/r4/r5 and invokes notifyEventCollisionSearch__7FighterFP14soCollisionLogP16soModuleAccesser
// with the original first three arguments. No instruction of fn_117_9E84
// reads or writes r6/r7, so its own code passes the incoming r6/r7 to that
// call site unchanged; r6/r7 are caller-saved in this ABI, however, and
// fn_117_9E84 makes intervening calls (0x946C and the virtual call) before
// it, so whether notifyEventCollisionSearch__7FighterFP14soCollisionLogP16soModuleAccesser observes E268's original r6/r7 values is not
// established by this evidence. The declaration mirrors the shape already
// accepted for E1D0's direct notifyEventCollisionSearch__7FighterFP14soCollisionLogP16soModuleAccesser tail call (void* event,
// soModuleAccesser* accesser, int kind, int index); the only semantically
// guaranteed transformation in E268's own 8 bytes is r3 -= 0xB8 with every
// other incoming register passed through unchanged.
class soModuleAccesser;

extern "C" void fn_117_9E84(void* object, void* event,
        soModuleAccesser* accesser, int kind, int index);

extern "C" void fn_117_E268(void* interface, void* event,
                            soModuleAccesser* accesser, int kind, int index) {
    fn_117_9E84(static_cast<unsigned char*>(interface) - 0xB8, event, accesser,
                kind, index);
}
}

