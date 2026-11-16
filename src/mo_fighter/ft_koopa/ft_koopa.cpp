#define FT_POOL_ROOT_NONPOLYMORPHIC
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_SOUND_GENERATOR_DTOR_NOINLINE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_KOOPA_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
#define WN_KINETIC_MEDIATOR_INLINE_DTOR
#define SO_KINETIC_ROTATION_FIELDWISE_ZERO // BYTES: the module's copy of soKineticEnergyNormal::getRotation writes three float zeros

#include <ft/ft_event_unit_instance.h> // before the event system headers (ft/ft_manager.h is included below)
#include <ft/builder/ft_dol_array_list.h>
// The 291-entry process vector is a whole class of sora_melee (the anim cmd one is declared in the list header).
FT_DOL_ARRAY_VECTOR(soStatusUniqProcess*, 291);
#include <ft/ft_class_info_impl.h>
#include <ft/koopa/ft_koopa.h>
#include <ft/koopa/ft_koopa_extend_param_accesser.h>
#include <ft/koopa/ft_koopa_transactor.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <ft/koopa/ft_koopa_special_s.h>
#include <ft/ft_change_succeed_work.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>

// The articles' sound builder teardown inlines this empty destructor (same form as ft_pit.cpp and ft_snake.cpp).
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }

#undef WN_ARTICLE_NATIVE_BUILDERS
#define FT_BC ftKoopaBuildConfig
#include <ft/builder/ft_builder_noinline.h>

void ftKoopa::change(void* work, int startKind) {
    onStart(startKind);
    Fighter::changeSucceedCore(work, 0x122);
}

#include <so/so_slow.h>

// Clamps value to [minimum, maximum] with two floating selects (the link map lists it as this file's CLAMP<float>;
// Giga Bowser's file calls this copy). HYPOTHESIS: written as an asm block, as Game & Watch's clampf: nw4r's FSelect is
// not expanded at the fighter units' optimization level and the __fsel intrinsic adds two frsp.
template <class T> T (CLAMP)(register T value, register T minimum, register T maximum) {
    register T lower;
    register T result;
    asm {
        fsubs lower, value, minimum
        fsel lower, lower, value, minimum
        fsubs result, lower, maximum
        fsel result, result, maximum, lower
    }
    return result;
}

void ftKoopa::processUpdate() {
    Fighter::processUpdate();
    if (soSlow::getInstance()->isEstimate() == true && !m_moduleAccesser->getStopModule().isStop()) {
        float first = m_moduleAccesser->getWorkManageModule().getFloat(0x11000014);
        float second = m_moduleAccesser->getWorkManageModule().getFloat(0x11000013);
        if (m_moduleAccesser->getStatusModule().getStatusKind() != 0x112) {
            first += soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfaa, 0);
            second += soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfab, 0);
        } else if (m_moduleAccesser->getWorkManageModule().getInt(0x20000002) == 1) {
            first -= soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfac, 0);
            second -= soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfad, 0);
        }
        float firstMinimum = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfa7, 0);
        float firstMaximum = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfa6, 0);
        first = CLAMP<float>(first, firstMinimum, firstMaximum);
        m_moduleAccesser->getWorkManageModule().setFloat(first, 0x11000014);
        float secondMinimum = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfa9, 0);
        float secondMaximum = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfa8, 0);
        second = CLAMP<float>(second, secondMinimum, secondMaximum);
        m_moduleAccesser->getWorkManageModule().setFloat(second, 0x11000013);
    }
}

bool ftKoopa::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    s8 group = acmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    switch (acmd->getType()) {
    case 5:
        // HYPOTHESIS: command 5 switches to Giga Bowser (the transformation of the final smash).
        if (static_cast<u8>(g_ftManager->getFighterCount(m_entryId)) > 1) {
            int fighterNo = g_ftManager->getFighterNo(m_entryId, m_taskId);
            u8 other = (u8)fighterNo == 0 ? 1 : 0;
            g_ftManager->toChange(m_entryId, other, true, true);
        }
        return true;
    }
    return Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
}

// HYPOTHESIS: the events of the Flying Slam: 8 with the catch request 0xee grabs the victim during the start statuses
// (0x11c/0x11d), 0xe releases it from the slam statuses (0x11e-0x121).
struct ftKoopaLinkEventView {
    int kind;
    bool done;
    int value;
    int request;
};

void ftKoopa::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    ftKoopaLinkEventView* event = reinterpret_cast<ftKoopaLinkEventView*>(eventInfo);
    switch (event->kind) {
    case 8:
        if (event->request == 0xee) {
            switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
            case 0x11c:
            case 0x11d:
                event->value = m_moduleAccesser->getModelModule().getCorrectNodeId(0x12c);
                event->done = true;
                m_moduleAccesser->getStatusModule().changeStatusRequest(0x11e, m_moduleAccesser);
                m_moduleAccesser->getWorkManageModule().setInt(m_moduleAccesser->getLinkModule().getNodeTaskId(0), 0x20000002);
                m_moduleAccesser->getWorkManageModule().onFlag(0x22000011);
                return;
            }
        }
        break;
    case 0xe:
        switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
        case 0x11e:
        case 0x11f:
        case 0x120:
        case 0x121:
            if (m_moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
                getCatchView(m_moduleAccesser).unk38(0);
                m_moduleAccesser->getWorkManageModule().offFlag(0x22000011);
                m_moduleAccesser->getWorkManageModule().offFlag(0x12000001);
                if (m_moduleAccesser->getStatusModule().getStatusKind() != 0x121 ||
                    m_moduleAccesser->getWorkManageModule().isFlag(0x22000010)) {
                    if (m_moduleAccesser->getWorkManageModule().isFlag(0x1200003e)) {
                        m_moduleAccesser->getWorkManageModule().offFlag(0x1200003e);
                    } else if (m_moduleAccesser->getSituationModule().getKind() == 0) {
                        m_moduleAccesser->getStatusModule().changeStatusRequest(0, m_moduleAccesser);
                    } else {
                        m_moduleAccesser->getStatusModule().changeStatusRequest(0xe, m_moduleAccesser);
                    }
                }
            }
            return;
        }
        break;
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

ftKineticEnergyController::~ftKineticEnergyController() { }

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt).
extern "C" {
extern soStatusUniqProcess lbl_27_bss_7D74;
extern soStatusUniqProcess lbl_102_bss_22C;
extern soStatusUniqProcess lbl_102_bss_23C;
extern soStatusUniqProcess lbl_102_bss_25C;
extern soStatusUniqProcess lbl_102_bss_24C;
extern soStatusUniqProcess lbl_102_bss_26C;
extern soStatusUniqProcess lbl_102_bss_27C;
extern soStatusUniqProcess lbl_102_bss_28C;
extern soStatusUniqProcess lbl_102_bss_29C;
extern soStatusUniqProcess lbl_102_bss_2AC;
}

ftKoopaExtendParamAccesser g_ftKoopaExtendParamAccesser;

ftClassInfoImpl<Fighter_Koopa, ftKoopa> g_ftClassInfoKoopa;

ftKoopa::ftKoopa(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftKoopaBuildConfig>(entryId,
                                         Fighter_Koopa,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Koopa)) {
    // Register the status processes in action order.
    soStatusUniqProcess* processes[17] = {0};
    processes[0] = &lbl_27_bss_7D74;
    processes[5] = &lbl_102_bss_22C;
    processes[6] = &lbl_102_bss_23C;
    processes[7] = &lbl_102_bss_25C;
    processes[8] = &lbl_102_bss_24C;
    processes[10] = &lbl_102_bss_26C;
    processes[11] = &lbl_102_bss_26C;
    processes[12] = &lbl_102_bss_27C;
    processes[13] = &lbl_102_bss_28C;
    processes[14] = &lbl_102_bss_29C;
    processes[15] = &lbl_102_bss_2AC;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 17);
}

// ---- the fighter overrides ------------------------------------------------------------------------------------------
// HYPOTHESIS: the works 0x10000040 / 0x11000013 / 0x11000014 hold the state of the breath (its charge and two parameters of
// the fighter), reset on every start together with the pending-change flags 0x1200003d / 0x1200003e.
void ftKoopa::onStart(int startKind) {
    m_moduleAccesser->getWorkManageModule().setInt(-1, 0x10000040);
    float first = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfa6, 0);
    m_moduleAccesser->getWorkManageModule().setFloat(first, 0x11000014);
    float second = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfa8, 0);
    m_moduleAccesser->getWorkManageModule().setFloat(second, 0x11000013);
    m_moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
    m_moduleAccesser->getWorkManageModule().offFlag(0x1200003e);
    Fighter::onStart(startKind);
}

// The breath starts through the transactor of the fighter.
// BYTES: the native function addresses the local static's guard, object and destructor registration separately
// instead of from one pooled BSS base. The setting is per function: unit-wide it breaks the pooled constructor and
// __sinit, and an in-class inline definition makes the static weak and moves it to the end of .bss.
#pragma pool_data off
bool ftKoopa::activeArticle(wnKoopaBreath* weapon, soModuleAccesser* acc) {
    static ftKoopaTransactor instance;
    return instance.activeArticle(weapon, acc);
}
#pragma pool_data reset

// The kinetic settings of the dummy fighter that takes over the move while Bowser transforms.
void ftKoopa::setupChangeSucceedWork(void* work) {
    Fighter::setupChangeSucceedWork(work);
    soModuleAccesser* acc = m_moduleAccesser;
    wnDummyFighterKineticSettingData hurt;
    hurt.point[0][0] = 0.0f;
    hurt.point[1][0] = soValueAccesser::getConstantFloat(acc, 0xbd6, 0);
    hurt.point[2][0] = 0.0f;
    hurt.point[3][0] = soValueAccesser::getConstantFloat(acc, 0xbd8, 0);
    hurt.point[0][1] = 0.0f;
    hurt.point[1][1] = soValueAccesser::getConstantFloat(acc, 0xbd1, 0);
    hurt.point[2][1] = 0.0f;
    hurt.point[3][1] = 0.0f;
    wnDummyFighterKineticSettingData hit;
    hit.point[0][0] = 0.0f;
    hit.point[1][0] = soValueAccesser::getConstantFloat(acc, 0xbbc, 0);
    hit.point[2][0] = 0.0f;
    hit.point[3][0] = -1.0f;
    hit.point[0][1] = 0.0f;
    hit.point[1][1] = 0.0f;
    hit.point[2][1] = 0.0f;
    hit.point[3][1] = 0.0f;
    Vec2f* speed = &acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(0xFFFF));
    soPostureModule* const posture = &acc->getPostureModule();
    static_cast<ftChangeSucceedWork*>(work)->activateDummyFighter(&posture->getPos(), posture->getScale(), posture->getLr(), speed, &hit.point[0][0], &hurt.point[0][0], 0xc000b);
}

// BYTES: defined after its first user (see ft/ft_change_succeed_work.h).
inline wnDummyFighterKineticSettingData::wnDummyFighterKineticSettingData() { }

// ---- the article holders --------------------------------------------------------------------------------------------
#include <ft/builder/ft_builder_article.h>

struct ftKoopaArticleKind : wnArticleKindInfo {
    ftKind getSubKind() const { return static_cast<ftKind>(subKind); }
    ftKoopaArticleKind() {
        kind = 0;
        subKind = Fighter_Koopa;
    }
};

// The data block of the breath comes from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_1496F0(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

FT_DEFINE_ARTICLE_HOLDER(, wnKoopaBreath, wnKoopaBreathData, 0, ftKoopaArticleKind, fn_27_1496F0)

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnKoopaBreath>::~wnArticleHolder();
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftKoopaArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftKoopaArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftKoopaArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftKoopaArticleMediator::~ftKoopaArticleMediator() { }
#pragma dont_inline off

s32 ftKoopaArticleMediator::getMediateNum() { return 1; }
void ftKoopaArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Breath (the rest are unused).

#define FT_KOOPA_POOL_BREATH_BASE static_cast<wnArticlePool<wnKoopaBreath, 6, soInstancePoolRoot>&>(m_pools)
#define FT_KOOPA_POOL_BREATH FT_KOOPA_POOL_BREATH_BASE.getSub()

#define FT_KOOPA_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(1, value) WN_ARTICLE_UNUSED_SLOT(2, value) WN_ARTICLE_UNUSED_SLOT(3, value) \
    WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

void ftKoopaArticleMediator::deactivate() {
    for (s32 i = 0; i < 6; ++i) {
        wnKoopaBreath* weapon = FT_KOOPA_POOL_BREATH.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftKoopaArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(6, WN_ARTICLE_SLOT_TAGS);
    FT_KOOPA_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftKoopaActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

s32 ftKoopaArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnKoopaBreath, 6, soInstancePoolRoot>, ftKoopaActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    FT_KOOPA_UNUSED_SLOTS(0)
    default: return 0;
    }
}

bool ftKoopaArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnKoopaBreath, 6, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
    FT_KOOPA_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftKoopaArticleActivator;
template <> struct ftKoopaArticleActivator<wnKoopaBreath> {
    static bool activate(wnKoopaBreath* weapon, soModuleAccesser* acc) { return ftKoopa::activeArticle(weapon, acc); }
};

soArticle* ftKoopaArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<ftKoopaArticleActivator<wnKoopaBreath> >(static_cast<wnArticlePool<wnKoopaBreath, 6, soInstancePoolRoot>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    FT_KOOPA_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftKoopaArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnKoopaBreath>(article, WN_ARTICLE_SLOT_TAGS);
    FT_KOOPA_UNUSED_SLOTS(true)
    default: return false;
    }
}

// The module's own copies of these empty destructors (the first unit of the module emits them).
#pragma dont_inline on
soTeam::~soTeam() { }
soResourceIdAccesser::~soResourceIdAccesser() { }
#pragma dont_inline off
soArticleMediator::~soArticleMediator() { }
#pragma dont_inline on
ftVirtualNodeMatrixPool::~ftVirtualNodeMatrixPool() { }
#pragma dont_inline off
