#pragma once
#include <types.h>
#include <so/situation/so_situation_module_impl.h>
#include <StaticAssert.h>
class soArticle;
class soModuleAccesser;
class soArticleMediator;
class soArticleEventObserver;
struct soLogAttackInfo;
template<class T> class soArray;
// Primary interface order verified against the concrete implementation vtable.
// HYPOTHESIS: unused selector source types are represented as int until their
// original enum declarations are recovered; argument counts and ABI are verified.
class soGenerateArticleManageModule {
public:
    virtual ~soGenerateArticleManageModule();
    virtual void activate();
    virtual void deactivate(bool treatAsRemoved); // HYPOTHESIS: the flag says the owner counts as removed (seen in StageObject::deactivate)
    // HYPOTHESIS: the third argument is typed void* (soLogAttackInfo*): callers that pass the same zero for both optional
    // arguments get a register copy (mr) for the second one only with an identical pointer type.
    // HYPOTHESIS: the two trailing arguments are boolean flags (not pointers): callers pass the same zero twice and the
    // second one is a register copy, which only a narrow parameter type reproduces.
    virtual soArticle* generate(int articleId, bool, bool);
    virtual void have(int articleId, int, int selector, int taskId);
    virtual void shoot(int articleId, int selector, bool);
    virtual void changeMotion(int articleId, int motionKind, bool);
    virtual void setFrame(int articleId, float);
    virtual void setRate(int articleId, float);
    virtual void changeStatus(int articleId, int statusKind, int selector);
    virtual void setVisibilityWhole(int articleId, u8, int selector);
    virtual void set2nd(int articleId, u8, int selector);
    virtual void setSituationKind(int articleId, SituationKind);
    virtual void entry(soArticle*);
    virtual void eject(int taskId);
    virtual void remove(int articleId, int selector);
    virtual void removeExist(int articleId, int selector);
    virtual void removeExistTaskId(int taskId);
    virtual bool isExist(int articleId);
    virtual int getNum(int articleId);
    virtual bool isGeneratable(int articleId);
    virtual int getActiveNum(int articleId);
    virtual int getGenerateMaxNum(int articleId);
    virtual int getGenerateRestNum(int articleId);
    virtual soArticle* getArticle(int articleId);
    virtual soArticle* getArticleFromNo(int articleId, int number);
    virtual void getArticleList(soArray<soArticle*>*, int articleId, int selector);
    virtual void setDynamicArticleMediator(soArticleMediator*);
    virtual void setAutoRecycle(bool);
};
static_assert(sizeof(soGenerateArticleManageModule) == 4, "Article interface layout is wrong!");
// Primary base verified by implementation RTTI at offset zero. Existing R.O.B.
// construction ABI retained; secondary observers and fields remain opaque.
class soGenerateArticleManageModuleImpl : public soGenerateArticleManageModule {
    u8 m_unreconstructed[0x38];
public:
    soGenerateArticleManageModuleImpl(soModuleAccesser*, soArray<soArticle*>*,
        soArticleMediator*, soArray<soArticleEventObserver>*);
    virtual ~soGenerateArticleManageModuleImpl();
};
static_assert(sizeof(soGenerateArticleManageModuleImpl) == 0x3c, "Article module layout is wrong!");
