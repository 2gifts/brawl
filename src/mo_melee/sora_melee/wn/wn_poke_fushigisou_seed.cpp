#include <wn/poke/wn_poke_fushigisou_seed.h>
#include <wn/wn_class_info_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_value_accesser.h>
#include <mt/mt_prng.h>

// Registers the Pokemon seed article class (kind 74) in the class info table during static initialization.
wnClassInfoImpl<74, wnPokeFushigisouSeed, wnPokeFushigisouSeedData> g_wnPokeFushigisouSeedClassInfo;

void wnPokeFushigisouSeed::onDeactivate() { }

// A hit on a target bursts the seed (status 2).
bool wnPokeFushigisouSeed::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        m_moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

void wnPokeFushigisouSeed::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    switch (statusKind) {
    case 2: {
        float threshold = soValueAccesser::getConstantFloat(moduleAccesser, 0xFA0, 0);
        if (randf() >= threshold) {
            Vec3f position = moduleAccesser->getPostureModule().getPos();
            if (!moduleAccesser->getWorkManageModule().isFlag(0x12000003)) {
                moduleAccesser->getEffectModule().req(ef_ptc_poke_fushigisou_tane_mghit, &position, &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
            } else {
                moduleAccesser->getEffectModule().req(ef_ptc_kb_poke_fushigisou_tane_mghit, &position, &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
            }
        }
        break;
    }
    case 1: {
        Vec3f position = moduleAccesser->getPostureModule().getPos();
        if (!moduleAccesser->getWorkManageModule().isFlag(0x12000003)) {
            moduleAccesser->getEffectModule().req(ef_ptc_poke_fushigisou_tane_mghit, &position, &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
        } else {
            moduleAccesser->getEffectModule().req(ef_ptc_kb_poke_fushigisou_tane_mghit, &position, &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
        }
        break;
    }
    }
}

// TODO: clean up, give variables real names
namespace jj_36EE34 {
extern "C" void __dl__FPv(void*);

extern "C" void* __dt__33wnPokeFushigisouSeedParamAccesserFv(void* ptr, int size) {
    if (ptr != 0 && size > 0)
        __dl__FPv(ptr);
    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_36EE74 {
extern "C" void* fn_27_36EE74(void* self) {
    return reinterpret_cast<u8*>(self) + 0x428;
}
}

// TODO: clean up, give variables real names
namespace jj_3700F0 {
extern "C" void* fn_27_3700F0(void* self) {
    return reinterpret_cast<u8*>(self) + 0x1c;
}
}

// TODO: clean up, give variables real names
namespace jj_3700F8 {
extern "C" void* fn_27_3700F8(void* self) {
    return reinterpret_cast<u8*>(self) + 0x80;
}
}

// TODO: clean up, give variables real names
namespace jj_370100 {
extern "C" void* fn_27_370100(void* self) {
    return reinterpret_cast<u8*>(self) + 0xb8;
}
}

// TODO: clean up, give variables real names
namespace jj_37057C {
typedef unsigned int u32;

struct Work;
typedef void (*WorkMethod)(Work*);

extern "C" WorkMethod __vt__19soGeneralWorkSimple[];

struct VirtualBase {
    u32 prefix[2];
};

struct VirtualWork : VirtualBase {
    virtual void method0();
    virtual void method1();
    virtual void method2();
    virtual void method3();
    virtual void method4();
    virtual void method5();
    virtual void method6();
    virtual void method7();
    virtual void method8();
    virtual void method9();
    virtual void method10();
    virtual void method11();
    virtual void method12();
    virtual void method13();
    virtual void method14();
    virtual void method15();
    virtual void method16();
    virtual void method17();
    virtual void method18();
    virtual void method19();
    virtual void method20();
    virtual void method21();
    virtual void method22();
    virtual void method23();
    virtual void method24();
    virtual void reset();
    virtual void method25();
};

struct Work {
    WorkMethod* secondary;
    bool isNull;
    WorkMethod* vtable;
    void* owner;
    u32 intCount;
    void* ints;
    u32 floatCount;
    void* floats;
    u32 flagCount;
};

struct Object {
    unsigned char data[0x44];
    Work work;
};

extern "C" Object* fn_27_37057C(Object* self) {
    Work* work = &self->work;
    work->isNull = false;
    work->owner = self;
    work->vtable = __vt__19soGeneralWorkSimple;
    work->secondary = __vt__19soGeneralWorkSimple + 2;
    work->intCount = 10;
    work->ints = self->data + 0x28;
    work->floatCount = 6;
    work->floats = self->data + 0x40;
    work->flagCount = 1;
    reinterpret_cast<VirtualWork*>(work)->reset();
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_370608 {
extern "C" void* fn_27_370608(void* self) {
    return reinterpret_cast<u8*>(self) + 0x44;
}
}

