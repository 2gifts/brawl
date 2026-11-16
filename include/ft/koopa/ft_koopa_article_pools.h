#pragma once

// The articles Koopa owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h). The fighter unit builds the full breath article: its weak destructor chain is emitted there.
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#ifdef FT_KOOPA_ARTICLE_POOLS // the fighter unit: the inside-event builder teardown calls these lists out of line
#include <ft/koopa/ft_koopa_event_units.h>
#endif
#define WN_STATUS_PROC_VECTOR2_DEFAULT_CTOR
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#include <wn/wn_module_accesser_builder.h>
#include <wn/koopa/wn_koopa_breath.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

typedef wnArticleHierarchy<wnKoopaBreath, 6, soInstancePoolRoot> ftKoopaBreathPool;

class ftKoopaArticleMediator : public soArticleMediator {
    ftKoopaBreathPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftKoopaArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftKoopaArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftKoopaArticleMediator, 5> ftKoopaArticleManageModuleBuilder;
static_assert(sizeof(ftKoopaArticleManageModuleBuilder) == 0xBDFC, "Koopa article manage builder layout is wrong!");
