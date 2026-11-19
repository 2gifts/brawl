#include <types.h>

// TODO: clean up, give variables real names
namespace jj_A9A8C {
struct fn27Obj {
    char pad[0x20];
    char* m_a;
    char* m_b;
    char* m_c;
    s32 m_count;
};

extern "C" {
void createTargetSearchUnit__18soTargetSearchUnitFP16soModuleAccesserP19UnkTargetSearchUnit(char* a, char* b, char* c);

void fn_27_A9A8C(fn27Obj* self) {
    s32 off20 = 0;
    s32 off18 = 0;
    s32 i = 0;
    for (; i < self->m_count; i++) {
        createTargetSearchUnit__18soTargetSearchUnitFP16soModuleAccesserP19UnkTargetSearchUnit(self->m_b + off18, self->m_a, self->m_c + off20);
        off20 += 0x20;
        off18 += 0x18;
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_A9B0C {
class SoraElem {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v6(s32 arg);
};

struct fn27A9B0CObj {
    char pad0[0x20];
    s32 m_arg;
    char* m_base;
    char pad28[4];
    s32 m_count;
};

extern "C" void fn_27_A9B0C(fn27A9B0CObj* self, s32 index) {
    u32 flag;

    if (index == -2) {
        flag = 0;
    } else if (index < 0) {
        flag = 1;
    } else {
        flag = self->m_count <= index;
    }

    if (flag != 1) {
        if (index == -2) {
            s32 offset;
            s32 i;
            i = 0;
            offset = 0;
            for (; i < self->m_count; ++i) {
                ((SoraElem*)(self->m_base + offset))->v6(self->m_arg);
                offset += 0x18;
            }
        } else {
            ((SoraElem*)(self->m_base + index * 0x18))->v6(self->m_arg);
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_A9BE4 {
extern "C" void resetIntervalCount__18soTargetSearchUnitFv(void*);

struct fn27A9BE4Obj {
    char pad0[0x24];
    char* m_base;
    char pad28[4];
    s32 m_count;
};

extern "C" void fn_27_A9BE4(fn27A9BE4Obj* self, s32 index) {
    u32 flag;

    if (index == -2) {
        flag = 0;
    } else if (index < 0) {
        flag = 1;
    } else {
        flag = self->m_count <= index;
    }

    if (flag != 1) {
        if (index == -2) {
            s32 offset;
            s32 i;
            i = 0;
            offset = 0;
            for (; i < self->m_count; ++i) {
                resetIntervalCount__18soTargetSearchUnitFv(self->m_base + offset);
                offset += 0x18;
            }
        } else {
            resetIntervalCount__18soTargetSearchUnitFv(self->m_base + index * 0x18);
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_A9CA0 {
class soWorkBase {
public:
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void vf5(s32 value);
    virtual void vf6(s32 value);
    virtual void vf7();
    virtual void vf8();
    virtual void vf9();
    virtual void vf10();
    virtual void vf11();
};

extern "C" void fn_27_A9CA0(soWorkBase* self) {
    self->vf5(-2);
    self->vf6(-2);
    self->vf11();
}
}

// TODO: clean up, give variables real names
namespace jj_A9D08 {
class soWaitObj {
public:
    virtual bool vf0();
    u8 m_pad0[0x8 - 0x4];
    u8 m_counterBuf[0x14 - 0x8];
    s32 m_0x14;
};

struct Obj {
    u8 pad0[0x20];
    u32 context;
    soWaitObj* base;
    u32 pad28;
    s32 count;
};

extern "C" bool isFoundTarget__18soTargetSearchUnitFP16soModuleAccesserUl(soWaitObj* self, u32 a, u32 b);

extern "C" bool fn_27_A9D08(Obj* obj, s32 index, u32 c) {
    u32 flag;

    if (index == -2) {
        flag = 0;
    } else if (index < 0) {
        flag = 1;
    } else {
        flag = obj->count <= index;
    }

    if (flag == 1)
        return false;

    if (index == -2)
        return false;

    return isFoundTarget__18soTargetSearchUnitFP16soModuleAccesserUl(obj->base + index, obj->context, c);
}
}

// TODO: clean up, give variables real names
namespace jj_A9D74 {
class soWaitObj {
public:
    virtual bool vf0();

    u8 m_pad0[0x8 - 0x4];
    u8 m_counterBuf[0x14 - 0x8];
    s32 m_0x14;
};

struct Object {
    u8 pad0[0x20];
    void* context;
    soWaitObj* nodes;
    u32 unk28;
    s32 count;
};

extern "C" bool isLostTarget__18soTargetSearchUnitFP16soModuleAccesserUl(soWaitObj*, u32, void*);

extern "C" u32 fn_27_A9D74(Object* self, s32 index, void* arg2) {
    u32 invalid;

    if (index == -2) {
        invalid = 0;
    } else if (index < 0) {
        invalid = 1;
    } else {
        invalid = self->count <= index;
    }

    if (invalid == 1) {
        return 0;
    }

    if (index == -2) {
        return 0;
    }

    return isLostTarget__18soTargetSearchUnitFP16soModuleAccesserUl(
        self->nodes + index,
        (u32)self->context,
        arg2
    );
}
}

// TODO: clean up, give variables real names
namespace jj_A9DE0 {
extern "C" void update__18soTargetSearchUnitFv(void* obj);

class fnFrameObj {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void setFrame(int frame);
    char pad1[0x1E - 4];
    unsigned char m_flag;
    char pad2[0x24 - 0x1F];
    char* m_base;
    char pad3[0x2C - 0x28];
    int m_count;
};

extern "C" void fn_27_A9DE0(fnFrameObj* self) {
    if (self->m_flag == 1) {
        self->setFrame(-2);
        self->m_flag = 0;
    }
    int off;
    int i;
    i = 0;
    off = 0;
    for (; i < self->m_count; i++) {
        update__18soTargetSearchUnitFv(self->m_base + off);
        off += 0x18;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_A9E6C {
extern "C" int fn_27_A9E6C(int unused, signed char c)
{
    return (signed char)c == 0x16;
}
}

// TODO: clean up, give variables real names
namespace jj_A9E80 {
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

struct Args {
    Args() {}

    Args(const Args& x) {
        for (int i = 0; i < 3; i++)
            data[i] = x.data[i];
    }

    virtual void a();
    virtual void b();
    virtual Value* at(int);

    int data[3];
};

inline Ref getArg(Args& a) {
    return Ref(a.at(0));
}

struct Input {
    virtual void a();
    virtual signed char kind();
    virtual signed char mode();
    virtual int count();
    virtual void b();
    virtual Args args();
};

struct Output {
    virtual void a();
    virtual void b();
    virtual void c();
    virtual void d();
    virtual void set(int);
    virtual void e();
    virtual void f();
    virtual void g();
    virtual void h();
    virtual void i();
    virtual void j();
    virtual void k();
    virtual void l();
    virtual void m();
    virtual void n();
    virtual void o();
    virtual void p();
    virtual void q();
    virtual void r();
    virtual void s();
    virtual void t();
    virtual int valid(signed char);
};

extern "C" int fn_27_A9E80(Output* self, Input* in) {
    if (!self->valid(in->kind()))
        return 0;

    signed char mode = in->mode();
    int count = in->count();

    if (mode <= -1 || mode >= 1)
        return 0;

    switch (mode) {
    case 0: {
        Args args = in->args();
        if (count == 0) {
            self->set(-2);
        } else {
            self->set(getValue(getArg(args)));
        }
        return 1;
    }
    }

    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_A9FFC {
extern "C" void fn_27_A9FFC(void) {}
}

// TODO: clean up, give variables real names
namespace jj_AA000 {
class fn27Inner {
public:
    virtual void vf0() = 0;
    virtual void vf1() = 0;
    virtual void vf2() = 0;
    virtual void vf3() = 0;
    virtual void vf4() = 0;
    virtual void vf5() = 0;
    virtual void vf6() = 0;
    virtual void vf7() = 0;
    virtual void vf8() = 0;
    virtual void vf9() = 0;
    virtual void vf10() = 0;
};

class fn27Outer {
public:
    u8 pad[0x30];
    fn27Inner* m_inner;
};

extern "C" void fn_27_AA000(fn27Outer* self) {
    self->m_inner->vf8();
}
}

// TODO: clean up, give variables real names
namespace jj_AA414 {
extern "C" void fn_27_A9FFC(void*);

extern "C" void fn_27_AA414(void* self) {
    fn_27_A9FFC(reinterpret_cast<u8*>(self) - 0x8);
}
}

// TODO: clean up, give variables real names
namespace jj_AA41C {
extern "C" void fn_27_A9E80(void*);

extern "C" void fn_27_AA41C(void* self) {
    fn_27_A9E80(reinterpret_cast<u8*>(self) - 0x14);
}
}

// TODO: clean up, give variables real names
namespace jj_AA424 {
extern "C" void fn_27_A9E6C(void*);

extern "C" void fn_27_AA424(void* self) {
    fn_27_A9E6C(reinterpret_cast<u8*>(self) - 0x14);
}
}

