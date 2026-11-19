#include <math.h>
#include <so/so_module_accesser.h>
#include <wn/pikachu/wn_pikachu_thunder_jolt2.h>
#include <wn/wn_class_info_impl.h>

// Registers the Pikachu thunder jolt 2 article class (kind 10) in the class info table during static initialization.
wnClassInfoImpl<10, wnPikachuThunderJolt2, wnPikachuThunderJolt2Data> g_wnPikachuThunderJolt2ClassInfo;

void wnPikachuThunderJolt2::setPosition(Vec3f* position, const Vec2f* direction, bool resetFrame, float lr) {
    soModuleAccesser* accesser = m_moduleAccesser;
    accesser->getPostureModule().setPos(position);
    if (resetFrame) {
        accesser->getMotionModule().setFrame(0.0f);
    }
    if (lr == 0.0f) {
        lr = accesser->getPostureModule().getLr();
    }
    float angle = atan2f(direction->m_x, direction->m_y);
    Vec3f rotation(57.29578f * (lr * angle), 0.0f, 0.0f);
    accesser->getPostureModule().setRot(&rotation, 0);
}

#include <new>
#include <so/so_array.h>
#include <so/collision/so_collision_attack_part.h>
#include <so/collision/so_collision_attack_module_impl.h>
// TODO: clean up, give variables real names
namespace jj_326D30 {
extern "C" {
void __dl__FPv(void* p);
}

extern "C" void* __dt__45wnPikachuThunderJolt2StatusUniqProcessRegularFv(void* p, s32 n) {
    if (p != nullptr && n > 0) {
        __dl__FPv(p);
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_326D70 {
extern "C" void* fn_27_326D70(void* self) {
    return reinterpret_cast<u8*>(self) + 0x7f8;
}
}

// TODO: clean up, give variables real names
namespace jj_3278D8 {
extern "C" void* fn_27_3278D8(void* self) {
    return reinterpret_cast<u8*>(self) + 0x1b4;
}
}

// TODO: clean up, give variables real names
namespace jj_3278E0 {

#pragma use_simple_stdpnew on

struct Bits {
    unsigned int a : 4;
    unsigned int b : 2;
    unsigned int c : 1;
    unsigned int rest : 25;
};

class AttackArrayView {
public:
    virtual void k0();
    virtual void k1();
    virtual void k2();
    virtual void k3();
    virtual int k4();
    virtual void k5();
    virtual void k6();
    virtual void k7();
    virtual void k8();
    virtual void k9();
    virtual void k10(soCollisionAttackPart*);
    virtual void k11();
    virtual void k12();
    virtual int k13();
    virtual int k14();
    virtual void k15();
    virtual void k16();
    virtual void k17();
    virtual void k18();
    virtual void k19();
    virtual void k20();
    virtual void k21();
    virtual void k22();
    virtual void k23();
    virtual void k24();
    virtual void k25();
    virtual void k26();
    virtual int k27();
    virtual int k28();
};

extern "C" {
extern unsigned char lbl_27_data_3B5B0[];
extern unsigned char lbl_27_data_46F8[];
extern unsigned char lbl_27_data_125F0[];
extern const float lbl_27_rodata_75B0;

void __construct_array(void*, void (*)(void*), void (*)(void*), unsigned long, int);
void __ct__21soCollisionAttackPartFv(void*);
void __dt__21soCollisionAttackPartFv(void*, int);
void __ct__16soCollisionGroupFv(void*);
void __dt__16soCollisionGroupFv(void*, int);
int resize__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfacelbblll(
    void*, void*, int, int, int, int, int);
void postInitialize__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfacell(
    void*, void*, int);
}

class AbsoluteArrayStorage {
    void* table;
public:
    AbsoluteArrayStorage() : table(lbl_27_data_125F0) {}
};

struct PaddedData {
    char prefix[36];
    float slipChance;
    float hitStopFrame;
    float hitStopDelay;
    char padding[56];

    PaddedData() {
        slipChance = lbl_27_rodata_75B0;
        hitStopFrame = lbl_27_rodata_75B0;
        hitStopDelay = lbl_27_rodata_75B0;
    }
};

extern "C" AttackArrayView* fn_27_3278E0(
    AttackArrayView* self, soModuleAccesser* accesser, int taskId,
    gfTask::Category taskCategory, soEventObserverRegistrationDesc* observerDesc) {
    unsigned char* bytes = reinterpret_cast<unsigned char*>(self);
    unsigned int* words = reinterpret_cast<unsigned int*>(self);

    {
        soCollisionAttackPart part(static_cast<soCollision::Category>(0), 0);

        words[0] = reinterpret_cast<unsigned int>(lbl_27_data_3B5B0);
        words[1] = reinterpret_cast<unsigned int>(lbl_27_data_3B5B0 + 0x48);
        words[2] &= 0x3fffff;

        __construct_array(
            bytes + 0xC, __ct__21soCollisionAttackPartFv,
            reinterpret_cast<void (*)(void*)>(__dt__21soCollisionAttackPartFv),
            0x90, 2);

        int d, c, b, a;
        a = self->k28();
        b = self->k27();
        c = self->k13();
        d = self->k14();

        int count =
            resize__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfacelbblll(
                bytes + 4, reinterpret_cast<void*>(2), self->k4(), d, c, b, a);

        for (int i = 0; i < count; ++i) {
            self->k10(&part);
        }
    }

    words[0x12C / 4] = reinterpret_cast<unsigned int>(lbl_27_data_46F8);
    words[0x130 / 4] = reinterpret_cast<unsigned int>(lbl_27_data_46F8 + 0x48);

    Bits* flags = reinterpret_cast<Bits*>(bytes + 0x134);
    flags->a = 0;
    flags->b = 1;
    flags->c = 0;

    __construct_array(
        bytes + 0x138, __ct__16soCollisionGroupFv,
        reinterpret_cast<void (*)(void*)>(__dt__16soCollisionGroupFv), 0x78, 1);
    postInitialize__23soArrayVectorCalculatorFR26soArrayVectorCalcInterfacell(
        bytes + 0x130, reinterpret_cast<void*>(1), 1);

    PaddedData x;
    soArray<soCollisionAttackAbsolute>* absolutes =
        reinterpret_cast<soArray<soCollisionAttackAbsolute>*>(
            new (bytes + 0x1B0) AbsoluteArrayStorage);

    new (bytes + 0x1B4) soCollisionAttackModuleImpl(
        accesser, taskId, static_cast<gfTask::Category>(taskCategory),
        reinterpret_cast<soArray<soCollisionAttackPart>*>(self),
        reinterpret_cast<soArray<soCollisionGroup>*>(bytes + 0x12C),
        absolutes, observerDesc, true);

    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_327E0C {
struct Vec3 { int x,y,z; };
struct Flags {
 unsigned char first:1, second:1, third:1, fourth:1, fifth:1, sixth:1, seventh:1, eighth:1;
 unsigned char ninth:1, tenth:1, reserved:6;
};
struct Param {
 int resource, team, teamOwner, teamAnother, index, owner;
 int reserved0, reserved1;
 Vec3 position;
 float facing;
 int parameter, variant, situation, status, reserved2, motion, reserved3;
 Flags flags;
};
struct Access;
class Object_vptr_0 {
public:
 virtual void slot0();
 virtual void slot1();
 virtual void slot2();
 virtual void slot3();
 virtual void slot4();
 virtual void slot5(int, int);
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
 virtual void slot34(int, Access*);
};
struct Modules { char pad[0x64]; Object_vptr_0* X; char pad2[8]; Object_vptr_0* Y; };
struct Access { char pad[0xd8]; Modules* modules; };
struct Object { char pad[0x60]; Access* access; char pad2[0x1f54]; int* P; };
extern "C" {
void activate__6WeaponFP14wnActivateDesc(void*, const Param*);
void fn_27_327E0C(Object* object, int resource, int team, Vec3 position, int parameter, float facing) {
 Param p;
 p.resource=resource;
 p.team=team;
 p.teamOwner=team;
 p.teamAnother=team;
 p.index=-1;
 p.owner=-1;
 p.reserved0=0;
 p.reserved1=0;
 p.position=position;
 p.facing=facing;
 p.parameter=parameter;
 p.variant=0;
 p.reserved2=0;
 p.motion=0x35f;
 p.reserved3=0;
 p.flags.first=1;
 p.flags.second=0;
 p.flags.third=0;
 p.flags.fourth=0;
 p.flags.fifth=1;
 p.flags.sixth=0;
 p.flags.seventh=0;
 p.flags.eighth=0;
 p.flags.ninth=0;
 p.flags.tenth=0;
 p.situation=3;
 p.status=0xff;
 activate__6WeaponFP14wnActivateDesc(object,&p);
 Access* access=object->access;
 Object_vptr_0* module=access->modules->X;
 module->slot5(*object->P,0x10000003);
 module->slot5(*object->P,0x10000004);
 access->modules->Y->slot34(0,access);
}
}
}

