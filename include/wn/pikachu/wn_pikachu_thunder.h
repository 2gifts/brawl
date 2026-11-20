#pragma once

// Pikachu's Thunder bolt article (article kind 0xB).

#include <wn/wn_weapon_builder.h>
#include <so/status/so_status_module_impl.h>
#include <ft/pikachu/ft_pikachu_article_pools.h>

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnPikachuThunderParam {
    float unk0;
    float unk4; // the speed the bolt grows with (the generate interval is derived from it)
    float unk8; // the life of the bolt
};

struct wnPikachuThunderData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnPikachuThunderParam* m_param; // +0x34
};

class wnPikachuThunderModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 4, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<13, 6, 1> GeneralWorkBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
};

// The status unique process of the bolt.
class wnPikachuThunderStatusUniqProcessRegular : public soStatusUniqProcess {
public:
    virtual ~wnPikachuThunderStatusUniqProcessRegular() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execMapCorrection(soModuleAccesser* moduleAccesser);
};

// The Thunder bolt: one link of the chain a Thunder cloud hangs from the sky. Work int 0x10000005 is the cloud, 0x10000006 /
// 0x10000007 the previous and the next bolt of the chain, 0x10000008 the life of the bolt, 0x1000000A / 0x1000000B the handles
// of the two effects; work floats 0x11000001 / 0x11000002 are the upper and lower end of the bolt, 0x11000003 the end the chain
// grows to, 0x11000004 the scale; flag 0x12000003 is set once the end position of the chain is recorded, flag 0x12000000 is set while the bolt is active.
// HYPOTHESIS: all of the meanings above.
class wnPikachuThunder : public wnWeaponBuilder<wnPikachuThunderModuleAccesserBuildConfig> {
public:
    const wnPikachuThunderParam* m_param; // +0x217C
    wnPikachuThunder(s32 articleId, const wnArticleBuildInfo& info, const wnPikachuThunderData* data);
    virtual ~wnPikachuThunder() { }
    // HYPOTHESIS: the last argument selects whether the first effect (the head of the bolt) is requested.
    void activate(int founderTaskId, int resourceId, int team, const Vec3f* position, wnPikachuCloud* cloud, bool withHead);
    virtual void onDeactivate();
    void ref_log_group(bool chain);
    void unref_log_group(bool chain);
    virtual void notifyEventCollisionAttack(float power, const soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    void change_team(int team, int teamOwner);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void updateNodeSRT();
    static float getThunderModelSize();
    s32 getThunderGenerateInterval();
    bool hitcheck(Vec3f* position);
    void deactive_cloud();
    void rec_cloud(wnPikachuCloud* cloud);
    wnPikachuCloud* get_cloud();
    void chain_setting(wnPikachuThunder* last);
    void rec_next_thunder(wnPikachuThunder* thunder);
    wnPikachuThunder* get_next_thunder();
    void rec_prev_thunder(wnPikachuThunder* thunder);
    virtual void remove();
    wnPikachuThunder* get_prev_thunder();
    void chain_last(wnPikachuThunder* thunder);
    void remove_chain();
    void rec_end_pos(float y);
};
