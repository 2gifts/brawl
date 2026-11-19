#include <so/collision/so_collision_shield_event_presenter_shield.h>

bool soCollisionShieldEventObserver::notifyEventCollisionShieldCheck() {
    return false;
}

#include <so/event/so_event_system.h>
// TODO: clean up, give variables real names
namespace jj_47220 {
extern "C" {
extern char lbl_27_data_4088[];
extern char lbl_27_data_4040[];
extern char lbl_27_data_3FD0[];
extern char lbl_27_data_3FD8[];
extern char __RTTI__11soEventUnit[];

void* getInstance__13soEventSystemFv();
void* getManager__13soEventSystemFl(void*, long);
void* __dynamic_cast(void*, long, const void*, const void*, long);
}

struct I9 {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual s16 get(void*);
};

struct Sys {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual s32 check(long);
};

struct Mgr {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual s32 check(long);
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void* s10(long);
};

struct Unit {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual s32 get();
};

struct Self {
    void* f0;
    void* f4;
    s16 f8;
    s16 fA;
    s32 fC;
};

inline s32 getResult(Self* self) {
    s16 id = self->fA;
    Mgr* mgr2 = (Mgr*)getManager__13soEventSystemFl(
        getInstance__13soEventSystemFv(), self->f8);
    if (mgr2->check(id) == 0) {
        return 0;
    }
    Unit* u = (Unit*)__dynamic_cast(
        mgr2->s10(id), 0, lbl_27_data_3FD0, __RTTI__11soEventUnit, 0);
    if (u == 0) {
        return 0;
    }
    return u->get();
}

extern "C" void* __ct__37soCollisionShieldEventPresenterShieldFP16soModuleAccesser(Self* self, void* arg1) {
    s32 result = 0;
    s32 eid;
    Mgr* mgr2;
    Mgr* mgr;
    char* ctx;
    char* obj;

    self->f0 = lbl_27_data_4088;
    ctx = *(char**)((char*)arg1 + 0xD8);
    obj = *(char**)(ctx + 0x80);
    eid = ((I9*)(obj + 8))->get(arg1);
    self->f4 = lbl_27_data_4040;
    self->f8 = eid;
    self->fA = 14;
    self->fC = result;

    if (eid > 0 && eid > -1) {
        if (((Sys*)getInstance__13soEventSystemFv())->check(eid) != 0) {
            mgr = (Mgr*)getManager__13soEventSystemFl(
                getInstance__13soEventSystemFv(), self->f8);
            if (mgr->check(self->fA) == 0) {
                self->fC = 0;
            } else {
                result = getResult(self);
                self->fC = result;
            }
        }
    }

    self->f0 = lbl_27_data_3FD8;
    self->f4 = lbl_27_data_3FD8 + 0x18;
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_473AC {
extern "C" void __dl__FPv(void*);

extern "C" void* fn_27_473AC(void* ptr, int count) {
    if (ptr != 0 && count > 0)
        __dl__FPv(ptr);
    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_473EC {
extern "C" void __dl__FPv(void*);

extern "C" void* fn_27_473EC(void* ptr, int count) {
    if (ptr != 0 && count > 0)
        __dl__FPv(ptr);
    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_4742C {
extern "C" {
void __dl__FPv(void*);
}

extern "C" void* __dt__37soCollisionShieldEventPresenterShieldFv(void* p, int n) {
    if (p != 0 && n > 0) {
        __dl__FPv(p);
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_4746C {
struct Obj {
    virtual void o0();
    virtual void o1(int, int, int, int, float, float, float);
};

struct Container {
    virtual void c0();
    virtual void c1();
    virtual Obj** c2(int);
    virtual void c3();
    virtual int c4();
};

struct Manager {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual int m4(int);
    virtual void m5();
    virtual void m6();
    virtual void m7();
    virtual void* m8(int);
};

struct Instance {
    virtual void i0();
    virtual void i1();
    virtual void i2();
    virtual void i3();
    virtual void i4();
    virtual void i5();
    virtual int i6(int);
};

struct EventUnit {
    virtual void u0();
    virtual void u1();
    virtual void u2();
    virtual void u3();
    virtual void u4();
    virtual void u5();
    virtual void u6();
    virtual Container* u7();
};

struct SoEventSystem {
    char pad0[8];
    short id1;
    short id2;
    Container* cached;
};

extern "C" {
extern char lbl_27_bss_BE0[];
extern char lbl_27_data_3FD0[];
extern char __RTTI__11soEventUnit[];
void* __dynamic_cast(void*, long, const void*, const void*, short);
Instance* getInstance__13soEventSystemFv();
Manager* getManager__13soEventSystemFl(Instance*, long);
}

extern "C" void fn_27_4746C(SoEventSystem* self, int arg1, int arg2, int arg3, int arg4, float f1, float f2, float f3) {
    Manager* mgr;
    int i;
    Container* list;
    short id1;
    short id2;
    Container* cur;
    EventUnit* unit;
    int flag;

    id1 = self->id1;
    if (id1 <= -1) {
        list = (Container*)lbl_27_bss_BE0;
        goto LOOP;
    }
    if (self->cached != 0) {
        goto C;
    }
    if (id1 <= -1 || getInstance__13soEventSystemFv()->i6(id1) == 0) {
        flag = 0;
        goto CHECK;
    }
    if (getManager__13soEventSystemFl(getInstance__13soEventSystemFv(), self->id1)->m4(self->id2) == 0) {
        self->cached = 0;
        flag = 0;
        goto CHECK;
    }
    id2 = self->id2;
    mgr = getManager__13soEventSystemFl(getInstance__13soEventSystemFv(), self->id1);
    if (mgr->m4(id2) == 0) {
        cur = 0;
    } else {
        unit = (EventUnit*)__dynamic_cast(mgr->m8(id2), 0, lbl_27_data_3FD0, __RTTI__11soEventUnit, 0);
        if (unit == 0) {
            cur = 0;
        } else {
            cur = unit->u7();
        }
    }
    self->cached = cur;
    flag = (cur != 0);
CHECK:
    if (flag == 0) {
        list = (Container*)lbl_27_bss_BE0;
        goto LOOP;
    }
C:
    mgr = getManager__13soEventSystemFl(getInstance__13soEventSystemFv(), self->id1);
    if (mgr->m4(self->id2) == 0) {
        self->cached = 0;
        list = (Container*)lbl_27_bss_BE0;
    } else {
        list = self->cached;
    }
LOOP:
    for (i = 0; i < list->c4(); i++) {
        Obj** p = list->c2(i);
        (*p)->o1(arg1, arg2, arg3, arg4, f1, f2, f3);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_476B8 {
struct Obj {
    virtual void o0();
    virtual void o1();
    virtual void o2(int, int, int, int);
};

struct Container {
    virtual void c0();
    virtual void c1();
    virtual Obj** c2(int);
    virtual void c3();
    virtual int c4();
};

struct Manager {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual int m4(int);
    virtual void m5();
    virtual void m6();
    virtual void m7();
    virtual void* m8(int);
};

struct Instance {
    virtual void i0();
    virtual void i1();
    virtual void i2();
    virtual void i3();
    virtual void i4();
    virtual void i5();
    virtual int i6(int);
};

struct EventUnit {
    virtual void u0();
    virtual void u1();
    virtual void u2();
    virtual void u3();
    virtual void u4();
    virtual void u5();
    virtual void u6();
    virtual Container* u7();
};

struct SoEventSystem {
    char pad0[8];
    short id1;
    short id2;
    Container* cached;
};

extern "C" {
extern char lbl_27_bss_BE0[];
extern char lbl_27_data_3FD0[];
extern char __RTTI__11soEventUnit[];
void* __dynamic_cast(void*, long, const void*, const void*, short);
Instance* getInstance__13soEventSystemFv();
Manager* getManager__13soEventSystemFl(Instance*, long);
}

extern "C" void fn_27_476B8(SoEventSystem* self, int arg1, int arg2, int arg3, int arg4) {
    Manager* mgr;
    int i;
    Container* list;
    short id1;
    short id2;
    Container* cur;
    EventUnit* unit;
    int flag;

    id1 = self->id1;
    if (id1 <= -1) {
        list = (Container*)lbl_27_bss_BE0;
        goto LOOP;
    }
    if (self->cached != 0) {
        goto C;
    }
    if (id1 <= -1 || getInstance__13soEventSystemFv()->i6(id1) == 0) {
        flag = 0;
        goto CHECK;
    }
    if (getManager__13soEventSystemFl(getInstance__13soEventSystemFv(), self->id1)->m4(self->id2) == 0) {
        self->cached = 0;
        flag = 0;
        goto CHECK;
    }
    id2 = self->id2;
    mgr = getManager__13soEventSystemFl(getInstance__13soEventSystemFv(), self->id1);
    if (mgr->m4(id2) == 0) {
        cur = 0;
    } else {
        unit = (EventUnit*)__dynamic_cast(mgr->m8(id2), 0, lbl_27_data_3FD0, __RTTI__11soEventUnit, 0);
        if (unit == 0) {
            cur = 0;
        } else {
            cur = unit->u7();
        }
    }
    self->cached = cur;
    flag = (cur != 0);
CHECK:
    if (flag == 0) {
        list = (Container*)lbl_27_bss_BE0;
        goto LOOP;
    }
C:
    mgr = getManager__13soEventSystemFl(getInstance__13soEventSystemFv(), self->id1);
    if (mgr->m4(self->id2) == 0) {
        self->cached = 0;
        list = (Container*)lbl_27_bss_BE0;
    } else {
        list = self->cached;
    }
LOOP:
    for (i = 0; i < list->c4(); i++) {
        Obj** p = list->c2(i);
        (*p)->o2(arg1, arg2, arg3, arg4);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_478D4 {

struct Elem {
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual unsigned int f5();
};
struct List {
    virtual void f2();
    virtual void f3();
    virtual Elem** f4(int);
    virtual void f5();
    virtual int f6();
};
struct Inst {
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f5();
    virtual void f6();
    virtual void f7();
    virtual void* f8(int);
};
struct Unit {
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f5();
    virtual void f6();
    virtual void f7();
    virtual void f8();
    virtual List* f9();
};
struct Sys {
    void* unk0;
    char pad4[4];
    short unk8;
    short unkA;
    List* unkC;
};
extern "C" {
void* __dynamic_cast(void*, long, const void*, const void*, short);
extern char lbl_27_bss_BE0[];
extern char lbl_27_data_3FD0[];
extern char __RTTI__11soEventUnit[];
}
extern "C" int fn_27_478D4(Sys* self) {
    soEventManager* mgr;
    List* v;
    Unit* p;
    short a;
    short id = self->unk8;
    int ok;
    List* list;
    int ret;
    int i;
    if (id <= -1) {
        list = (List*)lbl_27_bss_BE0;
        goto loop;
    }
    if (self->unkC != 0) goto blockB;
    if (id <= -1 || ((Inst*)soEventSystem::getInstance())->f8(id) == 0) {
        ok = 0;
        goto check;
    }
    if (soEventSystem::getInstance()->getManager(self->unk8)->getObserverCapacity(self->unkA) == 0) {
        self->unkC = 0;
        ok = 0;
        goto check;
    }
    a = self->unkA;
    mgr = soEventSystem::getInstance()->getManager(self->unk8);
    if (mgr->getObserverCapacity(a) == 0) {
        v = 0;
    } else {
        p = (Unit*)__dynamic_cast(mgr->getEventUnit(a), 0, lbl_27_data_3FD0, __RTTI__11soEventUnit, 0);
        if (p == 0) {
            v = 0;
        } else {
            v = p->f9();
        }
    }
    self->unkC = v;
    ok = (v != 0);
check:
    if (ok != 0) goto blockB;
    list = (List*)lbl_27_bss_BE0;
    goto loop;
blockB:
    if (soEventSystem::getInstance()->getManager(self->unk8)->getObserverCapacity(self->unkA) == 0) {
        self->unkC = 0;
        list = (List*)lbl_27_bss_BE0;
    } else {
        list = self->unkC;
    }
loop:
    ret = 0;
    for (i = 0; i < list->f6(); i++) {
        Elem* e = *list->f4(i);
        if (e->f5() == 1U) ret = 1;
    }
    return ret;
}
}

// TODO: clean up, give variables real names
namespace jj_47AF4 {
extern "C" void fn_27_47AF4(void) {}
}

// TODO: clean up, give variables real names
namespace jj_47AF8 {
extern "C" void __dt__37soCollisionShieldEventPresenterShieldFv(void*);

extern "C" void fn_27_47AF8(void* self) {
    __dt__37soCollisionShieldEventPresenterShieldFv(reinterpret_cast<u8*>(self) - 0x4);
}
}

