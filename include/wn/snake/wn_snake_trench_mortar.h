#pragma once

// Snake's Trench Mortar launcher (article kind 19) and the shells it pools (wnSnakeTrenchMortarBullet, three at a time).
// The unit that owns the launcher's constructor and the shell's destructor includes this header after defining
// WN_WEAPON_BUILDER and the other builder macros (see src/mo_melee/sora_melee/wn/wn_snake_trench_mortar.cpp); it does not
// go together with wn/snake/wn_snake_trench_mortar_bullet.h (the opaque shell).

#include <so/collision/so_collision_search_event_presenter.h>
#include <wn/wn_weapon_builder.h>
#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/wn_article_manage_module.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/weapon.h>
#include <ft/ft_common_data_accesser.h>
#include <wn/snake/wn_snake_mine.h>

class Vec3f;
class soArticle;

// HYPOTHESIS: meaning of the parameter block beyond the gravity values the shell constructor reads (+0x18, +0x1C).
struct wnSnakeTrenchMortarBulletParam {
    u8 unk0[0x18];
    float m_gravity;    // +0x18 (negated)
    float m_speedLimit; // +0x1C
};

// HYPOTHESIS: the static data block of the shell (same role as the other articles').
struct wnSnakeTrenchMortarBulletData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnSnakeTrenchMortarBulletParam* m_param; // +0x34
};

class wnSnakeTrenchMortarBulletModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soPostureModuleBuildConfig<3, soPostureModuleImpl> PostureModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soSituationModuleBuildConfigNull SituationModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soCollisionSearchModuleBuilder<soCollisionSearchModuleBuildConfigWeapon<soCollisionSearchModuleImpl> >
        CollisionSearchModuleBuilder;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soStopModuleBuildConfigNull StopModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 1> VisibilityModuleBuildConfig;
    typedef soCameraModuleBuildConfigNoSubject<soCameraModuleSimple> CameraModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<2, 2, 1>, 3, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef soReflectModuleBuildConfig<1, soReflectModuleImpl> ReflectModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnSnakeTrenchMortarBullet : public wnWeaponBuilder<wnSnakeTrenchMortarBulletModuleAccesserBuildConfig>,
                                  public soCollisionSearchEventObserver {
public:
    const wnSnakeTrenchMortarBulletParam* m_param; // +0x21FC
    u32 unk2200;
    const wnSnakeTrenchMortarBulletParam* unk2204; // HYPOTHESIS: the parameter block again
    float unk2208;
    Vec3f unk220C;
    wnKineticEnergyGravity m_energyGravity; // +0x2218: kinetic energy 2
    float unk222C;

    // The constructor lives in the shell's own unit.
    wnSnakeTrenchMortarBullet(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeTrenchMortarBulletData* data);
    virtual ~wnSnakeTrenchMortarBullet() { }
    // HYPOTHESIS: parameter meanings; the mortar's generate hands the founder, the etc resource id, the team, the muzzle
    // position and direction, and the two floats the mortar keeps (+0x7CE0, +0x7CE4). The body is in the shell's unit.
    void activate(int founderTaskId, int etcResourceId, int team, Vec3f* pos, Vec3f* dir, float unk1, float unk2);
    virtual void updateNodeSRT();
    virtual void processHit();
    virtual void processCollision();
    void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    bool notifyEventCollisionSearchCheck();
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
};
static_assert(sizeof(wnSnakeTrenchMortarBullet) == 0x2230, "wnSnakeTrenchMortarBullet layout is wrong!");

// ---- the shell pool of the mortar --------------------------------------------------------------------------------------
// The three shells are built in place, one holder each, from the owner's kind pair, heap and the shell data block.
typedef soKindInfoGeneric<2, 19> wnSnakeTrenchMortarKindInfo;
typedef soInstancePoolInfo<wnSnakeTrenchMortarBullet, 3, wnInstanceHolder, soIntToType<0> > wnSnakeTrenchMortarBulletPoolInfo;
typedef soTypeList<wnSnakeTrenchMortarBulletPoolInfo, soTypeListNullType> wnSnakeTrenchMortarBulletPoolList;

template <>
class wnInstanceHolder<wnSnakeTrenchMortarBullet, wnSnakeTrenchMortarKindInfo, soIntToType<0> > {
public:
    virtual ~wnInstanceHolder() { }
    // HYPOTHESIS: as the Toad's spore holder (wn/peach/wn_peach_kinopio.h): the owner is reached twice through a
    // reference dynamic_cast, the build info is constructed in place from the owner's kind pair and heap module.
    wnInstanceHolder(soModuleAccesser* acc) :
        m_instance(0, BuildInfo(&owner(acc).getKindInfo(), &acc->getHeapModule()),
                   static_cast<const wnSnakeTrenchMortarBulletData*>(
                       g_ftCommonDataAccesser.getWeaponData18(static_cast<ftKind>(ownerFighterKind(owner(acc))), &DataFlag(false).value))) { }
    wnSnakeTrenchMortarBullet* getEnergy() { return &m_instance; }
    wnSnakeTrenchMortarBullet* getInstance() { return &m_instance; }
private:
    struct BuildInfo : wnArticleBuildInfo {
        BuildInfo(const wnSimpleKindInfo* kindInfo, soHeapModule* heapModule) {
            this->kindInfo = reinterpret_cast<const wnArticleKindInfo*>(kindInfo);
            this->heapModule = heapModule;
        }
    };
    struct DataFlag {
        bool value;
        DataFlag(bool v) : value(v) { }
    };
    static Weapon& owner(soModuleAccesser* acc) { return dynamic_cast<Weapon&>(acc->getStageObject()); }
    static s32 ownerFighterKind(const Weapon& weapon) { return weapon.getKindInfo().subKind; }
    wnSnakeTrenchMortarBullet m_instance;
};

// The end of the sub pool chain: no shell.
template <class E>
class wnSnakeTrenchMortarPoolEnd {
    u32 m_terminal;
public:
    wnSnakeTrenchMortarPoolEnd(soModuleAccesser*) { }
    E* getInstanceAt(s32) { return NULL; }
    template <class Predicate>
    E* find(Predicate&) { return NULL; }
};

template <class Info, class Next>
struct wnSnakeTrenchMortarPoolPrev {
    typedef soInstancePoolSub<typename Info::PrevInfo, Next> Type;
};
template <class E, template <class, class, class> class H, class Init, class Next>
struct wnSnakeTrenchMortarPoolPrev<soInstancePoolInfo<E, 1, H, Init>, Next> {
    typedef wnSnakeTrenchMortarPoolEnd<E> Type;
};

// Sub pool of N shells: the pool of the first N - 1 shells, then the holder of the Nth.
template <class E, int N, template <class, class, class> class H, class Init>
class soInstancePoolSub<soInstancePoolInfo<E, N, H, Init>, wnSnakeTrenchMortarKindInfo> {
    typedef soInstancePoolInfo<E, N, H, Init> Info;
    typedef wnSnakeTrenchMortarKindInfo Next;
public:
    virtual ~soInstancePoolSub() { }
private:
    typename wnSnakeTrenchMortarPoolPrev<Info, Next>::Type m_next;
    H<E, Next, Init> m_holder;
public:
    soInstancePoolSub(soModuleAccesser* acc) : m_next(acc), m_holder(acc) { }
    E* getInstanceAt(s32 index) {
        if (index == N - 1) {
            return m_holder.getInstance();
        }
        return m_next.getInstanceAt(index);
    }
    template <class Predicate>
    E* find(Predicate& predicate) {
        if (predicate(&static_cast<soArticle&>(*m_holder.getInstance())) == true) {
            return m_holder.getInstance();
        }
        return m_next.find(predicate);
    }
    template <class Predicate, int L>
    E* find(Predicate& predicate, const soIntToType<L>&) { return find(predicate); }
};


template <>
class soInstancePool<wnSnakeTrenchMortarBulletPoolInfo, soInstancePoolRoot, wnSnakeTrenchMortarKindInfo, 0> : public soInstancePoolRoot {
public:
    typedef soInstancePoolSub<wnSnakeTrenchMortarBulletPoolInfo, wnSnakeTrenchMortarKindInfo> SubType;
    virtual ~soInstancePool() { }
private:
    SubType m_sub;
public:
    soInstancePool(soModuleAccesser* acc) : soInstancePoolRoot(acc), m_sub(acc) { }
    wnSnakeTrenchMortarBullet* getInstanceAt(s32 index) { return m_sub.getInstanceAt(index); }
    template <class Predicate, int L>
    wnSnakeTrenchMortarBullet* find(Predicate& predicate, const soIntToType<L>& tag) { return m_sub.find(predicate, tag); }
};

template <class TL, template <class, class, class, int> class Unit, class Root, class KindInfo, int Depth>
class soLineHierarchy;
template <class H, template <class, class, class, int> class Unit, class Root, class KindInfo, int Depth>
class soLineHierarchy<soTypeList<H, soTypeListNullType>, Unit, Root, KindInfo, Depth> : public Unit<H, Root, KindInfo, Depth> {
public:
    soLineHierarchy(soModuleAccesser* acc) : Unit<H, Root, KindInfo, Depth>(acc) { }
    ~soLineHierarchy() { }
};

// The mediator of the shell pool (soArticleMediatorImpl over the pool type list).
template <class PoolTL, class AssignTL, class KindInfo, class Extra>
class soArticleMediatorImpl : public soArticleMediator {
public:
    typedef soLineHierarchy<PoolTL, soInstancePool, soInstancePoolRoot, KindInfo, 0> PoolHierarchy;
    soArticleMediatorImpl(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~soArticleMediatorImpl() { }
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc);
    virtual bool isGeneratable(soModuleAccesser* acc, s32 articleId);
    virtual s32 getActiveNum(soModuleAccesser* acc, s32 articleId);
    virtual s32 getGenerateMaxNum(s32 articleId);
    virtual bool shoot(soModuleAccesser* acc, soArticle* article);
    virtual void deactivate();
    virtual s32 getMediateNum() { return 1; }
    virtual void setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }
private:
    template <int S, int L>
    inline soArticle* generateSlot(soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&);
    template <int S, int L>
    inline bool isGeneratableSlot(const soIntToType<S>&, const soIntToType<L>&);
    template <int S, int L>
    inline s32 getActiveNumSlot(const soIntToType<S>&, const soIntToType<L>&);
    PoolHierarchy m_pools;
    bool m_autoRecycle;
};

struct soArticleMediatorAssignmentNull { };
template <class Founder, class ShootAssignment>
struct soArticleMediatorAssignmentInfo {
    typedef Founder FounderType;
};
class wnSnakeTrenchMortar;

// HYPOTHESIS: the assignment list of the mediator (the founder is the launcher).
typedef soArticleMediatorImpl<wnSnakeTrenchMortarBulletPoolList,
                              soTypeList<soArticleMediatorAssignmentInfo<wnSnakeTrenchMortar, soArticleMediatorAssignmentNull>, soTypeListNullType>,
                              wnSnakeTrenchMortarKindInfo, soTypeListNullType>
    wnSnakeTrenchMortarArticleMediator;

template <class Mediator>
class soGenerateArticleManageModuleBuildConfig {
public:
    typedef Mediator MediatorType;
    enum { ArticleCount = 2 };
};

template <>
class soSelectInstanceHolder<wnSnakeTrenchMortarArticleMediator, soArticleMediatorNull> {
    wnSnakeTrenchMortarArticleMediator m_mediator;
public:
    soSelectInstanceHolder(soModuleAccesser* acc) : m_mediator(acc) { }
    ~soSelectInstanceHolder() { }
    soArticleMediator* getMediator() { return &m_mediator; }
};

// BYTES: the builder constructor expands the two-entry observer list constructor: __construct_array over 16 byte
// observers (the full class, SO_ARTICLE_EVENT_OBSERVER_FULL) and postInitialize. As the Toad's builder does for one entry.
template <>
class soArrayVector<soArticleEventObserver, 2> : public soArrayVectorAbstract<soArticleEventObserver> {
    s32 m_topIndex : sizeof(bit_width<2>) + 1;
    s32 m_lastIndex : sizeof(bit_width<2>) + 1;
    s32 m_size : sizeof(bit_width<2>) + 1;
    u32 m_isFull : 1;
    soArticleEventObserver m_elements[2];
public:
    soArrayVector(s32 size, s32) : m_topIndex(0), m_lastIndex(0), m_size(size), m_isFull(false) {
        soArrayVectorCalculator::postInitialize(*this, size, 2);
    }
    virtual s32 size() const;
    virtual ~soArrayVector() { }
    virtual s32 capacity() const;
    virtual bool isFull() const;
    virtual soArticleEventObserver& atFastAbstractSub(s32 index) const;
    virtual soArticleEventObserver& getArrayValueConst(s32 index);
    virtual s32 getTopIndex() const;
    virtual s32 getLastIndex() const;
    virtual void setSize(s32 size);
    virtual void setTopIndex(s32 topIndex);
    virtual void setLastIndex(s32 lastIndex);
    virtual void onFull();
    virtual void offFull();
};

// The article-side manage module: the native teardown destroys it as a base subobject.
class wnSnakeTrenchMortarGenerateArticleManageModuleImpl : public soGenerateArticleManageModuleImpl {
public:
    wnSnakeTrenchMortarGenerateArticleManageModuleImpl(soModuleAccesser* acc, soArray<soArticle*>* articles,
                                                       soArticleMediator* mediator, soArray<soArticleEventObserver>* observers) :
        soGenerateArticleManageModuleImpl(acc, articles, mediator, observers) { }
};

template <class BC>
class soGenerateArticleManageModuleBuilder;
template <class Mediator>
class soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfig<Mediator> > {
    typedef soGenerateArticleManageModuleBuildConfig<Mediator> BC;
    soArrayVector<soArticle*, BC::ArticleCount> m_articles;
    soArrayVector<soArticleEventObserver, BC::ArticleCount> m_observers;
    soSelectInstanceHolder<Mediator, soArticleMediatorNull> m_mediator;
    wnSnakeTrenchMortarGenerateArticleManageModuleImpl m_module;
public:
    soGenerateArticleManageModuleBuilder(soModuleAccesser* acc) __attribute__((never_inline)); // defined in the unit, where the original emits it
    ~soGenerateArticleManageModuleBuilder() { }
    void* getModule() { return &m_module; }
    soArticleMediator* getMediator() { return m_mediator.getMediator(); }
};

typedef soGenerateArticleManageModuleBuildConfig<wnSnakeTrenchMortarArticleMediator> wnSnakeTrenchMortarArticleBuildConfig;
typedef soGenerateArticleManageModuleBuilder<wnSnakeTrenchMortarArticleBuildConfig> wnSnakeTrenchMortarArticleManageModuleBuilder;

// ---- the launcher ------------------------------------------------------------------------------------------------------
struct wnSnakeTrenchMortarData {
    s32 m_resGroupNo;
};

class wnSnakeTrenchMortarModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soSituationModuleBuildConfigNull SituationModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soStatusModuleBuildConfigNull StatusModuleBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig;
    typedef wnSnakeTrenchMortarArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
    typedef wnAnimCmdModuleSubBuildConfigNoUniq<0, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0> AnimCmdModuleSubBuildConfig;
};

template <>
class wnWeaponBuilder<wnSnakeTrenchMortarModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnSnakeTrenchMortarModuleAccesserBuildConfig> m_modules;
    // HYPOTHESIS: as in the C4 switch builder; the native disabled command units take a word each.
    u32 m_modulesPad[1];
public:
    template <class BuildData>
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const BuildData& bd,
                    bool weaponFlag = true) :
        Weapon(name, bd.m_kind, bd.m_data, articleId, &info, this->m_modules.getModuleAccesser(), weaponFlag),
        m_modules(this, bd) {
        Weapon::initialize();
    }
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() { wnDeactivateArticleDescendants(m_modules.m_generateArticleBuilder); }
};

class wnSnakeTrenchMortar : public wnWeaponBuilder<wnSnakeTrenchMortarModuleAccesserBuildConfig> {
public:
    float unk7CE0; // HYPOTHESIS: the constructor clears these two words
    float unk7CE4;
    wnSnakeTrenchMortar(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeTrenchMortarData* data);
    virtual ~wnSnakeTrenchMortar();
    void activate(int etcResourceId, int team, float lr, int founderTaskId);
};
static_assert(sizeof(wnSnakeTrenchMortar) == 0x7CE8, "wnSnakeTrenchMortar layout is wrong!");
