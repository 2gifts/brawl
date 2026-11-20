// TODO: clean up - raw offset accesses into fighter/module structs; add named fields to the fighter class (or a local struct) instead of *(T*)(p + 0x..)
#define FT_ZELDA_ARTICLES_FULL
#define FT_POOL_ROOT_NONPOLYMORPHIC
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_NOINLINE_SEARCH_BUILDER
#define FT_ZELDA_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_reflector_group.h>
#include <so/so_module_hyp.h>
#include <ft/ft_class_info_impl.h>
#include <ft/zelda/ft_zelda.h>
#include <ft/zelda/ft_zelda_extend_param_accesser.h>
#include <so/article/so_article_mediator.h>
#include <ft/ft_owner.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/so_value_accesser.h>

#undef WN_ARTICLE_NATIVE_BUILDERS
#define FT_BC ftZeldaBuildConfig
#include <ft/builder/ft_builder_noinline.h>
#include <ft/ft_change_succeed_work.h>
#include <so/collision/so_collision_log.h>

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt).
extern "C" {
extern soStatusUniqProcess lbl_27_bss_7DD4;
extern soStatusUniqProcess lbl_104_bss_194;
extern soStatusUniqProcess lbl_104_bss_1A4;
extern soStatusUniqProcess lbl_104_bss_1D4;
extern soStatusUniqProcess lbl_104_bss_1E4;
extern soStatusUniqProcess lbl_104_bss_1B4;
}
extern soStatusUniqProcess g_ftZeldaStatusUniqProcessSpecialHi3;

ftZeldaExtendParamAccesser g_ftZeldaExtendParamAccesser;

ftClassInfoImpl<Fighter_Zelda, ftZelda> g_ftClassInfoZelda;

ftZelda::ftZelda(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftZeldaBuildConfig>(entryId,
                                         Fighter_Zelda,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Zelda)),
    unk18674(0),
    unk18675(0) {
    // Register the status processes in action order; entries without a process stay empty.
    soStatusUniqProcess* processes[10] = {0};
    processes[0] = &lbl_27_bss_7DD4;
    processes[1] = &lbl_104_bss_194;
    processes[2] = &lbl_104_bss_1A4;
    processes[3] = &lbl_104_bss_1D4;
    processes[4] = &lbl_104_bss_1E4;
    processes[7] = &lbl_104_bss_1B4;
    processes[8] = &g_ftZeldaStatusUniqProcessSpecialHi3;
    processes[9] = &lbl_104_bss_1D4;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 10);
    // The reflector (Nayru's Love): build the reflector group from the common data record (+0x90) and install it as group 2.
    void* reflectorRecord = *reinterpret_cast<void**>(reinterpret_cast<u8*>(m_data) + 0x90);
    u8 shape[0x20];
    ftReflectorGroupDraft group;
    FT_REFLECTOR_GROUP_CLEAR(group);
    ftUtil::convertReflectorData(reflectorRecord, shape, &group);
    m_moduleAccesser->getCollisionReflectorModule().add(reinterpret_cast<soCollisionReflectorGroupData*>(&group), 2);
    m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
    getPhysicsModule(m_moduleAccesser).unk6C(0x4B);
}

// HYPOTHESIS: the record at +0x80 of the common data holds the offsets (in the facing direction and up) of the fireball from
// the node of the head.
struct ftZeldaDeinOffset {
    u8 unk0[0xC];
    float m_x;
    float m_y;
};

// Din's Fire starts at the head node of the fighter, moved by the offset of the data record.
bool ftZelda::activeArticle(wnZeldaDein* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soModelModule& model = acc->getModelModule();
    Vec3f pos = model.getNodeGlobalPosition(model.getNodeId("HeadN"), false);
    const ftZeldaDeinOffset* offset = *reinterpret_cast<const ftZeldaDeinOffset* const*>(reinterpret_cast<const u8*>(g_ftCommonDataAccesser.getData(Fighter_Zelda)) + 0x80);
    pos.m_x += offset->m_x * acc->getPostureModule().getLr();
    pos.m_y += offset->m_y;
    pos.m_z = 0.0f;
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, fighter.getOwner()->getTeam(), acc->getPostureModule().getLr(), &pos);
    return true;
}

// The light bow of the Final Smash starts at the stage object of the fighter with its team.
bool ftZelda::activeArticle1(wnZeldaLightingBow* weapon, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, dynamic_cast<ftResourceIdAccesserImpl&>(*resources.getResourceIdAccesser()).getFinalResId(), acc->getTeamModule().getTeam()->getNo(), 0x45);
    return true;
}

// The light arrow of the Final Smash starts the same way.
bool ftZelda::activeArticle2(wnZeldaLightingBowArrow* weapon, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, dynamic_cast<ftResourceIdAccesserImpl&>(*resources.getResourceIdAccesser()).getFinalResId(), acc->getTeamModule().getTeam()->getNo(), 0x63);
    return true;
}

// ---- the article holders --------------------------------------------------------------------------------------------
// Articles borrow the owner's kind record and heap while their constructors run.
// The data lookup receives a separate snapshot of the owner's fighter kind.
#include <ft/builder/ft_builder_article.h>

struct ftZeldaArticleKind : wnArticleKindInfo {
    ftKind getSubKind() const { return static_cast<ftKind>(subKind); }
    ftZeldaArticleKind() {
        kind = 0;
        subKind = Fighter_Zelda;
    }
};


// Keep explicit holder bodies where the pool calls them out of line.
FT_DEFINE_ARTICLE_HOLDER_MEMBER(, wnZeldaDein, const wnZeldaDeinData, 0, ftZeldaArticleKind, getWeaponData<wnZeldaDeinData>)
FT_DEFINE_ARTICLE_HOLDER_MEMBER(, wnZeldaDeinS, const wnZeldaDeinSData, 1, ftZeldaArticleKind, getWeaponData<wnZeldaDeinSData>)
FT_DEFINE_ARTICLE_HOLDER_MEMBER(inline, wnZeldaLightingBow, const wnZeldaLightingBowData, 2, ftZeldaArticleKind, getWeaponData<wnZeldaLightingBowData>)
FT_DEFINE_ARTICLE_HOLDER_MEMBER(inline, wnZeldaLightingBowArrow, const wnZeldaLightingBowArrowData, 3, ftZeldaArticleKind, getWeaponData<wnZeldaLightingBowArrowData>)

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnZeldaDein>::~wnArticleHolder();
template wnArticleHolder<wnZeldaDeinS>::~wnArticleHolder();
template wnArticleHolder<wnZeldaLightingBow>::~wnArticleHolder();
template wnArticleHolder<wnZeldaLightingBowArrow>::~wnArticleHolder();
template wnArticleHolder<wnZeldaDein>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnZeldaDeinS>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftZeldaArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftZeldaArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftZeldaArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
inline wnZeldaLightingBow::~wnZeldaLightingBow() { }
inline wnZeldaLightingBowArrow::~wnZeldaLightingBowArrow() { }
ftZeldaArticleMediator::~ftZeldaArticleMediator() { }
#pragma dont_inline off

s32 ftZeldaArticleMediator::getMediateNum() { return 4; }
void ftZeldaArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Dein, 1 DeinS, 2 LightingBow, 3 LightingBowArrow (the rest are unused).

#define FT_ZELDA_POOL_DEIN_BASE static_cast<wnArticlePool<wnZeldaDein, 3, ftZeldaDeinSPool>&>(m_pools)
#define FT_ZELDA_POOL_DEIN FT_ZELDA_POOL_DEIN_BASE.getSub()
#define FT_ZELDA_POOL_DEINS_BASE static_cast<wnArticlePool<wnZeldaDeinS, 3, ftZeldaLightingBowPool>&>(m_pools)
#define FT_ZELDA_POOL_DEINS FT_ZELDA_POOL_DEINS_BASE.getSub()
#define FT_ZELDA_POOL_LIGHTINGBOW_BASE static_cast<wnArticlePool<wnZeldaLightingBow, 1, ftZeldaLightingBowArrowPool>&>(m_pools)
#define FT_ZELDA_POOL_LIGHTINGBOW FT_ZELDA_POOL_LIGHTINGBOW_BASE.getSub()
#define FT_ZELDA_POOL_LIGHTINGBOWARROW_BASE static_cast<wnArticlePool<wnZeldaLightingBowArrow, 1, soInstancePoolRoot>&>(m_pools)
#define FT_ZELDA_POOL_LIGHTINGBOWARROW FT_ZELDA_POOL_LIGHTINGBOWARROW_BASE.getSub()

#define FT_ZELDA_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

void ftZeldaArticleMediator::deactivate() {
    for (s32 i = 0; i < 3; ++i) {
        wnZeldaDein* weapon = FT_ZELDA_POOL_DEIN.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 3; ++i) {
        wnZeldaDeinS* weapon = FT_ZELDA_POOL_DEINS.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnZeldaLightingBow* weapon = FT_ZELDA_POOL_LIGHTINGBOW.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnZeldaLightingBowArrow* weapon = FT_ZELDA_POOL_LIGHTINGBOWARROW.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftZeldaArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(3, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerateMaxNum(3, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    FT_ZELDA_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftZeldaActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

s32 ftZeldaArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnZeldaDein, 3, ftZeldaDeinSPool>, ftZeldaActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotActiveNum<wnArticlePool<wnZeldaDeinS, 3, ftZeldaLightingBowPool>, ftZeldaActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotActiveNum<wnArticlePool<wnZeldaLightingBow, 1, ftZeldaLightingBowArrowPool>, ftZeldaActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotActiveNum<wnArticlePool<wnZeldaLightingBowArrow, 1, soInstancePoolRoot>, ftZeldaActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    FT_ZELDA_UNUSED_SLOTS(0)
    default: return 0;
    }
}

bool ftZeldaArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnZeldaDein, 3, ftZeldaDeinSPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotIsGeneratable<wnArticlePool<wnZeldaDeinS, 3, ftZeldaLightingBowPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotIsGeneratable<wnArticlePool<wnZeldaLightingBow, 1, ftZeldaLightingBowArrowPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotIsGeneratable<wnArticlePool<wnZeldaLightingBowArrow, 1, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
    FT_ZELDA_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftZeldaArticleActivator;
template <> struct ftZeldaArticleActivator<wnZeldaDein> {
    static bool activate(wnZeldaDein* weapon, soModuleAccesser* acc) { return ftZelda::activeArticle(weapon, acc); }
};
template <> struct ftZeldaArticleActivator<wnZeldaDeinS> {
    static bool activate(wnZeldaDeinS* weapon, soModuleAccesser* acc) { return weapon->activeArticle(acc); }
};
template <> struct ftZeldaArticleActivator<wnZeldaLightingBow> {
    static bool activate(wnZeldaLightingBow* weapon, soModuleAccesser* acc) { return ftZelda::activeArticle1(weapon, acc); }
};
template <> struct ftZeldaArticleActivator<wnZeldaLightingBowArrow> {
    static bool activate(wnZeldaLightingBowArrow* weapon, soModuleAccesser* acc) { return ftZelda::activeArticle2(weapon, acc); }
};

soArticle* ftZeldaArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<ftZeldaArticleActivator<wnZeldaDein> >(static_cast<wnArticlePool<wnZeldaDein, 3, ftZeldaDeinSPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerate<ftZeldaArticleActivator<wnZeldaDeinS> >(static_cast<wnArticlePool<wnZeldaDeinS, 3, ftZeldaLightingBowPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerate<ftZeldaArticleActivator<wnZeldaLightingBow> >(static_cast<wnArticlePool<wnZeldaLightingBow, 1, ftZeldaLightingBowArrowPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerate<ftZeldaArticleActivator<wnZeldaLightingBowArrow> >(static_cast<wnArticlePool<wnZeldaLightingBowArrow, 1, soInstancePoolRoot>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    FT_ZELDA_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftZeldaArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnZeldaDein>(article, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotShoot<wnZeldaDeinS>(article, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotShoot<wnZeldaLightingBow>(article, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotShoot<wnZeldaLightingBowArrow>(article, WN_ARTICLE_SLOT_TAGS);
    FT_ZELDA_UNUSED_SLOTS(true)
    default: return false;
    }
}


// FIXME: Test code present only to emit the shared builder functions; delete once ftZelda is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftZeldaInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftZeldaInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

// Empty virtuals and no-op helpers.
void fn_104_A024() {}
void fn_104_A470() {}
void fn_104_A5CC() {}
void fn_104_A5D8() {}
void fn_104_A618() {}
void fn_104_A61C() {}
void fn_104_A6AC() {}
void fn_104_A6D8() {}
void fn_104_A6DC() {}
void fn_104_A6E0() {}
void fn_104_A6E4() {}
void fn_104_A6E8() {}
void fn_104_A6EC() {}
void fn_104_A6F0() {}
void fn_104_A6F4() {}
void fn_104_A6F8() {}
void fn_104_A6FC() {}
void fn_104_A700() {}
void fn_104_A704() {}
void fn_104_A708() {}
void fn_104_A70C() {}
void fn_104_A710() {}
void fn_104_A71C() {}
void fn_104_A720() {}
void fn_104_A724() {}
void fn_104_A728() {}
void fn_104_A754() {}
void fn_104_A758() {}
void fn_104_A75C() {}
void fn_104_A760() {}
void fn_104_A764() {}
void fn_104_A788() {}
void fn_104_A78C() {}
void fn_104_A790() {}
void fn_104_A79C() {}
void fn_104_A7A0() {}
void fn_104_A7A4() {}
void fn_104_A7A8() {}
void fn_104_A7AC() {}
void fn_104_A7B8() {}
void fn_104_98C0() {}
void fn_104_CDA8() {}
void fn_104_D4B8() {}
void fn_104_D53C() {}
void fn_104_D624() {}
void fn_104_D70C() {}
void fn_104_D7F4() {}
void fn_104_D8DC() {}
void fn_104_D9C4() {}
void fn_104_DAAC() {}
void fn_104_DB94() {}

// Constant returns.
int fn_104_A304() { return 0; }
int fn_104_A714() { return 0; }
int fn_104_A780() { return 0; }
int fn_104_A794() { return 0; }
int fn_104_A7BC() { return 0; }
int fn_104_AB68() { return 0; }
int fn_104_D5E8() { return 0; }
int fn_104_D6D0() { return 0; }
int fn_104_D7B8() { return 0; }
int fn_104_D8A0() { return 0; }
int fn_104_D988() { return 0; }
int fn_104_DA70() { return 0; }
int fn_104_DB58() { return 0; }
int fn_104_DC40() { return 0; }
int fn_104_A7B0() { return 0x7ffe; }
int fn_104_BF28() { return 4; }
int fn_104_D4F4() { return 8; }

// Field getters.
u8 fn_104_7930(u8* p) { return *(u8*)(p + 0x4); }
u8 fn_104_A5D0(u8* p) { return *(u8*)(p + 0x44); }
int fn_104_A768(u8* p) { return *(int*)(p + 0x110); }
int fn_104_A934(u8* p) { return *(int*)(p + 0x20); }
int fn_104_AA10(u8* p) { return *(int*)(p + 0x18); }
int fn_104_AAA8(u8* p) { return *(int*)(p + 0x10); }

// Sub-object pointers.
u8* fn_104_A884(u8* p) { return p + 0x458; }
u8* fn_104_A88C(u8* p) { return p + 0x3C8; }
u8* fn_104_A894(u8* p) { return p + 0x8; }
u8* fn_104_A89C(u8* p) { return p + 0x84; }
u8* fn_104_A8A4(u8* p) { return p + 0x70; }
u8* fn_104_A8AC(u8* p) { return p + 0x5C; }
u8* fn_104_A8B4(u8* p) { return p + 0x48; }
u8* fn_104_A8BC(u8* p) { return p + 0x34; }
u8* fn_104_A8C4(u8* p) { return p + 0x20; }
u8* fn_104_A8CC(u8* p) { return p + 0x8; }
u8* fn_104_A7C4(u8* p, u32 idx) { return p + 4 + (idx << 3); }

// Byte flags.
void fn_104_CE90(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_104_CE9C(u8* p) { *(u8*)(p + 0x31) = 1; }
void fn_104_D4FC(s16* dst, s16* src) { *dst = *src; }

// Bit flags in the u32 array at +0x1C.
bool fn_104_A9B0(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); return (a[idx] & mask) != 0; }
void fn_104_A9CC(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] &= ~mask; }
void fn_104_A9E4(u8* p, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] = 0; }
void fn_104_A9F8(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] |= mask; }

// Float array at +0x14.
void fn_104_AA18(u8* p, u32 idx, float d) {
    if (d == 0.0f) {
        return;
    }
    float* a = *(float**)(p + 0x14);
    a[idx] /= d;
}
void fn_104_AA40(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] *= v; }
void fn_104_AA58(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] -= v; }
void fn_104_AA70(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] += v; }
void fn_104_AA88(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] = v; }
float fn_104_AA98(u8* p, u32 idx) { float* a = *(float**)(p + 0x14); return a[idx]; }

// Int array at +0x0C.
void fn_104_AAB0(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]--; }
void fn_104_AAC8(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]++; }
void fn_104_AAE0(u8* p, int d, u32 idx) {
    if (d == 0) {
        return;
    }
    int* a = *(int**)(p + 0xC);
    a[idx] /= d;
}
void fn_104_AB00(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] *= d; }
void fn_104_AB18(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] -= d; }
void fn_104_AB30(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] += d; }
void fn_104_AB48(u8* p, int v, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] = v; }

// Conditional sub-object pointers selected by an index.
u8* fn_104_BFB8(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_104_BFD0(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_104_BFE8(u8* p, int x) {
    if (x == 2) {
        return p + 0x3E4C;
    }
    if (x == 1) {
        return p + 0x1F30;
    }
    if (x == 0) {
        return p + 0x14;
    }
    return 0;
}
u8* fn_104_C020(u8* p, int x) {
    if (x == 2) {
        return p + 0x41CC;
    }
    if (x == 1) {
        return p + 0x20F0;
    }
    if (x == 0) {
        return p + 0x14;
    }
    return 0;
}

// Single-store and pointer-chain accessors.
void fn_104_BF30(u8* p, u8 v) { *(u8*)(p + 0x10000 - 0x17C) = v; }

// Virtual destructors: delete the object when the flag is positive, then return it.
void* fn_104_1E70(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_3CD0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_3D10(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_3E0C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_3EE8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_421C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_4618(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_4978(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_49B8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_6AD0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }

// Polymorphic view used only to emit virtual calls; the slots are never defined in this unit.
struct ftZeldaFlagByte {
    u8 flag : 1;
    u8 rest : 7;
};
struct ftZeldaVirtualSlots {
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
    virtual void slot20();
    virtual void slot21();
    virtual void slot22();
};

// Calls the slot-1 virtual of the object when byte 5 bit 7 is set and byte 6 is clear.

// Tail-call virtual slot (offset 0x58) of the object.
void fn_104_BFA8(void* p) { return ((ftZeldaVirtualSlots*)p)->slot20(); }

}

// The empty parameter-accesser base destructor is out of line in the fighter RELs (FT_PARAM_ACCESSER_TYPED_DTOR); the
// article destructors in this REL call this copy.
soParamAccesser::~soParamAccesser() { }

// BYTES: the original keeps a weak copy of this empty destructor that ftZeldaArticleMediator's destructor calls; the inline
// header form (FT_ARTICLE_MEDIATOR_DTOR_INLINE) would inline it there, so it is defined out of line here.
soArticleMediator::~soArticleMediator() { }

void** ftZelda::getExtendParam() {
    return m_data->extendParam;
}

void ftZelda::processUpdate() {
    Fighter::processUpdate();
    unk18675 = 0;
    unk18674 = 0;
}

void ftZelda::changeSucceedCore(void* work, int status) {
    Fighter::changeSucceedCore(work, status);
    m_moduleAccesser->getWorkManageModule().setInt(static_cast<ftChangeSucceedWork*>(work)->unk100[0], 0x10000040);
}

void ftZelda::change(void* work, int startKind) {
    onStart(startKind);
    changeSucceedCore(work, 0x11B);
}

bool ftZelda::notifyEventCollisionReflectorCheck() {
    if (unk18674 == 1) {
        return true;
    }
    if (unk18675 == 1) {
        m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
    }
    return Fighter::notifyEventCollisionReflectorCheck();
}

void ftZelda::notifyEventCollisionReflector(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    if (groupIndex == 2) {
        if (power >= m_moduleAccesser->getCollisionReflectorModule().getAttackLimit(groupIndex)) {
            unk18675 = 1;
            return;
        }
        unk18674 = 1;
    }
    Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
}

void ftZelda::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser);
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    int count = work.getInt(0x20000000);
    if (count < 8) {
        work.setInt(collisionLog->m_taskId, 0x20000001 + count);
        work.setInt(collisionLog->_35, 0x20000009 + count);
        work.setInt(collisionLog->m_collsionIndex, 0x20000011 + count);
        ++count;
    }
    work.setInt(count, 0x20000000);
}

ftKineticEnergyController::~ftKineticEnergyController() { }
// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

// TODO: clean up, give variables real names
namespace jj_3270 {
extern "C" void __dt__22ftZeldaArticleMediatorFv(void*, short);
extern "C" void __dl__FPv(void*);

extern "C" void* fn_104_3270(void* self, short flag) {
    if (self != 0) {
        __dt__22ftZeldaArticleMediatorFv(self, -1);
        if (flag > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_9984 {
extern "C" void onStart__7FighterFi(void* object, int param);

struct VBase {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5(int a, unsigned int b);
};

extern "C" {
void onStart__5ftIkeFi(void* object, int param) {
    onStart__7FighterFi(object, param);
    unsigned char* p = static_cast<unsigned char*>(object);
    void* a = *reinterpret_cast<void**>(p + 0x60);
    void* b = *reinterpret_cast<void**>(static_cast<unsigned char*>(a) + 0xd8);
    VBase* c = *reinterpret_cast<VBase**>(static_cast<unsigned char*>(b) + 0x64);
    c->v5(0, 0x10000040u);
}
}
}

// TODO: clean up, give variables real names
namespace jj_9A14 {
class Command {
public:
    virtual void f0();
    virtual signed char getGroup();
    virtual signed char getType();
};

class SelfCall {
public:
    char pad[0x3c];
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v29();
    virtual void v30();
    virtual void v31();
    virtual void v32();
    virtual void v33();
    virtual void v34();
    virtual void v35();
    virtual void v36();
    virtual void v37();
    virtual void v38();
    virtual void v39();
    virtual void v40();
    virtual void v41();
    virtual void v42();
    virtual void v43();
    virtual void v44();
    virtual void v45();
    virtual void v46();
    virtual bool check(signed char);
};

extern "C" {
extern void* g_ftManager;
int getFighterCount__9ftManagerCFi(void*, int);
int getFighterNo__9ftManagerCFii(void*, int, int);
void toChange__9ftManagerFiibb(void*, int, unsigned char, int, int);
bool notifyEventAnimCmd__7FighterFP9acAnimCmdP16soModuleAccesserl(void*, void*, void*, int);

bool fn_104_9A14(void* self, Command* command, void* accesser, int kind) {
    signed char group = command->getGroup();
    bool valid = reinterpret_cast<SelfCall*>(self)->check(group);
    if (!valid)
        return false;

    switch (command->getType()) {
    case 5: {
        unsigned char count = (unsigned char)getFighterCount__9ftManagerCFi(
            g_ftManager, *(int*)((char*)self + 0x10c));
        if (count > 1) {
            int enabled = getFighterNo__9ftManagerCFii(
                g_ftManager, *(int*)((char*)self + 0x10c),
                *(int*)((char*)self + 0x28));
            toChange__9ftManagerFiibb(
                g_ftManager, *(int*)((char*)self + 0x10c),
                !((unsigned char)enabled), 1, 0);
        }
        return true;
    }
    default:
        return notifyEventAnimCmd__7FighterFP9acAnimCmdP16soModuleAccesserl(self, command, accesser, kind);
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_9C24 {
extern "C" void notifyEventCollisionSearchCheck__7FighterFv(void);

extern "C" void fn_104_9C24(void) {
    notifyEventCollisionSearchCheck__7FighterFv();
}
}

// TODO: clean up, give variables real names
namespace jj_9E38 {
typedef unsigned long long u64;

struct Vec2f {
    float m_x;
    float m_y;
};

struct Vec3f {
    float m_x;
    float m_y;
    float m_z;
};

struct Parameters {
    Vec2f p[4];
};

struct Posture {
    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual void unused3();
    virtual Vec3f getPos();
    virtual void unused4();
    virtual void unused5();
    virtual void unused6();
    virtual void unused7();
    virtual float getLr();
    virtual void unused8();
    virtual void unused9();
    virtual void unused10();
    virtual void unused11();
    virtual void unused12();
    virtual void unused13();
    virtual void unused14();
    virtual void unused15();
    virtual void unused16();
    virtual void unused17();
    virtual void unused18();
    virtual void unused19();
    virtual float getScale();
};

struct Kinetic {
    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual void unused3();
    virtual void unused4();
    virtual void unused5();
    virtual void unused6();
    virtual void unused7();
    virtual void unused8();
    virtual void unused9();
    virtual void unused10();
    virtual void unused11();
    virtual void unused12();
    virtual u64 getSpeed(const short*);
};

struct WorkModule {
    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual void unused3();
    virtual unsigned int getFloat(unsigned int);
};

struct Modules {
    char pad0[0x0c];
    Posture* posture;
    char pad1[0x54];
    WorkModule* work;
    char pad2[0x14];
    Kinetic* kinetic;
};

struct Accesser {
    char pad[0xd8];
    Modules* modules;
};

struct DataContainer {
    char pad[0x88];
    float* data;
};

struct Object {
    char pad0[0x60];
    Accesser* accesser;
    char pad1[0x1860c];
    DataContainer* dataContainer;
};

extern "C" {
void setupChangeSucceedWork__7FighterFPv(void*);
void fn_104_A024(Parameters*);
void activateDummyFighter__19ftChangeSucceedWorkFP5Vec3fffP5Vec2fPfPfi(void*, const Vec3f&, float, float, const Vec2f&,
                   const Parameters&, const Parameters&, int);
float getConstantFloat__15soValueAccesserFP16soModuleAccesserUlUl(
    Accesser*, unsigned long, unsigned long);
extern const float lbl_104_rodata_8;
extern const float lbl_104_rodata_10;
}

extern "C" void fn_104_9E38(Object* object, void* output) {
    Posture* posture;
    setupChangeSucceedWork__7FighterFPv(object);

    Accesser* accesser = object->accesser;
    const float* data = object->dataContainer->data;

    Parameters b;
    fn_104_A024(&b);
    b.p[0].m_x = lbl_104_rodata_8;
    b.p[1].m_x =
        getConstantFloat__15soValueAccesserFP16soModuleAccesserUlUl(
            accesser, 0xbd6, 0);
    b.p[2].m_x = lbl_104_rodata_8;
    b.p[3].m_x =
        getConstantFloat__15soValueAccesserFP16soModuleAccesserUlUl(
            accesser, 0xbd8, 0);
    b.p[0].m_y = -data[2];
    b.p[1].m_y =
        getConstantFloat__15soValueAccesserFP16soModuleAccesserUlUl(
            accesser, 0xbd1, 0);
    b.p[2].m_y = data[3];
    b.p[3].m_y = data[3];

    Parameters a;
    fn_104_A024(&a);
    a.p[0].m_x = lbl_104_rodata_8;
    a.p[1].m_x =
        getConstantFloat__15soValueAccesserFP16soModuleAccesserUlUl(
            accesser, 0xbbc, 0);
    a.p[2].m_x = lbl_104_rodata_8;
    a.p[3].m_x = lbl_104_rodata_10;
    a.p[0].m_y = lbl_104_rodata_8;
    a.p[1].m_y = lbl_104_rodata_8;
    a.p[2].m_y = lbl_104_rodata_8;
    a.p[3].m_y = lbl_104_rodata_8;

    short flag = -1;
    Vec2f speed;
    *(u64*)&speed = accesser->modules->kinetic->getSpeed(&flag);

    posture = accesser->modules->posture;
    activateDummyFighter__19ftChangeSucceedWorkFP5Vec3fffP5Vec2fPfPfi(output, posture->getPos(), posture->getScale(),
                 posture->getLr(), speed, a, b, 0xe0013);

    *(unsigned int*)((char*)output + 0x100) =
        accesser->modules->work->getFloat(0x10000040);
}
}

// TODO: clean up, give variables real names
namespace jj_D4BC {
struct Vec2f;
class soModuleAccesser;

extern "C" void addSpeed__19ftKineticTransactorFPvPvP16soModuleAccesser(const Vec2f* vector, void* state, soModuleAccesser* accesser);

extern "C" void addSpeed__21ftKineticMediatorImplFPvP16soModuleAccesser(void* object, const Vec2f* vector, soModuleAccesser* accesser) {
    addSpeed__19ftKineticTransactorFPvPvP16soModuleAccesser(vector, static_cast<unsigned char*>(object) + 4, accesser);
}
}

// TODO: clean up, give variables real names
namespace jj_DD58 {
struct FtClassInfoObj {
    const void* vtable;
    unsigned int baseState;
    const void* typeInfo;
};

extern "C" {
extern const unsigned char lbl_104_data_7184[];
extern unsigned char g_ftClassInfoNull[];
void setClassInfo__11ftClassInfoF6ftKindP11ftClassInfo(FtClassInfoObj*, int, void*);
void __dt__11ftClassInfoFv(FtClassInfoObj*, short);
void __dl__FPv(void*);
}

extern "C" FtClassInfoObj* fn_104_DD58(FtClassInfoObj* info, short deleting) {
    if (info) {
        info->typeInfo = lbl_104_data_7184;
        info->vtable = lbl_104_data_7184 + 8;
        setClassInfo__11ftClassInfoF6ftKindP11ftClassInfo(info, 13, g_ftClassInfoNull);
        __dt__11ftClassInfoFv(info, 0);
        if (deleting > 0) operator delete(info);
    }
    return info;
}
}

// TODO: clean up, give variables real names
namespace jj_DDDC {
extern "C" {
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void __ct__7ftZeldaFlQ25Heaps8HeapTypeQ25Heaps8HeapTypeQ25Heaps8HeapType(void*, void*, int, void*, void*);
}

extern "C" void fn_104_DDDC(void*, void* a, int heap, void* b, void* c) {
    void* obj = __nw__FUlQ25Heaps8HeapType(0x18678, heap);
    if (obj != 0) {
        __ct__7ftZeldaFlQ25Heaps8HeapTypeQ25Heaps8HeapTypeQ25Heaps8HeapType(obj, a, heap, b, c);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_DE94 {
extern "C" void fn_104_9A14(void*);

extern "C" void fn_104_DE94(void* self) {
    fn_104_9A14(reinterpret_cast<u8*>(self) - 0x48);
}
}

// TODO: clean up, give variables real names
namespace jj_DF14 {
extern "C" void fn_104_9C24(void*);

extern "C" void fn_104_DF14(void* self) {
    fn_104_9C24(reinterpret_cast<u8*>(self) - 0xb8);
}
}

