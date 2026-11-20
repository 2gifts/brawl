#pragma once

#include <wn/wn_weapon_builder.h>

struct wnArticleBuildInfo;

#ifdef WN_KIRBY_PIKMIN_UNIT
// The article's own unit (the sora_melee unit mode of wn/zelda/wn_zelda_lighting_bow.h) sees the complete class.
#include <wn/wn_extend_param_accesser.h>
#include <so/so_array.h>

// HYPOTHESIS: the hit status element of the article's table (same spelling as in wn/pikmin/wn_pikmin_pikmin.h, which this unit does not include).
enum soCollisionHitStatus { soCollisionHitStatus_Unk0 };

// HYPOTHESIS: layout of the static article data; only the observed offsets are named.
struct wnKirbyPikminData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const acAnimCmdConv** m_animCmdTable[3][5]; // +0x34: the animation command lists of each group and variation
    float (*m_speedTable)[7]; // +0x70
    const float* m_param;     // +0x74
};

class wnKirbyPikminParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    wnKirbyPikminParamAccesser(const float* param) : wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(150) {
        m_floats[0] = param;
    }
    virtual ~wnKirbyPikminParamAccesser(); // BYTES: defined in the article unit, after the article constructor
};
static_assert(sizeof(wnKirbyPikminParamAccesser) == 0x2C, "Kirby's Pikmin parameter accesser extent");

// HYPOTHESIS: every argument follows the template names of the original's builder functions.
class wnKirbyPikminModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<4, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<2, 2, 1>, 3,
                                      soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 4, 0, 0, 1, 4, 0, 0, 1, 4, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
};

template <>
class soModuleAccesserBuilder<wnKirbyPikminModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnKirbyPikminModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser(); // BYTES: defined in wn_zelda_lighting_bow_builder_ctor.h (emission order)
};

class wnKirbyPikmin : public wnWeaponBuilder<wnKirbyPikminModuleAccesserBuildConfig> {
    const float* m_param;                                            // +0x2014
    wnKirbyPikminParamAccesser m_paramAccesser;                      // +0x2018
    const wnKirbyPikminData* m_data;                                 // +0x2044
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable0;  // +0x2048
    soAnimCmdAddressPackArraySeparate m_animCmdPack0;                // +0x2058
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable1;  // +0x2074
    soAnimCmdAddressPackArraySeparate m_animCmdPack1;                // +0x2084
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable2;  // +0x20A0
    soAnimCmdAddressPackArraySeparate m_animCmdPack2;                // +0x20B0
    soArrayContractibleTable<soCollisionHitStatus> m_hitStatusTable; // +0x20CC
public:
    wnKirbyPikmin(s32 articleId, const wnArticleBuildInfo& info, const wnKirbyPikminData* data);
    virtual ~wnKirbyPikmin();
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    bool reflectRequest(Vec2f* speedMul, Vec2f* accelMul);
    void shoot(Vec3f* position, float lr);
    // HYPOTHESIS: the argument meanings (the variation, the start position, the founder task, the facing, the motion rate, the
    // Kirby resource id of Olimar and the team number) follow the registers of the transactor's call.
    void activate(int variation, Vec3f* position, int founderTaskId, u32 resourceId, int teamNo, float lr, float motRate);
};
static_assert(sizeof(wnKirbyPikmin) == 0x20DC, "Kirby's Pikmin article extent");

#else

// HYPOTHESIS: the article data type the class info fetches for kind 150; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family's data role).
struct wnKirbyPikminData;

// Extent known from the class info createArray allocation (0x20DC); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnKirbyPikmin : public Weapon {
    union {
        u8 m_storage[0x20DC - sizeof(Weapon)];
        struct {
            u8 unkCC[0x2044 - sizeof(Weapon)];
            struct {
                u8 unk0[0x70];
                float (*unk70)[7];
            }* unk2044;
        };
    };
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnKirbyPikmin(s32 articleId, const wnArticleBuildInfo& info, const wnKirbyPikminData* data);
    virtual ~wnKirbyPikmin();
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    bool reflectRequest(Vec2f* speedMul, Vec2f* accelMul);
    void shoot(Vec3f* position, float lr);
    // HYPOTHESIS: the argument meanings (the variation, the start position, the founder task, the facing, the motion rate, the
    // Kirby resource id of Olimar and the team number) follow the registers of the transactor's call.
    void activate(int variation, Vec3f* position, int founderTaskId, u32 resourceId, int teamNo, float lr, float motRate);
};
static_assert(sizeof(wnKirbyPikmin) == 0x20DC, "Kirby's Pikmin article extent");

#endif
