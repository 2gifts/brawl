#pragma once

// The articles Zero Suit Samus owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

// Full article types let the fighter own their pools and teardown.
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_WEAPON_NO_COMMON_ENERGIES
#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it
// These observer lists call their shared sora_melee destructors (as in ft_pit_article_pools.h).
template <> class soEventUnitWithWorkArea<soStatusEventObserver, 16> : public soEventUnitImpl<soStatusEventObserver> {
    soInstanceManagerFullPropertyVector<soStatusEventObserver*, 16> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soStatusEventObserver>(managerId, eventId) { }
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const { return m_observerList.size(); }
};
template <> class soEventUnitWithWorkArea<soAnimCmdEventObserver, 32> : public soEventUnitImpl<soAnimCmdEventObserver> {
    soInstanceManagerFullPropertyVector<soAnimCmdEventObserver*, 32> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soAnimCmdEventObserver>(managerId, eventId) { }
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const { return m_observerList.size(); }
};
template <> class soEventUnitWithWorkArea<soLinkEventObserver, 2> : public soEventUnitImpl<soLinkEventObserver> {
    soInstanceManagerFullPropertyVector<soLinkEventObserver*, 2> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soLinkEventObserver>(managerId, eventId) { }
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const { return m_observerList.size(); }
};
template <> class soEventUnitWithWorkArea<soArticleEventObserver, 2> : public soEventUnitImpl<soArticleEventObserver> {
    soInstanceManagerFullPropertyVector<soArticleEventObserver*, 2> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soArticleEventObserver>(managerId, eventId) { }
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const { return m_observerList.size(); }
};
template <> class soEventUnitWithWorkArea<soModelEventObserver, 5> : public soEventUnitImpl<soModelEventObserver> {
    soInstanceManagerFullPropertyVector<soModelEventObserver*, 5> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soModelEventObserver>(managerId, eventId) { }
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const { return m_observerList.size(); }
};
#include <ft/samus/ft_samus_event_units.h> // the paralyzer bullet has the grapple beam's observer lists
template <> class soEventUnitWithWorkArea<soTurnEventObserver, 4> : public soEventUnitImpl<soTurnEventObserver> {
    soInstanceManagerFullPropertyVector<soTurnEventObserver*, 4> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soTurnEventObserver>(managerId, eventId) { }
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const { return m_observerList.size(); }
};
#include <wn/wn_module_accesser_builder.h>
// The two articles of this REL first: they declare the sora_melee vectors the other articles share.
#include <wn/samus/wn_s_zerosuit_whip.h>
#include <wn/samus/wn_szerosuit_armor.h>
#include <wn/zelda/wn_szerosuit_paralyzer.h>
#include <wn/zelda/wn_szerosuit_paralyzer_bullet.h>

static_assert(sizeof(wnSZerosuitWhip) == 0x1D50, "wnSZerosuitWhip layout is wrong!");

typedef wnArticleHierarchy<wnSZerosuitArmor, 8, soInstancePoolRoot> ftSZerosuitArmorPool;
typedef wnArticleHierarchy<wnSZerosuitWhip, 1, ftSZerosuitArmorPool> ftSZerosuitWhipPool;
typedef wnArticleHierarchy<wnSZerosuitParalyzerBullet, 2, ftSZerosuitWhipPool> ftSZerosuitBulletPool;
typedef wnArticleHierarchy<wnSZerosuitParalyzer, 1, ftSZerosuitBulletPool> ftSZerosuitParalyzerPool;

class ftSZerosuitArticleMediator : public soArticleMediator {
    ftSZerosuitParalyzerPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftSZerosuitArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftSZerosuitArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftSZerosuitArticleMediator, 10> ftSZerosuitArticleManageModuleBuilder;
