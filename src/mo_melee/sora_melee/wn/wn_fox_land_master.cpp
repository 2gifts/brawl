#define SO_STATUS_KINETIC_SLOT
#define WN_FOX_LAND_MASTER_SHELL_FULL
#define FT_MODULE_BUILDER
#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the article destructor expands the empty soParamAccesser base destructor
#define WN_FOX_LAND_MASTER_UNIT
#define WN_ANIMCMD_UNIT5_SELECT
#define WN_WEAPON_NO_COMMON_ENERGIES
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
#include <wn/wn_module_accesser_builder.h>
#include <wn/fox/wn_fox_blaster.h>
#include <wn/fox/wn_fox_land_master.h>
#include <wn/wn_extend_param_accesser.h>
#include <so/so_module_accesser.h>
#include <so/link/so_link_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/stop/so_stop_module_impl.h>
#include <so/damage/so_damage.h>
#include <so/damage/so_damage_util_actor.h>
#include <so/so_value_accesser.h>
#include <so/so_module_hyp.h>
#include <mt/mt_vector.h>
#include <so/so_slow.h>
#include <so/status/so_status_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <ft/ft_entry.h>
#include <wn/wn_activate_desc.h>
#include <so/controller/so_controller_module_impl.h>
#include <types.h>



// BYTES: the article destructor expands the damage observer base (it deregisters from the event manager).
static inline void removeDamageObserver(soEventObserver<soDamageEventObserver>* observer, s16 manageId) {
    if (soEventSystem::getInstance()->getInstanceManager().isContain(manageId) == true) {
        soEventSystem::getInstance()->getManager(observer->m_manageID)->eraseObserver(observer->m_unitID, observer->m_sendID);
    }
    observer->m_sendID = -1;
    observer->m_manageID = -1;
}
inline soEventObserver<soDamageEventObserver>::~soEventObserver() {
    removeDamageObserver(this, m_manageID);
}
inline soDamageEventObserver::~soDamageEventObserver() { }

// BYTES: the builder destructors expand these empty base destructors to nothing.
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }
inline soGeneralWorkAbstract::~soGeneralWorkAbstract() { }
inline soControllerModuleLinkRef::~soControllerModuleLinkRef() { }

// BYTES: the anim command unit builders' destructors expand these empty member destructors to nothing.
template <>
inline soArrayVector<acCmdInterpreterStackData, 8>::~soArrayVector() { }
template <>
inline soArrayContractibleTable<const acAnimCmdConv*>::~soArrayContractibleTable() { }

// BYTES: the kinetic energy holders' destructors expand the empty base destructor of the energies.
inline soKineticEnergy::~soKineticEnergy() { }

// The module builders inline these vector constructors. The DOL specializations only declare them, so the bodies
// live here and stay inline: an out-of-line copy would duplicate the unit that owns the symbol.
#define WN_VEC_FLL(T, C)                                                                                               \
    inline __attribute__((always_inline)) soArrayVector<T, C>::soArrayVector(s32 size, s32)                           \
        : m_topIndex(0), m_lastIndex(0), m_size(size), m_isFull(false) {                                              \
        soArrayVectorCalculator::postInitialize(*this, size, C);                                                      \
    }
#define WN_VEC_FL_EMPTY(T, C)                                                                                          \
    inline __attribute__((always_inline)) soArrayVector<T, C>::soArrayVector(s32)                                     \
        : m_topIndex(0), m_lastIndex(0), m_size(0), m_isFull(false) { }
#define WN_VEC_FILL(T, C)                                                                                              \
    inline __attribute__((always_inline)) soArrayVector<T, C>::soArrayVector(s32 size, const T& element, s32)         \
        : m_topIndex(0), m_lastIndex(0), m_size(0), m_isFull(false) {                                                 \
        size = soArrayVectorCalculator::resize(*this, size, this->isEmpty(), this->isFull(), this->capacity(),       \
                                                this->getTopIndex(), this->getLastIndex());                           \
        for (s32 i = 0; i < size; i++) {                                                                              \
            this->push(element);                                                                                      \
        }                                                                                                             \
    }

WN_VEC_FLL(soModelNodeSetUp, 5)
WN_VEC_FLL(soGroundShapeImpl, 1)
WN_VEC_FLL(soCollisionGroup, 1)
WN_VEC_FLL(soCollisionHitGroup, 1)
WN_VEC_FLL(soEffectContinual, 1)
WN_VEC_FLL(efScreenHandle, 1)
WN_VEC_FLL(soOtherAnim, 1)

WN_VEC_FL_EMPTY(soTransitionTermGroup, 1)
WN_VEC_FL_EMPTY(soArticle*, 2)
WN_VEC_FL_EMPTY(soStatusUniqProcess*, 10)

typedef soInstanceUnitFullProperty<soTransitionTerm> wnFoxLandMasterTransitionTermUnit;
typedef soInstanceUnitFullProperty<soKineticEnergy*> wnFoxLandMasterKineticEnergyUnit;
WN_VEC_FL_EMPTY(wnFoxLandMasterTransitionTermUnit, 4)
WN_VEC_FL_EMPTY(wnFoxLandMasterTransitionTermUnit, 8)
WN_VEC_FL_EMPTY(wnFoxLandMasterKineticEnergyUnit, 12)
WN_VEC_FL_EMPTY(acCmdInterpreterStackData, 8)

WN_VEC_FILL(soCollisionAttackPart, 4)
WN_VEC_FILL(soCollisionHitPart, 4)
WN_VEC_FILL(u32, 1)
WN_VEC_FILL(s32, 1)

inline __attribute__((always_inline))
soInstanceManagerFullPropertyVector<soKineticEnergy*, 12>::soInstanceManagerFullPropertyVector(bool p1)
    : m_arrayVector(0), m_unk1(p1) { }

inline __attribute__((always_inline))
soGeneralWorkSimple::soGeneralWorkSimple(s32* ints, u32 numInts, float* floats, u32 numFloats, u32* flags,
                                         u32 numFlags) :
    soGeneralWorkAbstract(false), m_intWorks(ints), m_intWorkSize(numInts), m_floatWorks(floats),
    m_floatWorkSize(numFloats), m_flagWorks(flags), m_flagWorkSize(numFlags) { }

inline __attribute__((always_inline))
soArrayContractibleTable<soMotionDataConst>::soArrayContractibleTable(soMotionDataConst* elements, s32 size)
    : m_elements(elements), m_size(size) {
    if (!elements) {
        m_size = 0;
    }
}

inline __attribute__((always_inline))
soArrayContractibleTable<const soStatusData>::soArrayContractibleTable(const soStatusData* elements, s32 size)
    : m_elements(elements), m_size(size) {
    if (!elements) {
        m_size = 0;
    }
}

inline __attribute__((always_inline))
soArrayContractibleTable<acAnimCmdConvPtr>::soArrayContractibleTable(acAnimCmdConvPtr* elements, s32 size)
    : m_elements(elements), m_size(size) {
    if (!elements) {
        m_size = 0;
    }
}

inline __attribute__((always_inline))
soArrayNull<soCollisionAttackAbsolute>::soArrayNull(s32, const soCollisionAttackAbsolute&) { }

// The singleton guards inline these as a vtable store. An out-of-line call is the article REL's other copy.
inline __attribute__((always_inline)) soArrayNull<soPartialAnim>::soArrayNull() { }
inline __attribute__((always_inline)) soArrayNull<u32>::soArrayNull() { }
inline __attribute__((always_inline)) soArrayNull<soControllerImpl>::soArrayNull() { }
inline __attribute__((always_inline)) soArrayNull<soControllerClatter>::soArrayNull() { }
inline __attribute__((always_inline)) soArrayNull<soEffectTime>::soArrayNull() { }

// The article constructor calls these builders out of line. Explicit instantiation keeps a copy for the linker.
template soInsideEventManageModuleBuilder<
    wnFoxLandMasterModuleAccesserBuildConfig::InsideEventManageModuleBuildConfig,
    wnFoxLandMasterModuleAccesserBuildConfig::InsideEventManageModuleTypes>::soInsideEventManageModuleBuilder();
template soModelModuleBuilder<
    wnFoxLandMasterModuleAccesserBuildConfig::ModelModuleBuildConfig>::soModelModuleBuilder(soModuleAccesser*, float, void*, soEventObserverRegistrationDesc*);
template soMotionModuleBuilder<
    wnFoxLandMasterModuleAccesserBuildConfig::MotionModuleBuildConfig>::soMotionModuleBuilder(soModuleAccesser*, void*);
template soGroundModuleBuilder<
    wnFoxLandMasterModuleAccesserBuildConfig::GroundModuleBuildConfig>::soGroundModuleBuilder(soModuleAccesser*, soGroundConditionChecker*);
template soCollisionAttackModuleBuilder<
    wnFoxLandMasterModuleAccesserBuildConfig::CollisionAttackModuleBuildConfig>::soCollisionAttackModuleBuilder(soModuleAccesser*, int, u8, soEventObserverRegistrationDesc*);
template soCollisionHitModuleBuilder<
    wnFoxLandMasterModuleAccesserBuildConfig::CollisionHitModuleBuildConfig>::soCollisionHitModuleBuilder(soModuleAccesser*, int, u8, soEventObserverRegistrationDesc*);
template soShakeModuleBuilder<
    wnFoxLandMasterModuleAccesserBuildConfig::ShakeModuleBuildConfig>::soShakeModuleBuilder(soModuleAccesser*, void*);
template soControllerModuleBuilder<
    wnFoxLandMasterModuleAccesserBuildConfig::ControllerModuleBuildConfig>::soControllerModuleBuilder(soModuleAccesser*, s16);
template soAnimCmdModuleBuilder<
    wnFoxLandMasterModuleAccesserBuildConfig::AnimCmdModuleBuildConfig>::soAnimCmdModuleBuilder(s16);
template soStatusModuleBuilder<
    wnFoxLandMasterModuleAccesserBuildConfig::StatusModuleBuildConfig>::soStatusModuleBuilder(soModuleAccesser*, const soStatusData*, void*);
template soKineticModuleBuilder<
    wnFoxLandMasterModuleAccesserBuildConfig::KineticModuleBuildConfig>::soKineticModuleBuilder(soModuleAccesser*);
template soGeneralWorkBuilder<
    wnFoxLandMasterModuleAccesserBuildConfig::GeneralWorkBuildConfig>::soGeneralWorkBuilder();
template soEffectModuleBuilder<
    wnFoxLandMasterModuleAccesserBuildConfig::EffectModuleBuildConfig>::soEffectModuleBuilder(soModuleAccesser*, void*, void*, void*, void*, soEventObserverRegistrationDesc*);
template soGenerateArticleManageModuleBuilder<wnFoxLandMasterArticleBuildConfig>::soGenerateArticleManageModuleBuilder(soModuleAccesser*);

wnFoxLandMasterParamAccesser::~wnFoxLandMasterParamAccesser() { }

// BYTES: the original's module accesser builder destructor is compact: the big module builder destructors stay calls.
#pragma dont_inline on
template <>
inline soModuleAccesserBuilder<wnFoxLandMasterModuleAccesserBuildConfig>::~soModuleAccesserBuilder() { }
#pragma dont_inline reset

inline __attribute__((always_inline))
wnWeaponBuilder<wnFoxLandMasterModuleAccesserBuildConfig>::wnWeaponBuilder(
    const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd) :
    Weapon(name, bd.m_kind, bd.m_data, articleId, &info, m_modules.getModuleAccesser(), true), m_modules(this, bd) {
    Weapon::initialize();
}

inline wnWeaponBuilder<wnFoxLandMasterModuleAccesserBuildConfig>::~wnWeaponBuilder() { }

void wnWeaponBuilder<wnFoxLandMasterModuleAccesserBuildConfig>::deactivateDescendantForce() {
    wnDeactivateArticleDescendants(m_modules.m_generateArticleBuilder);
}

wnFoxLandMaster::wnFoxLandMaster(s32 articleId, const wnArticleBuildInfo& info, const wnFoxLandMasterData* data) :
    wnWeaponBuilder<wnFoxLandMasterModuleAccesserBuildConfig>(
        "FoxLandMaster", articleId, info,
        wnModuleAccesserBuildData(33, data, info.getHeapModule()->getInstanceHeap(), static_cast<Heaps::HeapType>(0x1F),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1,
                                  &m_paramAccesser, 0, 0, data->unk4C, 0, 0, data->unk44, data->unk48)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

// TODO(S2): 86.5%. The original calls the (large) hit and attack module builder destructors out of line and expands the
// ground shape, posture interpolation and link connection vector destructors inline; ours does the opposite. The hit and
// attack builders are shared partial specializations (include/ft/builder/ft_module_builders.h) with implicit destructors,
// so the out-of-line form needs a declared destructor there (proposed shared patch, see the report).
wnFoxLandMaster::~wnFoxLandMaster() { }

// HYPOTHESIS: the status change notice the Landmaster sends to its link parents (event 0x83A with the new status kind).
class wnFoxLandMasterChangeStatusEvent : public soLinkEventArgs {
public:
    int m_statusKind;
    wnFoxLandMasterChangeStatusEvent(int eventKind, int statusKind) : soLinkEventArgs(eventKind), m_statusKind(statusKind) { }
};

// HYPOTHESIS: the link event 0x3E carries the stage object it concerns.
class wnFoxLandMasterStageObjectEvent : public soLinkEventArgs {
public:
    StageObject* m_stageObject;
};

// TODO(S2): 92.2%. The only difference is the float literal addressing: the original loads one pool base register (r31,
// the first literal of the unit's pool, 1.0) at the top and reads 0.0 / 1.0 / 60.0 through it; ours keeps 0.0 in its own
// register and loads the other two separately, so the saved registers shift by one. The literal order of the pool
// (1.0, 10, 20, 0, -1, 60) also differs from ours (1.0, 10, -1, 0, 20, 60): a builder constructor that comes before activate
// in the original uses 20.0 before -1.0 and 0.0.
// HYPOTHESIS: the fourth parameter is the heap type of the ground collision (it is passed straight to
// constructGrCollision); the header's older name for it was `life`.
void wnFoxLandMaster::activate(int founderTaskId, int resourceId, int team, Vec3f* __restrict pos, float lr, float scale,
                               int heapType) {
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0, *pos, lr, team, 0, 2, 0x80, 0,
                        0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute_Reference_Parent_Unknown_4, true);
    m_moduleAccesser->getControllerModule().update(getInput(), false);
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    work.onFlag(0x12000000);
    work.setFloat(m_param->unk58, 0x11000001);
    work.setFloat(0.0f, 0x11000004);
    work.setFloat(0.0f, 0x11000005);
    work.setFloat(0.0f, 0x11000006);
    work.setFloat(0.0f, 0x11000007);
    work.offFlag(0x12000003);
    work.offFlag(0x12000004);
    work.offFlag(0x12000005);
    work.offFlag(0x12000006);
    work.offFlag(0x12000007);
    work.offFlag(0x12000008);
    work.offFlag(0x12000009);
    work.offFlag(0x1200000A);
    work.offFlag(0x1200000B);
    work.setInt(team, 0x10000005);
    work.setFloat(0.0f, 0x21000000);
    work.setFloat(0.0f, 0x21000001);
    work.setFloat(0.0f, 0x21000002);
    work.setFloat(0.0f, 0x21000003);
    work.setFloat(0.0f, 0x21000004);
    work.setFloat(0.0f, 0x21000005);
    work.setFloat(0.0f, 0x21000006);
    work.setInt(60.0f * m_param->unk38 - (1.0f + scale) * m_param->unk3C, 0x10000006);
    soGroundModule& ground = m_moduleAccesser->getGroundModule();
    ground.constructGrCollision(resourceId, 0, static_cast<Heaps::HeapType>(heapType), 1, 0, 0);
    wnFoxLandMasterGroundModuleImpl& groundImpl = dynamic_cast<wnFoxLandMasterGroundModuleImpl&>(m_moduleAccesser->getGroundModule());
    groundImpl.updateVtxFromNode(1);
    groundImpl.setGrCollisionMode(true);
    groundImpl.setEventToGround(true, 0);
}

void wnFoxLandMaster::processUpdate() {
    Weapon::processUpdate();
    if (soSlow::getInstance()->isEstimate() == true) {
        int statusKind = m_moduleAccesser->getStatusModule().getStatusKind();
        if (statusKind != 8) {
            m_moduleAccesser->getWorkManageModule().decInt(0x10000007);
        }
        if (statusKind != 6) {
            m_moduleAccesser->getWorkManageModule().addFloat(m_param->unk90, 0x11000001);
        } else if (m_moduleAccesser->getWorkManageModule().isFlag(0x22000005)) {
            float speedY = m_moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(soKineticEnergy::ATTRIBUTE_MASK_ALL)).m_y;
            float decrease = 1.0f;
            if (speedY >= m_param->unk94) {
                decrease *= m_param->unk98;
            }
            m_moduleAccesser->getWorkManageModule().subFloat(decrease, 0x11000001);
        }
        if (m_moduleAccesser->getWorkManageModule().getFloat(0x11000001) > m_param->unk58) {
            m_moduleAccesser->getWorkManageModule().setFloat(m_param->unk58, 0x11000001);
        }
        m_moduleAccesser->getWorkManageModule().decInt(0x10000006);
        if (!m_moduleAccesser->getWorkManageModule().isFlag(0x1200000C) &&
            m_moduleAccesser->getWorkManageModule().getInt(0x10000006) <= m_param->unk50) {
            m_moduleAccesser->getWorkManageModule().onFlag(0x1200000C);
            m_moduleAccesser->getEffectModule().reqCommon(0, 0.0f);
        }
    }
}

void wnFoxLandMaster::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender,
                                      int unk4) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (sender->soGetKind() == StageObject_Fighter) {
        switch (sender->soGetSubKind()) {
        case Fighter_Fox:
        case Fighter_Falco:
        case Fighter_Wolf:
            switch (eventInfo->m_eventKind) {
            case 0x456:
                work.onFlag(0x22000001);
                return;
            case 0x457:
                work.onFlag(0x22000002);
                return;
            case 0x458:
                work.onFlag(0x22000003);
                return;
            case 0x459:
                work.onFlag(0x22000004);
                return;
            case 0:
            case 2:
            case 4:
                deactivate(false);
                break;
            }
            break;
        }
    } else {
        switch (eventInfo->m_eventKind) {
        case 0x3E:
            switch (static_cast<wnFoxLandMasterStageObjectEvent*>(eventInfo)->m_stageObject->soGetSubKind()) {
            case 5:
            case 15:
                moduleAccesser->getLinkModule().sendEventParents(3, *eventInfo);
                break;
            }
            break;
        }
    }
    Weapon::notifyEventLink(eventInfo, moduleAccesser, sender, unk4);
}

bool wnFoxLandMaster::notifyEventCollisionAttackCheck(u32 flags) {
    return false;
}

void wnFoxLandMaster::notifyEventCollisionHit(float power, soCollisionAttackData* attackData, u32 index, int unk4,
                                              soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog) {
    Weapon::notifyEventCollisionHit(power, attackData, index, unk4, moduleAccesser, collisionLog);
    m_moduleAccesser->getWorkManageModule().subInt(m_param->unk40 * power, 0x10000006);
}

void wnFoxLandMaster::notifyEventOnDamage(soDamage* damage, bool isReaction, soModuleAccesser* moduleAccesser) {
    soStopModule& stop = moduleAccesser->getStopModule();
    if (damage->m_reaction > 0.0f && !stop.isStop()) {
        int frame = soDamageUtilActor::getDamageHitStopFrame(moduleAccesser, damage, 1.0f, isReaction) >> 1;
        if (frame > 0) {
            moduleAccesser->getStopModule().setHitStopFrame(frame, 1);
            Vec2f offset(0.0f, 1.0f);
            getShakeModule(m_moduleAccesser).req(0, soValueAccesser::getConstantInt(m_moduleAccesser, 0x5DC3, 0), 0, &offset);
        }
    }
}

void wnFoxLandMaster::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData,
                                              soModuleAccesser* moduleAccesser) {
    wnFoxLandMasterChangeStatusEvent event(0x83A, statusKind);
    m_moduleAccesser->getLinkModule().sendEventParents(3, event);
}

// HYPOTHESIS: the slot loops reach the shell through the sub pool's index lookup (as the generic sub pool's getInstanceAt).
template <class Sub>
inline wnFoxLandMasterShell* wnFoxLandMasterShellAt(Sub& sub, s32 index) {
    if (index == 0) {
        return sub.getEnergy();
    }
    return 0;
}

// HYPOTHESIS: the mediator of the one shell (articles 1..16 are unused slots), as the Kinopio spore mediator.
template <class PoolTL, class AssignTL, class KindInfo, class Extra>
bool soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: {
        bool allActive;
        s32 i;
        for (i = 0; i < 1; ++i) {
            if (static_cast<soArticle&>(*wnFoxLandMasterShellAt(m_pools.getSub(), i)).isActiveArticle() == false) {
                allActive = false;
                goto done;
            }
        }
        allActive = true;
    done:
        return allActive == false;
    }
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(false)
    default: return false;
    }
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
s32 soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: {
        wnArticleSlotCounter counter;
        counter.m_isActive = wnArticleIsActive;
        counter.m_activeNum = 0;
        counter.m_inactiveNum = 0;
        for (s32 i = 0; i < 1; ++i) {
            wnArticleSlotCounter* c = &counter;
            if (c->m_isActive(&static_cast<soArticle&>(*wnFoxLandMasterShellAt(m_pools.getSub(), i))) == true) {
                ++c->m_activeNum;
            } else {
                ++c->m_inactiveNum;
            }
        }
        return counter.m_activeNum;
    }
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(0)
    default: return 0;
    }
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
s32 soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(0)
    default: return 0;
    }
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
bool soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::shoot(soModuleAccesser*, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnFoxLandMasterShell>(article, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(true)
    default: return false;
    }
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
void soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        if (!static_cast<soArticle&>(*wnFoxLandMasterShellAt(m_pools.getSub(), i)).setDeactivateDescendant()) {
            return;
        }
    }
}

template class wnExtendParamAccesser<3999, 45, 23999, 5, 0, 0>;

// TODO: clean up, give variables real names
namespace jj_3184F8 {
struct Vec2 { float x,y; };
struct Vec3 { float x,y,z; };
class Object_vptr_0 {
public:
virtual void slot0(); virtual void slot1(); virtual void slot2(); virtual void slot3();
virtual int slot4(unsigned); virtual void slot5(); virtual void slot6(); virtual void slot7(); virtual void slot8();
virtual float slot9(); virtual void slot10(); virtual void slot11(); virtual float slot12(unsigned);
};
struct Base3c { char pad[0x3c]; };
class Object_vptr_3c : public Base3c {
public:
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
virtual void slot23();
virtual void slot24();
virtual void slot25();
virtual void slot26();
virtual void slot27();
virtual void slot28();
virtual void slot29();
virtual void slot30();
virtual void slot31();
virtual void slot32();
virtual void slot33();
virtual void slot34();
virtual void slot35();
virtual void slot36();
virtual void slot37();
virtual void slot38();
virtual void slot39();
virtual void slot40();
virtual void slot41();
virtual void slot42();
virtual void slot43();
virtual void slot44();
virtual void slot45();
virtual void slot46();
virtual void slot47();
virtual void slot48();
virtual void slot49();
virtual void slot50();
virtual void slot51();
virtual void slot52();
virtual void slot53();
virtual void slot54();
virtual void slot55();
virtual void slot56();
virtual void slot57();
virtual void slot58();
virtual void slot59();
virtual void slot60();
virtual void slot61();
virtual void slot62();
virtual void slot63();
virtual void slot64();
virtual void slot65();
virtual void slot66();
virtual void slot67();
virtual void slot68();
virtual void slot69();
virtual void slot70();
virtual void slot71();
virtual void slot72();
virtual void slot73();
virtual void slot74();
virtual void slot75();
virtual void slot76();
virtual void slot77();
virtual void slot78();
virtual void slot79();
virtual void slot80();
virtual void slot81();
virtual void slot82();
virtual void slot83();
virtual void slot84();
virtual void slot85();
virtual void slot86();
virtual void slot87();
virtual void slot88();
virtual void slot89();
virtual void slot90();
virtual void slot91();
virtual void slot92();
virtual void slot93();
virtual void slot94();
virtual void slot95();
virtual void slot96();
virtual void slot97();
virtual void slot98();
virtual void slot99();
virtual void slot100();
virtual void slot101();
virtual void slot102();
virtual void slot103();
virtual void slot104();
virtual void slot105();
virtual void slot106();
virtual void slot107();
virtual void slot108();
virtual void slot109();
virtual void slot110();
virtual void slot111();
virtual void slot112();
virtual void slot113();
virtual void slot114();
virtual void slot115();
virtual void slot116();
virtual void slot117();
virtual void slot118();
virtual void slot119();
virtual void slot120();
virtual void slot121();
virtual void slot122();
virtual void slot123();
virtual void slot124();
};
struct Base8 { char pad[8]; };
class PosModule : public Base8 {
public:
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
virtual void slot23();
virtual void slot24();
virtual void slot25();
virtual void slot26();
virtual void slot27();
virtual void slot28();
virtual void slot29();
virtual void slot30();
virtual void slot31();
virtual void slot32();
virtual void slot33();
virtual void slot34();
virtual void slot35();
virtual Vec3 slot36(int, int);
};
struct Params { char pad[0x34]; float value; };
struct Owner { char pad[0x28]; int id; char pad2[0x4844]; Params* params; };
struct Modules { char pad[4]; PosModule* pos; char pad8[4]; Object_vptr_0* scale; char pad10[0x54]; Object_vptr_0* work; };
struct Accesser { char pad[8]; Owner* owner; char padc[0xcc]; Modules* modules; };
struct Self { char pad[0x18]; Object_vptr_3c object; char gap[0x4c]; };
extern "C" {
extern char g_ftRobotNullArticleStorage[];
extern char __RTTI__15wnFoxLandMaster[], __RTTI__11StageObject[];
extern float lbl_27_rodata_74BC, lbl_27_rodata_74C8;
void* __dynamic_cast(void*, long, const void*, const void*, long);
void __ct__26soArticleDeactivateCheckerFv(void*);
void __dt__26soArticleDeactivateCheckerFv(void*, short);
unsigned __cl__26soArticleDeactivateCheckerFP9soArticle(void*, void*);
void rot__5Vec2fCFfP5Vec2f(Vec2*, float, Vec2*);
void fn_27_31E338(Object_vptr_3c*, int, int, const Vec3*, Vec2*, float, float);
}
extern "C" void* fn_27_3184F8(Self* self, unsigned kind, Accesser* a) {
switch(kind) {
case 0: {
 void* selector;
 __ct__26soArticleDeactivateCheckerFv(&selector);
 if (__cl__26soArticleDeactivateCheckerFP9soArticle(&selector, (char*)self + 0xa0) == 1) self = (Self*)((char*)self + 0x18);
 else self = 0;
 if (!self) {
  self = (Self*)selector;
  if (self) self = (Self*)((char*)self - 0x88);
  if (!self) { __dt__26soArticleDeactivateCheckerFv(&selector, -1); return g_ftRobotNullArticleStorage; }
  ((Object_vptr_3c*)self)->slot124();
 }
 Params* p = ((Owner*)__dynamic_cast(a->owner, 0x3c, __RTTI__15wnFoxLandMaster, __RTTI__11StageObject, 1))->params;
 float f31 = a->modules->work->slot12(0x11000004);
 float f30 = a->modules->scale->slot9();
 Vec2 vec;
 vec.x = p->value * f30;
 vec.y = lbl_27_rodata_74BC;
 rot__5Vec2fCFfP5Vec2f(&vec, lbl_27_rodata_74C8 * (f31 * f30), &vec);
 Object_vptr_0* work = a->modules->work;
 int id = a->owner->id;
 const Vec3& pos = a->modules->pos->slot36(7, 0);
 fn_27_31E338((Object_vptr_3c*)self, id, work->slot4(0x10000005), &pos, &vec, f30, f31);
 __dt__26soArticleDeactivateCheckerFv(&selector, -1);
 return g_ftRobotNullArticleStorage;
}
case 1: return g_ftRobotNullArticleStorage;
case 2: return g_ftRobotNullArticleStorage;
case 3: return g_ftRobotNullArticleStorage;
case 4: return g_ftRobotNullArticleStorage;
case 5: return g_ftRobotNullArticleStorage;
case 6: return g_ftRobotNullArticleStorage;
case 7: return g_ftRobotNullArticleStorage;
case 8: return g_ftRobotNullArticleStorage;
case 9: return g_ftRobotNullArticleStorage;
case 10: return g_ftRobotNullArticleStorage;
case 11: return g_ftRobotNullArticleStorage;
case 12: return g_ftRobotNullArticleStorage;
case 13: return g_ftRobotNullArticleStorage;
case 14: return g_ftRobotNullArticleStorage;
case 15: return g_ftRobotNullArticleStorage;
case 16: return g_ftRobotNullArticleStorage;
default: return g_ftRobotNullArticleStorage;
}
}
}

