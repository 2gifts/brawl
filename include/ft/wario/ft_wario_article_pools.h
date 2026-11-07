#pragma once

// Wario's owned articles (article 0 Wario Bike, article 1 Wario Garlic) and their mediator.
// Native pool construction in the article manage module builder places Garlic innermost (+0x84 in builder) and Bike outermost (+0x2134),
// matching a forward soLineHierarchy over [Bike (id 0), Garlic (id 1)] with soKindInfoGeneric<0, 21>.
// HYPOTHESIS: template parameter lists not directly spelled by RTTI symbols.

#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#include <wn/wn_module_accesser_builder.h>
#include <wn/wn_article_pool.h>
#include <wn/wario/wn_wario_bike.h>
#include <wn/wario/wn_wario_garlic.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class ftWario;

typedef soKindInfoGeneric<0, 21> ftWarioArticleKindInfo;
typedef soInstancePoolInfo<wnWarioBike, 1, wnInstanceHolder, soIntToType<0> > ftWarioBikeInfo;
typedef soInstancePoolInfo<wnWarioGarlic, 1, wnInstanceHolder, soIntToType<1> > ftWarioGarlicInfo;

typedef soTypeList<ftWarioGarlicInfo, soTypeListNullType> ftWarioArticleList1;
typedef soTypeList<ftWarioBikeInfo, ftWarioArticleList1> ftWarioArticleList;

// Adapter unit tag so the 3-parameter soLineHierarchy from ft_builder_transition.h specializes Wario's forward
// article pool chain while keeping the exact soInstancePool<Info, Parent, KindInfo, Index> base types in symbols.txt.
template <class Info, class Parent>
struct ftWarioArticlePoolUnit;

template <>
class soLineHierarchy<ftWarioArticleList1, ftWarioArticlePoolUnit, soInstancePoolRoot>
    : public soInstancePool<ftWarioGarlicInfo, soInstancePoolRoot, ftWarioArticleKindInfo, 1> {
    typedef soInstancePool<ftWarioGarlicInfo, soInstancePoolRoot, ftWarioArticleKindInfo, 1> Base;
public:
    soLineHierarchy(soModuleAccesser* acc) : Base(acc) { }
    ~soLineHierarchy() { }
};

template <>
class soLineHierarchy<ftWarioArticleList, ftWarioArticlePoolUnit, soInstancePoolRoot>
    : public soInstancePool<ftWarioBikeInfo,
                            soLineHierarchy<ftWarioArticleList1, ftWarioArticlePoolUnit, soInstancePoolRoot>,
                            ftWarioArticleKindInfo, 0> {
    typedef soInstancePool<ftWarioBikeInfo,
                           soLineHierarchy<ftWarioArticleList1, ftWarioArticlePoolUnit, soInstancePoolRoot>,
                           ftWarioArticleKindInfo, 0> Base;
public:
    soLineHierarchy(soModuleAccesser* acc) : Base(acc) { }
    ~soLineHierarchy() { }
};

typedef soInstancePool<ftWarioGarlicInfo, soInstancePoolRoot, ftWarioArticleKindInfo, 1> ftWarioGarlicPool;
typedef soInstancePool<ftWarioBikeInfo,
                       soLineHierarchy<ftWarioArticleList1, ftWarioArticlePoolUnit, soInstancePoolRoot>,
                       ftWarioArticleKindInfo, 0> ftWarioBikePool;
typedef soLineHierarchy<ftWarioArticleList, ftWarioArticlePoolUnit, soInstancePoolRoot> ftWarioArticlePools;

// HYPOTHESIS: soArticleMediatorImpl<PoolList, Founder>
template <class PoolList, class Founder>
class soArticleMediatorImpl : public soArticleMediator {
    ftWarioArticlePools m_pools;
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
    ftWarioArticlePools& getPools() { return m_pools; }
};
typedef soArticleMediatorImpl<ftWarioArticleList, ftWario> ftWarioArticleMediator;

// HYPOTHESIS: selected mediator wrapper, destroyed through soSelectInstanceHolder's destructor.
template <class Mediator>
class soSelectInstanceHolder<Mediator, soArticleMediatorNull> {
    Mediator m_mediator;
public:
    soSelectInstanceHolder(soModuleAccesser* acc) : m_mediator(acc) { }
    ~soSelectInstanceHolder();
    soArticleMediator* getMediator() { return &m_mediator; }
};

// HYPOTHESIS: soGenerateArticleManageModuleBuilder<Config>
template <class Mediator, int N>
class soGenerateArticleManageModuleBuilder {
    soArrayVector<soArticle*, N> m_articles;
    soArrayVector<soArticleEventObserver, N> m_observers;
    soSelectInstanceHolder<Mediator, soArticleMediatorNull> m_mediator;
    soGenerateArticleManageModuleImpl m_module;
public:
    soGenerateArticleManageModuleBuilder(soModuleAccesser* acc);
    ~soGenerateArticleManageModuleBuilder();
    void* getModule() { return &m_module; }
    soArticleMediator* getMediator() { return m_mediator.getMediator(); }
};

FT_DOL_ARRAY_VECTOR(soArticle*, 5);

typedef soGenerateArticleManageModuleBuilder<ftWarioArticleMediator, 5> ftWarioArticleManageModuleBuilder;
static_assert(sizeof(ftWarioArticleManageModuleBuilder) == 0x4BC8, "Wario article manage builder layout is wrong!");
