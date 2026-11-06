#pragma once

// Full builder-based definitions of R.O.B.'s Gyro, Gyro holder and Diffusion Beam articles, for the unit that owns their
// constructors (ft_robot_articles.cpp, which defines WN_WEAPON_BUILDER). Other units see the opaque storage classes in
// ft_robot_article_pools.h.
#include <wn/wn_module_accesser_builder.h>
#include <wn/robot/wn_robot_gyro_status_uniq_process.h>
#include <ft/robot/ft_robot_article_info.h>
#include <wn/weapon.h>

WN_STATUS_PROC_VECTOR_ARG_CTOR(3);
WN_STATUS_PROC_VECTOR_ARG_CTOR(4);

// HYPOTHESIS: the common prefix of the static article data of the three articles.
struct wnRobotArticleData {
    s32 m_resGroupNo; // 0x00, read as a u8
    u8 unk4[0x30];
    const void* m_param; // 0x34
};

typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                             wnInsideEventManageModuleTypes> wnRobotInsideEventConfig;

class wnRobotGyroModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef wnRobotInsideEventConfig InsideEventManageModuleBuildConfig;
    typedef soPostureModuleBuildConfig<2, soPostureModuleImpl> PostureModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0,
        soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
        soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0,
        soCollisionAttackModuleImpl, 1, false, true> CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 1> VisibilityModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<2, 8, 1>, 3,
        soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0>
        AnimCmdModuleSubBuildConfig;
};

class wnRobotGyroHolderModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 5, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soSituationModuleBuildConfigNull SituationModuleBuildConfig;
    typedef soPostureModuleBuildConfigNull PostureModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soStopModuleBuildConfigNull StopModuleBuildConfig;
    typedef soVisibilityModuleBuildConfigNull VisibilityModuleBuildConfig;
    typedef soSoundModuleBuildConfigNull SoundModuleBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<6, soMotionModuleImpl, 0, 0,
        soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
        soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<2, 1, 3>, 3,
        soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 6, 0, 0, 1, 6, 0, 0, 1, 6, 0, 0>
        AnimCmdModuleSubBuildConfig;
};

class wnRobotFinalBeamModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef wnRobotInsideEventConfig InsideEventManageModuleBuildConfig;
    typedef soResourceModuleBuildConfigNull ResourceModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<4, soMotionModuleImpl, 0, 0,
        soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
        soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 5, 0,
        soCollisionAttackModuleImpl, 3, false, true> CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<4, soGeneralWorkBuildConfig<2, 2, 1>, 4,
        soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 4, 0, 0, 1, 4, 0, 0, 1, 4, 0, 0, 1, 4, 0, 0>
        AnimCmdModuleSubBuildConfig;
};

// The holder selects the kinetic singleton without a storage slot (no kinetic builder member), as the Fox afterimage does.
extern char lbl_27_bss_530[];
template <>
class soModuleAccesserBuilder<wnRobotGyroHolderModuleAccesserBuildConfig> : public utUnCopyable {
    typedef wnRobotGyroHolderModuleAccesserBuildConfig BC;
public:
    soInsideEventManageModuleBuilder<BC::InsideEventManageModuleBuildConfig, BC::InsideEventManageModuleTypes> unk194;
    soModuleAccesser m_moduleAccsr;
    soHeapModuleBuilder<BC::HeapModuleBuildConfig> m_heapModuleBuilder;
    soParamCustomizeModuleBuilder<BC::ParamCustomizeModuleBuildConfig> m_paramCustomizeModuleBuilder;
    soResourceModuleBuilder<BC::ResourceModuleBuildConfig> m_resourceModuleBuilder;
    soModelModuleBuilder<BC::ModelModuleBuildConfig> m_modelModuleBuilder;
    soMotionModuleBuilder<BC::MotionModuleBuildConfig> m_motionBuilder;
    soPostureModuleBuilder<BC::PostureModuleBuildConfig> m_postureModuleBuilder;
    soGroundModuleBuilder<BC::GroundModuleBuildConfig> m_groundModuleBuilder;
    soSituationModuleBuilder<BC::SituationModuleBuildConfig> m_situationModuleBuilder;
    soTeamModuleBuilder<BC::TeamModuleBuildConfig> m_teamBuilder;
    soCollisionAttackModuleBuilder<BC::CollisionAttackModuleBuildConfig> m_attackModuleBuilder;
    soCollisionHitModuleBuilder<BC::CollisionHitModuleBuildConfig> m_hitModuleBuilder;
    soCollisionShieldModuleBuilder<BC::CollisionShieldModuleBuildConfig> m_shieldModuleBuilder;
    soCollisionReflectorModuleBuilder<BC::CollisionReflectorModuleBuildConfig> m_reflectorModuleBuilder;
    ftNullCollisionAbsorberBuilder m_absorberBuilder; // the four bytes after the reflector builder
    soCollisionCatchModuleBuilder<BC::CollisionCatchModuleBuildConfig> m_collisionCatchModuleBuilder;
    BC::CollisionSearchModuleBuilder m_searchBuilder;
    soDamageModuleBuilder<BC::DamageModuleBuildConfig> m_damageModuleBuilder;
    soCatchModuleBuilder<BC::CatchModuleBuildConfig> m_catchModuleBuilder;
    soCaptureModuleBuilder<BC::CaptureModuleBuildConfig> m_captureModuleBuilder;
    soStopModuleBuilder<BC::StopModuleBuildConfig> m_stopModuleBuilder;
    soTurnModuleBuilder<BC::TurnModuleBuildConfig> m_turnModuleBuilder;
    soShakeModuleBuilder<BC::ShakeModuleBuildConfig> m_shakeModuleBuilder;
    soSoundModuleBuilder<BC::SoundModuleBuildConfig> m_soundModuleBuilder;
    soLinkModuleBuilder<BC::LinkModuleBuildConfig> m_linkModuleBuilder;
    soVisibilityModuleBuilder<BC::VisibilityModuleBuildConfig> m_visibilityModuleBuilder;
    soControllerModuleBuilder<BC::ControllerModuleBuildConfig> m_controllerModuleBuilder;
    soCameraModuleBuilder<BC::CameraModuleBuildConfig> m_cameraModuleBuilder;
    soWorkManageModuleBuilder<BC::WorkManageModuleBuildConfig> m_workManageModuleBuilder;
    soAnimCmdModuleBuilder<BC::AnimCmdModuleBuildConfig> m_animCmdBuilder;
    soStatusModuleBuilder<BC::StatusModuleBuildConfig> m_statusBuilder;
    soGeneralWorkBuilder<BC::GeneralWorkBuildConfig> m_generalWorkBuilder;
    BC::GenerateArticleManageModuleBuilder m_generateArticleBuilder;
    soEffectModuleBuilder<BC::EffectModuleBuildConfig> m_effectModuleBuilder;
    soComboModuleBuilder<BC::ComboModuleBuildConfig> m_comboModuleBuilder;
    soAreaModuleBuilder<BC::AreaModuleBuildConfig> m_areaBuilder;
    soPhysicsModuleBuilder<BC::PhysicsModuleBuildConfig> m_physicsModuleBuilder;
    soSlopeModuleBuilder<BC::SlopeModuleBuildConfig> m_slopeModuleBuilder;
    soShadowModuleBuilder<BC::ShadowModuleBuildConfig> m_shadowModuleBuilder;
    soItemManageModuleBuilder<BC::ItemManageModuleBuildConfig> m_itemManageModuleBuilder;
    soColorBlendModuleBuilder<BC::ColorBlendModuleBuildConfig> m_colorBlendModuleBuilder;
    soJostleModuleBuilder<BC::JostleModuleBuildConfig> m_jostleModuleBuilder;
    soAbnormalModuleBuilder<BC::AbnormalModuleBuildConfig> m_abnormalModuleBuilder;
    soSlowModuleBuilder<BC::SlowModuleBuildConfig> m_slowModuleBuilder;
    soReflectModuleBuilder<BC::ReflectModuleBuildConfig> m_reflectModuleBuilder;
    soGlowModuleBuilder<BC::GlowModuleBuildConfig> m_glowModuleBuilder;

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder() { }
    soModuleAccesser* getModuleAccesser() { return &m_moduleAccsr; }
};

// The Gyro holder keeps its energies in its own kinetic mediator (no appended common energies).
template <>
class wnWeaponBuilder<wnRobotGyroHolderModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnRobotGyroHolderModuleAccesserBuildConfig> m_modules;
    u32 m_modulesPad; // HYPOTHESIS: trailing alignment of the animation-command storage
public:
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd) :
        Weapon(name, bd.m_kind, bd.m_data, articleId, &info, m_modules.getModuleAccesser(), true),
        m_modules(this, bd) {
        Weapon::initialize();
    }
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() { soSingletonHolder<soArticleMediatorNull>::getInstance()->deactivate(); }
};

// Same as the generic builder with its two common energies; the descendant teardown is spelled with the null mediator
// singleton, which the original inlines with its guard.
template <>
class wnWeaponBuilder<wnRobotGyroModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnRobotGyroModuleAccesserBuildConfig> m_modules;
    wnKineticEnergyNormal m_energyNormal;
    soKineticEnergyRotNormal m_energyRotNormal;
public:
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd,
                    bool weaponFlag = true) :
        Weapon(name, bd.m_kind, bd.m_data, articleId, &info, this->m_modules.getModuleAccesser(), weaponFlag),
        m_modules(this, bd), m_energyNormal(), m_energyRotNormal() {
        Weapon::initialize();
        m_moduleAccesser->getKineticModule().addEnergy(&m_energyNormal, 0, soKineticEnergy::AttributeFlag(1), -1);
        m_moduleAccesser->getKineticModule().addEnergy(&m_energyRotNormal, 1, soKineticEnergy::AttributeFlag(1), -1);
    }
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() { soSingletonHolder<soArticleMediatorNull>::getInstance()->deactivate(); }
};

// Same as the generic builder with its two common energies; the descendant teardown is spelled with the null mediator
// singleton, which the original inlines with its guard.
template <>
class wnWeaponBuilder<wnRobotFinalBeamModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnRobotFinalBeamModuleAccesserBuildConfig> m_modules;
    wnKineticEnergyNormal m_energyNormal;
    soKineticEnergyRotNormal m_energyRotNormal;
public:
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd,
                    bool weaponFlag = true) :
        Weapon(name, bd.m_kind, bd.m_data, articleId, &info, this->m_modules.getModuleAccesser(), weaponFlag),
        m_modules(this, bd), m_energyNormal(), m_energyRotNormal() {
        Weapon::initialize();
        m_moduleAccesser->getKineticModule().addEnergy(&m_energyNormal, 0, soKineticEnergy::AttributeFlag(1), -1);
        m_moduleAccesser->getKineticModule().addEnergy(&m_energyRotNormal, 1, soKineticEnergy::AttributeFlag(1), -1);
    }
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() { soSingletonHolder<soArticleMediatorNull>::getInstance()->deactivate(); }
};

class wnRobotGyro : public wnWeaponBuilder<wnRobotGyroModuleAccesserBuildConfig> {
    wnRobotGyroStatusUniqProcessHold m_hold;
    wnRobotGyroStatusUniqProcessMove m_move;
    wnRobotGyroStatusUniqProcessStop m_stop;
    const void* m_param; // 0x202C
public:
    wnRobotGyro(s32 articleId, const ftRobotArticleConstructionInfo& info, void* data);
    virtual ~wnRobotGyro() { }
    virtual void onDeactivate();
    virtual void updateNodeSRT();
    virtual void notifyEventLink(soLinkEventArgs*, soModuleAccesser*, StageObject*, int);

    // HYPOTHESIS: source reference spelling; the callee reads all three position components.
    void activate(s32 founderTaskId, u32 resourceId, s32 team, const Vec3f& position, float lr, float power);
};

#include <wn/robot/wn_robot_beam.h>

class wnRobotGyroHolder : public wnWeaponBuilder<wnRobotGyroHolderModuleAccesserBuildConfig> {
public:
    wnRobotGyroHolder(s32 articleId, const ftRobotArticleConstructionInfo& info, void* data);
    virtual ~wnRobotGyroHolder();
    virtual void onDeactivate();
    virtual void notifyEventLink(soLinkEventArgs*, soModuleAccesser*, StageObject*, int);

    // HYPOTHESIS: source reference spelling, as for the Gyro activation above.
    void activate(s32 founderTaskId, u32 resourceId, s32 team, float lr, const Vec3f& position,
                  SituationKind situation);
};

class wnRobotFinalBeam : public wnWeaponBuilder<wnRobotFinalBeamModuleAccesserBuildConfig> {
    const void* m_param; // 0x2208
public:
    virtual bool notifyEventCollisionAttackCheck(u32) { return false; }
    wnRobotFinalBeam(s32 articleId, const ftRobotArticleConstructionInfo& info, void* data);
    virtual ~wnRobotFinalBeam();
    virtual void processUpdate();
    virtual void processFixPosition();
    virtual void notifyEventLink(soLinkEventArgs*, soModuleAccesser*, StageObject*, int);

    void activate(s32 founderTaskId, u32 resourceId, s32 team, const Vec3f* position, float lr,
                  s32 count, s32 selection);
};
