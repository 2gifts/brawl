#define WN_WEAPON_BUILDER // The fighter owns the complete Mantle article type.
#define WN_PARAM_CUSTOMIZE_INLINE_DTOR
#define WN_KINETIC_MEDIATOR_INLINE_DTOR
#define SO_KINETIC_ROTATION_FIELDWISE_ZERO
#define FT_COLLISION_ATTACK_FIGHTER_POWER_ARG
#define FT_EFFECT_SCREEN_SINGLETON_HOLDER
#define FT_SOUND_GENERATOR_DTOR_NOINLINE
#define FT_POOL_ROOT_NONPOLYMORPHIC
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_METAKNIGHT_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
#include <so/event/so_event_system.h>
class soStatusEventObserver;
class soAnimCmdEventObserver;
class soLinkEventObserver;
class soArticleEventObserver;
class soModelEventObserver;

// These Mantle observer units use the shared sora_melee constructor and teardown.
#define FT_METAKNIGHT_SHARED_EVENT_UNIT(Observer, Capacity) \
    template <> class soEventUnitWithWorkArea<Observer, Capacity> : public soEventUnitImpl<Observer> { \
        soInstanceManagerFullPropertyVector<Observer*, Capacity> m_observerList; \
    public: \
        soEventUnitWithWorkArea(s16 managerId, s16 eventId); \
        virtual ~soEventUnitWithWorkArea(); \
        virtual u32 getObserverNum() const { return m_observerList.size(); } \
    };
FT_METAKNIGHT_SHARED_EVENT_UNIT(soStatusEventObserver, 17)
FT_METAKNIGHT_SHARED_EVENT_UNIT(soAnimCmdEventObserver, 33)
FT_METAKNIGHT_SHARED_EVENT_UNIT(soLinkEventObserver, 2)
FT_METAKNIGHT_SHARED_EVENT_UNIT(soArticleEventObserver, 2)
FT_METAKNIGHT_SHARED_EVENT_UNIT(soModelEventObserver, 5)
#undef FT_METAKNIGHT_SHARED_EVENT_UNIT

#include <ft/builder/ft_dol_array_list.h>
// The six absolute-attack slots use the shared sora_melee vector implementation.
FT_DOL_ARRAY_VECTOR(soCollisionAttackAbsolute, 6);
#include <so/sound/so_sound_3d_generator_accesser_impl.h>
#include <so/so_value_accesser.h>
// Empty article interfaces have no resources to release in this unit.
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soParamAccesser::~soParamAccesser() { }
// Native mediator calls go directly to the fighter's typed update templates.
#include <ft/metaknight/ft_metaknight_kinetic_transactor.h>
#define FT_KINETIC_MEDIATOR_TRANSACTOR ftMetaknightKineticTransactor
#define FT_KINETIC_MEDIATOR_DIRECT_UPDATES
#include <ft/ft_class_info_impl.h>
#include <ft/metaknight/ft_metaknight.h>
#undef FT_KINETIC_MEDIATOR_DIRECT_UPDATES
#undef FT_KINETIC_MEDIATOR_TRANSACTOR
#include <ft/metaknight/ft_metaknight_status_uniq_process_final_end.h>
#include <gf/gf_task_scheduler.h>
#include <so/collision/so_collision_log.h>
#include <so/so_external_value_accesser.h>
#include <ft/metaknight/ft_metaknight_extend_param_accesser.h>
#include <ft/ft_owner.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/article/so_article_mediator.h>

#undef WN_ARTICLE_NATIVE_BUILDERS // Fighter kinetic teardown remains out of line.
#define FT_BC ftMetaknightBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

#pragma dont_inline on
soTeam::~soTeam() { }
soResourceIdAccesser::~soResourceIdAccesser() { }
#pragma dont_inline off
ftKineticEnergyController::~ftKineticEnergyController() { }
soArticleMediator::~soArticleMediator() { }

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt).
extern "C" {
extern soStatusUniqProcess lbl_27_bss_80A4;
extern soStatusUniqProcess lbl_111_bss_184;
extern soStatusUniqProcess lbl_111_bss_194;
extern soStatusUniqProcess lbl_111_bss_1C4;
extern soStatusUniqProcess lbl_111_bss_1B4;
extern soStatusUniqProcess lbl_111_bss_1A4;
}

ftMetaknightExtendParamAccesser g_ftMetaknightExtendParamAccesser;

ftClassInfoImpl<Fighter_MetaKnight, ftMetaknight> g_ftClassInfoMetaknight;

ftMetaknight::ftMetaknight(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftMetaknightBuildConfig>(entryId,
                                         Fighter_MetaKnight,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_MetaKnight)) {
    // Register the status processes in action order.
    soStatusUniqProcess* processes[16] = {0};
    processes[5] = &lbl_27_bss_80A4;
    processes[7] = &lbl_111_bss_184;
    processes[8] = &lbl_111_bss_194;
    processes[13] = &lbl_111_bss_1C4;
    processes[14] = &lbl_111_bss_1B4;
    processes[15] = &lbl_111_bss_1A4;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 16);
}

ftMetaknight::~ftMetaknight() { }

// ---- the fighter overrides ------------------------------------------------------------------------------------------
void ftMetaknight::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: the flag 0x1200003d marks a pending fighter change, cleared whenever the situation is not the air.
    if (kind != 2) {
        moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
    }
    // HYPOTHESIS: status 0x9b hides record 2 on the ground and shows it again in the air.
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x9b:
        if (kind == 0) {
            moduleAccesser->getVisibilityModule().setStatusDefault(2, 0, true);
        } else if (kind == 2) {
            moduleAccesser->getVisibilityModule().setStatusDefault(2, 1, true);
        }
        break;
    }
    Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser);
}

// Statuses outside the listed ones reset the visibility record 2 when it is shown.
void ftMetaknight::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    switch (statusKind) {
    case 0x1a: case 0x1b: case 0x1c:
    case 0x1e: case 0x1f: case 0x20:
    case 0x10c: case 0x10e: case 0x10f: case 0x115:
    case 0x11b: case 0x11c: case 0x11f: case 0x121:
        break;
    default:
        if (moduleAccesser->getVisibilityModule().get(2) == 2) {
            moduleAccesser->getVisibilityModule().setStatusDefault(2, 0, true);
        }
        break;
    }
}

// HYPOTHESIS: the Final Smash end status keeps the hit state; its entry points are in the status process unit.

bool ftMetaknight::notifyEventCollisionAttackCheck(u32 flags) {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x116) {
        g_ftMetaknightStatusUniqProcessFinalEnd.notifyEventCollisionAttackCheck(m_moduleAccesser);
    }
    return Fighter::notifyEventCollisionAttackCheck(flags);
}


// The first successful Final Smash hit stops the two external movement energies,
// starts the capture rumble, and hands the struck fighter to the link process.
void ftMetaknight::notifyEventCollisionAttackFighter(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    if (collisionLog != NULL && m_moduleAccesser->getStatusModule().getStatusKind() == 0x116) {
        StageObject* target = dynamic_cast<StageObject*>(
            gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Fighter, collisionLog->m_taskId));
        if (target != NULL && soExternalValueAccesser::getCollisionHitModule(target)->getStatus(
                collisionLog->m_collsionIndex, collisionLog->_35) == 0) {
            if (!moduleAccesser->getWorkManageModule().isFlag(0x22000010)) {
                moduleAccesser->getWorkManageModule().onFlag(0x22000010);
                moduleAccesser->getKineticModule().getEnergy(5)->disable();
                moduleAccesser->getKineticModule().getEnergy(6)->disable();
                moduleAccesser->getControllerModule().setRumble(12, 0, false, -1);
            }
            g_ftMetaknightStatusUniqProcessFinalEnd.notifyEventCollisionAttackFighter(power, collisionLog, moduleAccesser);
        }
    }
}

// These two motions fix the ground collision rhombus while they play.
void ftMetaknight::notifyEventChangeMotion(int motionKind, int, void*, soModuleAccesser* moduleAccesser) {
    if (motionKind >= 0x1be) {
        return;
    }
    if (motionKind < 0x1bc) {
        return;
    }
    return moduleAccesser->getGroundModule().setShapeFlag(soGroundShapeImpl::Shape_Rhombus_Modify_Flag_Fix, true, 0);
}

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of the article is given: the kind record of the owner (kind 0, sub kind Metaknight, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftMetaknightArticleKind : wnArticleKindInfo {
    ftKind getSubKind() const { return static_cast<ftKind>(subKind); }
    ftMetaknightArticleKind() {
        kind = 0;
        subKind = Fighter_MetaKnight;
    }
};

#include <ft/builder/ft_builder_article.h>
FT_DEFINE_ARTICLE_HOLDER_MEMBER(inline, wnMetaknightMantle, wnMetaknightMantleData, 0, ftMetaknightArticleKind, getWeaponData128)

// ---- the article of Meta Knight and its mediator --------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnMetaknightMantle>::~wnArticleHolder();
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftMetaknightArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftMetaknightArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftMetaknightArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftMetaknightArticleMediator::~ftMetaknightArticleMediator() { }
#pragma dont_inline off

s32 ftMetaknightArticleMediator::getMediateNum() { return 1; }
void ftMetaknightArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Mantle (the rest are unused).

#define FT_METAKNIGHT_POOL_MANTLE_BASE static_cast<wnArticlePool<wnMetaknightMantle, 1, soInstancePoolRoot>&>(m_pools)
#define FT_METAKNIGHT_POOL_MANTLE FT_METAKNIGHT_POOL_MANTLE_BASE.getSub()

#define FT_METAKNIGHT_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(1, value) WN_ARTICLE_UNUSED_SLOT(2, value) WN_ARTICLE_UNUSED_SLOT(3, value) \
    WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

// The Mantle starts at the stage object of the fighter with its team and the model and texture resources.
bool ftMetaknight::activeArticle(wnMetaknightMantle* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    int founderTaskId = acc->getStageObject().m_taskId;
    soResourceModule& resources = acc->getResourceModule();
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getMdlResId(), resources.getResourceIdAccesser()->getTexResId(), fighter.getOwner()->getTeam());
    return true;
}

void ftMetaknightArticleMediator::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnMetaknightMantle* weapon = FT_METAKNIGHT_POOL_MANTLE.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftMetaknightArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    FT_METAKNIGHT_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftMetaknightActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

s32 ftMetaknightArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnMetaknightMantle, 1, soInstancePoolRoot>, ftMetaknightActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    FT_METAKNIGHT_UNUSED_SLOTS(0)
    default: return 0;
    }
}

bool ftMetaknightArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnMetaknightMantle, 1, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
    FT_METAKNIGHT_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftMetaknightArticleActivator;
template <> struct ftMetaknightArticleActivator<wnMetaknightMantle> {
    static bool activate(wnMetaknightMantle* weapon, soModuleAccesser* acc) { return ftMetaknight::activeArticle(weapon, acc); }
};

soArticle* ftMetaknightArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<ftMetaknightArticleActivator<wnMetaknightMantle> >(static_cast<wnArticlePool<wnMetaknightMantle, 1, soInstancePoolRoot>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    FT_METAKNIGHT_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

// Shooting only type-checks the article.
bool ftMetaknightArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnMetaknightMantle>(article, WN_ARTICLE_SLOT_TAGS);
    FT_METAKNIGHT_UNUSED_SLOTS(true)
    default: return false;
    }
}

// Virtual-slot view used only to emit virtual tail calls; the slots are never defined in this unit.
struct ftMetaknightVirtualSlots {
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
};

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

// soValueAccesser::getValueVariation (same global as ft_marth.cpp)
extern int g_soValueVariation;
int fn_111_8CC8() { return g_soValueVariation; }

// soArticle::checkActivate: virtual slot 20 tail call.
void fn_111_9874(void* p) { return ((ftMetaknightVirtualSlots*)p)->slot20(); }

u8 fn_111_5F38(u8* p) { return *(u8*)(p + 0x4); }
void fn_111_8650() {}
void fn_111_8654() {}
void fn_111_8658() {}
int fn_111_87CC(u8* p) { return *(int*)(p + 0x20); }
int fn_111_88A8(u8* p) { return *(int*)(p + 0x18); }
void fn_111_A5EC() {}
void fn_111_AE98() {}

}

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
u8* fn_111_BB28(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
} // extern "C"
