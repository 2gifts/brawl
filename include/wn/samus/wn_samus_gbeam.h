#pragma once

// Samus's grapple beam (article kind 0x84, a lasso article like the Zero Suit whip: its statuses are the shared wnLasso ones). The
// article is built in sora_melee. Three views: the article unit (WN_WEAPON_BUILDER + WN_MARIO_FIREBALL_UNIT: module builder
// out of line), Samus's fighter unit (WN_WEAPON_BUILDER alone: the generic builder, whose weak destructor chain the fighter
// module keeps), and the call-side view (neither: the native extent only).

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_lasso_status_view.h>
#ifdef WN_WEAPON_BUILDER
#ifdef WN_MARIO_FIREBALL_UNIT
// BYTES: the sora_melee unit mode of the module accesser builder (member list macro, null builders, situation builder
// shadow) is shared with the Light Bow, see wn/zelda/wn_zelda_lighting_bow.h.
#define WN_ZELDA_LIGHTING_BOW_UNIT
#include <wn/zelda/wn_zelda_lighting_bow.h>
#endif
#include <wn/wn_anim_cmd_full_property.h>
#include <wn/wn_inside_event_all_units.h>
#endif

struct wnArticleBuildInfo;

// HYPOTHESIS: the lasso status view of wn/wn_lasso_status_view.h with slot 0xB8 taking an int (the beam calls it with 1). A copy of
// the class so that this unit does not depend on a change of the shared header: its slotB8() is declared without parameters.
class wnSamusGBeamStatusView : public wnLassoViewPad8 {
public:
    virtual void slot08();
    virtual void slot0C();
    virtual void slot10();
    virtual void slot14();
    virtual void unk18Attach(u32 archiveId, int, u32, u32, u32, int, int, int);
    virtual void slot1C();
    virtual void slot20();
    virtual void slot24();
    virtual void slot28();
    virtual void slot2C();
    virtual void unk30Int(int);
    virtual void slot34();
    virtual void slot38();
    virtual void slot3C();
    virtual void slot40();
    virtual void slot44();
    virtual void slot48();
    virtual void slot4C();
    virtual void slot50();
    virtual void slot54();
    virtual void slot58();
    virtual void slot5C();
    virtual void unk60Int(int);
    virtual void unk64Int(int);
    virtual void unk68VecPair(Vec3f* a, Vec3f* b);
    virtual void slot6C();
    virtual int unk70Int();
    virtual float unk74Float(int);
    virtual void slot78();
    virtual void slot7C();
    virtual void slot80();
    virtual void slot84();
    virtual int unk88Int();
    virtual void unk8CInt(int);
    virtual void slot90();
    virtual void slot94();
    virtual void slot98();
    virtual void slot9C();
    virtual void slotA0();
    virtual void unkA4Int(int);
    virtual int unkA8Int();
    virtual void slotAC();
    virtual void unkB0VecInt(Vec3f* pos, int, bool);
    virtual void unkB4Int(int);
    virtual void unkB8Int(int);
    virtual void unkBCInt(int);
    virtual void slotC0();
    virtual void slotC4();
    virtual int unkC8Int(int);
    virtual void unkCCVec(int, Vec3f* pos);
    virtual void slotD0();
    virtual int unkD4Int();
    virtual void slotD8();
    virtual void unkDCFloat(float);
    virtual int unkE0Int();
};

// HYPOTHESIS: all field names; the offsets follow the native code (parameter block read through +0x34 of the static data).
struct wnSamusGBeamParam {
    float unk0;
    s32 unk4;
    s32 unk8;
    float unkC;
    float unk10;
    float unk14; // read by activate
};

struct wnSamusGBeamData {
    s32 m_resGroupNo; // the constructor reads the low byte of this first word
    u8 unk4[0x30];
    const wnSamusGBeamParam* m_param; // +0x34
};

class wnSamusGBeamParamAccesser : public wnExtendParamAccesser<3999, 4, 23999, 2, 0, 0> {
public:
    wnSamusGBeamParamAccesser(const wnSamusGBeamParam* param) : wnExtendParamAccesser<3999, 4, 23999, 2, 0, 0>(0x84) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unkC;
        m_floats[2] = &param->unk10;
        m_floats[3] = &param->unk14;
        m_ints[0] = &param->unk4;
        m_ints[1] = &param->unk8;
    }
    virtual ~wnSamusGBeamParamAccesser();
};
#if defined(WN_WEAPON_BUILDER) && !defined(WN_MARIO_FIREBALL_UNIT)
inline wnSamusGBeamParamAccesser::~wnSamusGBeamParamAccesser() { } // the article unit defines it in place (emission order)
#endif

#ifdef WN_WEAPON_BUILDER
class wnSamusGBeamModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<12, 18, 0, 1, 0, 0, 4, 4, 1, 0, 1, 0, 0, 0, 0, 0, 0, 6, 0, wnAllInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef wnAllInsideEventManageModuleTypes InsideEventManageModuleTypes;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<10, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfigNull,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<9, soGeneralWorkBuildConfig<3, 3, 1>, 9, soTransitionModuleBuildConfig<soTypeList<soIntToType<2>, soTypeListNullType> > >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigFullProperty<1, 9, 0, 0, 2, 1, 10, 0, 0, 2, 1, 10, 0, 0, 2, 1, 10, 0, 0, 2>
        AnimCmdModuleSubBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    // HYPOTHESIS: the lasso articles keep their rope state in the physics module slot (see wn/wn_lasso_status_view.h).
    typedef soPhysicsModuleBuildConfigCap<0, soPhysicsModuleImpl> PhysicsModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
#ifdef WN_MARIO_FIREBALL_UNIT
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
#endif
};

// The beam's status builder: the generic one, except that it gives the status module no pre-check animation commands (the
// count after the pre-check data is 0, as in the other Samus articles and Sheik's needles, where most articles pass the
// status kind count).
// HYPOTHESIS: the original config carries this count as one more template argument (the map name is not known).
// BYTES: a partial specialization (the transition config stays a parameter), not an explicit one: its members are then
// emitted like the generic template's, getModule after the motion builder's and the constructor after it, as in the original;
// the members of an explicit specialization come out before the template getters.
template <typename TransitionConfig>
class soStatusModuleBuilder<wnStatusModuleBuildConfig<9, soGeneralWorkBuildConfig<3, 3, 1>, 9, TransitionConfig> > {
    typedef wnStatusModuleBuildConfig<9, soGeneralWorkBuildConfig<3, 3, 1>, 9, TransitionConfig> BC;
    soArrayContractibleTable<const soStatusData> m_statusTable;
    soGeneralWorkBuilder<typename BC::GeneralWorkBuildConfig> m_generalWorkBuilder;
    soTransitionModuleBuilder<typename BC::TransitionBuildConfig> m_transitionBuilder;
    soArraySelectHolder<1, soArrayVector<soStatusUniqProcess*, BC::StatusKindCap>, soSingletonHolder<soArrayNull<soStatusUniqProcess*> > > m_uniqProcs;
    soArrayQueueImpl<soArrayVector<s32, 1> > m_changeRequests;
    soStatusModuleImpl m_module;
public:
    soStatusModuleBuilder(soModuleAccesser* acc, const soStatusData* statusData, void* preCheckData) :
        m_statusTable(statusData, BC::DataCount),
        m_generalWorkBuilder(),
        m_transitionBuilder(),
        m_uniqProcs(),
        m_changeRequests(-1),
        m_module(acc, &m_statusTable, (soArray<soStatusUniqProcess*>*)m_uniqProcs.get(), m_generalWorkBuilder.getModule(),
                 m_transitionBuilder.getModule(), &m_changeRequests, preCheckData, 0, 1) { }
    soStatusModule* getModule() { return &m_module; }
};

#ifdef WN_MARIO_FIREBALL_UNIT
// BYTES: the beam's animation command sub builder constructor comes after the module accesser builder constructor and its
// weak copies in the original unit (right before activate). The shared template (wn_module_accesser_builder.h) defines the
// constructor in the class, which emits it right after the article constructor; this specialization has the same members
// and only declares it, and wn_samus_gbeam.cpp defines it in place (as the Toad does, wn/peach/wn_peach_kinopio.h).
template <>
class wnAnimCmdModuleSubBuilder<wnSamusGBeamModuleAccesserBuildConfig::AnimCmdModuleSubBuildConfig> {
    typedef wnSamusGBeamModuleAccesserBuildConfig::AnimCmdModuleSubBuildConfig BC;
    wnAnimCmdControlUnitSelect<BC::Unit0Enabled != 0, BC::Unit0Config>::Type m_unit0; // +0
    wnAnimCmdControlUnitSelect<BC::Unit1Enabled != 0, BC::Unit1Config>::Type m_unit1;
    wnAnimCmdControlUnitSelect<BC::Unit2Enabled != 0, BC::Unit2Config>::Type m_unit2;
    wnAnimCmdControlUnitSelect<BC::Unit3Enabled != 0, BC::Unit3Config>::Type m_unit3;
    wnAnimCmdUniqSelect<BC::Unit4Enabled != 0, BC::Unit4Config>::Type m_unit4;
    soAnimCmdControlUnitBuilderNull<void> m_unit5;
public:
    // BYTES: a standalone constructor in sora_melee (as the shared template has it), not inlined into the article's.
    wnAnimCmdModuleSubBuilder(soModuleAccesser* acc, const soModuleAccesserBuildData& bd) __attribute__((never_inline));
    ~wnAnimCmdModuleSubBuilder() { }
};

template <>
class soModuleAccesserBuilder<wnSamusGBeamModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnSamusGBeamModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser(); // BYTES: defined in the unit (emission order)
};
#endif

class wnSamusGBeam : public wnWeaponBuilder<wnSamusGBeamModuleAccesserBuildConfig> {
#else
// Call-side view keeps the native extent without instantiating the builder.
class wnSamusGBeam : public Weapon {
    u8 m_storage[0x1748 - sizeof(Weapon)];
#endif
    const wnSamusGBeamParam* m_param;            // +0x1748
    wnSamusGBeamParamAccesser m_paramAccesser;   // +0x174C
public:
    wnSamusGBeam(s32 articleId, const wnArticleBuildInfo& info, const wnSamusGBeamData* data);
#if defined(WN_WEAPON_BUILDER) && !defined(WN_MARIO_FIREBALL_UNIT)
    virtual ~wnSamusGBeam() { }
#else
    virtual ~wnSamusGBeam();
#endif
    // HYPOTHESIS: parameter names; attack is set by the fighter for the attack variants of the statuses (0x34, 0x36, 0x38).
    void activate(int founderTaskId, int etcResourceId, int team, float lr, const Vec3f* pos, bool attack);
};
static_assert(sizeof(wnSamusGBeam) == 0x178C, "Samus grapple beam article extent");
