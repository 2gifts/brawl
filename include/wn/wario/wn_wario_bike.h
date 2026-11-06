#pragma once
#include <wn/wn_weapon_builder.h>
#include <types.h>

// Proved parameter prefix: accessor10054 indexes fields through offset98.
// Complete backing allocation extent remains unreconstructed; no sizeof use.
// Named fields line up (offset, size and use) with the published Wario bike parameter table; the rest await
// parameter setup/accessor reconstruction. HYPOTHESIS: the names are descriptive, not the original's. Offsets are verified.
struct wnWarioBikeParam {
    u8 unk0[0xC];
    float groundTopSpeed;
    u8 unk10[0x4];
    float unk14;
    float airTopSpeed;
    u8 unk1C[0x4];
    float unk20;
    u8 unk24[0xC];
    float damageGrowth;
    float unk34;
    u8 unk38[0x8];
    float unk40;
    float unk44;
    float unk48;
    float airRotationSpeed;
    float landingBounceHeight;
    float unk54;
    u8 unk58[0x4];
    float maxWheelieAngle;
    float wheelieRotateRate;
    float unk64;
    float unk68;
    float unk6C;
    int slowFallOffFrames;
    u8 unk74[0x8];
    int turnFallChance;
    int unk80;
    u8 unk84[0x8];
    float unk8C;
    float unk90;
    float unk94;
    float unk98;
};

// RTTI5FB4 and constructor10318 establish the primary Weapon builder base.
struct wnWarioBikeModuleAccesserBuildConfig;
#ifndef WN_WEAPON_BUILDER
template <>
class wnWeaponBuilder<wnWarioBikeModuleAccesserBuildConfig> : public Weapon {
    u8 unkD0[0x2980 - sizeof(Weapon)];
public:
    virtual ~wnWeaponBuilder();
    virtual void deactivateDescendantForce();
};
#else
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_kinetic_mediator_impl.h>
#include <wn/wn_damage_transactor_actor.h>
#include <so/controller/so_controller_module_link_ref.h>
#include <wn/wario/wn_wario_bike_kinetic_transactor.h>
#include <wn/wario/wn_wario_bike_damage_transactor.h>

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnWarioBikeData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnWarioBikeParam* m_param; // +0x34
    u32 m_hitDataStorage;            // +0x38, start of an opaque set
};

class wnWarioBikeParamAccesser : public wnExtendParamAccesser<3999, 34, 23999, 5, 0, 0> {
public:
    explicit wnWarioBikeParamAccesser(const wnWarioBikeParam* param) __attribute__((never_inline));
    virtual ~wnWarioBikeParamAccesser() { }
};

typedef soTypeList<wnWarioBikeNormal0Info,
        soTypeList<wnWarioBikeGravityInfo,
        soTypeList<wnWarioBikeNormal2Info, soTypeListNullType> > > wnWarioBikeKineticEnergyTypeList;
typedef soKineticTransactorInfo<wnWarioBikeKineticTransactor, wnKineticTransactor,
                                wnKineticTransactor, soKineticTransactHelper>
    wnWarioBikeKineticTransactorInfo;

struct wnWarioBikeModuleAccesserBuildConfig : wnCommonBuildConfig {
    typedef soKineticModuleBuildConfig<soKineticModuleGenericImpl,
                                       soKineticMediatorImpl<wnWarioBikeKineticEnergyTypeList,
                                                             wnWarioBikeKineticTransactorInfo> >
        KineticModuleBuildConfig;
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 5, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<22, soMotionModuleImpl, 0, 0,
                                      soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    // HYPOTHESIS: the trailing arguments of the attack module configuration.
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 4, 1, soCollisionAttackModuleImpl, 4, true, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 4, 1, soCollisionHitModuleImpl, 0x3ff, true>
        CollisionHitModuleBuildConfig;
    typedef wnDamageModuleBuildConfig<soDamageModuleActor, 1,
                                      soSingletonHolder<wnWarioBikeDamageTransactorImpl>,
                                      soSingletonHolder<soDamageEffectorNull> >
        DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 2> VisibilityModuleBuildConfig;
    typedef wnControllerModuleBuildConfig<soControllerModuleLinkRef, 0, 0> ControllerModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<19, soGeneralWorkBuildConfig<8, 10, 1>, 19,
                                      soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 19, 0, 0, 1, 22, 0, 0, 1, 22, 0, 0, 1, 22, 0, 0>
        AnimCmdModuleSubBuildConfig;
    typedef soGeneralWorkBuildConfig<8, 2, 1> GeneralWorkBuildConfig;
    typedef soShadowModuleBuildConfig<soShadowModuleImpl> ShadowModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
};

template <>
class wnWeaponBuilder<wnWarioBikeModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnWarioBikeModuleAccesserBuildConfig> m_modules;
public:
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info,
                    const wnModuleAccesserBuildData& bd);
    virtual ~wnWeaponBuilder();
    virtual void deactivateDescendantForce();
};
#endif

// Proved prefix only: complete allocation size remains HYPOTHESIS.
// ConstructorF17C stores the same parameter pointer passed to the accessor at2984.
// The parameter accessor10054 indexes this storage through offset98.
class wnWarioBike : public wnWeaponBuilder<wnWarioBikeModuleAccesserBuildConfig> {
public:
#ifdef WN_WEAPON_BUILDER
    wnWarioBike(s32 articleId, const wnArticleBuildInfo& info, const wnWarioBikeData* data);
#endif
    void activate(int founderTaskId, int riderTaskId, int resourceId, int team, Vec3f* position, float lr);
    virtual void processUpdate();
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventCollisionAttack(float power, soCollisionLog*, soModuleAccesser*);
    virtual void notifyEventLink(soLinkEventArgs*, soModuleAccesser*, StageObject*, int);
    wnWarioBikeParam* m_param;
private:
#ifdef WN_WEAPON_BUILDER
    wnWarioBikeParamAccesser m_paramAccesser;
#else
    u8 unk2984[0xBC]; // Native parameter accessor; detailed fields remain opaque.
#endif
};
