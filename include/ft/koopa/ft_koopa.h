#pragma once

#include <ft/ft_common_data_accesser.h>
#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Koopa Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftKoopaInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftKoopaHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftKoopaParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftKoopaResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftKoopaModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<291, 483> ftKoopaAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<483, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftKoopaMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftKoopaCollisionShieldModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> ftKoopaLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<291, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftKoopaStatusModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (the breath pool). Only the fighter unit builds the real pool; the other
// units of the REL see an opaque block of the same size.
#ifdef FT_KOOPA_ARTICLE_POOLS
#include <ft/koopa/ft_koopa_article_pools.h>
typedef ftKoopaArticleManageModuleBuilder ftKoopaGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0xBDFC, Fighter_Koopa> ftKoopaGenerateArticleManageModuleBuilder;
#endif

class ftKoopaBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftKoopaInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftKoopaHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftKoopaParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftKoopaResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftKoopaAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftKoopaModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftKoopaMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftKoopaCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftKoopaLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftKoopaStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftKoopaGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnKoopaBreath;

class ftKoopa : public ftFighterBuilder<ftKoopaBuildConfig> {
    u8 unkTail[0x1416C - sizeof(ftFighterBuilder<ftKoopaBuildConfig>)];
    soArrayContractibleTable<const soStatusData> m_statusDataTable;
    ftData* m_data;
public:
    ftKoopa(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual void onStart(int startKind);
    // The hook the article mediator calls to start an article of the pool (HYPOTHESIS: the name follows the module map).
    static bool activeArticle(wnKoopaBreath* weapon, soModuleAccesser* acc);
};
