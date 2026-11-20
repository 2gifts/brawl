#include <types.h>
#include <wn/wn_value_accesser.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

wnValueAccesser::wnValueAccesser() : soValueAccesser(2) {}

int wnValueAccesser::getVariableIntCore(soModuleAccesser* moduleAccesser, u32 value, u32 extra) {
    if ((s32)value <= 20999) {
        return soValueAccesser::getVariableIntCore(moduleAccesser, value, extra);
    }
    return 0;
}

float wnValueAccesser::getConstantFloatCore(soModuleAccesser* moduleAccesser, u32 value, u32 extra) {
    if ((s32)value <= 2999) {
        return soValueAccesser::getConstantFloatCore(moduleAccesser, value, extra);
    }
    return 0.0f;
}

int wnValueAccesser::getConstantIntCore(soModuleAccesser* moduleAccesser, u32 value, u32 extra) {
    if ((s32)value <= 22999) {
        return soValueAccesser::getConstantIntCore(moduleAccesser, value, extra);
    }
    return 0;
}

// TODO: clean up, give variables real names
namespace jj_1DC918 {
extern "C" void __dt__15soValueAccesserFv(void* p, short p2);
extern "C" void __dl__FPv(void* p);

extern "C" void* __dt__15wnValueAccesserFv(void* p, short flag) {
    if (p != 0) {
        __dt__15soValueAccesserFv(p, 0);
        if (flag > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_1DC970 {
struct soInner {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f5();
    virtual void f6();
    virtual void f7();
    virtual void f8();
    virtual void f9();
    virtual void f10();
    virtual void f11();
    virtual void f12();
    virtual void f13();
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
};

struct soMid {
    char pad[0x64];
    soInner* m_inner;
};

struct soOuter {
    char pad[0xd8];
    soMid* m_mid;
};

extern "C" void fn_27_1DC970(void* unused, soOuter* obj) {
    return obj->m_mid->m_inner->f23();
}
}

// TODO: clean up, give variables real names
namespace jj_1DCB10 {
extern "C" void getConstantIndefiniteCore__15soValueAccesserFP16soModuleAccesserUlUl(void);

extern "C" void fn_27_1DCB10(void) {
    getConstantIndefiniteCore__15soValueAccesserFP16soModuleAccesserUlUl();
}
}

// HYPOTHESIS: local name of the weapon value accesser instance (as g_ykValueAccesser).
wnValueAccesser g_wnValueAccesser;
