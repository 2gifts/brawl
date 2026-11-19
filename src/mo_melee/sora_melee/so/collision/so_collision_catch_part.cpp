#include <so/collision/so_collision_shield_part.h>
#include <so/collision/templates/so_collision_catch_part.h>

void soCollisionCatchPart::reset() {
    unk0 = 0;
}

void soCollisionCatchPart::set(const soCollisionCatchData* data, soCollisionShieldPartUnit* unit) {
    m_data.m_offset0 = data->m_offset0;
    m_data.m_offset1 = data->m_offset1;
    m_data.m_offset2 = data->m_offset2;
    m_data.m_size = data->m_size;
    m_data.unk10 = data->unk10;
    m_data.unk14 = data->unk14;
    unk0 = 1;
    clTarget& target0 = m_targets.at(0);
    target0.m_4 = 2;
    clTarget& target1 = m_targets.at(1);
    target1.m_4 = (m_data.unk10 >> 4) & 0x3FF;
    clTarget& target2 = m_targets.at(2);
    target2.m_4 = (m_data.unk10 >> 1) & 7;
    clTarget& target3 = m_targets.at(3);
    target3.m_4 = m_data.unk14_0;
    clTarget& target4 = m_targets.at(4);
    target4.m_4 = 1;
    unit->m_targetArray = &m_targets;
}

// TODO: clean up, give variables real names
namespace jj_4AC40 {
extern "C" {
extern char lbl_27_data_3AD8[];
void __ct__8clTargetFv(void*);
void __dt__8clTargetFv(void*, int);
void __construct_array(void*, void (*)(void*), void (*)(void*), unsigned long, int);
void postInitialize__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfacell(void*, long, long);
}

class Holder {
public:
    virtual void v0();
    virtual u32* v1(int);
};

union Flags {
    u32 raw;
    struct {
        u32 pad : 8;
        u32 value : 4;
        u32 flag : 1;
        u32 rest : 19;
    } bits;
};

struct Obj {
    void* unk0;
    char pad4[0x18];
    void* vt;
    void* calc;
    Flags flags;
    char targets[0x30];
};

extern "C" void* __ct__20soCollisionCatchPartFQ211soCollision8Category(Obj* self, int arg1) {
    self->unk0 = 0;
    self->calc = lbl_27_data_3AD8 + 0x48;
    self->vt = lbl_27_data_3AD8;
    self->flags.raw &= 0x00ffffff;
    self->flags.bits.value = 6;
    self->flags.bits.flag = 0;
    __construct_array(self->targets, (void (*)(void*))__ct__8clTargetFv,
                      (void (*)(void*))__dt__8clTargetFv, 8, 6);
    postInitialize__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfacell(&self->calc, 6, 6);
    u32* p = ((Holder*)&self->vt)->v1(0);
    *p = 0x40;
    u32* q = ((Holder*)&self->vt)->v1(1);
    *q = 1 << arg1;
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_4AD20 {
extern "C" void __destroy_arr(void*, void (*)(void*, int), unsigned long, unsigned long);
extern "C" void __dl__FPv(void*);
extern "C" void __dt__8clTargetFv(void*, int);

extern "C" void* __dt__20soCollisionCatchPartFv(char* self, int flag) {
    if (self) {
        if (self + 0x1c) {
            __destroy_arr(self + 0x28, __dt__8clTargetFv, 8, 6);
        }
        if (flag > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_4B088 {
typedef unsigned int u32;
typedef unsigned char u8;

struct Fn27Obj {
    u32 count;
    u8 pad[0xC];
    float f10;
    u32 w14;
    struct {
        u32 a : 4;
        u32 b : 1;
        u32 c : 1;
        u32 d : 26;
    } w18;
};

extern "C" {
extern const float lbl_27_rodata_4F8;
void fn_27_55E8C(u32 a, u32 b, u32 c, u32 d, u32 e, Fn27Obj* f, float g, float h);

void fn_27_4B088(Fn27Obj* p, u32 a, u32 b, float f)
{
    if (p->count == 0) {
        return;
    }
    fn_27_55E8C(a, b, p->count - 1 == 0, p->w18.c, p->w14 >> 23, (Fn27Obj*)((u8*)p + 4), p->f10, (p->w18.b == 0) ? f : lbl_27_rodata_4F8);
}
}
}

// TODO: clean up, give variables real names
namespace jj_4B0E8 {
struct Color {
    unsigned char r, g, b, a;
};

extern "C" {
extern const Color lbl_27_rodata_4FC;
extern const Color lbl_27_rodata_500;
}

struct TargetBase {
    unsigned char padding[0x30];
};

struct Target : TargetBase {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f5();
    virtual void f6();
    virtual void setColors(int, const Color*, const Color*);
};

extern "C" void fn_27_4B0E8(int* self, int arg, Target* target)
{
    if (*self != 1) {
        Color first = lbl_27_rodata_4FC;
        Color second = lbl_27_rodata_500;
        target->setColors(arg, &first, &second);
    }
}
}

