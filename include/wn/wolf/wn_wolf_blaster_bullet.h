#pragma once

// Wolf's Blaster shot (article kind in the class info of wn_wolf_blaster_bullet.cpp). Units that build or destroy the article
// (its own unit and the fighter module's article pool) define WN_WEAPON_BUILDER and see the complete class; the others
// only activate it and see the shell at the end of this file.
#ifdef WN_WEAPON_BUILDER

#include <wn/wn_module_accesser_builder.h>
#include <wn/wn_extend_param_accesser.h>

FT_DOL_ARRAY_VECTOR(soCollisionAttackPart, 4);

struct wnWolfBlasterBulletParam {
    s32 unk0; // Native activation copies this into the lifetime field.
    float unk4;
    float unk8;
};
struct wnWolfBlasterBulletData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnWolfBlasterBulletParam* m_param;
};
class wnWolfBlasterBulletParamAccesser : public wnExtendParamAccesser<3999, 2, 23999, 1, 0, 0> {
public:
    explicit wnWolfBlasterBulletParamAccesser(const wnWolfBlasterBulletParam* param) :
        wnExtendParamAccesser<3999, 2, 23999, 1, 0, 0>(39) {
        m_floats[0] = &param->unk4;
        m_floats[1] = &param->unk8;
        m_ints[0] = &param->unk0;
    }
#ifdef WN_ZELDA_LIGHTING_BOW_UNIT
    virtual ~wnWolfBlasterBulletParamAccesser(); // BYTES: defined in the article unit, after the article constructor (emission order)
#else
    virtual ~wnWolfBlasterBulletParamAccesser() { }
#endif
};
static_assert(sizeof(wnWolfBlasterBulletParamAccesser) == 0x34, "Wolf bullet parameter accesser extent");

class wnWolfBlasterBulletModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16,32,4,2,4,4,4,4,2,4,4,4,4,4,1,1,0,0,0,
                                                  wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soResourceModuleBuildConfigNull ResourceModuleBuildConfig;
    typedef soModelModuleBuildConfig<5,0,soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2,soMotionModuleImpl,0,0,
        soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
        soMotionAnimObjCacheModuleBuildConfig<0,soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter,4,0,soCollisionAttackModuleImpl,1,false,true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5,soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<2,soGeneralWorkBuildConfig<2,2,1>,2,
        soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1,2,0,0,1,2,0,0,1,2,0,0,1,2,0,0> AnimCmdModuleSubBuildConfig;
#ifdef WN_ZELDA_LIGHTING_BOW_UNIT
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
#endif
};

#ifdef WN_ZELDA_LIGHTING_BOW_UNIT
// The sora_melee unit mode of the article (wn/zelda/wn_zelda_lighting_bow.h): the generic builder's member layout, spelled
// out so that the constructor can be defined by hand in the unit (wn_zelda_lighting_bow_builder_ctor.h).
template <>
class soModuleAccesserBuilder<wnWolfBlasterBulletModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnWolfBlasterBulletModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser(); // BYTES: defined in wn_zelda_lighting_bow_builder_ctor.h (emission order)
};
#endif
static_assert(sizeof(soModuleAccesserBuilder<wnWolfBlasterBulletModuleAccesserBuildConfig>) == 0x1A44,
              "Wolf bullet module builder extent");
static_assert(sizeof(wnWeaponBuilder<wnWolfBlasterBulletModuleAccesserBuildConfig>) == 0x213C,
              "Wolf bullet weapon builder extent");

class wnWolfBlasterBullet : public wnWeaponBuilder<wnWolfBlasterBulletModuleAccesserBuildConfig> {
    const wnWolfBlasterBulletParam* m_param;
    wnWolfBlasterBulletParamAccesser m_paramAccesser;
    s32 unk2174;
public:
    wnWolfBlasterBullet(s32 articleId, const wnArticleBuildInfo& info, const wnWolfBlasterBulletData* data);
#ifdef WN_ZELDA_LIGHTING_BOW_UNIT
    virtual ~wnWolfBlasterBullet();
#else
    virtual ~wnWolfBlasterBullet() { }
#endif
    // HYPOTHESIS: spelling of the unused second integer and mode parameter.
    void activate(int founderTaskId, int unk5, int team, const Vec3f* pos, int unkMode, EfID effectId,
                  float lr, float speed, float angle);
    virtual bool reflect();
    virtual float getCollisionLr(soModuleAccesser* moduleAccesser);
    virtual void processUpdate();
    virtual void updateNodeSRT();
private:
    void updateStretch();
};
static_assert(sizeof(wnWolfBlasterBullet) == 0x2178, "Wolf bullet allocation extent");

#else

#include <wn/wn_weapon_builder.h>

class wnWolfBlasterBullet : public Weapon {
    u8 m_unreconstructed[0x2178 - sizeof(Weapon)];
public:
    void activate(int founderTaskId, int unk5, int team, const Vec3f* pos, int unkMode, EfID effectId,
                  float lr, float speed, float angle);
};
static_assert(sizeof(wnWolfBlasterBullet) == 0x2178, "Wolf bullet allocation extent");

#endif
