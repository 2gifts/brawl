#define FT_POOL_ROOT_NONPOLYMORPHIC
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_SOUND_GENERATOR_DTOR_NOINLINE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_DOL_ARRAY_293_WHOLE // the 293-entry process and anim cmd vectors are sora_melee's whole classes (declared below)
#define FT_LUCARIO_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
#define WN_KINETIC_MEDIATOR_INLINE_DTOR
#define MT_VEC3F_SCALE_XY_IN_PLACE
#define FT_MODULE_ACCESSER_BUILDER_BASE_BUILD_DATA // the builder takes the base build data: its slope limit literal (20.0) stays in the pool
#include <ft/ft_event_unit_instance.h> // before the event system headers (ft/ft_manager.h is included below)
#include <ft/builder/ft_dol_array_list.h>
FT_DOL_ARRAY_VECTOR(soStatusUniqProcess*, 293);
FT_DOL_ARRAY_VECTOR(ftAnimCmdConvPtr, 293);
#include <so/so_module_hyp.h>
#include <so/so_value_accesser.h>
#include <ft/ft_class_info_impl.h>
#include <ft/lucario/ft_lucario.h>
#include <ft/lucario/ft_lucario_data.h>
#include <ft/lucario/ft_lucario_extend_param_accesser.h>
#include <ft/lucario/ft_lucario_transactor.h>
#include <so/article/so_article_mediator.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <so/so_slow.h>
#include <math.h>

// The fighter owns the shared out-of-line kinetic teardown.
#undef WN_ARTICLE_NATIVE_BUILDERS
#define FT_BC ftLucarioBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
// The articles' sound builder teardown inlines this empty destructor (same form as ft_pit.cpp and ft_snake.cpp).
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }

soStatusModuleImpl::~soStatusModuleImpl() { }

ftKineticEnergyController::~ftKineticEnergyController() { }

// The status process instances of the other units (unnamed objects of their .bss in symbols.txt). In the order of the
// units' static initialisers: 254 SpecialS, 264 SpecialSThrow, 274 SpecialHi, 284 SpecialHiRush, 294 SpecialHiRushEnd,
// 2A4 SpecialHiBound, 2B4 SpecialLw, 2C4 SpecialLwSplit, 2D4 SpecialLwAppear, 2E4 Final, 2F4 FinalEntry, 304 FinalHold,
// 314 FinalAttack, 324 FinalEnd; lbl_27_bss_897C is sora_melee's g_ftLucarioStatusUniqProcessSpecialN.
extern "C" {
extern soStatusUniqProcess lbl_27_bss_897C;
extern soStatusUniqProcess lbl_118_bss_254;
extern soStatusUniqProcess lbl_118_bss_274;
extern soStatusUniqProcess lbl_118_bss_2B4;
extern soStatusUniqProcess lbl_118_bss_2E4;
extern soStatusUniqProcess lbl_118_bss_264;
extern soStatusUniqProcess lbl_118_bss_284;
extern soStatusUniqProcess lbl_118_bss_294;
extern soStatusUniqProcess lbl_118_bss_2A4;
extern soStatusUniqProcess lbl_118_bss_2C4;
extern soStatusUniqProcess lbl_118_bss_2D4;
extern soStatusUniqProcess lbl_118_bss_2F4;
extern soStatusUniqProcess lbl_118_bss_304;
extern soStatusUniqProcess lbl_118_bss_314;
extern soStatusUniqProcess lbl_118_bss_324;
}

ftLucarioExtendParamAccesser g_ftLucarioExtendParamAccesser;

ftClassInfoImpl<Fighter_Lucario, ftLucario> g_ftClassInfoLucario;

ftLucario::ftLucario(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftLucarioBuildConfig>(entryId,
                                         Fighter_Lucario,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Lucario)) {
    // Register the status processes in action order; entries without a process stay empty.
    soStatusUniqProcess* processes[19] = {0};
    processes[0] = &lbl_27_bss_897C;
    processes[1] = &lbl_118_bss_254;
    processes[2] = &lbl_118_bss_274;
    processes[3] = &lbl_118_bss_2B4;
    processes[4] = &lbl_118_bss_2E4;
    processes[5] = &lbl_27_bss_897C;
    processes[6] = &lbl_27_bss_897C;
    processes[7] = &lbl_27_bss_897C;
    processes[8] = &lbl_118_bss_264;
    processes[9] = &lbl_118_bss_284;
    processes[10] = &lbl_118_bss_294;
    processes[11] = &lbl_118_bss_2A4;
    processes[12] = &lbl_118_bss_2C4;
    processes[13] = &lbl_118_bss_2D4;
    processes[15] = &lbl_118_bss_2F4;
    processes[16] = &lbl_118_bss_304;
    processes[17] = &lbl_118_bss_314;
    processes[18] = &lbl_118_bss_324;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 19);
    m_moduleAccesser->getCollisionShieldModule().add(static_cast<ftLucarioData*>(m_data)->m_shieldGroup, 1);
    getPhysicsModule(m_moduleAccesser).unk6C(0x13);
}

// BYTES: the native function addresses the local static's guard, object and destructor registration separately
// instead of from one pooled BSS base. The setting is per function: unit-wide it breaks the pooled constructor and
// __sinit, and an in-class inline definition makes the static weak and moves it to the end of .bss.
#pragma pool_data off
ftLucarioTransactor* ftLucarioTransactor::getInstance() {
    static ftLucarioTransactor instance;
    return &instance;
}
#pragma pool_data reset

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Lucario, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftLucarioArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftLucarioArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftLucarioArticleKind : wnArticleKindInfo {
    ftLucarioArticleKind() {
        kind = 0;
        subKind = Fighter_Lucario;
    }
    s32 getSubKind() const { return subKind; } // HYPOTHESIS: accessor spelling (the lookup reads the kind through a call, as in ft_ganon.cpp)
};

// The descriptor keeps the address of the kind temporary of the caller.
struct ftLucarioArticleBuildInfo : wnArticleBuildInfo {
    ftLucarioArticleBuildInfo(const wnArticleKindInfo& kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The articles are built from the data of the fighter kind.
template <>
inline wnArticleHolder<wnLucarioAuraBall>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(0, ftLucarioArticleBuildInfo(ftLucarioArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)),
               g_ftCommonDataAccesser.getWeaponData<wnLucarioAuraBallData>(ftLucarioArticleKind().getSubKind(), &ftLucarioArticleDataTag().m_resourceGroup)) { }
template <>
inline wnArticleHolder<wnLucarioQigong>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(1, ftLucarioArticleBuildInfo(ftLucarioArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)),
               g_ftCommonDataAccesser.getWeaponData<wnLucarioQigongData>(ftLucarioArticleKind().getSubKind(), &ftLucarioArticleDataTag().m_resourceGroup)) { }

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnLucarioAuraBall>::~wnArticleHolder();
template wnArticleHolder<wnLucarioQigong>::~wnArticleHolder();
template wnArticleHolder<wnLucarioAuraBall>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftLucarioArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftLucarioArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftLucarioArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftLucarioArticleMediator::~ftLucarioArticleMediator() { }
#pragma dont_inline off

s32 ftLucarioArticleMediator::getMediateNum() { return 2; }
void ftLucarioArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 AuraBall, 1 Qigong (the rest are unused).

#define FT_LUCARIO_POOL_AURABALL_BASE static_cast<wnArticlePool<wnLucarioAuraBall, 3, ftLucarioQigongPool>&>(m_pools)
#define FT_LUCARIO_POOL_AURABALL FT_LUCARIO_POOL_AURABALL_BASE.getSub()
#define FT_LUCARIO_POOL_QIGONG_BASE static_cast<wnArticlePool<wnLucarioQigong, 1, soInstancePoolRoot>&>(m_pools)
#define FT_LUCARIO_POOL_QIGONG FT_LUCARIO_POOL_QIGONG_BASE.getSub()

#define FT_LUCARIO_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(2, value) WN_ARTICLE_UNUSED_SLOT(3, value) WN_ARTICLE_UNUSED_SLOT(4, value) \
    WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) WN_ARTICLE_UNUSED_SLOT(7, value) \
    WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) WN_ARTICLE_UNUSED_SLOT(10, value) \
    WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) WN_ARTICLE_UNUSED_SLOT(13, value) \
    WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) WN_ARTICLE_UNUSED_SLOT(16, value)

void ftLucarioArticleMediator::deactivate() {
    for (s32 i = 0; i < 3; ++i) {
        wnLucarioAuraBall* weapon = FT_LUCARIO_POOL_AURABALL.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnLucarioQigong* weapon = FT_LUCARIO_POOL_QIGONG.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftLucarioArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(3, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    FT_LUCARIO_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftLucarioActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

s32 ftLucarioArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnLucarioAuraBall, 3, ftLucarioQigongPool>, ftLucarioActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotActiveNum<wnArticlePool<wnLucarioQigong, 1, soInstancePoolRoot>, ftLucarioActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    FT_LUCARIO_UNUSED_SLOTS(0)
    default: return 0;
    }
}

bool ftLucarioArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnLucarioAuraBall, 3, ftLucarioQigongPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotIsGeneratable<wnArticlePool<wnLucarioQigong, 1, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
    FT_LUCARIO_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftLucarioArticleActivator;
template <> struct ftLucarioArticleActivator<wnLucarioAuraBall> {
    static bool activate(wnLucarioAuraBall* weapon, soModuleAccesser* acc) { return ftLucarioTransactor::getInstance()->activeArticle1(weapon, acc); }
};
template <> struct ftLucarioArticleActivator<wnLucarioQigong> {
    static bool activate(wnLucarioQigong* weapon, soModuleAccesser* acc) { return ftLucario::activeArticle(weapon, acc); }
};

soArticle* ftLucarioArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<ftLucarioArticleActivator<wnLucarioAuraBall> >(static_cast<wnArticlePool<wnLucarioAuraBall, 3, ftLucarioQigongPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerate<ftLucarioArticleActivator<wnLucarioQigong> >(static_cast<wnArticlePool<wnLucarioQigong, 1, soInstancePoolRoot>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    FT_LUCARIO_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

template <int S, int L>
inline bool ftLucarioShootSlot0(soModuleAccesser* acc, soArticle* article, const soIntToType<S>&, const soIntToType<L>&) {
    return ftLucarioTransactor::getInstance()->shootArticle(&dynamic_cast<wnLucarioAuraBall&>(*article), acc);
}

template <int S, int L>
inline bool ftLucarioShootSlot1(soArticle* article, const soIntToType<S>&, const soIntToType<L>&) {
    (void)dynamic_cast<wnLucarioQigong&>(*article);
    return false;
}

bool ftLucarioArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return ftLucarioShootSlot0(acc, article, WN_ARTICLE_SLOT_TAGS);
    case 1: return ftLucarioShootSlot1(article, WN_ARTICLE_SLOT_TAGS);
    FT_LUCARIO_UNUSED_SLOTS(true)
    default: return false;
    }
}


// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {


}

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

// HYPOTHESIS: the work manage module maps the id to an effect handle, which the effect module then shows or hides.
void soModuleAccesser::set_visible(unsigned long id, bool visible) {
    soWorkManageModule& workManageModule = getWorkManageModule();
    soEffectModule& effectModule = getEffectModule();
    int handle = workManageModule.getInt(id);
    if (handle != 0) {
        effectModule.setVisible(handle, visible);
    }
}

void ftLucario::effect_suspend() {
    soModuleAccesser* acc = m_moduleAccesser;
    acc->set_visible(0x10000047, false);
    acc->set_visible(0x10000048, false);
    acc->set_visible(0x10000043, false);
    acc->set_visible(0x10000044, false);
}

void ftLucario::effect_resume() {
    soModuleAccesser* acc = m_moduleAccesser;
    acc->set_visible(0x10000047, true);
    acc->set_visible(0x10000048, true);
    acc->set_visible(0x10000043, true);
    acc->set_visible(0x10000044, true);
}

// Placeholders for weak inline methods of library classes (their names exist in sora_melee, so the REL copies cannot be renamed): the shim calls the inline method.
extern "C" {


}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
} // extern "C"

void ftLucario::setMachValidity(soModuleAccesser* moduleAccesser, bool valid) {
    ftLucario& fighter = dynamic_cast<ftLucario&>(moduleAccesser->getStageObject());
    fighter.m_moduleAccesser->getWorkManageModule().setFlag(valid, 0x1200003F);
}

void ftLucario::notifyEventOnDamage(soDamage* damage, bool isDamage, soModuleAccesser* moduleAccesser) {
    m_moduleAccesser->getWorkManageModule().setFlag(true, 0x1200003F);
    Fighter::notifyEventOnDamage(damage, isDamage, moduleAccesser);
}

void ftLucario::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) {
    if (kind != 0) {
        m_moduleAccesser->getWorkManageModule().setFlag(true, 0x1200003F);
    }
    Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser);
}

// BYTES: nw4r::math::FSelect is never expanded at the optimization level of the fighter units (-O2,s; it stays a call),
// so the two selects of the clamp are written with the instruction itself, as in the Pikachu units.
float clampf(register float value, register float min, register float max) {
    register float difference = value - min;
    asm { fsel value, difference, value, min }
    difference = value - max;
    asm { fsel value, difference, max, value }
    return value;
}

float ftLucario::calcAuraScale() {
    const float* parameters = static_cast<const float*>(m_data->extendParam[1]);
    float aura = m_moduleAccesser->getWorkManageModule().getFloat(0x11000013);
    float rate = clampf((aura - parameters[1]) / (parameters[3] - parameters[1]), 0.0f, 1.0f);
    float remaining = 1.0f - rate;
    return parameters[0] + (parameters[2] - parameters[0]) * (1.0f - remaining * remaining);
}

void ftLucario::notifyEventCollisionShield(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    if (groupIndex == 1) {
        moduleAccesser->getWorkManageModule().onFlag(0x22000010);
    }
    Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
}

void ftLucario::setAuraScale(float scale) {
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    soEffectModule& effects = m_moduleAccesser->getEffectModule();
    int firstEffect = work.getInt(0x10000047);
    int secondEffect = work.getInt(0x10000048);
    Vec3f auraScale(scale, scale, scale);
    effects.setScale(firstEffect, &auraScale);
    effects.setScale(secondEffect, &auraScale);
}

// The status processes whose pose helpers the node update calls.
class ftLucarioStatusUniqProcessSpecialHiRush {
public:
    static void set_pose(soModuleAccesser* moduleAccesser);
};
class ftLucarioStatusUniqProcessFinalHold {
public:
    static void set_rot_angle(soModuleAccesser* moduleAccesser);
};

void ftLucario::onActivate() { }

void ftLucario::onStart(int unk) {
    soModuleAccesser* acc = getModuleAccesser();
    soWorkManageModule& work = acc->getWorkManageModule();
    float rate = getAuraPowerRate(acc);
    work.setFloat(rate, 0x11000013);
    work.setFloat(rate, 0x11000014);
    setAuraScale(calcAuraScale());
    Fighter::onStart(unk);
    ftLucarioTransactor::getInstance()->init_auraball(acc);
    if (getOwner()->getSpycloak()) {
        m_moduleAccesser->getWorkManageModule().setInt(0, 0x10000047);
        m_moduleAccesser->getWorkManageModule().setInt(0, 0x10000048);
    } else {
        // The aura flames on both hands, scaled with the aura.
        soEffectModule& effects = m_moduleAccesser->getEffectModule();
        float scale = calcAuraScale();
        int firstEffect = effects.reqFollow(static_cast<EfID>(0x220001), 0x1f, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), scale,
                                            true, 0x10, 0, -1);
        int secondEffect = effects.reqFollow(static_cast<EfID>(0x220001), 0x35, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), scale,
                                             true, 0x10, 0, -1);
        m_moduleAccesser->getWorkManageModule().setInt(firstEffect, 0x10000047);
        m_moduleAccesser->getWorkManageModule().setInt(secondEffect, 0x10000048);
    }
    m_moduleAccesser->getEffectModule().removeCommon(0x1a);
    m_moduleAccesser->getWorkManageModule().setFlag(true, 0x1200003F);
}

// Rescales the aura flames when the aura power changed since the last frame.
inline void ftLucario::updateAuraScale(soModuleAccesser* moduleAccesser) {
    ftLucario& fighter = dynamic_cast<ftLucario&>(moduleAccesser->getStageObject());
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    float previous = work.getFloat(0x11000014);
    float current = work.getFloat(0x11000013);
    if (current != previous) {
        fighter.setAuraScale(fighter.calcAuraScale());
        work.setFloat(current, 0x11000014);
    }
}

void ftLucario::processUpdate() {
    Fighter::processUpdate();
    if (soSlow::getInstance()->isEstimate() == true) {
        soModuleAccesser* acc = getModuleAccesser();
        soWorkManageModule& work = acc->getWorkManageModule();
        work.setFloat(work.getFloat(0x11000013), 0x11000014);
        work.setFloat(getAuraPowerRate(acc), 0x11000013);
        updateAuraScale(acc);
        ftLucarioTransactor::getInstance()->processUpdate(acc);
    }
}

void ftLucario::updateNodeSRT() {
    soModuleAccesser* acc = m_moduleAccesser;
    switch (acc->getStatusModule().getStatusKind()) {
    case 0x11b:
        ftLucarioStatusUniqProcessSpecialHiRush::set_pose(acc);
        break;
    case 0x122:
    case 0x123:
    case 0x124:
        ftLucarioStatusUniqProcessFinalHold::set_rot_angle(acc);
        break;
    }
    float size = acc->getPostureModule().getScale();
    Vec3f scale;
    scale.m_z = size;
    scale.m_y = size;
    scale.m_x = size;
    Vec3f rotation;
    Vec3f position;
    position = acc->getPostureModule().getPos();
    rotation = acc->getPostureModule().getRot(0);
    rotation.m_y += acc->getPostureModule().getRotYLr();
    acc->getModelModule().setNodeSRT(0, &scale, &rotation, &position);
}

// HYPOTHESIS: the catch request (event 8) of a fighter Lucario grabs with the Force Palm (status 0x113): the receiver
// marks it done and answers with the node it is held by and Lucario's work int 0x10000033.
struct ftLucarioLinkEventCatch : soLinkEventArgs {
    int m_nodeId; // +0x8
    int m_request;
    int unk10;
    int unk14;
    int unk18;
};

void ftLucario::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    soStatusModule& status = moduleAccesser->getStatusModule();
    switch (eventInfo->m_eventKind) {
    case 8:
        if (status.getStatusKind() == 0x113) {
            ftLucarioLinkEventCatch* event = static_cast<ftLucarioLinkEventCatch*>(eventInfo);
            event->m_nodeId = moduleAccesser->getModelModule().getCorrectNodeId(0x12c);
            event->_0x4 = true;
            event->unk18 = moduleAccesser->getWorkManageModule().getInt(0x10000033);
            status.changeStatusRequest(0x11a, moduleAccesser);
            return;
        }
        break;
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}

float ftLucario::getAuraPowerRate(soModuleAccesser* moduleAccesser) {
    if (g_ftManager->m_isStamina) {
        return getAuraPowerRateHpMode(moduleAccesser);
    }
    return getAuraPowerRateNormal(moduleAccesser);
}

// The rate grows from parameters[0] to 1 up to the damage parameters[3] and on to parameters[1] at parameters[4].
float ftLucario::getAuraPowerRateNormal(soModuleAccesser* moduleAccesser) {
    const float* parameters = static_cast<const float*>(g_ftCommonDataAccesser.getData(Fighter_Lucario)->extendParam[2]);
    float damage = moduleAccesser->getDamageModule().getDamage(0);
    float rate;
    if (damage <= parameters[3]) {
        rate = parameters[0] +
               (1.0f - parameters[0]) * clampf((damage - parameters[2]) / (parameters[3] - parameters[2]), 0.0f, 1.0f);
    } else {
        rate = 1.0f + (parameters[1] - 1.0f) * clampf((damage - parameters[3]) / (parameters[4] - parameters[3]), 0.0f, 1.0f);
    }
    float handicap = clampf(soValueAccesser::getConstantFloat(moduleAccesser, 0xdb2, 0), -1.0f, 1.0f);
    return clampf(parameters[5] * (rate / (1.0f + 0.5f * handicap)), parameters[7], parameters[6]);
}

// The stamina match version: the rate falls with the remaining share of the hit points.
float ftLucario::getAuraPowerRateHpMode(soModuleAccesser* moduleAccesser) {
    Fighter& fighter = dynamic_cast<Fighter&>(moduleAccesser->getStageObject());
    const float* parameters = static_cast<const ftLucarioData*>(g_ftCommonDataAccesser.getData(Fighter_Lucario))->m_auraHpModeParam;
    float hitPointMax = fighter.getOwner()->getHitPointMax();
    float hitPoint = fighter.getOwner()->getHitPoint();
    float percent = clampf(100.0f * (static_cast<float>(fabs(hitPointMax)) > 1.1920929e-7f ? hitPoint / hitPointMax : 1.0f), 0.0f, 100.0f);
    float rate;
    if (hitPointMax <= 1.0f) {
        rate = parameters[1];
    } else if (percent <= parameters[3]) {
        rate = parameters[1] -
               (parameters[1] - 1.0f) * clampf((percent - parameters[4]) / (parameters[3] - parameters[4]), 0.0f, 1.0f);
    } else {
        rate = 1.0f - (1.0f - parameters[0]) * clampf((percent - parameters[3]) / (parameters[2] - parameters[3]), 0.0f, 1.0f);
    }
    float handicap = clampf(soValueAccesser::getConstantFloat(moduleAccesser, 0xdb2, 0), -1.0f, 1.0f);
    rate /= 1.0f + 0.5f * handicap;
    return clampf(rate, parameters[6], parameters[5]);
}

// The Aura Sphere of the Force Palm starts in front of the fighter (the offsets differ on the ground and in the air)
// with the current aura power.
bool ftLucario::activeArticle(wnLucarioQigong* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soPostureModule& posture = acc->getPostureModule();
    float lr = posture.getLr();
    Vec3f pos = acc->getPostureModule().getPos();
    Vec3f offset(0.0f, 0.0f, 0.0f);
    if (acc->getSituationModule().getKind() == 0) {
        offset.m_x = 0.0f;
        offset.m_y = soValueAccesser::getConstantFloat(acc, 0xfb5, 0);
        offset.m_z = soValueAccesser::getConstantFloat(acc, 0xfb4, 0);
    } else {
        offset.m_x = 0.0f;
        offset.m_y = soValueAccesser::getConstantFloat(acc, 0xfb7, 0);
        offset.m_z = soValueAccesser::getConstantFloat(acc, 0xfb6, 0);
    }
    Vec3fScale(&offset, &offset, posture.getScale());
    pos.m_x += offset.m_z * lr;
    pos.m_y += offset.m_y;
    float powerMul = acc->getWorkManageModule().getFloat(0x11000013);
    int founderTaskId = fighter.getTaskId();
    weapon->activate(founderTaskId, fighter.getOwner()->getTeam(), lr, &pos, powerMul);
    return false;
}

#pragma dont_inline on
soTeam::~soTeam() { }
soResourceIdAccesser::~soResourceIdAccesser() { }
#pragma dont_inline off
soArticleMediator::~soArticleMediator() { }
#pragma dont_inline on
ftVirtualNodeMatrixPool::~ftVirtualNodeMatrixPool() { }
soParamAccesser::~soParamAccesser() { }
#pragma dont_inline off
