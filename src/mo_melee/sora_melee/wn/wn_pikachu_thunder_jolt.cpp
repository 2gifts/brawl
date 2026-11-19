#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#include <wn/wn_module_accesser_builder.h>
#include <wn/pikachu/wn_pikachu_thunder_jolt.h>
#include <wn/wn_class_info_impl.h>

// Registers the Pikachu thunder jolt article (kind 9) in the class info table during static initialization.
wnClassInfoImpl<9, wnPikachuThunderJolt, wnPikachuThunderJoltData> g_wnPikachuThunderJoltClassInfo;

#include <so/stageobject.h>
#include <so/kinetic/so_kinetic_energy.h>
// TODO: clean up, give variables real names
namespace jj_324C2C {
extern "C" void* fn_27_324C2C(void* self) {
    return reinterpret_cast<u8*>(self) + 0x7f8;
}
}

// TODO: clean up, give variables real names
namespace jj_325794 {
extern "C" void* fn_27_325794(void* self) {
    return reinterpret_cast<u8*>(self) + 0x30;
}
}

// TODO: clean up, give variables real names
namespace jj_32579C {
typedef unsigned char u8;
typedef int s32;

struct Base {
    void* m_v0;
    u8 m_flag;
};

struct fnSub : Base {
    virtual void f0(); virtual void f1(); virtual void f2(); virtual void f3();
    virtual void f4(); virtual void f5(); virtual void f6(); virtual void f7();
    virtual void f8(); virtual void f9(); virtual void f10(); virtual void f11();
    virtual void f12(); virtual void f13(); virtual void f14(); virtual void f15();
    virtual void f16(); virtual void f17(); virtual void f18(); virtual void f19();
    virtual void f20(); virtual void f21(); virtual void f22(); virtual void f23();
    virtual void f24(); virtual void f25();
    void* m_self;
    s32 m_6;
    void* m_p18;
    s32 m_one;
    void* m_p1c;
    s32 m_one2;
};

struct fnOuter {
    u8 pad[0x20];
    fnSub sub;
};

extern "C" {
extern char __vt__19soGeneralWorkSimple[];

void* fn_27_32579C(fnOuter* p) {
    fnSub* s = &p->sub;
    s->m_flag = 0;
    s->m_self = p;
    *(void**)((char*)s + 8) = __vt__19soGeneralWorkSimple;
    s->m_v0 = (char*)__vt__19soGeneralWorkSimple + 8;
    s->m_6 = 6;
    s->m_p18 = (char*)p + 0x18;
    s->m_one = 1;
    s->m_p1c = (char*)p + 0x1c;
    s->m_one2 = 1;
    s->f25();
    return p;
}
}
}

// TODO: clean up, give variables real names
namespace jj_3259F0 {
extern "C" void* fn_27_3259F0(void* self) {
    return reinterpret_cast<u8*>(self) + 0x20;
}
}

// TODO: clean up, give variables real names
namespace jj_325C8C {
struct Flags {
    unsigned char first : 1, second : 1, third : 1, fourth : 1;
    unsigned char fifth : 1, sixth : 1, seventh : 1, eighth : 1;
    unsigned char ninth : 1, tenth : 1, reserved : 6;
};
struct Init {
    int resource, team, teamOwner, teamAnother, index, owner, reserved0, reserved1;
    Vec3f position;
    float facing;
    int parameter, variant, situation, status, reserved2, motion, reserved3;
    Flags flags;
};
struct LaunchParam { int first; float angle; float power; };
extern "C" {
extern char __RTTI__21wnKineticEnergyNormal[], __RTTI__15soKineticEnergy[];
void activate__6WeaponFP14wnActivateDesc(void*, const Init*);
void* __dynamic_cast(void*, long, const void*, const void*, long);
double sin(double);
double cos(double);
}
static inline const LaunchParam* getLaunchParam(const StageObject* object) {
    return *reinterpret_cast<const LaunchParam* const*>(reinterpret_cast<const char*>(object) + 0x1F90);
}
static inline void finish(StageObject* object, int workValue, float facing) {
    soModuleAccesser* acc = object->m_moduleAccesser;
    soWorkManageModule& work = acc->getWorkManageModule();
    work.setInt(getLaunchParam(object)->first, 0x10000003);
    work.setInt(getLaunchParam(object)->first, 0x10000004);
    soKineticEnergy* kinetic = acc->getKineticModule().getEnergy(0);
    char* energy = static_cast<char*>(__dynamic_cast(kinetic, 0, __RTTI__21wnKineticEnergyNormal, __RTTI__15soKineticEnergy, 1));
    float angle = getLaunchParam(object)->angle;
    float power = getLaunchParam(object)->power;
    float vy = power * (float)sin(angle);
    float vx = facing * (power * (float)cos(angle));
    Vec2f velocity(vx, vy);
    *reinterpret_cast<Vec2f*>(energy + 0x8) = velocity;
    acc->getStatusModule().changeStatusForce(0, acc);
    acc->getWorkManageModule().setInt(workValue, 0x10000005);
}
extern "C" void activate__20wnPikachuThunderJoltFiUlP5Vec3fiif(StageObject* object, int resource, int team, const Vec3f* position,
    int parameter, int workValue, float facing) {
    Init init;
    init.resource = resource;
    init.team = team;
    init.teamOwner = team;
    init.teamAnother = team;
    init.index = -1;
    init.owner = -1;
    init.reserved0 = 0;
    init.reserved1 = 0;
    __memcpy(&init.position, position, sizeof(Vec3f));
    init.facing = facing;
    init.parameter = parameter;
    init.variant = 0;
    init.situation = 2;
    init.status = 0x80;
    init.reserved2 = 0;
    init.motion = 0x35f;
    init.reserved3 = 0;
    init.flags.first = 1;
    init.flags.second = 0;
    init.flags.third = 0;
    init.flags.fourth = 0;
    init.flags.fifth = 1;
    init.flags.sixth = 0;
    init.flags.seventh = 0;
    init.flags.eighth = 0;
    init.flags.ninth = 0;
    init.flags.tenth = 0;
    activate__6WeaponFP14wnActivateDesc(object, &init);
    finish(object, workValue, facing);
}
}

// TODO: clean up, give variables real names
namespace jj_326130 {
extern "C" int notifyEventAnimCmd__6WeaponFP9acAnimCmdP16soModuleAccesseri(void* self);

class soNullable {
public:
    virtual bool isNull();
    virtual signed char slot1();
    virtual signed char slot2();
    virtual void slot3();
    virtual int slot4(u32 flag);
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void slot21();
    virtual void slot22();
    virtual void slot23();
    virtual void slot24();
    virtual void slot25();
    virtual void slot26();
    virtual void slot27();
    virtual void slot28();
    virtual void slot29();
    virtual void slot30();
    virtual void slot31();
    virtual void slot32();
    virtual void slot33();
    virtual void slot34();
    virtual void slot35();
    virtual void slot36();
    virtual void slot37();
    virtual void slot38();
    virtual void slot39();
    virtual void slot40();
    virtual void slot41();
    virtual void slot42();
    virtual void slot43();
    virtual void slot44();
    virtual void slot45();
    virtual void slot46();
    virtual void slot47();
    virtual void slot48();
    virtual void slot49();
    virtual void slot50();
    virtual void slot51();
    virtual void slot52();
    virtual void slot53();
    virtual void slot54();
    virtual void slot55();
    virtual void slot56();
    virtual void slot57();
    virtual void slot58();
    virtual void slot59();
    virtual void slot60();
    virtual void slot61();
    virtual void slot62();
    virtual void slot63();
    virtual void slot64();
    virtual void slot65();
    virtual void slot66();
    virtual void slot67();
    virtual void slot68();
    virtual void slot69();
    virtual void slot70();
    virtual void slot71();
    virtual void slot72();
    virtual void slot73();
    virtual void slot74();
    virtual void slot75();
    virtual void slot76();
    virtual void slot77();
    virtual void slot78();
    virtual void slot79();
    virtual void slot80();
    virtual void slot81();
    virtual void slot82();
    virtual void slot83();
    virtual void slot84();
    virtual void slot85();
    virtual void slot86();
    virtual void slot87();
    virtual void slot88();
    virtual void slot89();
    virtual void slot90();
    virtual void slot91();
    virtual void slot92();
    virtual void slot93();
    virtual void slot94();
    virtual void slot95();
    virtual void slot96();
    virtual void slot97();
    virtual void slot98();
    virtual void slot99();
    virtual void slot100();
    virtual void slot101();
    virtual void slot102();
    virtual void slot103();
    virtual void slot104();
    virtual void slot105();
    virtual void slot106(int a, int b, void* c, void* d);
};

class FtAccesser {
public:
    char pad0[0x54];
    soNullable* m_0x54;
    char pad1[0xc];
    soNullable* m_0x64;
};

class FtFighter {
public:
    char pad[0xd8];
    FtAccesser* m_0xd8;
};

struct ObjBase {
    char pad[0x3c];
};

class Obj : public ObjBase {
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
    virtual void vf27();
    virtual void vf28();
    virtual void vf29();
    virtual void vf30();
    virtual void vf31();
    virtual void vf32();
    virtual void vf33();
    virtual void vf34();
    virtual void vf35();
    virtual void vf36();
    virtual void vf37();
    virtual void vf38();
    virtual void vf39();
    virtual void vf40();
    virtual void vf41();
    virtual void vf42();
    virtual void vf43();
    virtual void vf44();
    virtual void vf45();
    virtual void vf46();
    virtual int vf47(signed char node);

    char pad40[0x20];
    FtFighter* m_fighter;
};

extern "C" int notifyEventAnimCmd__20wnPikachuThunderJoltFP9acAnimCmdP16soModuleAccesseri(Obj* self, soNullable* arg) {
    if (notifyEventAnimCmd__6WeaponFP9acAnimCmdP16soModuleAccesseri(self) != 0) {
        return 1;
    }
    signed char kind = arg->slot2();
    signed char node = arg->slot1();
    if (self->vf47(node) == 0) {
        return 0;
    }
    if (kind <= -1 || kind >= 0xb) {
        return 0;
    }
    switch (kind) {
    case 6: {
        FtFighter* fighter = self->m_fighter;
        int v = fighter->m_0xd8->m_0x64->slot4(0x10000005);
        if (v != -1) {
            FtAccesser* acc = fighter->m_0xd8;
            acc->m_0x54->slot106(3, v, fighter, acc);
        }
        return 1;
    }
    }
    return 0;
}
}

