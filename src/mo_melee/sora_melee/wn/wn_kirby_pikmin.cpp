#include <so/so_value_accesser.h>
#include <wn/pikmin/wn_kirby_pikmin.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_kinetic_energy_normal.h>

// Registers the Kirby's Pikmin article class (kind 150) in the class info table during static initialization.
wnClassInfoImpl<150, wnKirbyPikmin, wnKirbyPikminData> g_wnKirbyPikminClassInfo;

bool wnKirbyPikmin::notifyEventCollisionAttackCheck(u32 flags) {
    if ((flags & 7) != 0) {
        return false;
    }
    int reflected = 0;
    if ((flags & 8) != 0) {
        if (m_moduleAccesser->getReflectModule().isReflect() == 1) {
            reflected = 1;
        }
    }
    if (reflected != 0) {
        Vec2f accel(1.0f, 1.0f);
        float speedY = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA0, 0);
        Vec2f speed(-m_moduleAccesser->getReflectModule().getSpeedMul(), speedY);
        return reflectRequest(&speed, &accel);
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

void wnKirbyPikmin::shoot(Vec3f* position, float lr) {
    if (m_moduleAccesser->getLinkModule().isModelConstraint() == true) {
        m_moduleAccesser->getLinkModule().removeModelConstraint(true);
    }
    m_moduleAccesser->getStatusModule().changeStatusForce(1, m_moduleAccesser);
    m_moduleAccesser->getPostureModule().setPos(position);
    m_moduleAccesser->getPostureModule().setLr(lr);
    updateNodeSRT();
    wnKineticEnergyNormal* movement = dynamic_cast<wnKineticEnergyNormal*>(
        m_moduleAccesser->getKineticModule().getEnergy(0));
    if (movement) {
        const float* parameters = unk2044->unk70[m_moduleAccesser->getWorkManageModule().getInt(0x10000005)];
        movement->m_speed = Vec2f(parameters[2] * (m_moduleAccesser->getPostureModule().getLr() < 0.0f ? -1 : 1), parameters[4]);
        float speedLimitX = parameters[3];
        movement->m_accel = Vec2f(0.0f, -parameters[0]);
        movement->m_speedLimit = Vec2f(speedLimitX, parameters[1]);
        movement->enable();
    }
}

// TODO: clean up, give variables real names
namespace jj_30C42C {
extern "C" void* fn_27_30C42C(void* self) {
    return reinterpret_cast<u8*>(self) + 0x7f8;
}
}

// TODO: clean up, give variables real names
namespace jj_30CF94 {
extern "C" void* fn_27_30CF94(void* self) {
    return reinterpret_cast<u8*>(self) + 0xd8;
}
}

// TODO: clean up, give variables real names
namespace jj_30CF9C {
extern "C" void fn_27_30CF9C(void) {}
}

// TODO: clean up, give variables real names
namespace jj_30D244 {
typedef unsigned char u8;
extern "C" void __ct__22soVisibilityModuleImplFP16soModuleAccesserP16soVisibilityDatai(void* self, int a, int b, int c);

extern "C" void* fn_27_30D244(void* self, int a, int b) {
    __ct__22soVisibilityModuleImplFP16soModuleAccesserP16soVisibilityDatai(self, a, b, 1);
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_30EBD8 {
extern "C" {
extern char __RTTI__21wnKineticEnergyNormal[];
extern char __RTTI__15soKineticEnergy[];
extern const float lbl_27_rodata_65B0;
extern const float lbl_27_rodata_65B4;
void* __dynamic_cast(void*, long, const void*, const void*, short);
void notifyEventChangeStatus__6WeaponFiiP12soStatusDataP16soModuleAccesser(void*, int);
}

struct Vec2f {
    float x, y;
    Vec2f() {}
    Vec2f(float a, float b) : x(a), y(b) {}
};

static inline const float& asFloat(const unsigned long long& v) {
    return *(const float*)&v;
}

struct Kinetic {
    virtual void d0();
    virtual void d1();
    virtual void d2();
    virtual void d3();
    virtual void d4();
    virtual void d5();
    virtual void* get(int);
};

struct Work {
    virtual void d0();
    virtual void d1();
    virtual void d2();
    virtual void d3();
    virtual int getInt(unsigned long);
};

struct Modules {
    char pad0[0x64];
    Work* work;
    char pad1[0x7c - 0x68];
    Kinetic* kinetic;
};

struct Accesser {
    char pad[0xd8];
    Modules* modules;
};

struct Entry {
    char pad[0x14];
    float x;
    float y;
};

struct Table {
    char pad[0x70];
    Entry* entries;
};

struct Energy {
    virtual void d0();
    virtual void d1();
    virtual unsigned long long getRaw();
    virtual void d3();
    virtual void d4();
    virtual void d5();
    virtual void d6();
    virtual void d7();
    virtual void d8();
    virtual void d9();
    virtual void d10();
    virtual void d11();
    virtual void update();
    char pad4[4];
    Vec2f m8;
    char pad10[8];
    Vec2f m18;
    Vec2f m20;
    Vec2f m28;
};

struct Self {
    char pad[0x60];
    Accesser* accesser;
    char pad2[0x2044 - 0x64];
    Table* table;
};

extern "C" void fn_27_30EBD8(Self* self, int arg) {
    notifyEventChangeStatus__6WeaponFiiP12soStatusDataP16soModuleAccesser(self, arg);
    if (arg == 2) {
        Energy* e = (Energy*)__dynamic_cast(
            self->accesser->modules->kinetic->get(0), 0,
            __RTTI__21wnKineticEnergyNormal, __RTTI__15soKineticEnergy, 0);
        if (e != 0) {
            Entry* ent = &self->table->entries[self->accesser->modules->work->getInt(0x10000005)];
            e->m8 = Vec2f(asFloat(e->getRaw()), lbl_27_rodata_65B4);
            e->m18 = Vec2f(lbl_27_rodata_65B4, lbl_27_rodata_65B4);
            e->m20 = Vec2f(ent->x, lbl_27_rodata_65B0);
            e->m28 = Vec2f(ent->y, lbl_27_rodata_65B4);
            e->update();
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_30F788 {
extern "C" void fn_27_30C060(void*);

extern "C" void fn_27_30F788(void* self) {
    fn_27_30C060(reinterpret_cast<u8*>(self) - 0x40);
}
}

// TODO: clean up, give variables real names
namespace jj_30F790 {
extern "C" void fn_27_30C060(void*);

extern "C" void fn_27_30F790(void* self) {
    fn_27_30C060(reinterpret_cast<u8*>(self) - 0x90);
}
}

// TODO: clean up, give variables real names
namespace jj_30F798 {
extern "C" void fn_27_30C060(void*);

extern "C" void fn_27_30F798(void* self) {
    fn_27_30C060(reinterpret_cast<u8*>(self) - 0x88);
}
}

// TODO: clean up, give variables real names
namespace jj_30F7A0 {
extern "C" void __dt__13wnKirbyPikminFv(void*);

extern "C" void fn_27_30F7A0(void* self) {
    __dt__13wnKirbyPikminFv(reinterpret_cast<u8*>(self) - 0x40);
}
}

// TODO: clean up, give variables real names
namespace jj_30F7A8 {
extern "C" void fn_27_30EBD8(void*);

extern "C" void fn_27_30F7A8(void* self) {
    fn_27_30EBD8(reinterpret_cast<u8*>(self) - 0x64);
}
}

// TODO: clean up, give variables real names
namespace jj_30F7B0 {
extern "C" void notifyEventCollisionAttackCheck__13wnKirbyPikminFUl(void*);

extern "C" void fn_27_30F7B0(void* self) {
    notifyEventCollisionAttackCheck__13wnKirbyPikminFUl(reinterpret_cast<u8*>(self) - 0x70);
}
}

// TODO: clean up, give variables real names
namespace jj_30F7B8 {
extern "C" void __dt__13wnKirbyPikminFv(void*);

extern "C" void fn_27_30F7B8(void* self) {
    __dt__13wnKirbyPikminFv(reinterpret_cast<u8*>(self) - 0x90);
}
}

// TODO: clean up, give variables real names
namespace jj_30F7C0 {
extern "C" void fn_27_30F510(void*);

extern "C" void fn_27_30F7C0(void* self) {
    fn_27_30F510(reinterpret_cast<u8*>(self) - 0x88);
}
}

// TODO: clean up, give variables real names
namespace jj_30F7C8 {
extern "C" void __dt__13wnKirbyPikminFv(void*);

extern "C" void fn_27_30F7C8(void* self) {
    __dt__13wnKirbyPikminFv(reinterpret_cast<u8*>(self) - 0x88);
}
}

