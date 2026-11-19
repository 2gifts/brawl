#include <revolution/OS/OSError.h>
#include <so/article/so_article.h>
#include <so/article/so_article_mediator.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/event/so_event_system.h>
#include <so/so_array.h>
#include <so/stageobject.h>
#include <types.h>

void soGenerateArticleManageModuleImpl::getArticleList(soArray<soArticle*>* articles, int articleId, int selector) {
    switch (selector) {
    case 0:
        for (int i = 0; i < m_articles->size(); i++) {
            if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
                articles->push(m_articles->at(i));
            }
        }
        break;
    case 1:
        for (int i = 0; i < m_articles->size(); i++) {
            if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
                articles->push(m_articles->at(i));
                break;
            }
        }
        break;
    case 2:
        for (int i = m_articles->size() - 1; i >= 0; i--) {
            if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
                articles->push(m_articles->at(i));
                break;
            }
        }
        break;
    }
}

void soGenerateArticleManageModuleImpl::shoot(int articleId, int selector, bool unlink) {
    soArticle* article;
    switch (selector) {
    case 1:
        for (int i = m_articles->size() - 1; i >= 0; i--) {
            if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
                int count = m_mediator->getMediateNum();
                article = m_articles->at(i);
                bool shot;
                if (article->getArticleId() < count) {
                    shot = static_cast<soArticleOperator&>(*m_mediator).shoot(m_moduleAccesser, article);
                } else {
                    if (m_dynamicMediator == NULL) {
                        shot = false;
                    } else {
                        shot = m_dynamicMediator->shoot(m_moduleAccesser, article);
                    }
                }
                if (unlink == true || !shot) {
                    eject(article->getTaskId());
                }
                break;
            }
        }
        break;
    case 0:
    case 2:
        for (int i = m_articles->size() - 1; i >= 0; i--) {
            if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
                int count = m_mediator->getMediateNum();
                article = m_articles->at(i);
                bool shot;
                if (article->getArticleId() < count) {
                    shot = static_cast<soArticleOperator&>(*m_mediator).shoot(m_moduleAccesser, article);
                } else {
                    if (m_dynamicMediator == NULL) {
                        shot = false;
                    } else {
                        shot = m_dynamicMediator->shoot(m_moduleAccesser, article);
                    }
                }
                if (unlink == true || !shot) {
                    eject(article->getTaskId());
                }
                if (selector == 2) {
                    break;
                }
            }
        }
        break;
    }
}

void soGenerateArticleManageModuleImpl::activate() { }

void soGenerateArticleManageModuleImpl::removeExist(int articleId, int selector) {
    if (isExist(articleId) == true) {
        remove(articleId, selector);
    }
}

bool soGenerateArticleManageModuleImpl::isExist(int articleId) {
    for (int i = 0; i < m_articles->size(); i++) {
        if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
            return true;
        }
    }
    return false;
}

int soGenerateArticleManageModuleImpl::getNum(int articleId) {
    int num = 0;
    for (int i = 0; i < m_articles->size(); i++) {
        if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
            num++;
        }
    }
    return num;
}

void soGenerateArticleManageModuleImpl::changeMotion(int articleId, int motionKind, bool b) {
    if (!isExist(articleId)) {
        OSReport("\x91\xB6\x8D\xDD\x82\xB5\x82\xC8\x82\xA2" "Article" "\x82\xD6\x82\xCC" "changeMotion" "\x96\xBD\x97\xDF\x82\xC5\x82\xB7" " kind:%d");
        return;
    }
    for (int i = 0; i < m_articles->size(); i++) {
        if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
            m_articles->at(i)->changeMotion(motionKind, b);
        }
    }
}

void soGenerateArticleManageModuleImpl::setFrame(int articleId, float frame) {
    if (isExist(articleId)) {
        for (int i = 0; i < m_articles->size(); i++) {
            if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
                m_articles->at(i)->setFrame(frame);
            }
        }
    }
}

void soGenerateArticleManageModuleImpl::setRate(int articleId, float rate) {
    if (isExist(articleId)) {
        for (int i = 0; i < m_articles->size(); i++) {
            if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
                m_articles->at(i)->setRate(rate);
            }
        }
    }
}

void soGenerateArticleManageModuleImpl::setSituationKind(int articleId, SituationKind situationKind) {
    if (isExist(articleId)) {
        for (int i = 0; i < m_articles->size(); i++) {
            if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
                m_articles->at(i)->setSituationKind(situationKind);
            }
        }
    }
}

bool soGenerateArticleManageModuleImpl::isObserv(char unk1) {
    return unk1 == 0x10;
}

bool soGenerateArticleManageModuleImpl::isGeneratable(int articleId) {
    if (articleId < m_mediator->getMediateNum()) {
        return m_mediator->isGeneratable(m_moduleAccesser, articleId);
    }
    if (m_dynamicMediator != NULL) {
        return m_dynamicMediator->isGeneratable(m_moduleAccesser, articleId);
    }
    return false;
}

int soGenerateArticleManageModuleImpl::getActiveNum(int articleId) {
    return m_mediator->getActiveNum(m_moduleAccesser, articleId);
}

int soGenerateArticleManageModuleImpl::getGenerateMaxNum(int articleId) {
    return m_mediator->getGenerateMaxNum(articleId);
}

int soGenerateArticleManageModuleImpl::getGenerateRestNum(int articleId) {
    return getGenerateMaxNum(articleId) - getActiveNum(articleId);
}

soArticle* soGenerateArticleManageModuleImpl::getArticle(int articleId) {
    for (int i = 0; i < m_articles->size(); i++) {
        if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
            return m_articles->at(i);
        }
    }
    return NULL;
}

soArticle* soGenerateArticleManageModuleImpl::getArticleFromNo(int articleId, int number) {
    int no = 0;
    for (int i = 0; i < m_articles->size(); i++) {
        if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
            if (number == no) {
                return m_articles->at(i);
            }
            no++;
        }
    }
    return NULL;
}

void soGenerateArticleManageModuleImpl::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4) {
    if (unk4 == 1 && eventInfo->m_eventKind == 0x3c) {
        eject(stageObject->m_taskId);
    }
}

void soGenerateArticleManageModuleImpl::setDynamicArticleMediator(soArticleMediator* mediator) {
    m_dynamicMediator = mediator;
}

void soGenerateArticleManageModuleImpl::setAutoRecycle(bool enabled) {
    m_mediator->setAutoRecycle(enabled);
    if (m_dynamicMediator != NULL) {
        m_dynamicMediator->setAutoRecycle(enabled);
    }
}

soArticle* soGenerateArticleManageModuleImpl::generate(int articleId, void* logAttackInfo, bool syncOwnerStatus) {
    soModuleAccesser* moduleAccesser = static_cast<soModuleAccesser*>(logAttackInfo);
    if (moduleAccesser == NULL) {
        moduleAccesser = m_moduleAccesser;
    }
    soArticle* article;
    if (articleId < m_mediator->getMediateNum()) {
        article = m_mediator->generate(articleId, moduleAccesser);
    } else {
        if (m_dynamicMediator == NULL) {
            return reinterpret_cast<soArticle*>(g_ftRobotNullArticleStorage);
        }
        article = m_dynamicMediator->generate(articleId, moduleAccesser);
    }
    article->setSyncOwnerStatus(syncOwnerStatus);
    return article;
}

void soGenerateArticleManageModuleImpl::setVisibilityWhole(int articleId, u8 visible, int selector) {
    if (isExist(articleId)) {
        soArrayVector<soArticle*, 24> articles;
        getArticleList(&articles, articleId, selector);
        for (int i = 0; i < articles.size(); i++) {
            articles.at(i)->setVisibilityWhole(visible);
        }
    }
}

void soGenerateArticleManageModuleImpl::set2nd(int articleId, u8 second, int selector) {
    if (isExist(articleId)) {
        soArrayVector<soArticle*, 24> articles;
        getArticleList(&articles, articleId, selector);
        for (int i = 0; i < articles.size(); i++) {
            articles.at(i)->set2nd(second);
        }
    }
}

void soGenerateArticleManageModuleImpl::have(int articleId, int arg, int selector, int taskId) {
    if (isExist(articleId)) {
        if ((u32)taskId == (u32)-1) {
            taskId = m_moduleAccesser->m_stageObject->m_taskId;
        }
        soArrayVector<soArticle*, 24> articles;
        getArticleList(&articles, articleId, selector);
        for (int i = 0; i < articles.size(); i++) {
            articles.at(i)->have(taskId, arg, -1);
        }
    }
}

void soGenerateArticleManageModuleImpl::eject(int taskId) {
    int j;
    soArticle* article;
    s32 removeId;
    for (int i = m_articles->size() - 1; i >= 0; i--) {
        if (m_articles->at(i) != NULL && (u32)taskId == (u32)m_articles->at(i)->getTaskId()) {
            article = m_articles->at(i);
            int count = m_observers->size();
            for (j = 0; j < count; j++) {
                s16 manageId = article->getArticleEventManageId();
                soArticleEventObserver& observer = m_observers->at(j);
                if (manageId != -1) {
                    removeId = observer.m_manageID;
                    if (manageId == removeId) {
                        if (soEventSystem::getInstance()->getInstanceManager().isContain(removeId) == true) {
                            soEventSystem::getInstance()->getManager(observer.m_manageID)->eraseObserver(observer.m_unitID, observer.m_sendID);
                        }
                        observer.m_sendID = -1;
                        observer.m_manageID = -1;
                    }
                }
            }
            m_articles->erase(i);
            article->unlinkOwner(1);
        }
    }
}

void soGenerateArticleManageModuleImpl::removeExistTaskId(int taskId) {
    int j;
    soArticle* article;
    s32 removeId;
    for (int i = m_articles->size() - 1; i >= 0; i--) {
        if (m_articles->at(i) != NULL && (u32)taskId == (u32)m_articles->at(i)->getTaskId()) {
            soArticle* removed = m_articles->at(i);
            article = m_articles->at(i);
            int count = m_observers->size();
            for (j = 0; j < count; j++) {
                s16 manageId = article->getArticleEventManageId();
                soArticleEventObserver& observer = m_observers->at(j);
                if (manageId != -1) {
                    removeId = observer.m_manageID;
                    if (manageId == removeId) {
                        if (soEventSystem::getInstance()->getInstanceManager().isContain(removeId) == true) {
                            soEventSystem::getInstance()->getManager(observer.m_manageID)->eraseObserver(observer.m_unitID, observer.m_sendID);
                        }
                        observer.m_sendID = -1;
                        observer.m_manageID = -1;
                    }
                }
            }
            m_articles->erase(i);
            article->unlinkOwner(1);
            removed->remove();
        }
    }
}

void soGenerateArticleManageModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    int j;
    soArticle* article;
    s32 removeId;
    for (int i = 0; i < m_articles->size(); i++) {
        if (m_articles->at(i) != NULL) {
            soArticle* removed = m_articles->at(i);
            if (removed->isSyncOwnerStatus() == true) {
                article = m_articles->at(i);
                int count = m_observers->size();
                for (j = 0; j < count; j++) {
                    s16 manageId = article->getArticleEventManageId();
                    soArticleEventObserver& observer = m_observers->at(j);
                    if (manageId != -1) {
                        removeId = observer.m_manageID;
                        if (manageId == removeId) {
                            if (soEventSystem::getInstance()->getInstanceManager().isContain(removeId) == true) {
                                soEventSystem::getInstance()->getManager(observer.m_manageID)->eraseObserver(observer.m_unitID, observer.m_sendID);
                            }
                            observer.m_sendID = -1;
                            observer.m_manageID = -1;
                        }
                    }
                }
                m_articles->erase(i);
                article->unlinkOwner(1);
                removed->remove();
            }
        }
    }
}

void soGenerateArticleManageModuleImpl::changeStatus(int articleId, int statusKind, int selector) {
    int j;
    soArticle* article;
    s32 removeId;
    if (!isExist(articleId)) {
        OSReport("\x91\xB6\x8D\xDD\x82\xB5\x82\xC8\x82\xA2" "Article" "\x82\xD6\x82\xCC" "changeStatus" "\x96\xBD\x97\xDF\x82\xC5\x82\xB7" " kind:%d");
        return;
    }
    soArrayVector<soArticle*, 24> articles;
    getArticleList(&articles, articleId, selector);
    for (int i = 0; i < articles.size(); i++) {
        if (articles.at(i)->isSyncOwnerStatus() == true) {
            soArticle* removed = m_articles->at(i);
            article = m_articles->at(i);
            int count = m_observers->size();
            for (j = 0; j < count; j++) {
                s16 manageId = article->getArticleEventManageId();
                soArticleEventObserver& observer = m_observers->at(j);
                if (manageId != -1) {
                    removeId = observer.m_manageID;
                    if (manageId == removeId) {
                        if (soEventSystem::getInstance()->getInstanceManager().isContain(removeId) == true) {
                            soEventSystem::getInstance()->getManager(observer.m_manageID)->eraseObserver(observer.m_unitID, observer.m_sendID);
                        }
                        observer.m_sendID = -1;
                        observer.m_manageID = -1;
                    }
                }
            }
            m_articles->erase(i);
            article->unlinkOwner(1);
            removed->remove();
        } else {
            articles.at(i)->changeStatus(statusKind);
        }
    }
}

void soGenerateArticleManageModuleImpl::remove(int articleId, int selector) {
    if (isExist(articleId)) {
        switch (selector) {
        case 1: {
            int j;
            soArticle* article;
            s32 removeId;
            for (int i = 0; i < m_articles->size(); i++) {
                if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
                    soArticle* removed = m_articles->at(i);
                    article = m_articles->at(i);
                    int count = m_observers->size();
                    for (j = 0; j < count; j++) {
                        s16 manageId = article->getArticleEventManageId();
                        soArticleEventObserver& observer = m_observers->at(j);
                        if (manageId != -1) {
                            removeId = observer.m_manageID;
                            if (manageId == removeId) {
                                if (soEventSystem::getInstance()->getInstanceManager().isContain(removeId) == true) {
                                    soEventSystem::getInstance()->getManager(observer.m_manageID)->eraseObserver(observer.m_unitID, observer.m_sendID);
                                }
                                observer.m_sendID = -1;
                                observer.m_manageID = -1;
                            }
                        }
                    }
                    m_articles->erase(i);
                    article->unlinkOwner(1);
                    removed->remove();
                    break;
                }
            }
            break;
        }
        case 0:
        case 2: {
            int i;
            soArticle* removed;
            s32 removeId;
            int j;
            soArticle* article;
            for (i = m_articles->size() - 1; i >= 0; i--) {
                if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
                    removed = m_articles->at(i);
                    article = m_articles->at(i);
                    int count = m_observers->size();
                    for (j = 0; j < count; j++) {
                        s16 manageId = article->getArticleEventManageId();
                        soArticleEventObserver& observer = m_observers->at(j);
                        if (manageId != -1) {
                            removeId = observer.m_manageID;
                            if (manageId == removeId) {
                                if (soEventSystem::getInstance()->getInstanceManager().isContain(removeId) == true) {
                                    soEventSystem::getInstance()->getManager(observer.m_manageID)->eraseObserver(observer.m_unitID, observer.m_sendID);
                                }
                                observer.m_sendID = -1;
                                observer.m_manageID = -1;
                            }
                        }
                    }
                    m_articles->erase(i);
                    article->unlinkOwner(1);
                    removed->remove();
                    if (selector == 2) {
                        break;
                    }
                }
            }
            break;
        }
        }
    }
}

#include <so/so_instance_manager.h>
// TODO: clean up, give variables real names
namespace jj_932A0 {
class Mgr {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3(s16 a, s16 b);
};

class soEventSystem {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual u32 v6(s16 id);
};

extern "C" soEventSystem* getInstance__13soEventSystemFv();
extern "C" Mgr* getManager__13soEventSystemFl(soEventSystem*, s32);

struct Ev {
    s16 m0;
    s16 m2;
    s16 m4;
    s16 m6;
    s16 m8;
};

class Elem {
public:
    virtual void e0();
    virtual void e1();
    virtual void e2();
    virtual void e3();
    virtual void e4();
    virtual void e5();
    virtual void e6();
    virtual s16 e7();
    virtual void e8();
    virtual void e9();
    virtual void e10();
    virtual void e11();
    virtual void e12();
    virtual void e13();
    virtual void e14();
    virtual void e15();
    virtual void e16();
    virtual void e17();
    virtual void e18();
    virtual void e19();
    virtual void e20();
    virtual void e21();
    virtual s32 e22();
    virtual void e23();
    virtual void e24();
    virtual void e25();
    virtual void e26(s32 a);
};

class ElemList {
public:
    virtual void l0();
    virtual Elem** at(s32 i);
    virtual void l2();
    virtual s32 size();
    virtual void l4();
    virtual void l5();
    virtual void l6();
    virtual void l7();
    virtual void l8();
    virtual void l9();
    virtual void l10();
    virtual void l11();
    virtual void remove(s32 i);
};

class EvList {
public:
    virtual void l0();
    virtual Ev* at(s32 i);
    virtual void l2();
    virtual s32 size();
};

class Obj {
public:
    virtual void o0();
    virtual void o1();
    virtual void o2();
    virtual void o3();
    virtual void o4();
    virtual void o5();
    virtual void o6();
};

struct Owner {
    void* vt;
    char pad[0x28];
    ElemList* m2c;
    Obj* m30;
    EvList* m34;
    Obj* m38;

    inline void run(u32 mode) {
        if (mode == 1U) {
            s32 j;
            Elem* b;
            s16 y;
            s32 i;
            Elem* a;
            for (i = m2c->size() - 1; i >= 0; i--) {
                a = *m2c->at(i);
                b = *m2c->at(i);
                s32 n = m34->size();
                for (j = 0; j < n; j++) {
                    s16 x = b->e7();
                    Ev* ev = m34->at(j);
                    if (x != -1) {
                        y = ev->m4;
                        if (x == y) {
                            if (getInstance__13soEventSystemFv()->v6(y) == 1U) {
                                getManager__13soEventSystemFl(getInstance__13soEventSystemFv(), ev->m4)->v3(ev->m6, ev->m8);
                            }
                            ev->m8 = -1;
                            ev->m4 = -1;
                        }
                    }
                }
                m2c->remove(i);
                b->e26(1);
                a->e5();
            }
            m2c->l8();
            m30->o6();
            if (m38 != 0) {
                m38->o6();
            }
        } else {
            for (s32 i = m2c->size() - 1; i >= 0; i--) {
                if (*m2c->at(i) != 0) {
                    switch ((*m2c->at(i))->e22()) {
                    case 0: {
                        Elem* a;
                        s32 j;
                        Elem* b;
                        s16 y;
                        a = *m2c->at(i);
                        b = *m2c->at(i);
                        s32 n = m34->size();
                        for (j = 0; j < n; j++) {
                            s16 x = b->e7();
                            Ev* ev = m34->at(j);
                            if (x != -1) {
                                y = ev->m4;
                                if (x == y) {
                                    if (getInstance__13soEventSystemFv()->v6(y) == 1U) {
                                        getManager__13soEventSystemFl(getInstance__13soEventSystemFv(), ev->m4)->v3(ev->m6, ev->m8);
                                    }
                                    ev->m8 = -1;
                                    ev->m4 = -1;
                                }
                            }
                        }
                        m2c->remove(i);
                        b->e26(1);
                        a->e5();
                        break;
                    }
                    case 1: {
                        s16 y;
                        s32 j;
                        Elem* b;
                        b = *m2c->at(i);
                        s32 n = m34->size();
                        for (j = 0; j < n; j++) {
                            s16 x = b->e7();
                            Ev* ev = m34->at(j);
                            if (x != -1) {
                                y = ev->m4;
                                if (x == y) {
                                    if (getInstance__13soEventSystemFv()->v6(y) == 1U) {
                                        getManager__13soEventSystemFl(getInstance__13soEventSystemFv(), ev->m4)->v3(ev->m6, ev->m8);
                                    }
                                    ev->m8 = -1;
                                    ev->m4 = -1;
                                }
                            }
                        }
                        m2c->remove(i);
                        b->e26(1);
                        break;
                    }
                    case 2:
                        break;
                    }
                }
            }
        }
    }
};

extern "C" void fn_27_932A0(Owner* self, u32 mode) {
    self->run(mode);
}
}

// TODO: clean up, give variables real names
namespace jj_94E00 {
struct Item {
 virtual u32 state();
 virtual void v1();
 virtual void v2();
 virtual void v3();
 virtual void v4();
 virtual void v5();
 virtual void v6();
 virtual short id();
 virtual u32 key();
 virtual void v9();
 virtual void v10();
 virtual void v11();
 virtual void v12();
 virtual void v13();
 virtual void v14();
 virtual void v15();
 virtual void v16();
 virtual void v17();
 virtual void v18();
 virtual void v19();
 virtual void v20();
 virtual void v21();
 virtual void v22();
 virtual void v23();
 virtual void v24();
 virtual void setup(int, void*);
};
struct Entry {
 virtual void set(short, int);
 virtual void v1();
 short value;
};
struct List {
 virtual void v0();
 virtual Item** at(int);
 virtual void v2();
 virtual int size();
 virtual void v4();
 virtual void v5();
 virtual void v6();
 virtual void v7();
 virtual void v8();
 virtual void v9();
 virtual void add(Item**);
};
struct Entries {
 virtual void v0();
 virtual Entry* at(int);
 virtual void v2();
 virtual int size();
};
struct Checker {
 virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
 virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
 virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
 virtual void v12(); virtual void v13(); virtual int test(u32);
};
struct Inner { char pad[0x28]; void* data; };
struct Sub { char pad[0x54]; Checker* checker; };
struct Context { char pad[8]; Inner* inner; char pad2[0xcc]; Sub* sub; };
struct Self { char pad[0x28]; Context* context; List* list; char pad2[4]; Entries* entries; };
inline bool contains(Self* self, u32 key) {
 for (int i = 0; i < self->list->size(); i++) {
  if (*self->list->at(i) && (*self->list->at(i))->key() == key) return true;
 }
 return false;
}
extern "C" void fn_27_94E00(Self* self, Item* item) {
 self->list->size();
 if (item->state() != 1) {
  u32 key = item->key();
  if (contains(self, key) != true) {
   item->setup(1, self->context->inner->data);
   Checker* checker = self->context->sub->checker;
   if (checker->test(item->key())) {
    self->list->add(&item);
    int count = self->entries->size();
    Entry* entry;
    for (int i = 0; i < count; i++) {
     entry = self->entries->at(i);
     if (entry->value == -1) {
      entry->set(item->id(), -1);
      return;
     }
    }
   }
  }
 }
}
}

// TODO: clean up, give variables real names
namespace jj_95028 {


extern "C" {
extern char lbl_27_data_C910[];
extern char __RTTI__11soEventUnit[];
void* __dynamic_cast(void*, long, const void*, const void*, short);
}

struct Self {
    void* vt;
    s16 a;
    s16 b;
    s16 c;
};

class Class_Article {
public:
    virtual void __dt__19soCaptureModuleImplFv();
    virtual void activate__19soCaptureModuleImplFv();
    virtual void deactivate__19soCaptureModuleImplFv();
    virtual void setNodes__19soCaptureModuleImplFiif();
    virtual void initInfo__19soCaptureModuleImplFv();
    virtual void isCapture__19soCaptureModuleImplFv();
    virtual s16 getCatchTaskId__19soCaptureModuleImplFv(Self*, s8);
    virtual void fn_27_6636C();
    virtual void getNodeOffset__19soCaptureModuleImplFb();
};

extern "C" void fn_27_95028(Self* self, s32 arg1, s8 arg2) {
    soEventManager* mgr;
    u32 f31 = 0;
    s32 f30 = 0;
    s32 f4 = 0;
    s32 f5 = 0;
    s16 ret;
    s16 id;
    s16 a = self->a;

    if (a >= 0 && self->b >= 0) {
        f5 = 1;
    }
    if (f5 != 0 && self->c > -1) {
        f4 = 1;
    }
    if (f4 != 0) {
        if (soEventSystem::getInstance()->getInstanceManager().isContain(a) == 1) {
            f30 = 1;
        }
    }
    if (f30 != 0) {
        if (soEventSystem::getInstance()->getManager(self->a)->getObserverCapacity(self->b) == 1) {
            f31 = 1;
        }
    }
    if (f31 == 1) {
        id = self->a;
        if (soEventSystem::getInstance()->getInstanceManager().isContain(id) == 1) {
            soEventSystem::getInstance()->getManager(self->a)->eraseObserver(self->b, self->c);
        }
        self->c = -1;
        self->a = -1;
    }
    if (arg1 > -1 && self->b > -1) {
        if (soEventSystem::getInstance()->getInstanceManager().isContain(arg1) != 0) {
            if (soEventSystem::getInstance()->getManager(arg1)->isNullUnit(self->b) != 1) {
                mgr = soEventSystem::getInstance()->getManager(arg1);
                if (mgr->getObserverCapacity(self->b) == 0) {
                    ret = -1;
                } else {
                    soEventUnit* unit = mgr->getEventUnit(self->b);
                    Class_Article* cast = (Class_Article*)__dynamic_cast(
                        unit, 0, lbl_27_data_C910, __RTTI__11soEventUnit, 0);
                    if (cast == 0) {
                        ret = -1;
                    } else {
                        ret = cast->getCatchTaskId__19soCaptureModuleImplFv(self, arg2);
                    }
                }
                self->c = ret;
                if (ret > -1 && ret > -1) {
                    self->a = (s16)arg1;
                }
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_9745C {
extern "C" void notifyEventChangeStatus__33soGenerateArticleManageModuleImplFiiP12soStatusDataP16soModuleAccesser(void*);

extern "C" void fn_27_9745C(void* self) {
    notifyEventChangeStatus__33soGenerateArticleManageModuleImplFiiP12soStatusDataP16soModuleAccesser(reinterpret_cast<u8*>(self) - 0x4);
}
}

// TODO: clean up, give variables real names
namespace jj_97464 {
extern "C" void fn_27_9536C(void*);

extern "C" void fn_27_97464(void* self) {
    fn_27_9536C(reinterpret_cast<u8*>(self) - 0x10);
}
}

// TODO: clean up, give variables real names
namespace jj_9746C {
extern "C" void isObserv__33soGenerateArticleManageModuleImplFc(void*);

extern "C" void fn_27_9746C(void* self) {
    isObserv__33soGenerateArticleManageModuleImplFc(reinterpret_cast<u8*>(self) - 0x10);
}
}

// TODO: clean up, give variables real names
namespace jj_97474 {
extern "C" void notifyEventLink__33soGenerateArticleManageModuleImplFP15soLinkEventArgsP16soModuleAccesserP11StageObjecti(void*);

extern "C" void fn_27_97474(void* self) {
    notifyEventLink__33soGenerateArticleManageModuleImplFP15soLinkEventArgsP16soModuleAccesserP11StageObjecti(reinterpret_cast<u8*>(self) - 0x1c);
}
}

