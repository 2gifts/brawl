#pragma once

#include <ft/ft_common_data_accesser.h>
#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Ike Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftIkeInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftIkeHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftIkeParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftIkeResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftIkeModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<295, 500> ftIkeAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<500, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftIkeMotionModuleBuildConfig;
typedef soStatusModuleBuildConfig<295, soGeneralWorkBuildConfig<18, 19, 8>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftIkeStatusModuleBuildConfig;
typedef soCollisionSearchModuleBuilder<soCollisionSearchModuleBuildConfigParts<2, soCollisionSearchModuleImpl> > ftIkeCollisionSearchModuleBuilder;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (the sword pool). Only the fighter unit builds the real pool; the other
// units of the REL see an opaque block of the same size.
#ifdef FT_IKE_ARTICLE_POOLS
#include <ft/ike/ft_ike_article_pools.h>
typedef ftIkeArticleManageModuleBuilder ftIkeGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x2384, Fighter_Ike> ftIkeGenerateArticleManageModuleBuilder;
#endif

class ftIkeBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftIkeInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftIkeHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftIkeParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftIkeResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftIkeAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftIkeModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftIkeMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftIkeStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftIkeCollisionSearchModuleBuilder CollisionSearchModuleBuilder;
    typedef ftIkeGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnIkeSword;

class ftIke : public ftFighterBuilder<ftIkeBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusDataTable;
    ftData* m_data;
public:
    ftIke(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual void onStart(int startKind);
    // The hooks the article mediator calls to start and to throw the sword (HYPOTHESIS: names follow the module map).
    static bool activeArticle(wnIkeSword* weapon, soModuleAccesser* acc);
    static bool shootArticle(wnIkeSword* weapon, soModuleAccesser* acc);
};
