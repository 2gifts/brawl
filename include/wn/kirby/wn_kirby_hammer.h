#pragma once
// wnKirbyHammer article (article kind 0x6).
#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>
WN_STATUS_PROC_VECTOR_ARG_CTOR(2);
// The static data block of the article (read by its wnModuleAccesserBuildData). HYPOTHESIS: layout of the untouched part.
struct wnKirbyHammerData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
};
class wnKirbyHammerModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soSituationModuleBuildConfigNull SituationModuleBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<2, soGeneralWorkBuildConfig<2, 2, 1>, 2, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigNoUniq<1, 2, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0> AnimCmdModuleSubBuildConfig;
};

// The hammer unit builds this article with WN_STATUS_NO_TAIL and WN_WEAPON_NO_COMMON_ENERGIES. Those macros rewrite the
// shared templates for a whole translation unit, so the fighter (which also pools articles that keep the tail and the
// common energies) specializes just this config. The article unit itself does not define FT_KIRBY_ARTICLE_POOLS.
#if defined(WN_WEAPON_BUILDER) && defined(FT_KIRBY_ARTICLE_POOLS)
template <>
class soStatusModuleBuilder<wnStatusModuleBuildConfig<2, soGeneralWorkBuildConfig<2, 2, 1>, 2, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > > {
    soArrayContractibleTable<const soStatusData> m_statusTable;
    soGeneralWorkBuilder<soGeneralWorkBuildConfig<2, 2, 1> > m_generalWorkBuilder;
    soTransitionModuleBuilder<soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > m_transitionBuilder;
    soArraySelectHolder<1, soArrayVector<soStatusUniqProcess*, 2>, soSingletonHolder<soArrayNull<soStatusUniqProcess*> > > m_uniqProcs;
    soArrayQueueImpl<soArrayVector<s32, 1> > m_changeRequests;
    soStatusModuleImpl m_module;
public:
    soStatusModuleBuilder(soModuleAccesser* acc, const soStatusData* statusData, void* preCheckData) :
        m_statusTable(statusData, 2),
        m_generalWorkBuilder(),
        m_transitionBuilder(),
        m_uniqProcs(),
        m_changeRequests(-1),
        m_module(acc, &m_statusTable, (soArray<soStatusUniqProcess*>*)m_uniqProcs.get(), m_generalWorkBuilder.getModule(),
                 m_transitionBuilder.getModule(), &m_changeRequests, preCheckData, 2, 1) { }
    soStatusModule* getModule() { return &m_module; }
};

template <>
class wnWeaponBuilder<wnKirbyHammerModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnKirbyHammerModuleAccesserBuildConfig> m_modules;
    u32 m_modulesPad;
public:
    // A member template so the module builders are instantiated in the hammer unit, which calls this, and not in the fighter.
    template <class BuildData>
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const BuildData& bd,
                    bool weaponFlag = true) :
        Weapon(name, bd.m_kind, bd.m_data, articleId, &info, this->m_modules.getModuleAccesser(), weaponFlag),
        m_modules(this, bd) {
        Weapon::initialize();
    }
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() {
        wnDeactivateArticleDescendants(this->m_modules.m_generateArticleBuilder);
    }
};
#endif

class wnKirbyHammer : public wnWeaponBuilder<wnKirbyHammerModuleAccesserBuildConfig> {
public:
    wnKirbyHammer(s32 articleId, const wnArticleBuildInfo& info, const wnKirbyHammerData* data);
    virtual ~wnKirbyHammer() { }
    void activate(int founderTaskId, int modelResourceId, int flag);
};
