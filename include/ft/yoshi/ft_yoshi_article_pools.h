#pragma once

// The articles Yoshi owns and the mediator that hands them out. The pools are the instance pools of the fighter
// (ft/builder/ft_builder_kinetic.h) with a wnInstanceHolder per article; the first entry of the type list is the innermost
// pool, so the fire cannon sits right after the root and the star is the outermost level.
// HYPOTHESIS: names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#include <wn/yoshi/wn_yoshi_star.h>
#include <wn/yoshi/wn_yoshi_tamago.h>
#include <wn/yoshi/wn_yoshi_wing.h>
#include <ft/yoshi/ft_yoshi_article_calls.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class ftYoshi;

// Article ids: 0 Star, 1 Egg, 2 Wing, 3 Fire breath, 4 Fire cannon.
typedef soKindInfoGeneric<0, 4> ftYoshiArticleKindInfo;
typedef soInstancePoolInfo<wnYoshiFireCannon, 2, wnInstanceHolder, soIntToType<4> > ftYoshiFireCannonInfo;
typedef soInstancePoolInfo<wnYoshiFireBreath, 4, wnInstanceHolder, soIntToType<3> > ftYoshiFireBreathInfo;
typedef soInstancePoolInfo<wnYoshiWing, 1, wnInstanceHolder, soIntToType<2> > ftYoshiWingInfo;
typedef soInstancePoolInfo<wnYoshiTamago, 4, wnInstanceHolder, soIntToType<1> > ftYoshiTamagoInfo;
typedef soInstancePoolInfo<wnYoshiStar, 2, wnInstanceHolder, soIntToType<0> > ftYoshiStarInfo;

typedef soTypeList<ftYoshiFireCannonInfo, soTypeListNullType> ftYoshiArticleList1;
typedef soTypeList<ftYoshiFireCannonInfo, soTypeList<ftYoshiFireBreathInfo, soTypeListNullType> > ftYoshiArticleList2;
typedef soTypeList<ftYoshiFireCannonInfo, soTypeList<ftYoshiFireBreathInfo, soTypeList<ftYoshiWingInfo, soTypeListNullType> > > ftYoshiArticleList3;
typedef soTypeList<ftYoshiFireCannonInfo, soTypeList<ftYoshiFireBreathInfo, soTypeList<ftYoshiWingInfo,
    soTypeList<ftYoshiTamagoInfo, soTypeListNullType> > > > ftYoshiArticleList4;
typedef soTypeList<ftYoshiFireCannonInfo, soTypeList<ftYoshiFireBreathInfo, soTypeList<ftYoshiWingInfo,
    soTypeList<ftYoshiTamagoInfo, soTypeList<ftYoshiStarInfo, soTypeListNullType> > > > > ftYoshiArticleList;

typedef soLineInvertHierarchy<ftYoshiArticleList, soInstancePool, soInstancePoolRoot, ftYoshiArticleKindInfo> ftYoshiArticlePools;

// The pool of every article (the unit of the hierarchy at its index).
typedef soInstancePool<ftYoshiFireCannonInfo, soInstancePoolRoot, ftYoshiArticleKindInfo, 0> ftYoshiFireCannonPool;
typedef soInstancePool<ftYoshiFireBreathInfo, soLineInvertHierarchy<ftYoshiArticleList1, soInstancePool, soInstancePoolRoot, ftYoshiArticleKindInfo, 0>,
                       ftYoshiArticleKindInfo, 1> ftYoshiFireBreathPool;
typedef soInstancePool<ftYoshiWingInfo, soLineInvertHierarchy<ftYoshiArticleList2, soInstancePool, soInstancePoolRoot, ftYoshiArticleKindInfo, 1>,
                       ftYoshiArticleKindInfo, 2> ftYoshiWingPool;
typedef soInstancePool<ftYoshiTamagoInfo, soLineInvertHierarchy<ftYoshiArticleList3, soInstancePool, soInstancePoolRoot, ftYoshiArticleKindInfo, 2>,
                       ftYoshiArticleKindInfo, 3> ftYoshiTamagoPool;
typedef soInstancePool<ftYoshiStarInfo, soLineInvertHierarchy<ftYoshiArticleList4, soInstancePool, soInstancePoolRoot, ftYoshiArticleKindInfo, 3>,
                       ftYoshiArticleKindInfo, 4> ftYoshiStarPool;

// HYPOTHESIS: soArticleMediatorImpl<type list of the pools, founder>: the original takes more arguments.
template <class PoolList, class Founder>
class soArticleMediatorImpl : public soArticleMediator {
    ftYoshiArticlePools m_pools;
    u8 m_autoRecycle;
public:
    soArticleMediatorImpl(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~soArticleMediatorImpl();
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc);
    virtual bool shoot(soModuleAccesser* acc, soArticle* article);
    virtual s32 getMediateNum();
    virtual void setAutoRecycle(bool enabled);
    virtual void deactivate();
    virtual s32 getGenerateMaxNum(s32 articleId);
    virtual s32 getActiveNum(soModuleAccesser* acc, s32 articleId);
    virtual bool isGeneratable(soModuleAccesser* acc, s32 articleId);
    ftYoshiArticlePools& getPools() { return m_pools; }
};
typedef soArticleMediatorImpl<ftYoshiArticleList, ftYoshi> ftYoshiArticleMediator;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder<Config>: the article lists, the mediator with the pools of the
// articles, and the module itself.
template <class Mediator, int N>
class soGenerateArticleManageModuleBuilder {
    soArrayVector<soArticle*, N> m_articles;
    soArrayVector<soArticleEventObserver, N> m_observers;
    wnSelectedArticleMediator<Mediator> m_mediator;
    soGenerateArticleManageModuleImpl m_module;
public:
    soGenerateArticleManageModuleBuilder(soModuleAccesser* acc);
    ~soGenerateArticleManageModuleBuilder();
    void* getModule() { return &m_module; }
    soArticleMediator* getMediator() { return m_mediator.getMediator(); }
};

// The lists of the module live in sora_melee (only declared here).
FT_DOL_ARRAY_VECTOR(soArticle*, 5);

typedef soGenerateArticleManageModuleBuilder<ftYoshiArticleMediator, 5> ftYoshiArticleManageModuleBuilder;
static_assert(sizeof(ftYoshiArticleManageModuleBuilder) == 0x19E5C, "Yoshi article manage builder layout is wrong!");
