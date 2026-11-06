#pragma once

// The article PokeZenigame owns (the Water Gun shot) and the mediator that hands it out. The pools are chained base first
// (see include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/poke/wn_poke_zenigame_water.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

typedef wnArticleHierarchy<wnPokeZenigameWater, 10, soInstancePoolRoot> ftPokeZenigameWaterPool;

class ftPokeZenigameArticleMediator : public soArticleMediator {
    ftPokeZenigameWaterPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftPokeZenigameArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftPokeZenigameArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftPokeZenigameArticleMediator, 5> ftPokeZenigameArticleManageModuleBuilder;
static_assert(sizeof(ftPokeZenigameArticleManageModuleBuilder) == 0xE2BC, "PokeZenigame article manage builder layout is wrong!");
