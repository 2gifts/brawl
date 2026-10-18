#pragma once

#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Sonic Build Configuration
////////////////////////////////////////

// Template arguments follow the module map's builder instance names.
typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftSonicInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftSonicHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftSonicParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftSonicResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftSonicModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<295, 497> ftSonicAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<497, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftSonicMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftSonicCollisionShieldModuleBuildConfig;
typedef soCollisionReflectorModuleBuildConfigGroups<3, 20, 2, soCollisionShieldEventPresenterReflector, soCollisionShieldModuleImpl> ftSonicCollisionReflectorModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<7, soLinkModuleImpl> ftSonicLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<295, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftSonicStatusModuleBuildConfig;
typedef soCollisionSearchModuleBuilder<soCollisionSearchModuleBuildConfig<soCollisionSearchModuleImpl> > ftSonicCollisionSearchModuleBuilder;

// HYPOTHESIS: this fighter has its own kinetic transactor (changeKinetic is in a later translation unit)
// Sonic's own updateEnergy<E>: with the ordinary kinetic types the energy is updated if it is enabled and not suspended;
// with the Sonic specific types (100-117, HYPOTHESIS: the spin dash / Super Sonic kinetic types) it is updated unconditionally,
// by the energy's ordinary updateEnergy or, for some energy and type pairs, by a type specific update that lives in sora_melee
// (the original passes it a one-byte tag object; HYPOTHESIS: an empty soIntToType<type> tag).
// MATCH-ONLY: the original builds one volatile marker byte per case on the stack.
class ftKineticEnergyGravity;
class ftKineticEnergyController;
class ftKineticEnergyStop;
extern "C" {
void fn_27_3BD860(ftKineticEnergyGravity* energy, u8* tag, soModuleAccesser* acc);
void fn_27_3BE2B0(ftKineticEnergyGravity* energy, u8* tag, soModuleAccesser* acc);
void fn_27_3BDDB4(ftKineticEnergyController* energy, u8* tag, soModuleAccesser* acc);
void fn_27_3BE568(ftKineticEnergyController* energy, u8* tag, soModuleAccesser* acc);
void fn_27_3BD5B8(ftKineticEnergyStop* energy, u8* tag, soModuleAccesser* acc);
void fn_27_3BD698(ftKineticEnergyStop* energy, u8* tag, soModuleAccesser* acc);
void fn_27_3BD914(ftKineticEnergyStop* energy, u8* tag, soModuleAccesser* acc);
void fn_27_3BDE6C(ftKineticEnergyStop* energy, u8* tag, soModuleAccesser* acc);
void fn_27_3BE364(ftKineticEnergyStop* energy, u8* tag, soModuleAccesser* acc);
}

struct ftSonicKineticNoTag { };
struct ftSonicKineticTag {
    u8 m_value;
};
template <class E, int Type>
struct ftSonicKineticUpdate {
    typedef ftSonicKineticNoTag Tag;
    static void run(E* energy, Tag&, soModuleAccesser* acc) { energy->updateEnergy(acc); }
};
#define FT_SONIC_KINETIC_SPECIAL(E, Type, Fn)                                                                  \
    template <>                                                                                                \
    struct ftSonicKineticUpdate<E, Type> {                                                                     \
        typedef ftSonicKineticTag Tag;                                                                         \
        static void run(E* energy, Tag& tag, soModuleAccesser* acc) { tag.m_value = 0; Fn(energy, &tag.m_value, acc); }         \
    }
FT_SONIC_KINETIC_SPECIAL(ftKineticEnergyGravity, 103, fn_27_3BD860);
FT_SONIC_KINETIC_SPECIAL(ftKineticEnergyGravity, 114, fn_27_3BE2B0);
FT_SONIC_KINETIC_SPECIAL(ftKineticEnergyController, 108, fn_27_3BDDB4);
FT_SONIC_KINETIC_SPECIAL(ftKineticEnergyController, 117, fn_27_3BE568);
FT_SONIC_KINETIC_SPECIAL(ftKineticEnergyStop, 100, fn_27_3BD5B8);
FT_SONIC_KINETIC_SPECIAL(ftKineticEnergyStop, 101, fn_27_3BD698);
FT_SONIC_KINETIC_SPECIAL(ftKineticEnergyStop, 104, fn_27_3BD914);
FT_SONIC_KINETIC_SPECIAL(ftKineticEnergyStop, 107, fn_27_3BDE6C);
FT_SONIC_KINETIC_SPECIAL(ftKineticEnergyStop, 115, fn_27_3BE364);
#define FT_SONIC_KINETIC_CASE(n) \
    case n: { \
        typename ftSonicKineticUpdate<E, n>::Tag tag; \
        volatile u8 marker = 0; \
        ftSonicKineticUpdate<E, n>::run(energy, tag, acc); \
        break; \
    }
class ftSonicKineticTransactor : public ftKineticTransactor {
public:
    static void changeKinetic(int mode, void* pools, soModuleAccesser* acc);
    template <typename E>
    static void updateEnergy(E* energy, soModuleAccesser* acc) {
        if (acc->getKineticModule().getKineticType() <= 99) {
            if (energy->isEnable() == true && !energy->isSuspend()) {
                energy->updateEnergy(acc);
            }
        } else {
            switch (acc->getKineticModule().getKineticType()) {
                FT_SONIC_KINETIC_CASE(100)
                FT_SONIC_KINETIC_CASE(101)
                FT_SONIC_KINETIC_CASE(102)
                FT_SONIC_KINETIC_CASE(103)
                FT_SONIC_KINETIC_CASE(104)
                FT_SONIC_KINETIC_CASE(105)
                FT_SONIC_KINETIC_CASE(106)
                FT_SONIC_KINETIC_CASE(107)
                FT_SONIC_KINETIC_CASE(108)
                FT_SONIC_KINETIC_CASE(109)
                FT_SONIC_KINETIC_CASE(110)
                FT_SONIC_KINETIC_CASE(111)
                FT_SONIC_KINETIC_CASE(112)
                FT_SONIC_KINETIC_CASE(113)
                FT_SONIC_KINETIC_CASE(114)
                FT_SONIC_KINETIC_CASE(115)
                FT_SONIC_KINETIC_CASE(116)
                FT_SONIC_KINETIC_CASE(117)
            }
        }
    }
};

// HYPOTHESIS: this fighter has its own kinetic transactor (changeKinetic is in a later translation unit)
typedef soKineticModuleBuildConfigMediator<soKineticModuleGenericImpl, ftKineticMediatorImplT<ftSonicKineticTransactor> > ftSonicKineticModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (wnSonicGimmickJump pools), not reconstructed yet
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x4FFC, Fighter_Sonic> ftSonicGenerateArticleManageModuleBuilder;

class ftSonicBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftSonicInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftSonicHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftSonicParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftSonicResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftSonicAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftSonicModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftSonicMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftSonicCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftSonicCollisionReflectorModuleBuildConfig CollisionReflectorModuleBuildConfig;
    typedef ftSonicLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftSonicStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftSonicCollisionSearchModuleBuilder CollisionSearchModuleBuilder;
    typedef ftSonicKineticModuleBuildConfig KineticModuleBuildConfig;
    typedef ftSonicGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnSonicGimmickJump;
class wnSonicSuperSonic;
struct ftData;

// The constructor's 0xD5D8 allocation fixes the size; the tail is the status table and the common data pointer (as for Lucas).
class ftSonic : public ftFighterBuilder<ftSonicBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusTable; // +0xD5C4
    ftData* m_commonData; // +0xD5D4
public:
    ftSonic(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual void onStart(int param);
    virtual void processCollision();
    virtual void processUpdate();
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionAttackFighter(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4);
    virtual void notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser);
    virtual void analyzeSeal(void* seal);
    virtual void notifyEventOnDamage(soDamage* damage, bool unk, soModuleAccesser* moduleAccesser);
    virtual void onSetMetal(bool metal);
    virtual void notifyEventChangeMotion(int motionKind, int prevMotionKind, void* motionData, soModuleAccesser* moduleAccesser);
    // The hooks the article mediator calls to start an article of the pool (HYPOTHESIS: names follow the module map).
    static bool activeArticle(wnSonicGimmickJump* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnSonicSuperSonic* weapon, soModuleAccesser* acc);
};
static_assert(sizeof(ftFighterBuilder<ftSonicBuildConfig>) == 0xD5C4, "Sonic status table offset is wrong");
static_assert(sizeof(ftSonic) == 0xD5D8, "Sonic allocation size is wrong!");
