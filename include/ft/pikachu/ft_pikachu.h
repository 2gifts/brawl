#pragma once

#include <ft/ft_common_data_accesser.h>
#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Pikachu Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftPikachuInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftPikachuHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftPikachuParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftPikachuResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftPikachuModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<288, 486> ftPikachuAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<486, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftPikachuMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftPikachuCollisionShieldModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> ftPikachuLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<288, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftPikachuStatusModuleBuildConfig;

// HYPOTHESIS: this fighter has its own kinetic transactor (updateEnergy and changeKinetic)
class ftPikachuKineticTransactor : public ftKineticTransactor {
public:
    static void changeKinetic(int mode, void* pools, soModuleAccesser* acc);
    // The Quick Attack aiming mode (0x64): the controller energy only accelerates with the stick (see updateEnergySub).
    static void changeKineticSub(bool* result, void* pools, soModuleAccesser* acc);
    // HYPOTHESIS: sets the speed of the energy from the regulated stick position.
    static void updateEnergySub(soKineticEnergyNormal* energy, void* unused, soModuleAccesser* acc);
    template <typename E>
    static void updateEnergy(E* energy, soModuleAccesser* acc) {
        if (energy->isEnable() != true) {
            return;
        }
        if (energy->isSuspend()) {
            return;
        }
        return energy->updateEnergy(acc);
    }
};
typedef soKineticModuleBuildConfigMediator<soKineticModuleGenericImpl, ftKineticMediatorImplT<ftPikachuKineticTransactor> > ftPikachuKineticModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (the article pools). Only the fighter unit builds the real pools; the other
// units of the REL see an opaque block of the same size.
#ifdef FT_PIKACHU_ARTICLE_POOLS
#include <ft/pikachu/ft_pikachu_article_pools.h>
typedef ftPikachuArticleManageModuleBuilder ftPikachuGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x22AE8, Fighter_Pikachu> ftPikachuGenerateArticleManageModuleBuilder;
#endif

class ftPikachuBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftPikachuInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftPikachuHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftPikachuParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftPikachuResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftPikachuAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftPikachuModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftPikachuMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftPikachuCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftPikachuLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftPikachuStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftPikachuKineticModuleBuildConfig KineticModuleBuildConfig;
    typedef ftPikachuGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class soCameraModule;
class wnPikachuThunderJolt;
class wnPikachuCloud;
class wnPikachuVortex;
class wnPikachuThunder;

class ftPikachu : public ftFighterBuilder<ftPikachuBuildConfig> {
    u8 unkTail[0x2AE34 - sizeof(ftFighterBuilder<ftPikachuBuildConfig>)];
    soArrayContractibleTable<const soStatusData> m_statusDataTable;
    ftData* m_data;
public:
    ftPikachu(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual void onActivate();
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionAttackFighter(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    void** getExtendParam();
    static void* getThunderParam();
    void rec_cloud_pointer(wnPikachuCloud* cloud);
    // The cloud article of Thunder is kept in a work int of the fighter (HYPOTHESIS: names follow the module map).
    void notify_thunder_reflect();
    void adjust_camera_subject(soCameraModule* source);
    static wnPikachuCloud* get_cloud_pointer(soModuleAccesser* acc);
    static void rec_cloud_pointer1(soModuleAccesser* acc, wnPikachuCloud* cloud);
    // The hooks the article mediator calls to start an article of the pool (HYPOTHESIS: names follow the module map).
    static bool activeArticle(wnPikachuThunderJolt* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnPikachuCloud* weapon, soModuleAccesser* acc);
    static bool activeArticle2(wnPikachuVortex* weapon, soModuleAccesser* acc);
    static bool activeArticle3(wnPikachuThunder* weapon, soModuleAccesser* acc);
};
