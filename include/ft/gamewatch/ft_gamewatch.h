#pragma once

#include <ft/ft_common_data_accesser.h>
#include <ft/ft_fighter_builder.h>
#include <so/anim/so_anim_obj.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// GameWatch Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftGameWatchInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftGameWatchHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftGameWatchParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftGameWatchResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftGameWatchModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<313, 514> ftGameWatchAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<514, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftGameWatchMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftGameWatchCollisionShieldModuleBuildConfig;
typedef soCollisionReflectorModuleBuildConfigGroups<3, 20, 2, soCollisionShieldEventPresenterReflector, soCollisionShieldModuleImpl> ftGameWatchCollisionReflectorBaseModuleBuildConfig;
// HYPOTHESIS: the absorber builder (part kind 4) follows the reflector builder
typedef soCollisionShieldModuleBuildConfigGroups<4, 1, 1, soCollisionShieldEventPresenterAbsorber, soCollisionShieldModuleImpl> ftGameWatchCollisionAbsorberModuleBuildConfig;
typedef soCollisionReflectorModuleBuildConfigWithAbsorber<ftGameWatchCollisionReflectorBaseModuleBuildConfig, ftGameWatchCollisionAbsorberModuleBuildConfig> ftGameWatchCollisionReflectorModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> ftGameWatchLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<313, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftGameWatchStatusModuleBuildConfig;

// HYPOTHESIS: this fighter has its own kinetic transactor (changeKinetic is in a later translation unit)
FT_KINETIC_TRANSACTOR(ftGameWatchKineticTransactor);
typedef soKineticModuleBuildConfigMediator<soKineticModuleGenericImpl, ftKineticMediatorImplT<ftGameWatchKineticTransactor, ftKineticTransactor> > ftGameWatchKineticModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (the article pools). Only the fighter unit builds the real pools; the other
// units of the REL see an opaque block of the same size.
#ifdef FT_GAMEWATCH_ARTICLE_POOLS
#include <ft/gamewatch/ft_gamewatch_article_pools.h>
typedef ftGameWatchArticleManageModuleBuilder ftGameWatchGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x1771C, Fighter_GameWatch> ftGameWatchGenerateArticleManageModuleBuilder;
#endif

class ftGameWatchBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftGameWatchInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftGameWatchHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftGameWatchParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftGameWatchResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftGameWatchAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftGameWatchModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftGameWatchMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftGameWatchCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftGameWatchCollisionReflectorModuleBuildConfig CollisionReflectorModuleBuildConfig;
    typedef ftGameWatchLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftGameWatchStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftGameWatchKineticModuleBuildConfig KineticModuleBuildConfig;
    typedef ftGameWatchGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnGameWatchSausage;
class wnGameWatchRescue;
class wnGameWatchParachute;
class wnGameWatchPanel;
class wnGameWatchOil;
class wnGameWatchOctopus;
class wnGameWatchNormalWeapon;

class ftGameWatch : public ftFighterBuilder<ftGameWatchBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusDataTable; // 0x1FDA8, 0x10 bytes
    ftData* m_data;
    soAnimObj m_animObj;
public:
    ftGameWatch(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    // The hooks the article mediator calls to start an article of the pool (HYPOTHESIS: names follow the module map).
    static bool activeArticle(wnGameWatchSausage* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnGameWatchRescue* weapon, soModuleAccesser* acc);
    static bool activeArticle2(wnGameWatchParachute* weapon, soModuleAccesser* acc);
    static bool activeArticle3(wnGameWatchPanel* weapon, soModuleAccesser* acc);
    static bool activeArticle4(wnGameWatchOil* weapon, soModuleAccesser* acc);
    static bool activeArticle5(wnGameWatchOctopus* weapon, soModuleAccesser* acc);
    static bool activeArticle6(wnGameWatchNormalWeapon* weapon, soModuleAccesser* acc);
};
static_assert(sizeof(ftGameWatch) == 0x1FDE4, "Class is the wrong size!");
