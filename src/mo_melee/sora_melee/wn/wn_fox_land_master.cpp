#define FT_MODULE_BUILDER
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
#include <types.h>

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

template class wnExtendParamAccesser<3999, 45, 23999, 5, 0, 0>;
