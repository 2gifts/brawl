#pragma once
#include <so/anim/so_anim_obj.h>
#include <wn/wn_module_accesser_builder.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/gamewatch/wn_game_watch_octopus_link_event_collision.h>
#include <wn/gamewatch/wn_game_watch_octopus_status_uniq_process_common.h>

struct wnGameWatchOctopusParam {
    float unk0;
    float unk4;
    float unk8;
};

class wnGameWatchOctopusParamAccesser : public wnExtendParamAccesser<3999, 3, 23999, 0, 0, 0> {
public:
    explicit wnGameWatchOctopusParamAccesser(const wnGameWatchOctopusParam* param) :
        wnExtendParamAccesser<3999, 3, 23999, 0, 0, 0>(122) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
    }
    virtual ~wnGameWatchOctopusParamAccesser() { }
};
static_assert(sizeof(wnGameWatchOctopusParamAccesser) == 0x34, "Octopus parameter accesser extent");

struct wnGameWatchOctopusData {
    s32 m_resGroupNo;              // +0x00; read by native constructor
    u8 unk4[0x30];                 // +0x04..+0x33
    const wnGameWatchOctopusParam* m_param;
    soSet<soCollisionHitData::Simple>* m_hitData;
};

class wnGameWatchOctopusModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                  wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<7, soMotionModuleImpl, 0, 1,
                                      soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 8, 0,
                                               soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 4, 1,
                                            soCollisionHitModuleImpl, 0x3ff, true>
        CollisionHitModuleBuildConfig;
    // Native map/constructor identify soDamageModuleImpl with one soDamage array and singleton null
    // transactor/effector holders. Existing article-side declaration spells this wnDamageModuleBuildConfig.
    typedef wnDamageModuleBuildConfig<soDamageModuleImpl, 1,
                                      soSingletonHolder<soDamageTransactorNull>,
                                      soSingletonHolder<soDamageEffectorNull> > DamageModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<4, soGeneralWorkBuildConfig<2, 2, 2>, 4,
                                      soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 4, 0, 0,
                                                1, 7, 0, 0,
                                                1, 7, 0, 0,
                                                1, 7, 0, 0> AnimCmdModuleSubBuildConfig;
};


class wnGameWatchOctopus : public wnWeaponBuilder<wnGameWatchOctopusModuleAccesserBuildConfig> {
    const wnGameWatchOctopusParam* m_param;
    wnGameWatchOctopusParamAccesser m_paramAccesser;
    soAnimObj m_animObj;
public:
    wnGameWatchOctopus(s32 articleId, const wnArticleBuildInfo& info, const wnGameWatchOctopusData* data);
    virtual ~wnGameWatchOctopus() { }
    // HYPOTHESIS: source reference qualification; the caller passes a position pointer.
    void activate(int founderTaskId, int resourceId, int team, const Vec3f& position,
                  float lr, float unkFloatFromDB2, float fighterColor);
    virtual void deactivate(bool force);
    virtual void updateNodeSRT();
    virtual void notifyEventCollisionAttack(float power, soCollisionLog*, soModuleAccesser*);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventCollisionHit(float power, soCollisionAttackData* attackData, u32 index, int unk6,
                                         soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* other, int unk4);
    virtual void adjustParentGroundCollision(int unk1, float* collisionData);
};
static_assert(sizeof(wnWeaponBuilder<wnGameWatchOctopusModuleAccesserBuildConfig>) == 0x28BC, "Octopus builder extent");
// HYPOTHESIS: trailing article storage is unverified; these declarations model the observed members.
