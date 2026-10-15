#pragma once

// The articles Lucas owns and the mediator that hands them out: PK Flash, PK Fire (two bolts), PK Thunder (two heads), PK
// Starstorm (sixteen stars) and the Rope Snake. The pools are chained base first, the last article at the lowest offset
// (see include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names.

#include <wn/wn_article_pool.h>
#include <wn/lucas/wn_lucas_himohebi.h>
#include <wn/lucas/wn_lucas_pk_fire.h>
#include <wn/lucas/wn_lucas_pk_flash.h>
#include <wn/lucas/wn_lucas_pk_star_storm.h>
#include <wn/lucas/wn_lucas_pk_thunder.h>

typedef wnArticleHierarchy<wnLucasHimohebi, 1, soInstancePoolRoot> ftLucasHimohebiPool;
typedef wnArticleHierarchy<wnLucasPKStarStorm, 16, ftLucasHimohebiPool> ftLucasStarStormPool;
typedef wnArticleHierarchy<wnLucasPKThunder, 2, ftLucasStarStormPool> ftLucasThunderPool;
typedef wnArticleHierarchy<wnLucasPKFire, 2, ftLucasThunderPool> ftLucasFirePool;
typedef wnArticleHierarchy<wnLucasPKFlash, 1, ftLucasFirePool> ftLucasFlashPool;

class ftLucasArticleMediator : public soArticleMediator {
    ftLucasFlashPool m_pools;
    u8 m_autoRecycle;
public:
    ftLucasArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftLucasArticleMediator();
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc);
    virtual bool shoot(soModuleAccesser* acc, soArticle* article);
    virtual s32 getMediateNum();
    virtual void setAutoRecycle(bool enabled);
    virtual void deactivate();
    virtual s32 getGenerateMaxNum(s32 articleId);
    virtual s32 getActiveNum(soModuleAccesser* acc, s32 articleId);
    virtual bool isGeneratable(soModuleAccesser* acc, s32 articleId);
};

// The list of the articles of the module lives in sora_melee (only declared here).
FT_DOL_ARRAY_VECTOR(soArticle*, 5);

typedef wnArticleManageModuleBuilder<ftLucasArticleMediator, 5> ftLucasArticleManageModuleBuilder;
static_assert(sizeof(ftLucasArticleManageModuleBuilder) == 0x40814, "Lucas article manage builder layout is wrong!");
