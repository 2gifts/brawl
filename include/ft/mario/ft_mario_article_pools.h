#pragma once

// The articles Mario owns and the mediator that hands them out: the cape (one), the fireballs (five), the Pump (F.L.U.D.D., one)
// and the Final Smash flame (one). The pools are chained base first (see include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class Vec2f;
class Vec3f;
struct wnMarioFireballData;
struct wnMarioPumpData;
struct wnMarioMantleData;
struct wnMarioHugeFlameData;

// The fireball. The full class lives in wn/mario/wn_mario_fireball.h (a weapon builder specialization that cannot coexist with
// this Fighter-based builder unit), so the fighter unit sees an opaque shell of the right size.
class wnMarioFireball : public Weapon {
    u8 m_unreconstructed[0x1F44 - sizeof(Weapon)];
public:
    wnMarioFireball(s32 articleId, const wnArticleBuildInfo& info, const wnMarioFireballData* data);
    virtual ~wnMarioFireball();
};
static_assert(sizeof(wnMarioFireball) == 0x1F44, "Mario fireball layout is wrong!");

// The Pump (F.L.U.D.D., the down special). Same reason for the shell (the class is in the Pump unit of the REL).
class wnMarioPump : public Weapon {
    u8 m_unreconstructed[0x18A28 - sizeof(Weapon)];
public:
    wnMarioPump(s32 articleId, const wnArticleBuildInfo& info, const wnMarioPumpData* data);
    virtual ~wnMarioPump();
    void activate(int founderTaskId, int modelResourceId, int team, int etcResourceId);
    void setCharge(float charge);
};

// The cape article.
// HYPOTHESIS: the size is calibrated against the size of the builder.
class wnMarioMantle : public Weapon {
    u8 m_unreconstructed[0x1358 - sizeof(Weapon)];
public:
    wnMarioMantle(s32 articleId, const wnArticleBuildInfo& info, const wnMarioMantleData* data);
    virtual ~wnMarioMantle();
    void activate(int founderTaskId, int etcResourceId, int team, int situationKind);
};

// The Final Smash flame (wn/mario/wn_mario_huge_flame.h).
class wnMarioHugeFlame : public Weapon {
    u8 m_unreconstructed[0x1FBC - sizeof(Weapon)];
public:
    wnMarioHugeFlame(s32 articleId, const wnArticleBuildInfo& info, const wnMarioHugeFlameData* data);
    virtual ~wnMarioHugeFlame();
    void activate(int founderTaskId, int resourceId, int team, float lr);
    void toFly(Vec3f* position);
};

typedef wnArticleHierarchy<wnMarioHugeFlame, 1, soInstancePoolRoot> ftMarioHugeFlamePool;
typedef wnArticleHierarchy<wnMarioPump, 1, ftMarioHugeFlamePool> ftMarioPumpPool;
typedef wnArticleHierarchy<wnMarioFireball, 5, ftMarioPumpPool> ftMarioFireballPool;
typedef wnArticleHierarchy<wnMarioMantle, 1, ftMarioFireballPool> ftMarioMantlePool;

class ftMarioArticleMediator : public soArticleMediator {
    ftMarioMantlePool m_pools;
    u8 m_autoRecycle;
public:
    ftMarioArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftMarioArticleMediator();
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc);
    virtual bool shoot(soModuleAccesser* acc, soArticle* article);
    virtual s32 getMediateNum();
    virtual void setAutoRecycle(bool enabled);
    virtual void deactivate();
    virtual s32 getGenerateMaxNum(s32 articleId);
    virtual s32 getActiveNum(soModuleAccesser* acc, s32 articleId);
    virtual bool isGeneratable(soModuleAccesser* acc, s32 articleId);
};

// The lists of the module live in sora_melee (only declared here).
FT_DOL_ARRAY_VECTOR(soArticle*, 5);

typedef wnArticleManageModuleBuilder<ftMarioArticleMediator, 5> ftMarioArticleManageModuleBuilder;
static_assert(sizeof(ftMarioArticleManageModuleBuilder) == 0x25AA8, "Mario article manage builder layout is wrong!");
