#pragma once
// wnSamusBomb: Samus's morph ball bomb (article kind 0x54). The class is built in sora_melee; the fighter module sees the
// call-side view below. The bomb watches its own search collision and falls with a gravity energy.
#include <wn/wn_weapon_builder.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <so/collision/so_collision_search_event_presenter.h>

struct wnArticleBuildInfo;

// The parameter block of the article. HYPOTHESIS: names; the offsets follow the native code.
struct wnSamusBombParam {
    s32 m_life; // +0x00: copied to work int 0x10000005 when the bomb is set
    u8 unk4[0xC];
    float m_gravity;    // +0x10
    float m_speedLimit; // +0x14
    float m_fallSpeed;  // +0x18: vertical speed the gravity energy starts with
    float m_bounceScale;    // +0x1C: speed kept by a bounce
    float m_minBounceSpeed; // +0x20: slower than this the bomb comes to rest
};

// The static data block of the article. HYPOTHESIS: layout of the untouched part.
struct wnSamusBombData {
    s32 m_resGroupNo; // the constructor reads the low byte of this first word
    u8 unk4[0x30];
    const wnSamusBombParam* m_param; // +0x34
};

#if defined(WN_WEAPON_BUILDER) && defined(WN_MARIO_FIREBALL_UNIT) // the article unit (a fighter unit with builders sees the call-side view)
// BYTES: the sora_melee unit mode of the module accesser builder is shared with the Light Bow (wn/zelda/wn_zelda_lighting_bow.h).
#define WN_ZELDA_LIGHTING_BOW_UNIT
#include <wn/zelda/wn_zelda_lighting_bow.h>
#include <wn/wn_anim_cmd_full_property.h>
#include <wn/wn_inside_event_all_units.h>
#include <wn/wn_collision_search_module_builder.h>

class wnSamusBombModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<12, 20, 0, 1, 0, 0, 4, 4, 1, 0, 1, 0, 0, 3, 0, 0, 0, 6, 0, wnAllInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef wnAllInsideEventManageModuleTypes InsideEventManageModuleTypes;
    typedef soModelModuleBuildConfig<1, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfigNull,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<2, soGeneralWorkBuildConfig<2, 2, 1>, 2, soTransitionModuleBuildConfig<soTypeList<soIntToType<2>, soTypeListNullType> > >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigFullProperty<1, 2, 0, 0, 2, 1, 2, 0, 0, 2, 1, 2, 0, 0, 2, 1, 2, 0, 0, 2> AnimCmdModuleSubBuildConfig;
    typedef wnKineticModuleBuildConfigSized<wnKineticModuleImpl, soKineticMediatorNull, 3> KineticModuleBuildConfig; // three energies
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
};

// The article has a real search collision builder; this unit owns its code.
template <>
struct wnArticleSearchSelect<wnSamusBombModuleAccesserBuildConfig> {
    typedef soCollisionSearchModuleBuilder<wnCollisionSearchModuleBuildConfigArticle<soCollisionSearchModuleImpl> > Builder;
    typedef u8 Category;
};

template <>
class soModuleAccesserBuilder<wnSamusBombModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnSamusBombModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser();
};

class wnSamusBomb : public wnWeaponBuilder<wnSamusBombModuleAccesserBuildConfig>, public soCollisionSearchEventObserver {
#else
// Call-side view keeps the native extent without instantiating the builder.
// Call-side view keeps the native extent without instantiating the builder.
class wnSamusBomb : public Weapon {
    u8 m_storage[0x1A74 - sizeof(Weapon)];
    u8 m_observerStorage[0xC];
#endif
public:
    const wnSamusBombParam* m_param;        // +0x1A80
    wnKineticEnergyGravity m_energyGravity; // +0x1A84: the fall (energy 2)

    wnSamusBomb(s32 articleId, const wnArticleBuildInfo& info, const wnSamusBombData* data);
#if defined(WN_WEAPON_BUILDER) && defined(WN_MARIO_FIREBALL_UNIT)
    virtual ~wnSamusBomb() { }
#else
    virtual ~wnSamusBomb();
#endif
    void activate(int founderTaskId, int etcResourceId, int team);
    const wnSamusBombData* getData();
    void toShoot(float lr, float x, float y, float z);
#if defined(WN_WEAPON_BUILDER) && defined(WN_MARIO_FIREBALL_UNIT)
    virtual void updateNodeSRT();
    virtual bool notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* moduleAccesser, int index);
    virtual bool notifyEventCollisionSearchCheck();
    virtual void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool reflect();
    virtual bool hop();
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    // The two halves of a rebound (inlined): all the speed goes to the movement energy, then it is split again, scaled.
    void gatherSpeed();
    void splitBounceSpeed();
#endif
};
static_assert(sizeof(wnSamusBomb) == 0x1A98, "Samus bomb article extent");
