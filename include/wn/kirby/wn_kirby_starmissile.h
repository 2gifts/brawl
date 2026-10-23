#pragma once

// Kirby's star missile article (article kind 8).

#include <wn/wn_weapon_builder.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <so/link/so_link_event_presenter.h>
#include <so/status/so_status_module_impl.h>

// The parameter block of the article.
// HYPOTHESIS: meaning of the parameters.
struct wnKirbyStarmissileParam {
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    float unk10;
    float unk14;
    float unk18;
    float unk1C;
    float unk20;
    float unk24;
    float unk28;
    float unk2C;
    float unk30; // gravity (negated)
    float unk34; // fall speed limit (negated)
    float unk38;
    float unk3C;
    float unk40;
    float unk44;
    s32 unk48;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnKirbyStarmissileData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnKirbyStarmissileParam* m_param; // +0x34
};

class wnKirbyStarmissileModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<2, soGeneralWorkBuildConfig<3, 3, 1>, 2, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnKirbyStarmissile : public wnWeaponBuilder<wnKirbyStarmissileModuleAccesserBuildConfig> {
public:
    const wnKirbyStarmissileParam* m_param; // +0x1FC0
private:
    wnKineticEnergyGravity m_energyGravity; // +0x1FC4
public:
    wnKirbyStarmissile(s32 articleId, const wnArticleBuildInfo& info, const wnKirbyStarmissileData* data);
    virtual ~wnKirbyStarmissile() { }
    virtual void onDeactivate() { }
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    // MATCH-ONLY: the status process was its own original object (wn_kirby_starmissile_status_uniq_process.o) and calls
    // this out of line.
    const wnKirbyStarmissileData* getData() __attribute__((never_inline));
    // HYPOTHESIS: parameter names beyond the position, facing and mode. Mode 0 flies straight, mode 1 along an angle, mode 2 is
    // the stationary star.
    void activate(int founderTaskId, int resourceId, int team, const Vec3f* position, float lr, float unk, int unk2, int mode);
};

// HYPOTHESIS: the link event (kind 0x1A) the missile sends through link 0 when it bounces off a surface (m_lr is the
// bounce direction) and when its status ends.
struct soLinkEventStarShot : soLinkEventArgs {
    soLinkEventStarShot(int eventKind) : soLinkEventArgs(eventKind), m_lr(0.0f), unk10(false) { }
    float m_lr;
    u8 unkC[4];
    bool unk10;
};

class wnKirbyStarmissileStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnKirbyStarmissileStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};
