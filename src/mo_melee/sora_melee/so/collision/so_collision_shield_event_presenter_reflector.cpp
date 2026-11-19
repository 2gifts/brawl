#include <so/collision/so_collision_shield_event_presenter_reflector.h>

bool soCollisionReflectorEventObserver::notifyEventCollisionReflectorCheck() {
    return false;
}

#include <so/event/so_event_system.h>
// TODO: clean up, give variables real names
namespace jj_47B00 {

extern "C" {
extern char lbl_27_data_4088[];
extern char lbl_27_data_4190[];
extern char lbl_27_data_4118[];
extern char lbl_27_data_4120[];
extern char __RTTI__11soEventUnit[];
void* __dynamic_cast(void*, long, const void*, const void*, long);
}

class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual short slot7(void*);
    virtual void slot8();
};

struct Sys {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual int check(long);
};

struct Mgr {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual int check(long);
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void* get(long);
};

struct Unit {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual int get();
};

struct Self {
    void* f0;
    void* f4;
    short f8;
    short fA;
    int fC;
};

inline int getResult(Self* self) {
    short id = self->fA;
    Mgr* mgr = (Mgr*)soEventSystem::getInstance()->getManager(self->f8);
    if (mgr->check(id) == 0)
        return 0;
    Unit* u = (Unit*)__dynamic_cast(mgr->get(id), 0, lbl_27_data_4118, __RTTI__11soEventUnit, 0);
    if (u == 0)
        return 0;
    return u->get();
}

extern "C" void* __ct__40soCollisionShieldEventPresenterReflectorFP16soModuleAccesser(Self* self, void* arg1) {
    int result = 0;
    int eid;
    Mgr* mgr;
    char* ctx;
    char* obj;

    self->f0 = lbl_27_data_4088;
    ctx = *(char**)((char*)arg1 + 0xD8);
    obj = *(char**)(ctx + 0x80);
    eid = ((Object_vptr_0*)(obj + 8))->slot7(arg1);
    self->f4 = lbl_27_data_4190;
    self->f8 = eid;
    self->fA = 15;
    self->fC = result;

    if (eid > 0 && eid > -1) {
        if (((Sys*)soEventSystem::getInstance())->check(eid) != 0) {
            mgr = (Mgr*)soEventSystem::getInstance()->getManager(self->f8);
            if (mgr->check(self->fA) == 0) {
                self->fC = 0;
            } else {
                result = getResult(self);
                self->fC = result;
            }
        }
    }

    self->f0 = lbl_27_data_4120;
    self->f4 = lbl_27_data_4120 + 0x18;
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_47C8C {
extern "C" void __dl__FPv(void*);

extern "C" void* fn_27_47C8C(void* p, int n) {
    if (p) {
        if (n > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_47CCC {
extern "C" {
void __dl__FPv(void*);
void* __dt__40soCollisionShieldEventPresenterReflectorFv(void* p, int n) {
    if (p != 0 && n > 0) {
        __dl__FPv(p);
    }
    return p;
}
}
}

// TODO: clean up, give variables real names
namespace jj_47F58 {
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
extern char lbl_27_bss_C00[];
extern char lbl_27_data_4118[];
extern char __RTTI__11soEventUnit[];
void* __dynamic_cast(void*, long, const void*, const void*, short);
Instance* getInstance__13soEventSystemFv();
Manager* getManager__13soEventSystemFl(Instance*, long);

void fn_27_47F58(SoEventSystem* self, int arg1, int arg2, int arg3, int arg4) {
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
        list = (Container*)lbl_27_bss_C00;
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
        unit = (EventUnit*)__dynamic_cast(mgr->m8(id2), 0, lbl_27_data_4118, __RTTI__11soEventUnit, 0);
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
        list = (Container*)lbl_27_bss_C00;
        goto LOOP;
    }
C:
    mgr = getManager__13soEventSystemFl(getInstance__13soEventSystemFv(), self->id1);
    if (mgr->m4(self->id2) == 0) {
        self->cached = 0;
        list = (Container*)lbl_27_bss_C00;
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
}

// TODO: clean up, give variables real names
namespace jj_48174 {
struct Obj;

struct Container {
    virtual void c0();
    virtual void c1();
    virtual Obj** c2(int);
    virtual void c3();
    virtual int c4();
};

struct Obj {
    virtual void o0();
    virtual void o1();
    virtual void o2();
    virtual unsigned int o3();
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

struct SoEventSystem {
    char pad0[8];
    short id1;
    short id2;
    Container* cached;
};

extern "C" {
extern char lbl_27_bss_C00[];
extern char lbl_27_data_4118[];
extern char __RTTI__11soEventUnit[];
void* __dynamic_cast(void*, long, const void*, const void*, short);
Instance* getInstance__13soEventSystemFv();
Manager* getManager__13soEventSystemFl(Instance*, long);

inline Container* resolveList(SoEventSystem* self) {
    short id1;
    short id2;
    Manager* mgr;
    Container* cur;
    EventUnit* unit;
    int flag;

    id1 = self->id1;
    if (id1 <= -1) {
        return (Container*)lbl_27_bss_C00;
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
        unit = (EventUnit*)__dynamic_cast(mgr->m8(id2), 0, lbl_27_data_4118, __RTTI__11soEventUnit, 0);
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
        return (Container*)lbl_27_bss_C00;
    }
C:
    mgr = getManager__13soEventSystemFl(getInstance__13soEventSystemFv(), self->id1);
    if (mgr->m4(self->id2) == 0) {
        self->cached = 0;
        return (Container*)lbl_27_bss_C00;
    }
    return self->cached;
}

int fn_27_48174(SoEventSystem* self) {
    Container* list = resolveList(self);
    int result = 0;
    int i = 0;
    while (i < list->c4()) {
        Obj** p = list->c2(i);
        if ((*p)->o3() == 1) {
            result = 1;
        }
        i++;
    }
    return result;
}
}
}

// TODO: clean up, give variables real names
namespace jj_48394 {
extern "C" void __dt__40soCollisionShieldEventPresenterReflectorFv(void*);

extern "C" void fn_27_48394(void* self) {
    __dt__40soCollisionShieldEventPresenterReflectorFv(reinterpret_cast<u8*>(self) - 0x4);
}
}

