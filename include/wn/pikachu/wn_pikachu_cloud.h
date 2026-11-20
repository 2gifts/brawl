#pragma once

// Pikachu's Thunder cloud article (article kind 12). The cloud owns the four thunder bolts it drops: it keeps its own article
// manage module whose mediator holds a pool of four wnPikachuThunder (built in place, one holder each).

#include <wn/wn_weapon_builder.h>
#include <wn/wn_article_pool.h>
#include <wn/wn_article_manage_module.h>
#include <ft/ft_common_data_accesser.h>

FT_DOL_ARRAY_VECTOR(soArticle*, 4); // the members are sora_melee's (only declared here)

struct wnPikachuThunderData;
struct wnArticleBuildInfo;
class wnPikachuCloud;
class ftPikachu;
class soCollisionAttackModule;

// HYPOTHESIS: the static data block of the cloud (only the resource group number is read by its constructor).
struct wnPikachuCloudData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
};

// The thunder bolt (the class lives in sora_melee). Opaque shell: the extent is the one of the holders of the cloud's pool
// (each holder is a vptr plus one bolt).
// HYPOTHESIS: the extent; the fighter's own pool header keeps another one for the bolts it owns.
class wnPikachuThunder : public Weapon {
    u8 m_unreconstructed[0x2180 - sizeof(Weapon)];
public:
    wnPikachuThunder(s32 articleId, const wnArticleBuildInfo& info, const wnPikachuThunderData* data);
    virtual ~wnPikachuThunder();
    bool activeArticle(soModuleAccesser* acc);
    // The bolts of the cloud form a chain (the newest is appended to the oldest). HYPOTHESIS: all parameter types; the bodies
    // live in the bolt's own unit (names from the module map).
    void activate(int ownerTaskId, int etcResId, int team, const Vec3f* position, wnPikachuCloud* cloud, bool first);
    void change_team(int team, int etcResId);
    void chain_last(wnPikachuThunder* thunder);
    void remove_chain();
    void deactive_cloud();
    void rec_end_pos(float y);
    void ref_log_group(bool ref);
    void unref_log_group(bool ref);
    bool hitcheck(Vec3f* position);
    static int getThunderGenerateInterval();
    static int getThunderModelSize();
};
static_assert(sizeof(wnPikachuThunder) == 0x2180, "wnPikachuThunder layout is wrong!");

// ---- the thunder pool ---------------------------------------------------------------------------------------------------
typedef soKindInfoGeneric<2, 12> wnPikachuCloudKindInfo;
typedef soInstancePoolInfo<wnPikachuThunder, 4, wnInstanceHolder, soIntToType<0> > wnPikachuCloudThunderPoolInfo;
typedef soTypeList<wnPikachuCloudThunderPoolInfo, soTypeListNullType> wnPikachuCloudThunderPoolList;

// The four bolts are built in place from the owner's kind pair and heap module and the bolt data block of the owner's fighter
// kind.
template <>
class wnInstanceHolder<wnPikachuThunder, wnPikachuCloudKindInfo, soIntToType<0> > {
public:
    virtual ~wnInstanceHolder() { }
    wnInstanceHolder(soModuleAccesser* acc) :
        m_instance(0, BuildInfo(&owner(acc).getKindInfo(), &acc->getHeapModule()),
                   static_cast<const wnPikachuThunderData*>(
                       g_ftCommonDataAccesser.getWeaponData32(static_cast<ftKind>(ownerFighterKind(owner(acc))), &DataFlag(false).value))) { }
    wnPikachuThunder* getEnergy() { return &m_instance; }
    wnPikachuThunder* getInstance() { return &m_instance; }
private:
    struct BuildInfo : wnArticleBuildInfo {
        BuildInfo(const wnSimpleKindInfo* kindInfo, soHeapModule* heapModule) {
            this->kindInfo = reinterpret_cast<const wnArticleKindInfo*>(kindInfo);
            this->heapModule = heapModule;
        }
    };
    // HYPOTHESIS: a second zero byte (the original keeps the tag of the holder next to the flag).
    struct DataFlag {
        bool pad;
        bool value;
        DataFlag(bool v) : pad(v), value(v) { }
    };
    static Weapon& owner(soModuleAccesser* acc) { return dynamic_cast<Weapon&>(acc->getStageObject()); }
    static s32 ownerFighterKind(const Weapon& weapon) { return weapon.getKindInfo().subKind; }
    wnPikachuThunder m_instance;
};

template <class E>
class wnPikachuCloudPoolEnd {
    u32 m_terminal;
public:
    wnPikachuCloudPoolEnd(soModuleAccesser*) { }
    E* getInstanceAt(s32) { return NULL; }
    template <class Predicate>
    E* find(Predicate&) { return NULL; }
};

template <class Info, class Next>
struct wnPikachuCloudPoolPrev {
    typedef soInstancePoolSub<typename Info::PrevInfo, Next> Type;
};
template <class E, template <class, class, class> class H, class Init, class Next>
struct wnPikachuCloudPoolPrev<soInstancePoolInfo<E, 1, H, Init>, Next> {
    typedef wnPikachuCloudPoolEnd<E> Type;
};

#define WN_PIKACHU_CLOUD_SUB_POOL_BODY(E, N, H, Init) \
    typedef soInstancePoolInfo<E, N, H, Init> Info; \
    typedef wnPikachuCloudKindInfo Next; \
public: \
    virtual ~soInstancePoolSub(); \
private: \
    typename wnPikachuCloudPoolPrev<Info, Next>::Type m_next; \
    H<E, Next, Init> m_holder; \
public: \
    soInstancePoolSub(soModuleAccesser* acc) : m_next(acc), m_holder(acc) { } \
    E* getInstanceAt(s32 index) { \
        if (index == N - 1) { \
            return m_holder.getInstance(); \
        } \
        return m_next.getInstanceAt(index); \
    } \
    template <class Predicate> \
    E* find(Predicate& predicate) { \
        if (predicate(&static_cast<soArticle&>(*m_holder.getInstance())) == true) { \
            return m_holder.getInstance(); \
        } \
        return m_next.find(predicate); \
    } \
    template <class Predicate, int L> \
    E* find(Predicate& predicate, const soIntToType<L>&) { return find(predicate); }
template <class E, int N, template <class, class, class> class H, class Init>
class soInstancePoolSub<soInstancePoolInfo<E, N, H, Init>, wnPikachuCloudKindInfo> {
    WN_PIKACHU_CLOUD_SUB_POOL_BODY(E, N, H, Init)
};

// The single-instance sub pool of ft/builder/ft_builder_kinetic.h matches the first bolt too: this one wins.
template <>
class soInstancePoolSub<soInstancePoolInfo<wnPikachuThunder, 1, wnInstanceHolder, soIntToType<0> >, wnPikachuCloudKindInfo> {
    WN_PIKACHU_CLOUD_SUB_POOL_BODY(wnPikachuThunder, 1, wnInstanceHolder, soIntToType<0>)
};

#define WN_PIKACHU_CLOUD_SUB_POOL_DTOR(N)                                                                                      \
    template <>                                                                                                                \
    inline soInstancePoolSub<soInstancePoolInfo<wnPikachuThunder, N, wnInstanceHolder, soIntToType<0> >, wnPikachuCloudKindInfo>:: \
        ~soInstancePoolSub() { }
#pragma dont_inline on
inline soInstancePoolSub<soInstancePoolInfo<wnPikachuThunder, 1, wnInstanceHolder, soIntToType<0> >, wnPikachuCloudKindInfo>::
    ~soInstancePoolSub() { }
WN_PIKACHU_CLOUD_SUB_POOL_DTOR(2)
WN_PIKACHU_CLOUD_SUB_POOL_DTOR(3)
WN_PIKACHU_CLOUD_SUB_POOL_DTOR(4)
#pragma dont_inline off

// The pool level and the hierarchy over the sub pools. HYPOTHESIS: the original names are soInstancePool / soLineHierarchy over the
// type list; the module symbols name them like the article pools of wn/wn_article_pool.h (and with the count of the fighter's pool).
template <>
class wnArticlePool<wnPikachuThunder, 3, soInstancePoolRoot> : public soInstancePoolRoot {
public:
    typedef soInstancePoolSub<wnPikachuCloudThunderPoolInfo, wnPikachuCloudKindInfo> SubType;
    virtual ~wnArticlePool() { }
private:
    SubType m_sub;
public:
    wnArticlePool(soModuleAccesser* acc) : soInstancePoolRoot(acc), m_sub(acc) { }
    SubType& getSub() { return m_sub; }
};

template <>
class wnArticleHierarchy<wnPikachuThunder, 3, soInstancePoolRoot> : public wnArticlePool<wnPikachuThunder, 3, soInstancePoolRoot> {
public:
    wnArticleHierarchy(soModuleAccesser* acc) : wnArticlePool<wnPikachuThunder, 3, soInstancePoolRoot>(acc) { }
    virtual ~wnArticleHierarchy() { }
};

// The mediator of the thunder pool.
template <class PoolTL, class AssignTL, class KindInfo, class Extra>
class soArticleMediatorImpl : public soArticleMediator {
public:
    typedef wnArticleHierarchy<wnPikachuThunder, 3, soInstancePoolRoot> PoolHierarchy;
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
class wnPikachuCloud;

typedef soArticleMediatorImpl<wnPikachuCloudThunderPoolList,
                              soTypeList<soArticleMediatorAssignmentInfo<wnPikachuCloud, soArticleMediatorAssignmentNull>, soTypeListNullType>,
                              wnPikachuCloudKindInfo, soTypeListNullType>
    wnPikachuCloudArticleMediator;

template <class Mediator>
class soGenerateArticleManageModuleBuildConfig {
public:
    typedef Mediator MediatorType;
    enum { ArticleCount = 4 };
};

template <>
class soSelectInstanceHolder<wnPikachuCloudArticleMediator, soArticleMediatorNull> {
    wnPikachuCloudArticleMediator m_mediator;
public:
    soSelectInstanceHolder(soModuleAccesser* acc) : m_mediator(acc) { }
    ~soSelectInstanceHolder() { }
    soArticleMediator* getMediator() { return &m_mediator; }
};

template <class BC>
class soGenerateArticleManageModuleBuilder {
    typedef typename BC::MediatorType Mediator;
    soArrayVector<soArticle*, BC::ArticleCount> m_articles;
    soArrayVector<soArticleEventObserver, BC::ArticleCount> m_observers;
    soSelectInstanceHolder<Mediator, soArticleMediatorNull> m_mediator;
    soGenerateArticleManageModuleImpl m_module;
public:
    soGenerateArticleManageModuleBuilder(soModuleAccesser* acc) :
        m_articles(0), m_observers(BC::ArticleCount, 0), m_mediator(acc),
        m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
    ~soGenerateArticleManageModuleBuilder() { }
    void* getModule() { return &m_module; }
    soArticleMediator* getMediator() { return m_mediator.getMediator(); }
};

typedef soGenerateArticleManageModuleBuildConfig<wnPikachuCloudArticleMediator> wnPikachuCloudArticleBuildConfig;
typedef soGenerateArticleManageModuleBuilder<wnPikachuCloudArticleBuildConfig> wnPikachuCloudArticleManageModuleBuilder;

// ---- the cloud --------------------------------------------------------------------------------------------------------
// No ground, hit, damage or camera modules; one motion, one attack box, one status with six transitions, and the generate-article
// module over the thunder pool.
class wnPikachuCloudModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<15, 4, 1> GeneralWorkBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef wnPikachuCloudArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnPikachuCloud : public wnWeaponBuilder<wnPikachuCloudModuleAccesserBuildConfig> {
public:
    wnPikachuCloud(s32 articleId, const wnArticleBuildInfo& info, const wnPikachuCloudData* data);
    virtual ~wnPikachuCloud() { }
    virtual void onDeactivate();
    // HYPOTHESIS: parameter meanings: the founder task, the team, the position, the etc resource id and the Pikachu that calls it.
    void activate(int founderTaskId, int team, const Vec3f* position, int etcResId, ftPikachu* owner);
    // Starts a bolt of the cloud that owns the accesser: the bolt joins the chain of the cloud.
    static bool activeArticle(wnPikachuThunder* thunder, soModuleAccesser* acc);
    // The work ints keep the owner and the head of the chain of bolts.
    void rec_pikachu(ftPikachu* pikachu);
    ftPikachu* get_pikachu();
    void rec_thunder(wnPikachuThunder* thunder);
    wnPikachuThunder* get_thunder();
    Vec3f get_thunder_generate_pos();
    bool thunder_hitcheck(Vec3f* position);
    void change_head_thunder(wnPikachuThunder* thunder);
    void deactive_thunder(int thunder);
    void notify_reflect(int team, int etcResId);
    void set_change_team();
    void alloc_log_group();
    void free_log_group();
    void ref_log_group(soCollisionAttackModule* attack);
    void unref_log_group(soCollisionAttackModule* attack);
    bool isDropGround();
    void call_pikachu_hit_effect();
    void call_ground_hit_effect(Vec3f* position);
    void call_effect(Vec3f* position, EfID effectId);
    void set_pikachu_pos(float y);
};
static_assert(sizeof(wnPikachuCloud) == 0xA5FC, "wnPikachuCloud layout is wrong!");
