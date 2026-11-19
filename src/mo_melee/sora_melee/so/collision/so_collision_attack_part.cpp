#include <so/collision/so_collision_attack_part.h>
#include <cl/cl_collision.h>

void soCollisionAttackPart::reset() {
    m_status = Status_Inactive;
}

void soCollisionAttackPart::resetStatus() {
    m_status = Status_Set;
}

void soCollisionAttackPart::setTargetCategory(soCollision::CategoryMask categoryMask) {
    m_attackData.m_targetCategory = categoryMask;
    m_clTargetArrayVector.at(1).m_4 = categoryMask;
}

void soCollisionAttackPart::set(newCollision* collision) {
    if (m_status == Status_Inactive) {
        m_status = Status_Set;
    }
    {
        clTarget& target = m_clTargetArrayVector.at(0);
        target.m_4 = 0x22;
    }
    if (m_attackData.m_setOffKind >> 1) {
        m_clTargetArrayVector.at(0).m_4 |= 1;
    }
    if (m_attackData.m_isShieldable) {
        m_clTargetArrayVector.at(0).m_4 |= 4;
    }
    {
        clTarget& target = m_clTargetArrayVector.at(5);
        target.m_0 = 1;
    }
    if (m_attackData.m_isReflectable) {
        m_clTargetArrayVector.at(0).m_4 |= 8;
        m_clTargetArrayVector.at(5).m_0 |= 2;
    }
    if (m_attackData.m_isAbsorbable) {
        m_clTargetArrayVector.at(0).m_4 |= 0x10;
        m_clTargetArrayVector.at(5).m_0 |= 4;
    }
    {
        clTarget& target = m_clTargetArrayVector.at(1);
        target.m_4 = m_attackData.m_targetCategory;
    }
    {
        clTarget& target = m_clTargetArrayVector.at(2);
        target.m_4 = m_attackData.m_targetSituation;
    }
    {
        clTarget& target = m_clTargetArrayVector.at(3);
        target.m_4 = m_attackData.m_targetPart;
    }
    {
        clTarget& target = m_clTargetArrayVector.at(4);
        target.m_4 = 3;
    }
    if (m_attackData.m_isInvalidXlu) {
        m_clTargetArrayVector.at(4).m_4 |= 4;
    }
    collision->m_targets = &m_clTargetArrayVector;
    collision->m_isInfinite = false;
}

clTarget::clTarget() { m_4 = -1; m_0 = -1; }
clTarget::~clTarget() { }

template class soArrayVector<clTarget, 7>;
template class soArrayVectorAbstract<clTarget>;

soCollisionAttackPart::~soCollisionAttackPart() { }


soCollisionAttackPart::soCollisionAttackPart(soCollision::Category category, bool isTeam) : m_status(Status_Inactive), m_attackData(), m_clTargetArrayVector(isTeam == true ? 7 : 6, 0) {
    clTarget& t0 = m_clTargetArrayVector.at(0);
    t0.m_0 = 1;
    clTarget& t1 = m_clTargetArrayVector.at(1);
    t1.m_0 = 1 << category;
    clTarget& t2 = m_clTargetArrayVector.at(2);
    t2.m_0 = 0;
    m_attackData.m_size = -1.0f;
    if (isTeam == true) {
        m_clTargetArrayVector.at(6).m_4 = 1;
    }
}

// TODO: clean up, give variables real names
namespace jj_38C08 {
typedef unsigned int u32;
struct State {
    int kind;
    u32 unk4;
    float data[3];
    float scale;
    unsigned char pad18[0x1C];
    u32 flags34;
    u32 flags38;
    u32 unk3c;
    u32 flags40;
};
extern "C" float lbl_27_rodata_490;
extern "C" void fn_27_55E8C(void*, void*, bool, u32, u32, void*, float, float);
inline float choose(State* self, float value) {
    if (((self->flags38 >> 22) & 1) == 0) return value;
    return lbl_27_rodata_490;
}
extern "C" void fn_27_38C08(State* self, void* a, void* b, float value) {
    if (self->kind != 0) {
    value = choose(self, value);
    fn_27_55E8C(a, b, self->kind == 1, self->flags40 >> 31,
                  self->flags34 >> 23, self->data, self->scale, value);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_38E28 {
class soVirtBase {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void vf5();
    virtual void vf6();
    virtual void vf7();
    virtual void vf8();
    virtual void vf9();
    virtual void vf10();
    virtual void vf11();
    virtual void vf12();
    virtual void vf13();
    virtual void vf14();
    virtual void vf15();
    virtual void vf16();
    virtual void vf17();
    virtual void vf18();
    virtual void vf19();
    virtual void vf20();
    virtual void vf21();
    virtual void vf22();
    virtual void vf23();
    virtual void vf24();
    virtual void vf25();
    virtual void vf26();
};

extern "C" void fn_27_38E28(soVirtBase* p) {
    p->vf24();
}
}

// TODO: clean up, give variables real names
namespace jj_38E38 {
class soVirtBase {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void vf5();
    virtual void vf6();
    virtual void vf7();
    virtual void vf8();
    virtual void vf9();
    virtual void vf10();
    virtual void vf11();
    virtual void vf12();
    virtual void vf13();
    virtual void vf14();
    virtual void vf15();
    virtual void vf16();
    virtual void vf17();
    virtual void vf18();
    virtual void vf19();
    virtual void vf20();
    virtual void vf21();
    virtual void vf22();
    virtual void vf23();
    virtual void vf24();
    virtual void vf25();
    virtual void vf26();
};

extern "C" {
void fn_27_38E38(soVirtBase* p) {
    p->vf24();
}
}
}

// TODO: clean up, give variables real names
namespace jj_38F44 {
extern "C" void shift__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfacebll(
    void*, int, int, int);

class Obj {
public:
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual int f3();
    virtual int f4();
    virtual void f5();
    virtual void f6();
    virtual void f7();
    virtual void f8();
    virtual void f9();
    virtual void f10();
    virtual void f11();
    virtual void f12();
    virtual int f13();
    virtual void f14();
    virtual void f15();
    virtual void f16();
    virtual void f17();
    virtual void f18();
    virtual void f19();
    virtual void f20();
    virtual void f21();
    virtual void f22();
    virtual void f23();
    virtual void f24();
    virtual void f25();
    virtual void f26();
    virtual int f27();
    virtual void f28();
    virtual int f29(int);
};

extern "C" void fn_27_38F44(Obj* self) {
    shift__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfacebll(
        (char*)self + 4, self->f4(), self->f13(), self->f27());
    self->f29(self->f3() - 1);
}
}

// TODO: clean up, give variables real names
namespace jj_390FC {
extern "C" void pop__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfacebll(void*, s32, s32, s32);

class Obj {
public:
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual s32 f3();
    virtual s32 f4();
    virtual void f5();
    virtual void f6();
    virtual void f7();
    virtual void f8();
    virtual void f9();
    virtual void f10();
    virtual void f11();
    virtual void f12();
    virtual s32 f13();
    virtual void f14();
    virtual void f15();
    virtual void f16();
    virtual void f17();
    virtual void f18();
    virtual void f19();
    virtual void f20();
    virtual void f21();
    virtual void f22();
    virtual void f23();
    virtual void f24();
    virtual void f25();
    virtual void f26();
    virtual void f27();
    virtual s32 f28();
    virtual void f29(s32);
};

extern "C" void fn_27_390FC(Obj* self) {
    s32 a, b;
    b = self->f28();
    a = self->f13();
    s32 c = self->f4();
    pop__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfacebll((void*)((u8*)self + 4), c, a, b);
    s32 t4 = self->f3();
    self->f29(t4 - 1);
}
}

// TODO: clean up, give variables real names
namespace jj_392D8 {
extern "C" void erase__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfacelllll(
    void*, int, int, int, int, int);

class Array {
public:
    virtual void v08();
    virtual void v0C();
    virtual void v10();
    virtual int size();
    virtual void v18();
    virtual void v1C();
    virtual void v20();
    virtual void v24();
    virtual void v28();
    virtual void v2C();
    virtual void v30();
    virtual void v34();
    virtual void v38();
    virtual int v3C();
    virtual bool v40();
    virtual void v44();
    virtual void v48();
    virtual void v4C();
    virtual void v50();
    virtual void v54();
    virtual void v58();
    virtual void v5C();
    virtual void v60();
    virtual void v64();
    virtual void v68();
    virtual void v6C();
    virtual void v70(int);
    virtual int v74();
    virtual int v78();
    virtual void v7C(int);

    int calc;
};

extern "C" void fn_27_392D8(Array* self, int index) {
    erase__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfacelllll(
        &self->calc, index, self->size(), self->v3C(), self->v74(), self->v78());
    self->v7C(self->size() - 1);
}
}

// TODO: clean up, give variables real names
namespace jj_3945C {
extern "C" void clear__23soArrayVectorCalculatorFR26soArrayVectorCalcInterface(void* calc);

struct Base {
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
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v29(int);
    virtual void v30();
    virtual void v31();
};

extern "C" void fn_27_3945C(Base* self) {
    clear__23soArrayVectorCalculatorFR26soArrayVectorCalcInterface((char*)self + 4);
    self->v29(0);
}
}

// TODO: clean up, give variables real names
namespace jj_39524 {
class soGeneralWorkAbstractStub {
public:
    virtual void vfunc0();
    virtual void vfunc1();
    virtual void vfunc2();
    virtual bool vfunc3();
};

extern "C" {
bool fn_27_39524(soGeneralWorkAbstractStub* self) {
    return !self->vfunc3();
}
}
}

