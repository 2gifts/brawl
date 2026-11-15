#pragma once

// Fox's Blaster, the gun in the hand (article kind 30; the shots are wn/fox/wn_fox_blaster_bullet.h). Units that build
// or destroy the article (its own unit and the fighter module's article pool) define WN_WEAPON_BUILDER and see the
// complete class; the others only activate it and see the shell at the end of this file.

#include <wn/wn_weapon_builder.h>

struct wnArticleBuildInfo;

#ifdef WN_WEAPON_BUILDER
#include <wn/wn_extend_param_accesser.h>

// HYPOTHESIS: layout of the untouched part of the static article data (as the other articles' data blocks).
struct wnFoxBlasterData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const float* m_param; // +0x34, HYPOTHESIS: one float parameter
};

class wnFoxBlasterParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    wnFoxBlasterParamAccesser(const float* param) : wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(30) {
        m_floats[0] = param;
    }
#ifdef WN_ZELDA_LIGHTING_BOW_UNIT
    virtual ~wnFoxBlasterParamAccesser(); // BYTES: defined in the article unit, after the article constructor (emission order)
#else
    virtual ~wnFoxBlasterParamAccesser() { }
#endif
};
static_assert(sizeof(wnFoxBlasterParamAccesser) == 0x2C, "Fox Blaster parameter accesser extent");

// No kinetic, ground, collision, reflect or glow modules; a color blend module, three motions with one other-animation
// slot, three model event observers and the common four command threads (template arguments from the link map names
// of the builders the article's unit keeps).
class wnFoxBlasterModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 3, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1,
                                      soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > StatusModuleBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 1, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soReflectModuleBuildConfigNull ReflectModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0> AnimCmdModuleSubBuildConfig;
#ifdef WN_ZELDA_LIGHTING_BOW_UNIT
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
#endif
};

#ifdef WN_ZELDA_LIGHTING_BOW_UNIT
// The article's own unit (the sora_melee unit mode of wn/zelda/wn_zelda_lighting_bow.h): the generic builder's member
// layout, spelled out so that the constructor can be defined by hand (wn_zelda_lighting_bow_builder_ctor.h).
template <>
class soModuleAccesserBuilder<wnFoxBlasterModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnFoxBlasterModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    // The destructor is too big to expand in the weapon builder's: the unit keeps one weak copy.
    ~soModuleAccesserBuilder() { }
    soModuleAccesser* getModuleAccesser(); // BYTES: defined in wn_zelda_lighting_bow_builder_ctor.h (emission order)
};
#else
// The fighter module owns a copy of the article's destructor.
// The Blaster's status builder ends without the tail word (as the Illusion's in this unit); its teardown is out of line.
template <>
class soStatusModuleBuilder<wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > > {
    soArrayContractibleTable<const soStatusData> m_statusTable;
    soGeneralWorkBuilder<soGeneralWorkBuildConfig<2, 2, 1> > m_generalWorkBuilder;
    soTransitionModuleBuilder<soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > m_transitionBuilder;
    soArraySelectHolder<1, soArrayVector<soStatusUniqProcess*, 1>, soSingletonHolder<soArrayNull<soStatusUniqProcess*> > > m_uniqProcs;
    soArrayQueueImpl<soArrayVector<s32, 1> > m_changeRequests;
    soStatusModuleImpl m_module;
public:
    soStatusModuleBuilder(soModuleAccesser* acc, const soStatusData* statusData, void* preCheckData) :
        m_statusTable(statusData, 1),
        m_generalWorkBuilder(),
        m_transitionBuilder(),
        m_uniqProcs(),
        m_changeRequests(-1),
        m_module(acc, &m_statusTable, (soArray<soStatusUniqProcess*>*)m_uniqProcs.get(), m_generalWorkBuilder.getModule(),
                 m_transitionBuilder.getModule(), &m_changeRequests, preCheckData, 1, 1) { }
    // BYTES: the Blaster destructor calls this teardown out of line (the weak copy at 0x3CB8).
    ~soStatusModuleBuilder() __attribute__((never_inline)) { }
    soStatusModule* getModule() { return &m_module; }
};

// The Blaster has no kinetic module, so its builder adds none of the common energies.
template <>
class wnWeaponBuilder<wnFoxBlasterModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnFoxBlasterModuleAccesserBuildConfig> m_modules;
    // HYPOTHESIS: as in the Illusion's builder, the native module storage ends one word later than the generic builder.
    u32 m_modulesPad;
public:
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() {
        wnDeactivateArticleDescendants(this->m_modules.m_generateArticleBuilder);
    }
};
#endif

class wnFoxBlaster : public wnWeaponBuilder<wnFoxBlasterModuleAccesserBuildConfig> {
    const float* m_param;                        // +0x1D4C
    wnFoxBlasterParamAccesser m_paramAccesser; // +0x1D50
public:
    wnFoxBlaster(s32 articleId, const wnArticleBuildInfo& info, const wnFoxBlasterData* data);
#ifdef WN_ZELDA_LIGHTING_BOW_UNIT
    virtual ~wnFoxBlaster();
#else
    virtual ~wnFoxBlaster() { }
#endif
    // HYPOTHESIS: parameter types follow the fighter transactor's activeArticle1, which forwards its model and texture
    // resource ids and two flags; the founder task id comes first and the team last.
    void activate(int founderTaskId, u32 mdlResId, u32 texResId, bool unk1, bool unk2, int team);
    virtual void notifyEventLink(soLinkEventArgs* event, soModuleAccesser* moduleAccesser, StageObject* sender,
                                 int selector);
};

#else

struct wnFoxBlasterData;

class wnFoxBlaster : public Weapon {
    u8 m_unreconstructed[0x1D7C - sizeof(Weapon)];
public:
    wnFoxBlaster(s32 articleId, const wnArticleBuildInfo& info, const wnFoxBlasterData* data);
    void activate(int founderTaskId, u32 mdlResId, u32 texResId, bool unk1, bool unk2, int team);
    virtual void notifyEventLink(soLinkEventArgs* event, soModuleAccesser* moduleAccesser, StageObject* sender,
                                 int selector);
    virtual ~wnFoxBlaster();
};

#endif
static_assert(sizeof(wnFoxBlaster) == 0x1D7C, "Fox Blaster article extent");
