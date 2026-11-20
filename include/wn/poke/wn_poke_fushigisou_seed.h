#pragma once

#include <wn/wn_weapon_builder.h>

#ifdef WN_POKE_FUSHIGISOU_SEED_UNIT // the article unit builds the real class; every other unit sees the call-side view below
#include <wn/wn_extend_param_accesser.h>
#include <wn/zelda/wn_zelda_lighting_bow.h>
#include <wn/wn_anim_cmd_full_property.h>
#include <wn/wn_inside_event_all_units.h>

struct wnArticleBuildInfo;

// The status unique process array of this article is the shared null array (the article never registers a process), not a vector.
template <>
class soArraySelectHolder<1, soArrayVector<soStatusUniqProcess*, 3>, soSingletonHolder<soArrayNull<soStatusUniqProcess*> > > {
    soSingletonHolder<soArrayNull<soStatusUniqProcess*> > m_holder;
public:
    soArraySelectHolder() { }
    soArray<soStatusUniqProcess*>* get() { return soSingletonHolder<soArrayNull<soStatusUniqProcess*> >::getInstance(); }
    ~soArraySelectHolder() { }
};

// An effect module with no continual or screen arrays of its own (the null singletons are handed in); only the word list is built.
// HYPOTHESIS: source spelling of the selection (the map name has the arguments 0,1,0,0,-1).
template <typename T>
class soEffectModuleBuilder<wnEffectModuleBuildConfig<0, 1, 0, 0, -1, T> > {
    u8 unk0[4];
    soArrayVector<u32, 1> m_u32s; // +0x4
    T m_effectModule;             // +0x14
public:
    soEffectModuleBuilder(soModuleAccesser* acc, void* nodeData, void* emitData, void* commonData, void* screenData,
                          soEventObserverRegistrationDesc* regDesc) :
        m_u32s(1, 0, 0),
        m_effectModule(acc, soSingletonHolder<soArrayNull<soEffectContinual> >::getInstance(), nodeData, &m_u32s,
                       soSingletonHolder<soArrayNull<soEffectTime> >::getInstance(), emitData, regDesc, commonData, -1,
                       screenData, soSingletonHolder<soArrayNull<efScreenHandle> >::getInstance()) { }
    // BYTES: the original destroys this builder through an out-of-line destructor (the Pokemon Trainer's articles share it).
    ~soEffectModuleBuilder() __attribute__((never_inline)) { }
    T* getModule() { return &m_effectModule; }
};

// HYPOTHESIS: parameter block of the seed: one int and one float; names follow the offsets of the native code.
struct wnPokeFushigisouSeedParam {
    s32 unk0;
    float unk4;
};

// HYPOTHESIS: layout of the untouched part of the static article data (as the other articles' data blocks).
struct wnPokeFushigisouSeedData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnPokeFushigisouSeedParam* m_param; // +0x34
};

class wnPokeFushigisouSeedParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 1, 0, 0> {
public:
    explicit wnPokeFushigisouSeedParamAccesser(const wnPokeFushigisouSeedParam* param) :
        wnExtendParamAccesser<3999, 1, 23999, 1, 0, 0>(74) {
        m_floats[0] = &param->unk4;
        m_ints[0] = &param->unk0;
    }
    virtual ~wnPokeFushigisouSeedParamAccesser() { }
};
static_assert(sizeof(wnPokeFushigisouSeedParamAccesser) == 0x30, "Pokemon seed parameter accesser extent");

class wnPokeFushigisouSeedModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    // The capacities beyond the eleventh come from the event unit table of the original constructor (only the turn unit has any).
    typedef soInsideEventManageModuleBuildConfig<10, 18, 0, 1, 0, 0, 4, 4, 0, 0, 1, 0, 0, 0, 0, 0, 0, 6, 0,
                                                 wnAllInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef wnAllInsideEventManageModuleTypes InsideEventManageModuleTypes;
    typedef soModelModuleBuildConfig<1, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfigNull,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soSoundModuleBuildConfigNull SoundModuleBuildConfig;
    typedef soResourceModuleBuildConfigNull ResourceModuleBuildConfig;
    typedef wnKineticModuleBuildConfigSized<wnKineticModuleImpl, soKineticMediatorNull, 2> KineticModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<10, 6, 1> GeneralWorkBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfigNull, 3,
                                      soTransitionModuleBuildConfig<soTypeList<soIntToType<1>, soTypeListNullType> > >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigFullProperty<1, 3, 0, 0, 1, 1, 3, 0, 0, 2, 1, 3, 0, 0, 2, 1, 3, 0, 0, 2>
        AnimCmdModuleSubBuildConfig;
    typedef wnEffectModuleBuildConfig<0, 1, 0, 0, -1, soEffectModuleImpl> EffectModuleBuildConfig;
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
};

template <>
class soModuleAccesserBuilder<wnPokeFushigisouSeedModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnPokeFushigisouSeedModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd) __attribute__((never_inline));
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser();
};

class wnPokeFushigisouSeed : public wnWeaponBuilder<wnPokeFushigisouSeedModuleAccesserBuildConfig> {
    const wnPokeFushigisouSeedParam* m_param;          // +0x1674
    wnPokeFushigisouSeedParamAccesser m_paramAccesser; // +0x1678
public:
    wnPokeFushigisouSeed(s32 articleId, const wnArticleBuildInfo& info, const wnPokeFushigisouSeedData* data);
    virtual ~wnPokeFushigisouSeed() { }
    // HYPOTHESIS: names; the transactor passes the article kind flag, the unused word is -1 there.
    void activate(int founderTaskId, int team, Vec3f* position, int unk, int kind, float lr, float speed);
    virtual void onDeactivate();
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
};
static_assert(sizeof(wnPokeFushigisouSeed) == 0x16A8, "Pokemon seed article extent");
#else
// HYPOTHESIS: the article data type the class info fetches for kind 74; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family data role).
struct wnPokeFushigisouSeedData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x16A8); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnPokeFushigisouSeed : public Weapon {
    u8 m_storage[0x16A8 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info createArray can construct articles.
    wnPokeFushigisouSeed(s32 articleId, const wnArticleBuildInfo& info, const wnPokeFushigisouSeedData* data);
    virtual ~wnPokeFushigisouSeed();
    virtual void onDeactivate();
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
};
static_assert(sizeof(wnPokeFushigisouSeed) == 0x16A8, "Pokemon seed article extent");
#endif
