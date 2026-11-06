#pragma once

// The articles Ness owns and the mediator that hands them out: PK Flash, PK Fire (two flames), PK Thunder (two heads), PK
// Starstorm (sixteen stars), the Yo-Yo and its head. The pools are chained base first, the last article at the lowest offset
// (see include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

// Full article types let the fighter own their pools and teardown.
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#include <wn/wn_module_accesser_builder.h>
#include <wn/ness/wn_ness_pkflash.h>
#include <wn/ness/wn_ness_pk_fire.h>
#include <wn/ness/wn_ness_pk_thunder.h>
#include <wn/ness/wn_ness_pk_star_storm.h>
#include <wn/ness/wn_ness_yoyo.h>
#include <wn/ness/wn_ness_yoyo_head.h>
#undef WN_ARTICLE_NATIVE_BUILDERS // the fighter defines the shared kinetic destructor out of line (ft_builder_noinline.h)

typedef wnArticleHierarchy<wnNessYoyoHead, 1, soInstancePoolRoot> ftNessYoyoHeadPool;
typedef wnArticleHierarchy<wnNessYoyo, 1, ftNessYoyoHeadPool> ftNessYoyoPool;
typedef wnArticleHierarchy<wnNessPKStarStorm, 16, ftNessYoyoPool> ftNessStarStormPool;
typedef wnArticleHierarchy<wnNessPKThunder, 2, ftNessStarStormPool> ftNessThunderPool;
typedef wnArticleHierarchy<wnNessPKFire, 2, ftNessThunderPool> ftNessFirePool;
typedef wnArticleHierarchy<wnNessPKFlash, 1, ftNessFirePool> ftNessFlashPool;

class ftNessArticleMediator : public soArticleMediator {
    ftNessFlashPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftNessArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftNessArticleMediator();
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc);
    virtual bool shoot(soModuleAccesser* acc, soArticle* article);
    virtual s32 getMediateNum();
    virtual void setAutoRecycle(bool enabled);
    virtual void deactivate();
    virtual s32 getGenerateMaxNum(s32 articleId);
    virtual s32 getActiveNum(soModuleAccesser* acc, s32 articleId);
    virtual bool isGeneratable(soModuleAccesser* acc, s32 articleId);
};

// The lists of the module hold five entries although the mediator knows six articles. They live in sora_melee (only declared here).
FT_DOL_ARRAY_VECTOR(soArticle*, 5);

typedef wnArticleManageModuleBuilder<ftNessArticleMediator, 5> ftNessArticleManageModuleBuilder;
static_assert(sizeof(ftNessArticleManageModuleBuilder) == 0x42A0C, "Ness article manage builder layout is wrong!");
