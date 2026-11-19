// The sora_melee unit has its own out-of-line accessors (see the header).
#define SO_TERRITORY_MODULE_IMPL_OUT_OF_LINE
#include <so/territory/so_territory_module_impl.h>
#include <types.h>

void soTerritoryModuleImpl::activate() {
    initAreaInstance();
}

bool soTerritoryModuleImpl::isObserv(char unk1) {
    return unk1 == 0x15;
}

soHyp::soTerritoryRect* soTerritoryModuleImpl::getRect() {
    return &m_rect;
}

// TODO: clean up, give variables real names
namespace jj_A7E40 {
extern "C" {
void* getInstance__13soEventSystemFv();
void* getManager__13soEventSystemFl(void*, long);
void __dl__FPv(void*);
}

class soAnimCmdEventObserver;

template <class T>
struct soEventObserver {
    static void __vt(int, int);
};

struct EvSys {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f5();
    virtual u32 f6(s32 id);
};

struct EvMgr {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3(s16 a, s16 b);
};

struct Obj {
    char pad0[8];
    void* vt;
    s16 mC;
    s16 mE;
    s16 m10;
};

extern "C" void* __dt__21soTerritoryModuleImplFv(void* p, s32 flag) {
    if (p != nullptr) {
        Obj* o = (Obj*)p;
        if ((char*)p + 8 != nullptr && (char*)o + 8 != nullptr) {
            s16 id = o->mC;
            o->vt = (void*)soEventObserver<soAnimCmdEventObserver>::__vt;
            u32 r = ((EvSys*)getInstance__13soEventSystemFv())->f6(id);
            if (r == 1) {
                EvMgr* mgr = (EvMgr*)getManager__13soEventSystemFl(
                    getInstance__13soEventSystemFv(), o->mC);
                mgr->m3(o->mE, o->m10);
            }
            o->m10 = -1;
            o->mC = -1;
        }
        if (flag > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_A81B8 {
class Manager {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void process(int, int, int);
};

struct Inner {
    char pad[0x90];
    Manager* manager;
};

struct Holder {
    char pad[0xD8];
    Inner* inner;
};

struct Owner {
    char pad0[0x14];
    Holder* holder;
    char pad1[0x3C - 0x18];
    int a;
    int b;
    int c;
    int d;
    int e;
};

extern "C" void initAreaInstance__21soTerritoryModuleImplFv(Owner* self) {
    Manager* manager = self->holder->inner->manager;

    if (self->a != -1) manager->process(self->a, 0, -1);
    if (self->b != -1) manager->process(self->b, 0, -1);
    if (self->c != -1) manager->process(self->c, 0, -1);
    if (self->d != -1) manager->process(self->d, 0, -1);
    if (self->e != -1) manager->process(self->e, 0, -1);
}
}

// TODO: clean up, give variables real names
namespace jj_A82BC {
extern float lbl_27_rodata_D34;
extern float lbl_27_rodata_D30;

struct Vec2 {
    float x, y;
    Vec2() {}
    Vec2(float a, float b) : x(a), y(b) {}
};

struct Vec3 {
    float x, y, z;
};

class soNullable {
public:
    virtual void isNull();
    virtual void __dt__21soTerritoryModuleImplFv();
    virtual void createAreaDataSetAll__21soTerritoryModuleImplFv();
    virtual void initAreaInstance__21soTerritoryModuleImplFv();
    virtual void activate__21soTerritoryModuleImplFv();
    virtual void createTerritory__21soTerritoryModuleImplFi();
    virtual void setEnable__21soTerritoryModuleImplFi();
    virtual void isTouch__21soTerritoryModuleImplFi();
    virtual void isOutSide__21soTerritoryModuleImplFv();
    virtual void getRect__21soTerritoryModuleImplFv();
    virtual void setRect__21soTerritoryModuleImplFPQ25soHyp15soTerritoryRect();
};

class Obj0 {
public:
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual Vec3 s4();
};

class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12(int, int, int);
    virtual void slot13();
    virtual void slot14(int);
    virtual void slot15();
    virtual void slot16();
    virtual void slot17(int, Vec2*, Vec2*);
};

struct Inner {
    char pad[0xc];
    Obj0* obj;
    char pad2[0x80];
    Object_vptr_0* manager;
};

struct Access {
    char pad[0xd8];
    Inner* inner;
};

struct FlagData {
    u32 bits;
    u32 val;
};

struct Owner {
    char pad0[0x14];
    Access* access;
    float f18, f1c, f20, f24;
    FlagData* flag;
    Vec3 position;
    u8 done, active;
    char pad3a[2];
    int a, b, c, d, e;
};

inline u32 test(int n, u32 bits, u8 active) {
    return active == 0 ? 0 : ((1 << n) & bits) != 0;
}

extern "C" void createTerritory__21soTerritoryModuleImplFi(Owner* self, u32 arg) {
    Inner* inner = self->access->inner;
    float scale = lbl_27_rodata_D34;
    float x = self->f1c - self->f18;
    x *= scale;

    float range[4];
    range[0] = -x;
    range[1] = x;
    range[2] = self->f20 - self->f24;
    range[2] *= scale;
    range[3] = -range[2];

    Object_vptr_0* mgr = inner->manager;
    self->position = inner->obj->s4();

    int a = self->a;
    if (a != -1) {
        mgr->slot14(a);
        if (test(a, self->flag->bits, self->active) == 1)
            mgr->slot12(a, 1, -1);
        else
            mgr->slot12(a, 0, -1);

        if (test(self->a, self->flag->bits, self->active) == 1 && arg == 1) {
            Vec2 pos(lbl_27_rodata_D30, lbl_27_rodata_D30);
            Vec2 size(range[1] - range[0], range[2] - range[3]);
            mgr->slot17(self->a, &pos, &size);
        }
    }

    int b = self->b;
    if (b != -1) {
        mgr->slot14(b);
        if (test(b, self->flag->bits, self->active) == 1)
            mgr->slot12(b, 1, -1);
        else
            mgr->slot12(b, 0, -1);

        if (test(self->b, self->flag->bits, self->active) == 1 && arg == 1) {
            Vec2 pos;
            pos.y = range[2] + (float)(self->flag->val >> 1);
            pos.x = lbl_27_rodata_D30;
            Vec2 size((range[1] + (float)self->flag->val) -
                          (range[0] - (float)self->flag->val),
                      (float)self->flag->val);
            mgr->slot17(self->b, &pos, &size);
        }
    }

    int c = self->c;
    if (c != -1) {
        mgr->slot14(c);
        if (test(c, self->flag->bits, self->active) == 1)
            mgr->slot12(c, 1, -1);
        else
            mgr->slot12(c, 0, -1);

        if (test(self->c, self->flag->bits, self->active) == 1 && arg == 1) {
            Vec2 pos;
            pos.y = range[3] - (float)(self->flag->val >> 1);
            pos.x = lbl_27_rodata_D30;
            Vec2 size((range[1] + (float)self->flag->val) -
                          (range[0] - (float)self->flag->val),
                      (float)self->flag->val);
            mgr->slot17(self->c, &pos, &size);
        }
    }

    int d = self->d;
    if (d != -1) {
        mgr->slot14(d);
        if (test(d, self->flag->bits, self->active) == 1)
            mgr->slot12(d, 1, -1);
        else
            mgr->slot12(d, 0, -1);

        if (test(self->d, self->flag->bits, self->active) == 1 && arg == 1) {
            Vec2 pos;
            pos.x = range[1] + (float)(self->flag->val >> 1);
            pos.y = lbl_27_rodata_D30;
            Vec2 size((float)self->flag->val,
                      (range[2] + (float)self->flag->val) -
                          (range[3] - (float)self->flag->val));
            mgr->slot17(self->d, &pos, &size);
        }
    }

    int e = self->e;
    if (e != -1) {
        mgr->slot14(e);
        if (test(e, self->flag->bits, self->active) == 1)
            mgr->slot12(e, 1, -1);
        else
            mgr->slot12(e, 0, -1);

        if (test(self->e, self->flag->bits, self->active) == 1 && arg == 1) {
            Vec2 pos;
            pos.x = range[0] - (float)(self->flag->val >> 1);
            pos.y = lbl_27_rodata_D30;
            Vec2 size((float)self->flag->val,
                      (range[2] + (float)self->flag->val) -
                          (range[3] - (float)self->flag->val));
            mgr->slot17(self->e, &pos, &size);
        }
    }

    self->done = 1;
}
}

// TODO: clean up, give variables real names
namespace jj_A8AFC {
class soNullable { public: virtual void isNull(); virtual void __dt__21soTerritoryModuleImplFv(); virtual void createAreaDataSetAll__21soTerritoryModuleImplFv(); virtual void initAreaInstance__21soTerritoryModuleImplFv(); virtual void activate__21soTerritoryModuleImplFv(); virtual void createTerritory__21soTerritoryModuleImplFi(); virtual void setEnable__21soTerritoryModuleImplFi(); virtual void isTouch__21soTerritoryModuleImplFi(); virtual void isOutSide__21soTerritoryModuleImplFv(); virtual void getRect__21soTerritoryModuleImplFv(); virtual void setRect__21soTerritoryModuleImplFPQ25soHyp15soTerritoryRect(); };
class Object_vptr_0 { public: virtual void slot0(); virtual void slot1(); virtual void slot2(); virtual void slot3(); virtual void slot4(); virtual void slot5(); virtual void slot6(); virtual void slot7(); virtual void slot8(); virtual void slot9(); virtual void slot10(); virtual void slot11(); virtual void slot12(int,int,int); };
struct Inner { char pad[0x90]; Object_vptr_0* object; };
struct Access { char pad[0xd8]; Inner* inner; };
struct Owner { char pad[0x14]; Access* access; char pad2[0x10]; unsigned int* bits; char pad3[0xd]; unsigned char active; char pad4[2]; int a,b,c,d,e; };
inline unsigned int test(unsigned int bits, unsigned char active, int n) {
 return active == 0 ? 0 : ((1 << n) & bits) != 0;
}
extern "C" void setEnable__21soTerritoryModuleImplFi(Owner* self, int arg) {
 Object_vptr_0* obj = self->access->inner->object;
 int n = self->a;
 if (n != -1) { if (test(*self->bits,self->active,n) == 1) obj->slot12(n,arg,-1); }
 n = self->b;
 if (n != -1) { if (test(*self->bits,self->active,n) == 1) obj->slot12(n,arg,-1); }
 n = self->c;
 if (n != -1) { if (test(*self->bits,self->active,n) == 1) obj->slot12(n,arg,-1); }
 n = self->d;
 if (n != -1) { if (test(*self->bits,self->active,n) == 1) obj->slot12(n,arg,-1); }
 n = self->e;
 if (n != -1) { if (test(*self->bits,self->active,n) == 1) obj->slot12(n,arg,-1); }
}
}

// TODO: clean up, give variables real names
namespace jj_A8D3C {
struct Ids {
    int values[4];
};

struct Object {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual unsigned int disabled();
    virtual void v9();
    virtual void v10();
    virtual unsigned int test(int);

    char pad04[0x35];
    unsigned char enabled;
    char pad3A[6];
    Ids ids;

    bool selected(unsigned int mask, int id) {
        bool result;
        if (enabled == 0) {
            result = false;
        } else {
            result = ((1 << id) & mask) != 0;
        }
        return result;
    }
};

extern "C" bool isTouch__21soTerritoryModuleImplFi(Object* self, unsigned int mask) {
    Ids ids = self->ids;
    if (self->disabled() == 1U)
        return false;

    for (int i = 0; i < 4; ++i) {
        int id = ids.values[i];
        if (id != -1 && self->selected(mask, id) && self->test(id) == 1U)
            return true;
    }
    return false;
}
}

// TODO: clean up, give variables real names
namespace jj_A8E30 {
struct Object {
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual int query(int);

    char padding[0x38];
    int value;
};

extern "C" bool isOutSide__21soTerritoryModuleImplFv(Object* self) {
    int value = self->value;
    if (value == -1) {
        return true;
    }
    return self->query(value) != 1;
}
}

// TODO: clean up, give variables real names
namespace jj_A8E94 {
struct Value {
    int tag;
    int value;
};

struct Ref {
    Value* p;
    bool flag;

    Ref(Value* q) : p(q), flag(false) {}

    int get() const {
        return flag == 1 ? 0 : p->value;
    }
};

inline int getValue(Ref ref) {
    return ref.get();
}

struct Holder {
    virtual void v0();
    virtual void v1();
    virtual Value* get(u32);
    int data[3];
};

inline Ref getRef(Holder& h) {
    return Ref(h.get(0));
}

class Obj {
public:
    virtual void v0();
    virtual signed char kind();
    virtual signed char mode();
    virtual void v3();
    virtual void v4();
    virtual Holder fetch();
};

class Self {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v7(u8);
    virtual void v8(bool);
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
    virtual int valid(signed char);
    unsigned char pad[0x34];
    unsigned char m38;
};

extern "C" int notifyEventAnimCmd__21soTerritoryModuleImplFP9acAnimCmdP16soModuleAccesserl(Self* self, Obj* arg) {
    if (self->valid(arg->kind()) == 0) {
        return 0;
    }
    signed char mode = arg->mode();
    arg->v3();
    if (mode <= -1 || mode >= 2) {
        return 0;
    }
    switch (mode) {
    case 0:
        self->v7(self->m38);
        return 1;
    case 1: {
        Holder h = arg->fetch();
        self->v8(getValue(getRef(h)) != 0);
        return 1;
    }
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_A901C {
typedef float f32;

struct Vec4 {
    f32 x, y, z, w;
};

struct Dst {
    char pad[0x18];
    Vec4 v;
};

extern "C" void setRect__21soTerritoryModuleImplFPQ25soHyp15soTerritoryRect(Dst* dst, const Vec4* src) {
    dst->v = *src;
}
}

