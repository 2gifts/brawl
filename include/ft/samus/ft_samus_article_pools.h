#pragma once

// The articles Samus owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it
#include <ft/samus/ft_samus_event_units.h>

// The G beam: the full article (generic builder), because the fighter module keeps a weak copy of its destructor chain.
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#include <wn/wn_module_accesser_builder.h>
// The beam's status process list and two-entry interpreter stacks are sora_melee vectors.
FT_DOL_ARRAY_VECTOR(soStatusUniqProcess*, 9);
FT_DOL_ARRAY_VECTOR(acCmdInterpreterStackData, 2);
#include <wn/samus/wn_samus_gbeam.h>
#pragma dont_inline off

// The articles' call-side views (their classes and destructors live in sora_melee).
#include <wn/samus/wn_samus_cshot.h>
#include <wn/samus/wn_samus_bomb.h>
#include <wn/samus/wn_samus_missile.h>
#include <wn/samus/wn_samus_laser.h>
#include <wn/samus/wn_samus_laser2.h>
#include <wn/samus/wn_samus_supermissile.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class Vec3f;
struct wnSamusGunData;

// The gun (sora_melee). Opaque shell: wn/samus/wn_samus_gun.h has no call-side view yet.
class wnSamusGun : public Weapon {
    u8 m_unreconstructed[0x1834 - sizeof(Weapon)];
public:
    wnSamusGun(s32 articleId, const wnArticleBuildInfo& info, const wnSamusGunData* data);
    virtual ~wnSamusGun();
    void activate(int founderTaskId, int unk1, int unk2, int unk3, int unk4, int team);
};
static_assert(sizeof(wnSamusGun) == 0x1834, "wnSamusGun layout is wrong!");

typedef wnArticleHierarchy<wnSamusGBeam, 1, soInstancePoolRoot> ftSamusGBeamPool;
typedef wnArticleHierarchy<wnSamusSuperMissile, 2, ftSamusGBeamPool> ftSamusSuperMissilePool;
typedef wnArticleHierarchy<wnSamusGun, 1, ftSamusSuperMissilePool> ftSamusGunPool;
typedef wnArticleHierarchy<wnSamusLaser2, 4, ftSamusGunPool> ftSamusLaser2Pool;
typedef wnArticleHierarchy<wnSamusLaser, 1, ftSamusLaser2Pool> ftSamusLaserPool;
typedef wnArticleHierarchy<wnSamusMissile, 3, ftSamusLaserPool> ftSamusMissilePool;
typedef wnArticleHierarchy<wnSamusBomb, 3, ftSamusMissilePool> ftSamusBombPool;
typedef wnArticleHierarchy<wnSamusCShot, 4, ftSamusBombPool> ftSamusCShotPool;

class ftSamusArticleMediator : public soArticleMediator {
    ftSamusCShotPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftSamusArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftSamusArticleMediator();
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
FT_DOL_ARRAY_VECTOR(soArticle*, 10);

typedef wnArticleManageModuleBuilder<ftSamusArticleMediator, 10> ftSamusArticleManageModuleBuilder;
static_assert(sizeof(ftSamusArticleManageModuleBuilder) == 0x1D7D4, "Samus article manage builder layout is wrong!");
