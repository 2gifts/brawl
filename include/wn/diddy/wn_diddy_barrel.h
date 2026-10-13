#pragma once

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_kinetic_mediator_impl.h>
#include <wn/wn_damage_transactor_actor.h>

// Offsets are established by the native article constructor's accesser setup.
// HYPOTHESIS: fields without names remain opaque; only their offsets and types are used.
struct wnDiddyBarrelParam {
    s32 unk0;
    float unk4;
    float unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    float unk18;
    float unk1C;
    float unk20;
    float unk24;
    float unk28;
    float unk2C;
    s32 unk30;
    float unk34;
    float unk38;
};

static_assert(sizeof(wnDiddyBarrelParam) == 0x3C, "Diddy Barrel parameter extent");

struct wnDiddyBarrelData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnDiddyBarrelParam* m_param; // +0x34
    u32 m_hitDataStorage;               // +0x38, start of an opaque set; its full extent is unknown
};
static_assert(sizeof(wnDiddyBarrelData) == 0x3C, "Diddy Barrel observed data prefix");

class wnDiddyBarrelParamAccesser : public wnExtendParamAccesser<3999, 10, 23999, 5, 0, 0> {
public:
    explicit wnDiddyBarrelParamAccesser(const wnDiddyBarrelParam* param) __attribute__((never_inline)) :
        wnExtendParamAccesser<3999, 10, 23999, 5, 0, 0>(0x37) {
        m_floats[0] = &param->unk4;
        m_floats[1] = &param->unk8;
        m_floats[2] = &param->unk18;
        m_floats[3] = &param->unk1C;
        m_floats[4] = &param->unk20;
        m_floats[5] = &param->unk24;
        m_floats[6] = &param->unk28;
        m_floats[7] = &param->unk2C;
        m_floats[8] = &param->unk34;
        m_floats[9] = &param->unk38;
        m_ints[0] = &param->unk0;
        m_ints[1] = &param->unkC;
        m_ints[2] = &param->unk10;
        m_ints[3] = &param->unk14;
        m_ints[4] = &param->unk30;
    }
    virtual ~wnDiddyBarrelParamAccesser() { }
};
static_assert(sizeof(wnDiddyBarrelParamAccesser) == 0x64, "Diddy Barrel parameter accesser extent");

#include <wn/diddy/wn_diddy_barrel_kinetic_transactor.h>
#include <wn/diddy/wn_diddy_barrel_damage_transactor.h>

typedef soTypeList<soInstancePoolInfo<soKineticEnergyNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<0, 1> >,
        soTypeList<soInstancePoolInfo<wnKineticEnergyGravity, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<1, 1> >,
        soTypeList<soInstancePoolInfo<soKineticEnergyNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<2, 2> >,
        soTypeList<soInstancePoolInfo<soKineticEnergyGroundMovement, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<3, 8> >,
        soTypeList<soInstancePoolInfo<soKineticEnergyWindNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<4, 4> >,
        soTypeListNullType> > > > > wnDiddyBarrelKineticEnergyTypeList;
typedef soKineticTransactorInfo<wnDiddyBarrelKineticTransactor, wnKineticTransactor,
                                wnDiddyBarrelKineticTransactor, soKineticTransactHelper>
    wnDiddyBarrelKineticTransactorInfo;
struct wnDiddyBarrelModuleAccesserBuildConfig : wnCommonBuildConfig {
    typedef soKineticModuleBuildConfig<soKineticModuleGenericImpl,
                                       soKineticMediatorImpl<wnDiddyBarrelKineticEnergyTypeList,
                                                             wnDiddyBarrelKineticTransactorInfo> >
        KineticModuleBuildConfig;
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 5, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<soTypeList<soIntToType<4>,
                                                 soTypeListNullType> >,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false,
                                               true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 1, 1, soCollisionHitModuleImpl, 0x3ff, true>
        CollisionHitModuleBuildConfig;
    typedef wnDamageModuleBuildConfig<soDamageModuleActor, 1,
                                     soSingletonHolder<wnDiddyBarrelDamageTransactorImpl>,
                                     soSingletonHolder<soDamageEffectorNull> >
        DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<3, 6, 1>, 3,
                                      soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0>
        AnimCmdModuleSubBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
};

// Native RTTI and constructor show that this builder contains only module-builder storage;
// the five-energy kinetic mediator belongs to the kinetic module, not an appended energy tail.
template <>
class wnWeaponBuilder<wnDiddyBarrelModuleAccesserBuildConfig> : public Weapon {
    wnModuleAccesserBuilder<wnDiddyBarrelModuleAccesserBuildConfig> m_modules;
public:
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info,
                    const wnModuleAccesserBuildData& bd) __attribute__((never_inline));
    virtual ~wnWeaponBuilder();
    virtual void deactivateDescendantForce();
};
static_assert(sizeof(wnWeaponBuilder<wnDiddyBarrelModuleAccesserBuildConfig>) == 0x2580,
              "Diddy Barrel module builder extent");

class wnDiddyBarrelStatusUniqProcess : public soStatusUniqProcess {
public:
    static wnDiddyBarrelStatusUniqProcess* getInstance();
    virtual ~wnDiddyBarrelStatusUniqProcess();
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

class wnDiddyBarrel : public wnWeaponBuilder<wnDiddyBarrelModuleAccesserBuildConfig> {
    // HYPOTHESIS: ownership of the unwritten word between the generic modules
    // and the first observed Diddy field is unresolved; its meaning is unknown.
    u32 unk2580;
    const wnDiddyBarrelData* m_data;            // +0x2584
    const wnDiddyBarrelParam* m_param;          // +0x2588
    wnDiddyBarrelParamAccesser m_paramAccesser; // +0x258C
public:
    wnDiddyBarrel(s32 articleId, const wnArticleBuildInfo& info, const wnDiddyBarrelData* data);
    virtual ~wnDiddyBarrel() { }
    void activate(int founderTaskId, int unk1, int team, Vec3f* pos, Vec2f* speed, bool unk5, float lr, float rot);
    virtual void onDeactivate();
    virtual void processUpdate();
    void toShoot(Vec3f* pos, float lr, Vec2f* speed, float value, float unk2, bool unk0);
    virtual bool notifyEventAnimCmd(acAnimCmd*, soModuleAccesser*, int);
    virtual void notifyEventCollisionAttack(float power, soCollisionLog*, soModuleAccesser*);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual bool isCanEat();
};
static_assert(sizeof(wnDiddyBarrel) == 0x25F0, "Diddy Barrel object extent");
