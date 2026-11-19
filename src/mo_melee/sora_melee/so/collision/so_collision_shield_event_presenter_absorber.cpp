#include <so/collision/so_collision_shield_event_presenter_absorber.h>

// TODO: clean up, give variables real names
namespace jj_4839C {
extern "C" {
extern char lbl_27_data_4088[];
extern char lbl_27_data_42A0[];
extern char lbl_27_data_4228[];
extern char lbl_27_data_4230[];
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

extern "C" void* __ct__39soCollisionShieldEventPresenterAbsorberFP16soModuleAccesser(Self* self, void* arg1) {
    s32 result = 0;
    Mgr* mgr2;
    s16 temp;
    s32 eid;
    char* ctx;
    char* obj;

    self->f0 = lbl_27_data_4088;
    ctx = *(char**)((char*)arg1 + 0xD8);
    obj = *(char**)(ctx + 0x80);
    eid = ((I9*)(obj + 8))->get(arg1);
    self->f4 = lbl_27_data_42A0;
    self->f8 = eid;
    self->fA = 16;
    self->fC = result;

    if (eid > 0 && eid > -1) {
        if (((Sys*)getInstance__13soEventSystemFv())->check(eid) != 0) {
            Mgr* mgr = (Mgr*)getManager__13soEventSystemFl(
                getInstance__13soEventSystemFv(), self->f8);
            if (mgr->check(self->fA) == 0) {
                result = 0;
                self->fC = result;
            } else {
                temp = self->fA;
                mgr2 = (Mgr*)getManager__13soEventSystemFl(
                    getInstance__13soEventSystemFv(), self->f8);
                if (mgr2->check(temp) == 0) {
                    result = 0;
                } else {
                    Unit* u = (Unit*)__dynamic_cast(
                        mgr2->s10(temp), 0, lbl_27_data_4228, __RTTI__11soEventUnit, 0);
                    if (u == 0) {
                        result = 0;
                    } else {
                        result = u->get();
                    }
                }
                self->fC = result;
            }
        }
    }

    self->f0 = lbl_27_data_4230;
    self->f4 = lbl_27_data_4230 + 0x18;
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_48528 {
extern "C" void __dl__FPv(void*);

extern "C" void* fn_27_48528(void* ptr, int count) {
    if (ptr && count > 0)
        __dl__FPv(ptr);
    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_48568 {
extern "C" void __dl__FPv(void*);

extern "C" void* __dt__39soCollisionShieldEventPresenterAbsorberFv(void* ptr, s32 size) {
    void* result = ptr;
    if (ptr != 0) {
        if (size > 0) {
            __dl__FPv(ptr);
        }
    }
    return result;
}
}

// TODO: clean up, give variables real names
namespace jj_487F4 {
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
    virtual unsigned int o2();
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
extern char lbl_27_bss_C20[];
extern char lbl_27_data_4228[];
extern char __RTTI__11soEventUnit[];
void* __dynamic_cast(void*, long, const void*, const void*, short);
Instance* getInstance__13soEventSystemFv();
Manager* getManager__13soEventSystemFl(Instance*, long);

int fn_27_487F4(SoEventSystem* self) {
    Manager* mgr;
    int result;
    Container* list;
    short id1;
    short id2;
    Container* cur;
    EventUnit* unit;
    int flag;
    int i;

    id1 = self->id1;
    if (id1 <= -1) {
        list = (Container*)lbl_27_bss_C20;
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
        unit = (EventUnit*)__dynamic_cast(mgr->m8(id2), 0, lbl_27_data_4228, __RTTI__11soEventUnit, 0);
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
        list = (Container*)lbl_27_bss_C20;
        goto LOOP;
    }
C:
    mgr = getManager__13soEventSystemFl(getInstance__13soEventSystemFv(), self->id1);
    if (mgr->m4(self->id2) == 0) {
        self->cached = 0;
        list = (Container*)lbl_27_bss_C20;
    } else {
        list = self->cached;
    }
LOOP:
    result = 0;
    for (i = 0; i < list->c4(); i++) {
        Obj** p = list->c2(i);
        if ((*p)->o2() == 1) {
            result = 1;
        }
    }
    return result;
}
}
}

// TODO: clean up, give variables real names
namespace jj_48A0C {
extern "C" s32 notifyEventCollisionAbsorberCheck__32soCollisionAbsorberEventObserverFv(void) {
    return 0x0;
}
}

// TODO: clean up, give variables real names
namespace jj_48A14 {
extern "C" void __dt__39soCollisionShieldEventPresenterAbsorberFv(void*);

extern "C" void fn_27_48A14(void* self) {
    __dt__39soCollisionShieldEventPresenterAbsorberFv(reinterpret_cast<u8*>(self) - 0x4);
}
}

