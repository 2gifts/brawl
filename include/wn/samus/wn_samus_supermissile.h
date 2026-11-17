#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the parameter block of the super missile; meaning of the fields; only the int at +0x8 is read here.
struct wnSamusSuperMissileParam {
    float unk0;
    s32 unk4; // HYPOTHESIS: the life set into work int 0x10000005 when the missile starts (status init reads it as an int)
    s32 unk8; // HYPOTHESIS: the life renewed into work int 0x10000006 after a reflect or hop
    float unkC; // HYPOTHESIS: the starting speed, times the facing direction (status init)
    float unk10;
    float unk14;
};

// HYPOTHESIS: the static data block of the article; layout of the untouched part.
struct wnSamusSuperMissileData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnSamusSuperMissileParam* m_param; // +0x34
};

struct wnArticleBuildInfo;

#if defined(WN_WEAPON_BUILDER) && defined(WN_MARIO_FIREBALL_UNIT) // the article unit (a fighter unit with builders sees the call-side view)
// BYTES: the sora_melee unit mode of the module accesser builder is shared with the Light Bow (wn/zelda/wn_zelda_lighting_bow.h).
#define WN_ZELDA_LIGHTING_BOW_UNIT
#include <wn/zelda/wn_zelda_lighting_bow.h>
#include <wn/wn_anim_cmd_full_property.h>
#include <wn/wn_inside_event_all_units.h>

// The missile's configuration (the same module builders, its own builder copies).
class wnSamusSuperMissileModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<12, 18, 0, 1, 0, 0, 4, 4, 1, 0, 1, 0, 0, 0, 0, 0, 0, 6, 0, wnAllInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef wnAllInsideEventManageModuleTypes InsideEventManageModuleTypes;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
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
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
};

template <>
class soModuleAccesserBuilder<wnSamusSuperMissileModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnSamusSuperMissileModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser();
};

class wnSamusSuperMissile : public wnWeaponBuilder<wnSamusSuperMissileModuleAccesserBuildConfig> {
    const wnSamusSuperMissileParam* m_param; // +0x1920, copied from the data block (HYPOTHESIS: not read in this unit)
#else
// Call-side view keeps the native extent without instantiating the builder.
class wnSamusSuperMissile : public Weapon {
    u8 m_storage[0x1924 - sizeof(Weapon)];
#endif
public:
    wnSamusSuperMissile(s32 articleId, const wnArticleBuildInfo& info, const wnSamusSuperMissileData* data);
#if defined(WN_WEAPON_BUILDER) && defined(WN_MARIO_FIREBALL_UNIT)
    virtual ~wnSamusSuperMissile() { }
#else
    virtual ~wnSamusSuperMissile();
#endif
    void activate(int founderTaskId, int etcResourceId, int team);
    const wnSamusSuperMissileData* getData();
    void toShoot(float lr, float speed);
    virtual void updateNodeSRT();
    virtual bool notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* moduleAccesser, int index);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual bool reflect();
    virtual bool hop();
};
static_assert(sizeof(wnSamusSuperMissile) == 0x1924, "Samus super missile article extent");
