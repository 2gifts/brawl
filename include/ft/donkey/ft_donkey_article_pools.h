#pragma once

// The articles Donkey owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

// Full article types let the fighter own their pools and teardown.
#define WN_STATUS_PROC_VECTOR2_DEFAULT_CTOR
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#include <wn/wn_module_accesser_builder.h>
#include <wn/donkey/wn_donkey_barrel_conga.h>
#include <wn/donkey/wn_donkey_sound_effect.h>

typedef wnArticleHierarchy<wnDonkeySoundEffect, 4, soInstancePoolRoot> ftDonkeySoundEffectPool;
typedef wnArticleHierarchy<wnDonkeyBarrelConga, 1, ftDonkeySoundEffectPool> ftDonkeyBarrelCongaPool;

class ftDonkeyArticleMediator : public soArticleMediator {
    ftDonkeyBarrelCongaPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftDonkeyArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftDonkeyArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftDonkeyArticleMediator, 5> ftDonkeyArticleManageModuleBuilder;
static_assert(sizeof(ftDonkeyArticleManageModuleBuilder) == 0xA0B0, "Donkey article manage builder layout is wrong!");
