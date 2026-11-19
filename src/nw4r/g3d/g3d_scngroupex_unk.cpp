#include <types.h>

// Function bodies below are taken from https://github.com/jj-link/brawl.

// TODO: clean up, give variables real names
extern "C" unsigned int fn_801B4400(void) {
    return 0x53474558;
}

// TODO: clean up, give variables real names
extern "C" void fn_801B4A38(void) {}

extern "C" void fn_801B4A3C(void) {}

extern "C" void fn_801B4A40(void) {}

// TODO: clean up, give variables real names
extern "C" void fn_801B4A44(void) {}

extern "C" void fn_801B4A48(void) {}

extern "C" void fn_801B4A4C(void) {}

extern "C" s32 fn_801B3CBC(void) {
    return 0x0;
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" u32 fn_801B3FB4(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x18);
}

// TODO: clean up, give variables real names
namespace jj_801B4AE8 {
extern "C" {

extern unsigned char lbl_8040ABF8[];

// TODO: clean up, give variables real names
unsigned char* fn_801B4AE8(void) {
    return lbl_8040ABF8;
}

}
}

namespace jj_801B3DF4 {
extern "C" {

struct Obj {
    int x0;
    int x4;
    int x8;
    int xC;
    int x10;
    int x14;
    int x18;
    int x1C;
};

void fn_801B3FBC(void*, int, int, int, int);

// TODO: clean up, give variables real names
void fn_801B3DF4(struct Obj* p, int a) {
    if (p->x1C == 0) return;
    fn_801B3FBC(p, p->x1C, p->x10, p->x4, a);
}

}
}

namespace jj_801B3E18 {
extern "C" {

struct Obj {
    int pad0[2];
    int x8;
    int padC[2];
    int x14;
    int pad18[2];
    int x20;
};

extern void fn_801B3FBC(Obj*, int, int, int, int);

// TODO: clean up, give variables real names
void fn_801B3E18(Obj* obj, int arg) {
    int v = obj->x20;
    if (v == 0) {
        return;
    }
    fn_801B3FBC(obj, v, obj->x14, obj->x8, arg);
}

}
}

namespace jj_801B4AB8 {
class Obj {
public:
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual char* f3() const;
};

// TODO: clean up, give variables real names
extern "C" char* fn_801B4AB8(const Obj* self) {
    return self->f3() + 4;
}
}

namespace jj_801B4564 {
struct Vt {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f5();
    virtual void f6();
    virtual void f7();
    virtual void f8();
    virtual void f9(u32* p);
};

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_801B4564(u8* self, u32* arg1) {
    u32 tmp = *arg1;
    Vt* o = *(Vt**)(self + 0xe8);
    o->f9(&tmp);
}
}

namespace jj_801B4918 {
extern "C" {
void fn_801AC1AC(void* a, int b);

// TODO: clean up, give variables real names
void* fn_801B4918(void* arg0) {
    if (arg0 != 0) {
        fn_801AC1AC(arg0, 0);
    }
    return arg0;
}
}
}

namespace jj_801B48D8 {
extern "C" {
    void __dl__FPv(void*);

    void* fn_801B48D8(void* slot, int destroy) {
        if (slot != 0 && destroy > 0) {
            __dl__FPv(slot);
        }
        return slot;
    }
}
}

namespace jj_801B49F8 {
extern "C" {
extern void __dl__FPv(void*);

// TODO: clean up, give variables real names
void* fn_801B49F8(void* arg0, int arg1) {
    if (arg0 != 0 && arg1 > 0) {
        __dl__FPv(arg0);
    }
    return arg0;
}
}
}

namespace jj_801B4A50 {
#pragma optimization_level 1

extern "C" {
extern unsigned char lbl_8040ABF8[0x10];
extern unsigned char lbl_8040ABA0[0x10];
extern unsigned char lbl_8040AB80[0x10];
extern unsigned char lbl_8040AA68[0x10];

// TODO: clean up, give variables real names
int fn_801B4A50(void* unused, unsigned int* p) {
    if (*p == (unsigned int)lbl_8040ABF8) return 1;
    unsigned int v = *p;
    if (v == (unsigned int)lbl_8040ABA0) return 1;
    if (v == (unsigned int)lbl_8040AB80) return 1;
    return (unsigned int)__cntlzw((unsigned int)lbl_8040AA68 - v) >> 5;
}
}
}

// TODO: clean up, give variables real names
namespace jj_801B4374 {
class Obj {
public:
    virtual void v0() = 0;
    virtual void v1() = 0;
    virtual void v2() = 0;
    virtual void v3() = 0;
    virtual void v4() = 0;
    virtual void v5() = 0;
    virtual void v6() = 0;
    virtual void v7() = 0;
    virtual void v8() = 0;
    virtual void v9() = 0;
    virtual void v10() = 0;
    virtual u32 get() = 0;
};

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" s32 fn_801B4374(Obj* arg0, Obj* arg1) {
    u32 t = arg1->get();
    u32 s = arg0->get();
    if (s == t) {
        if (*(u32*)((char*)arg1 + 0x18) == *(u32*)((char*)arg0 + 0x18)) {
            return 1;
        }
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_801B3CC4 {
extern unsigned char lbl_8040ABF8[];
class Class_804664C0 {
public:
    virtual int fn_801B4A50(void*);
    virtual void fn_801B45A0();
    virtual void fn_801B4918();
    virtual void fn_801B4AE8();
    virtual void fn_801B4AB8();
    virtual void fn_801AB788();
    virtual void fn_801AAC9C();
    virtual void fn_801AAE34();
    virtual void fn_801AAFDC();
    virtual void fn_801AAFE8();
    virtual void fn_801AAA70();
    virtual void fn_801ABE70();
    virtual void fn_801ABFFC();
    char pad[0xe4];
    int value;
};
class Class_80466418 {
public:
    virtual void fn_801B3BDC();
    virtual int fn_801B35D0(Class_804664C0*, int, int);
    virtual void fn_801B3C80();
    virtual void fn_801B3C74();
    virtual void fn_801B3C44();
    virtual void fn_801AB788();
    virtual void fn_801AAC9C();
    virtual void fn_801AAE34();
    virtual int fn_801AAFDC();
    virtual void fn_801AAFE8();
    virtual void fn_801AAA70();
    virtual void fn_801ABE70();
    virtual int fn_801ABFFC(int);
};
struct Self {
    void* vtable;
    Class_80466418* first;
    Class_80466418* second;
};
// TODO: clean up, give variables real names
extern "C" int fn_801B3CC4(Self* self, Class_804664C0* arg, int a, int b) {
    if (!self->second->fn_801AAFDC())
        return self->second->fn_801B35D0(arg, a, b);
    Class_80466418* inner = self->second;
    bool matches = false;
    if (arg) {
        unsigned char* type = lbl_8040ABF8;
        if (arg->fn_801B4A50(&type))
            matches = true;
    }
    Class_804664C0* cast = matches ? arg : 0;
    bool found = false;
    if (cast && inner->fn_801ABFFC(cast->value))
        found = true;
    if (found)
        return 0;
    return self->first->fn_801B35D0(arg, a, b);
}
}

