#pragma once

#include <wn/mario/wn_mario_pump_water.h>
#include <wn/wn_article_pool.h>
#include <so/controller/so_controller_module_link_ref.h>

// F.L.U.D.D.'s five float parameters drive aiming and muzzle-position spread.
// HYPOTHESIS: original field and wrapper type spellings.
struct wnMarioPumpParam {
    float m_neutralAngle, m_upAngle, m_downAngle, m_angleStep, m_spreadY;
};
struct wnMarioPumpData {
    u8 unk0[0x34];
    const wnMarioPumpParam* m_param;
};
class wnMarioPumpParamAccesser : public wnExtendParamAccesser<3999, 5, 23999, 0, 0, 0> {
public:
    wnMarioPumpParamAccesser(const wnMarioPumpParam* param) : wnExtendParamAccesser<3999, 5, 23999, 0, 0, 0>(2) {
        m_floats[0] = &param->m_neutralAngle;
        m_floats[1] = &param->m_upAngle;
        m_floats[2] = &param->m_downAngle;
        m_floats[3] = &param->m_angleStep;
        m_floats[4] = &param->m_spreadY;
    }
    virtual ~wnMarioPumpParamAccesser() { }
};

// HYPOTHESIS: source wrapper names. The native mediator owns one pool of 12 water articles.
typedef wnArticleHierarchy<wnMarioPumpWater, 12, soInstancePoolRoot> wnMarioPumpWaterPool;
class wnMarioPumpMediator : public soArticleMediator {
    wnMarioPumpWaterPool m_pools;
    u8 m_autoRecycle;
public:
    wnMarioPumpMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~wnMarioPumpMediator() { }
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc);
    virtual bool shoot(soModuleAccesser* acc, soArticle* article);
    virtual s32 getMediateNum();
    virtual void setAutoRecycle(bool enabled);
    virtual void deactivate();
    virtual s32 getGenerateMaxNum(s32 articleId);
    virtual s32 getActiveNum(soModuleAccesser*, s32 articleId);
    virtual bool isGeneratable(soModuleAccesser*, s32 articleId);
};
FT_DOL_ARRAY_VECTOR(soArticle*, 1);
typedef wnArticleManageModuleBuilder<wnMarioPumpMediator, 1> wnMarioPumpArticleManageModuleBuilder;

// The null kinetic selection follows this status module without the tail word
// used by the status builders that precede an owned kinetic module.
typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<2, 2, 1>, 3,
    soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > wnMarioPumpStatusBuildConfig;
template <>
class soStatusModuleBuilder<wnMarioPumpStatusBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusTable;
    soGeneralWorkBuilder<soGeneralWorkBuildConfig<2, 2, 1> > m_generalWork;
    soTransitionModuleBuilder<soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > m_transition;
    soArraySelectHolder<1, soArrayVector<soStatusUniqProcess*, 3>, soSingletonHolder<soArrayNull<soStatusUniqProcess*> > > m_processes;
    soArrayQueueImpl<soArrayVector<s32, 1> > m_requests;
    soStatusModuleImpl m_module;
public:
    soStatusModuleBuilder(soModuleAccesser* acc, const soStatusData* data, void* preCheck) :
        m_statusTable(data, 3), m_generalWork(), m_transition(), m_processes(), m_requests(-1),
        m_module(acc, &m_statusTable, m_processes.get(), m_generalWork.getModule(), m_transition.getModule(), &m_requests,
                 preCheck, 3, 1) { }
    soStatusModule* getModule() { return &m_module; }
};
static_assert(sizeof(soStatusModuleBuilder<wnMarioPumpStatusBuildConfig>) == 0x1C4, "F.L.U.D.D. status builder extent");

class wnMarioPumpModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<6, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soPostureModuleBuildConfig<1, soPostureModuleSimple> PostureModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soSituationModuleBuildConfigNull SituationModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnControllerModuleBuildConfig<soControllerModuleLinkRef, 0, 0> ControllerModuleBuildConfig;
    typedef wnMarioPumpStatusBuildConfig StatusModuleBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig;
    typedef wnMarioPumpArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 6, 0, 0, 1, 6, 0, 0, 1, 6, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};

// F.L.U.D.D. follows Mario and owns no movement energies. Keep this specialization
// local to its config so the water articles in the same unit retain their energies.
template <>
class wnWeaponBuilder<wnMarioPumpModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnMarioPumpModuleAccesserBuildConfig> m_modules;
    // HYPOTHESIS: trailing alignment of the animation-command storage, as in the no-common-energies builder.
    u32 m_modulesPad;
public:
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd,
                    bool weaponFlag = true) :
        Weapon(name, bd.m_kind, bd.m_data, articleId, &info, m_modules.getModuleAccesser(), weaponFlag), m_modules(this, bd) {
        Weapon::initialize();
    }
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() { wnDeactivateArticleDescendants(m_modules.m_generateArticleBuilder); }
};

class wnMarioPump : public wnWeaponBuilder<wnMarioPumpModuleAccesserBuildConfig> {
    const wnMarioPumpParam* m_param;
    wnMarioPumpParamAccesser m_paramAccesser;
public:
    wnMarioPump(s32 articleId, const wnArticleBuildInfo& info, const wnMarioPumpData* data);
    virtual ~wnMarioPump() { }
    void activate(int founderTaskId, int modelResourceId, int team, int etcResourceId);
    void setCharge(float charge);
    static bool activeArticle(wnMarioPumpWater*, soModuleAccesser*);
};
static_assert(sizeof(wnMarioPump) == 0x18A28, "Mario F.L.U.D.D. article layout");
