#pragma once

// The articles of Snake that share the ladder module configuration, up to the grenade launcher sight. Split from
// ft/snake/ft_snake_article_pools.h so the sora_melee unit of the sight (which has a different unit mode than the fighter) can
// include them alone.

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/snake/wn_snake_status_builder.h>
#include <so/controller/so_controller_module_link_ref.h>

struct wnArticleBuildInfo;

// The one-status builder of the articles that move (the ladder and the grenade launcher parts). Unlike the C4 switch
// and the Nikita launcher (wn/snake/wn_snake_status_builder.h, followed by the empty null kinetic builder) it keeps the
// generic builder's tail word before their eight-energy kinetic builder. The native teardown of both is one function.
// HYPOTHESIS: a separate config type; both spellings tear down the same members.
struct wnSnakeOneStatusMoveBuildConfig : public wnSnakeOneStatusBuildConfig { };
// The generic builder with its tail word. Its process holder keeps the plain null-array spelling: with the singleton
// one (the C4 switch builder's) MWCC stops inlining the shared holder teardown in both builders.
// HYPOTHESIS: holder spelling of this stand-in type (natively both builders are one type).
template <>
class soStatusModuleBuilder<wnSnakeOneStatusMoveBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusTable;
    soGeneralWorkBuilder<soGeneralWorkBuildConfig<2, 2, 1> > m_generalWork;
    soTransitionModuleBuilder<soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > m_transition;
    soArraySelectHolder<1, soArrayVector<soStatusUniqProcess*, 1>, soArrayNull<soStatusUniqProcess*> > m_processes;
    soArrayQueueImpl<soArrayVector<s32, 1> > m_requests;
    soStatusModuleImpl m_module;
    u32 m_tail;
public:
    soStatusModuleBuilder(soModuleAccesser* acc, const soStatusData* data, void* preCheck) :
        m_statusTable(data, 1), m_generalWork(), m_transition(), m_processes(), m_requests(-1),
        m_module(acc, &m_statusTable, (soArray<soStatusUniqProcess*>*)m_processes.get(), m_generalWork.getModule(),
                 m_transition.getModule(), &m_requests, preCheck, 1, 1) { }
    soStatusModule* getModule() { return &m_module; }
};


class wnSnakeLadderModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<5, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnSnakeOneStatusMoveBuildConfig StatusModuleBuildConfig; // the common eight-energy kinetic module follows
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 5, 0, 0, 1, 5, 0, 0, 1, 5, 0, 0> AnimCmdModuleSubBuildConfig;
};

// The grenade launcher sight (parameter accesser kind 0x88). It is steered with the controller of its owner.
// HYPOTHESIS: meaning of the parameters.
struct wnSnakeRgb6SightParam {
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    float unk10;
    float unk14;
};
struct wnSnakeRgb6SightData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnSnakeRgb6SightParam* m_param; // +0x34
};

class wnSnakeRgb6SightParamAccesser : public wnExtendParamAccesser<3999, 6, 23999, 0, 0, 0> {
public:
    wnSnakeRgb6SightParamAccesser(const wnSnakeRgb6SightParam* param) : wnExtendParamAccesser<3999, 6, 23999, 0, 0, 0>(0x88) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
        m_floats[3] = &param->unkC;
        m_floats[4] = &param->unk10;
        m_floats[5] = &param->unk14;
    }
    virtual ~wnSnakeRgb6SightParamAccesser() { }
};

class wnSnakeRgb6SightModuleAccesserBuildConfig : public wnSnakeLadderModuleAccesserBuildConfig {
public:
    // HYPOTHESIS: two motions (the other-animation slots and the command tables are two).
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 2, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef wnControllerModuleBuildConfig<soControllerModuleLinkRef, 0, 0> ControllerModuleBuildConfig;
#ifdef WN_SNAKE_RGB6SIGHT_UNIT
    // BYTES: in its own unit the sight's status builder ends without the tail word (0x1BC bytes), as the C4 switch's one.
    typedef wnSnakeOneStatusBuildConfig StatusModuleBuildConfig;
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
#endif
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0> AnimCmdModuleSubBuildConfig;
};

#ifdef WN_SNAKE_RGB6SIGHT_UNIT // the sora_melee unit mode of the module accesser builder (as for the Samus bomb)
template <>
class soModuleAccesserBuilder<wnSnakeRgb6SightModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnSnakeRgb6SightModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser();
};
#endif

class wnSnakeRgb6Sight : public wnWeaponBuilder<wnSnakeRgb6SightModuleAccesserBuildConfig> {
public:
    const wnSnakeRgb6SightParam* m_param;         // +0x1F80
    wnSnakeRgb6SightParamAccesser m_paramAccesser; // +0x1F84

    wnSnakeRgb6Sight(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeRgb6SightData* data);
    virtual ~wnSnakeRgb6Sight() { }
    void activate(int founderTaskId, int finalResourceId, int team);
#ifdef WN_SNAKE_RGB6SIGHT_UNIT
    void enableOperate();
    void unableOperate();
    virtual void renderDebug();
    virtual void processUpdate();
    virtual void processFixPosition();
#endif
};
static_assert(sizeof(wnSnakeRgb6Sight) == 0x1FC4, "wnSnakeRgb6Sight layout is wrong!");
