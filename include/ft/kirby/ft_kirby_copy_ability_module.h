#pragma once

#include <ft/ft_event_unit_instance.h>
#include <ft/ft_transactor.h>
#include <so/situation/so_situation_event_presenter.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/so_array.h>
#include <so/so_module_accesser.h>
#include <so/so_instance_manager.h>
#include <types.h>

class soArticle;
class soModuleAccesser;
class soCollisionLog;
class soCollisionAttackModule;
class soStatusData;
class soEventUnit;

// The article units of the dynamic article mediator (the code is in sora_melee: so_article_dynamic_unit.o).
// HYPOTHESIS: layout from the copy assignment (word 0, word 3, then 20 words); the words in between are not copied.
class soArticleDynamicUnit {
    struct Rest {
        u32 words[20];
    };
    u32 m_id;
    u32 unk4;
    u32 unk8;
    u32 m_word3;
    Rest m_rest;
public:
    soArticleDynamicUnit();
    ~soArticleDynamicUnit();
    soArticleDynamicUnit& operator=(const soArticleDynamicUnit& other) {
        m_id = other.m_id;
        m_word3 = other.m_word3;
        m_rest = other.m_rest;
        return *this;
    }
};
static_assert(sizeof(soArticleDynamicUnit) == 0x60, "soArticleDynamicUnit is the wrong size");

// One group of the dynamic article mediator (so_article_dynamic_group.o).
// HYPOTHESIS: the group is an event unit record (its assignment is the one of soInstanceUnit<soEventUnit*>); the record's
// element is the list of the group's units.
class soArticleDynamicGroup : public soInstanceUnit<soEventUnit*> {
public:
    soArticleDynamicGroup();
    ~soArticleDynamicGroup();
    void setUnitList(soArrayVectorAbstract<soArticleDynamicUnit>* units);
};
static_assert(sizeof(soArticleDynamicGroup) == 8, "soArticleDynamicGroup is the wrong size");

// Hands the copy ability's articles out (so_article_dynamic_mediator.o).
// HYPOTHESIS: the virtual order is fitted to the slots the copy ability module calls (slot 6 deactivate, slot 11
// setAutoRecycle); the others are only placed.
class soArticleActivateTransactor;
class soArticleOperateTransactor;
class wnArticleList;
class soArticleDynamicMediator {
public:
    soArticleDynamicMediator(soArrayVector<soArticleDynamicGroup, 7>* groups);
    virtual ~soArticleDynamicMediator();
    virtual void unk1();
    virtual void unk2();
    virtual void unk3();
    virtual void unk4();
    virtual void unk5();
    virtual void deactivate();
    virtual void unk7();
    virtual void unk8();
    virtual void unk9();
    virtual void unk10();
    virtual void setAutoRecycle(bool autoRecycle);
private:
    u8 m_storage[0x18];
public:
    void disposeGroup(int groupId);
    wnArticleList* getArticleArray(int groupId, int articleId);
    bool hasGroup(int groupId);
    void setActivateTransactor(soArticleActivateTransactor& transactor);
    void setOperateTransactor(soArticleOperateTransactor& transactor);
    int getArrayNum();
    int getGroupId(int index);
};

// The two interfaces the copy ability module implements for the mediator.
class soArticleActivateTransactor {
public:
    virtual bool activeArticle(soArticle* article, soModuleAccesser* moduleAccesser);
};
class soArticleOperateTransactor {
public:
    virtual bool shootArticle(soArticle* article, soModuleAccesser* moduleAccesser);
    virtual void unk1();
};

struct soModelInfo {
    u32 unk0;
    u32 unk4;
    u32 unk8;
    u32 unkC;
    u8 unk10;
    soModelInfo() { init(); }
    void init();
};

// One weapon (article) of a copy ability, 12 bytes.
struct ftKirbyCopyAbilityWeaponInfo {
    int unk0;
    int unk4;
    int articleId;
};

// One entry of the table of copy abilities (16 bytes, indexed by copy kind).
struct ftKirbyCopyAbilityInfo {
    const ftKirbyCopyAbilityWeaponInfo* weapons;
    int count;
    int unk8;
    int unkC;
    const ftKirbyCopyAbilityWeaponInfo& getWeaponInfo(int index) const __attribute__((never_inline));
};

class ftKirbyCopyAbilityModule : public soArticleActivateTransactor, public soArticleOperateTransactor {
    soModuleAccesser* m_moduleAccesser;                  // +0x08
    soArticleDynamicMediator m_mediator;                 // +0x0C
    soArrayVector<soArticleDynamicGroup, 7> m_groups;    // +0x28
    soArrayVector<soArticleDynamicUnit, 4> m_units[7];   // +0x6C
    s32 unkB40;
    soModelInfo m_modelInfo;                             // +0xB44
    s32 unkB58;
    s32 unkB5C;
    s32 m_copyKind;                                      // +0xB60
public:
    ftKirbyCopyAbilityModule(soModuleAccesser* moduleAccesser);
    ~ftKirbyCopyAbilityModule();
    // HYPOTHESIS: the mediator sits in the module accesser of the owner (the module accesser reaches it through the article module).
    static const ftKirbyCopyAbilityInfo* getCopyAbilityInfo(int copyKind);
    static int getCopyTopStatusKind(int copyKind);
    ftTransactor* getCopyAbilityProcess();
    void disposeCopyAbilityAll();
    void disposeCopyAbility(int copyKind);
    virtual bool activeArticle(soArticle* article, soModuleAccesser* moduleAccesser);
    virtual bool shootArticle(soArticle* article, soModuleAccesser* moduleAccesser);
    void notifyEventChangeSituation(SituationKind kind, SituationKind prevKind);
    void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    bool notifyEventCollisionShieldCheck();
    void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    bool notifyEventCollisionSearchCheck();
    void processUpdate();
    void processCollision();
    void processFixPosition();
};
