#include <so/reflect/so_reflect_module.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/poke/wn_poke_lizardon_breath.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <mt/mt_prng.h>
#include <math.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

bool wnPokeLizardonBreath::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

bool wnPokeLizardonBreath::reflect() {
    int life = m_moduleAccesser->getWorkManageModule().getInt(0x10000005);
    if (life > 0) {
        float multiplier = m_moduleAccesser->getReflectModule().getLifeMul();
        life = (int)(life * multiplier);
        m_moduleAccesser->getWorkManageModule().setInt(life, 0x10000005);
    }
    return Weapon::reflect();
}

void wnPokeLizardonBreath::activate(int founderTaskId, int team, const Vec3f& position, int variation,
                                    float lr, float speed, float scale, float speedMul, float angle) {
    wnActivateDesc desc(founderTaskId, 0xFFFF, 0xFFFF, 0xFFFF, -1, -1, 0, 0, position,
                        lr, team, unk1648->unk4, 5, 0x80, 0, 0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getWorkManageModule().setInt(variation, 0x10000006);
    m_moduleAccesser->getWorkManageModule().setInt(unk1648->unk8, 0x10000005);
    m_moduleAccesser->getWorkManageModule().setFloat(scale, 0x11000001);
    m_moduleAccesser->getGroundModule().setOffsetY(unk1648->unk0, 0);
    m_moduleAccesser->getPostureModule().setScale(scale);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    float randomSpeed = unk1648->unkC + (unk1648->unk10 - unk1648->unkC) * randf();
    speedMul *= speed * randomSpeed;
    energy.m_speed = Vec2f(lr * (speedMul * (float)cos(angle)), speedMul * (float)sin(angle));
    updateNodeSRT();
}

#include <nw4r/math/math_arithmetic.h>
#include <mt/mt_common.h>
// TODO: clean up, give variables real names
namespace jj_369AF0 {
class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual int slot2();
    virtual void slot3();
    virtual int slot4();
    virtual int slot5();
};
class Class_18ED8 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3(void*, int, short*, int);
    virtual void slot4();
    virtual void slot5();
};
struct Arg2 { int pad; Object_vptr_0* heap; };
struct Build { int data[18]; };
struct FlagByte { unsigned char flag : 1; unsigned char rest : 7; };

extern "C" {
extern char g_soParamAccesserNull[];
extern char __vt__24soKineticEnergyRotNormal[], lbl_27_data_92200[], lbl_27_data_9221C[], lbl_27_data_92488[];
extern float lbl_27_rodata_7B40, lbl_27_rodata_7B44;
void __ct__25wnModuleAccesserBuildDataFlPCvQ25Heaps8HeapTypeQ25Heaps8HeapTypeQ25Heaps8HeapTypeUclP15soParamAccesserlllllll(void*, int, const void*, int, int, int, unsigned char, int, void*, int, int, int, int, int, int, int);
void __ct__6WeaponFPCclPCvlPCvP16soModuleAccesseri(void*, void*, int, int, int, void*, void*, int);
void fn_27_36A664(void*, void*, void*);
void fn_27_36BA88(void*, void*, void*);
void __ct__21wnKineticEnergyNormalFv(void*);
void initialize__6WeaponFv(void*);
void deactivate__6WeaponFb(void*, int);
}

extern "C" void* __ct__20wnPokeLizardonBreathFlRC18wnArticleBuildInfoPC24wnPokeLizardonBreathData(char* self, int arg1, Arg2* arg2, const int* arg3) {
    Build build;
    float tbl[15];
    int t22;
    int t23;
    Object_vptr_0* r24 = *(Object_vptr_0**)((char*)arg2 + 4);
    t23 = r24->slot5();
    t22 = r24->slot4();
    int t6 = r24->slot2();
    unsigned char id = *arg3 & 0xff;
    __ct__25wnModuleAccesserBuildDataFlPCvQ25Heaps8HeapTypeQ25Heaps8HeapTypeQ25Heaps8HeapTypeUclP15soParamAccesserlllllll(&build, 13, arg3, t6, t22, t23, id, -1,
                 g_soParamAccesserNull, 0, 0, 0, 0, 0, 0, 0);
    __ct__6WeaponFPCclPCvlPCvP16soModuleAccesseri(self, lbl_27_data_92200, build.data[1], build.data[2], arg1, arg2, self + 0x538, 1);
    char* sub = self + 0xcc;
    *(char**)(self+0x3c)=lbl_27_data_92488;
    *(char**)(self+0x40)=lbl_27_data_92488+0x64;
    *(char**)(self+0x48)=lbl_27_data_92488+0x70;
    *(char**)(self+0x54)=lbl_27_data_92488+0x84;
    *(char**)(self+0x64)=lbl_27_data_92488+0xdc;
    *(char**)(self+0x70)=lbl_27_data_92488+0xec;
    *(char**)(self+0x7c)=lbl_27_data_92488+0x100;
    *(char**)(self+0x88)=lbl_27_data_92488+0x118;
    *(char**)(self+0x90)=lbl_27_data_92488+0x124;
    fn_27_36A664(sub, self, &build);
    fn_27_36BA88(sub+0x11dc, sub+0x46c, &build);
    __ct__21wnKineticEnergyNormalFv(self+0x15cc);

    tbl[12] = lbl_27_rodata_7B40;
    tbl[9] = lbl_27_rodata_7B44;
    tbl[13] = lbl_27_rodata_7B40;
    tbl[14] = lbl_27_rodata_7B40;
    tbl[10] = lbl_27_rodata_7B44;
    tbl[11] = lbl_27_rodata_7B44;
    tbl[6] = lbl_27_rodata_7B40;
    tbl[7] = lbl_27_rodata_7B40;
    tbl[8] = lbl_27_rodata_7B40;
    tbl[3] = lbl_27_rodata_7B44;
    tbl[4] = lbl_27_rodata_7B44;
    tbl[5] = lbl_27_rodata_7B44;
    tbl[0] = lbl_27_rodata_7B44;
    tbl[1] = lbl_27_rodata_7B44;
    tbl[2] = lbl_27_rodata_7B44;
    int* ti = (int*)tbl;

    self[0x1608]=0;
    ((FlagByte*)(self+0x1609))->flag = 1;
    self[0x160a]=0;
    *(char**)(self+0x1604)=__vt__24soKineticEnergyRotNormal;
    *(int*)(self+0x160c) = ti[0];
    *(int*)(self+0x1610) = ti[1];
    *(int*)(self+0x1614) = ti[2];
    *(int*)(self+0x1618) = ti[3];
    *(int*)(self+0x161c) = ti[4];
    *(int*)(self+0x1620) = ti[5];
    *(int*)(self+0x1624) = ti[6];
    *(int*)(self+0x1628) = ti[7];
    *(int*)(self+0x162c) = ti[8];
    *(int*)(self+0x1630) = ti[9];
    *(int*)(self+0x1634) = ti[10];
    *(int*)(self+0x1638) = ti[11];
    *(int*)(self+0x163c) = ti[12];
    *(int*)(self+0x1640) = ti[13];
    *(int*)(self+0x1644) = ti[14];
    initialize__6WeaponFv(self);

    struct ShortSlot { short n; short pad; };
    ShortSlot m;
    ShortSlot n;
    n.n=1;
    ((Class_18ED8*)*(void**)(*(char**)(*(char**)(self+0x60)+0xd8)+0x7c))->slot3(self+0x15cc,0,&n.n,-1);
    m.n=1;
    ((Class_18ED8*)*(void**)(*(char**)(*(char**)(self+0x60)+0xd8)+0x7c))->slot3(self+0x1604,1,&m.n,-1);

    *(char**)(self+0x3c)=lbl_27_data_9221C;
    *(char**)(self+0x40)=lbl_27_data_9221C+0x64;
    *(char**)(self+0x48)=lbl_27_data_9221C+0x70;
    *(char**)(self+0x54)=lbl_27_data_9221C+0x84;
    *(char**)(self+0x64)=lbl_27_data_9221C+0xdc;
    *(char**)(self+0x70)=lbl_27_data_9221C+0xec;
    *(char**)(self+0x7c)=lbl_27_data_9221C+0x100;
    *(char**)(self+0x88)=lbl_27_data_9221C+0x118;
    *(char**)(self+0x90)=lbl_27_data_9221C+0x124;
    *(int*)(self+0x1648)=arg3[13];
    deactivate__6WeaponFb(self,0);
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_36A65C {
extern "C" void* fn_27_36A65C(void* self) {
    return reinterpret_cast<u8*>(self) + 0x46c;
}
}

// TODO: clean up, give variables real names
namespace jj_36B3F0 {
extern "C" void* fn_27_36B3F0(void* self) {
    return reinterpret_cast<u8*>(self) + 0x1c;
}
}

// TODO: clean up, give variables real names
namespace jj_36B3F8 {
extern "C" void* fn_27_36B3F8(void* self) {
    return reinterpret_cast<u8*>(self) + 0x28;
}
}

// TODO: clean up, give variables real names
namespace jj_36B400 {
extern "C" void* fn_27_36B400(void* self) {
    return reinterpret_cast<u8*>(self) + 0xb8;
}
}

// TODO: clean up, give variables real names
namespace jj_36B408 {
extern "C" void* fn_27_36B408(void* self) {
    return reinterpret_cast<u8*>(self) + 0x14;
}
}

// TODO: clean up, give variables real names
namespace jj_36B7AC {
typedef unsigned long u32;
typedef void (*WorkMethod)(void*);

extern "C" WorkMethod __vt__19soGeneralWorkSimple[];

struct WorkBase {
    WorkMethod* vptr0;
    bool nullable;
};

struct WorkInterface : WorkBase {
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
    virtual void clearWorkAll();
};

struct Work : WorkInterface {
    void* data1;
    int count1;
    void* data2;
    int count2;
    void* data3;
    int count3;
};

inline void initialize(Work* w, void* p2, void* p3) {
    u32* fields = (u32*)w;
    fields[2] = (u32)__vt__19soGeneralWorkSimple;
    fields[0] = (u32)(__vt__19soGeneralWorkSimple + 2);
    fields[4] = 11;
    fields[5] = (u32)p2;
    fields[6] = 7;
    fields[7] = (u32)p3;
    fields[8] = 1;
    w->clearWorkAll();
}

extern "C" void* fn_27_36B7AC(char* p) {
    Work* w = (Work*)(p + 0x4c);
    w->nullable = false;
    w->data1 = p;
    initialize(w, p + 0x2c, p + 0x48);
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_36BA80 {
extern "C" void* fn_27_36BA80(void* self) {
    return reinterpret_cast<u8*>(self) + 0x4c;
}
}

// TODO: clean up, give variables real names
namespace jj_36D2CC {
#pragma fp_contract off
struct Vec2 { float x; float y; };
extern "C" {
int notifyEventAnimCmd__6WeaponFP9acAnimCmdP16soModuleAccesseri(void*);
void* __dynamic_cast(void*, long, const void*, const void*, short);
double atan2(double, double);
double cos(double);
double sin(double);
float angle__5Vec2fCFPC5Vec2f(const Vec2* lhs, const Vec2* rhs);
extern const float lbl_27_rodata_7B40[];
extern char __RTTI__21wnKineticEnergyNormal[];
extern char __RTTI__15soKineticEnergy[];
}
inline Vec2 unpack(unsigned long long bits) { return *(Vec2*)&bits; }
struct gfTask_base { char pad[0x3c]; };
class gfTask : public gfTask_base {
public:
    virtual void slot0(); virtual void slot1(); virtual void slot2(); virtual void slot3();
    virtual void slot4(); virtual void slot5(); virtual void slot6(); virtual void slot7();
    virtual void slot8(); virtual void slot9(); virtual void slot10(); virtual void slot11();
    virtual void slot12(); virtual void slot13(); virtual void slot14(); virtual void slot15();
    virtual void slot16(); virtual void slot17(); virtual void slot18(); virtual void slot19();
    virtual void slot20(); virtual void slot21(); virtual void slot22(); virtual void slot23();
    virtual void slot24(); virtual void slot25(); virtual void slot26(); virtual void slot27();
    virtual void slot28(); virtual void slot29(); virtual void slot30(); virtual void slot31();
    virtual void slot32(); virtual void slot33(); virtual void slot34(); virtual void slot35();
    virtual void slot36(); virtual void slot37(); virtual void slot38(); virtual void slot39();
    virtual void slot40(); virtual void slot41(); virtual void slot42(); virtual void slot43();
    virtual void slot44(); virtual void slot45(); virtual void slot46();
    virtual int fn_27_24CC58(signed char);
};
class NullableModule {
public:
    virtual void isNull();
    virtual signed char slot1();
    virtual signed char slot2();
};
class KineticEnergy {
public:
    virtual void isNull();
    virtual void updateEnergy();
    virtual unsigned long long getSpeed();
    unsigned char p04;
    unsigned char p05;
    char pad06[2];
    Vec2 speed;
};
inline void setSpeed(KineticEnergy* e, float x, float y) { Vec2 out; out.y = y; out.x = x; e->speed = out; }
class Kinetic {
public:
    virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3();
    virtual void s4(); virtual void s5();
    virtual KineticEnergy* getEnergy(int index);
};
class Posture {
public:
    virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3();
    virtual void s4(); virtual void s5(); virtual void s6(); virtual void s7();
    virtual void s8(); virtual void s9();
};
struct FlagVecModule_base { char pad[8]; };
class FlagVecModule : public FlagVecModule_base {
public:
    virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3();
    virtual void s4(); virtual void s5(); virtual void s6(); virtual void s7();
    virtual void s8(); virtual void s9(); virtual void s10(); virtual void s11();
    virtual void s12(); virtual void s13(); virtual void s14(); virtual void s15();
    virtual void s16(); virtual void s17(); virtual void s18(); virtual void s19();
    virtual void s20(); virtual void s21(); virtual void s22(); virtual void s23();
    virtual void s24(); virtual void s25(); virtual void s26(); virtual void s27();
    virtual void s28(); virtual void s29(); virtual void s30(); virtual void s31();
    virtual void s32(); virtual void s33(); virtual void s34(); virtual void s35();
    virtual void s36(); virtual void s37(); virtual void s38(); virtual void s39();
    virtual void s40(); virtual void s41(); virtual void s42(); virtual void s43();
    virtual void s44(); virtual void s45(); virtual void s46(); virtual void s47();
    virtual void s48(); virtual void s49(); virtual void s50(); virtual void s51();
    virtual void s52(); virtual void s53(); virtual void s54(); virtual void s55();
    virtual void s56();
    virtual unsigned long long getSumSpeed(int flags, int unk);
};
struct Modules {
    char pad0[0xc];
    Posture* posture;
    FlagVecModule* vecmod;
    char pad1[0x68];
    Kinetic* kinetic;
};
struct soModuleAccesser { char pad[0xd8]; Modules* modules; };
extern "C" int fn_27_36D2CC(gfTask* self, NullableModule* m, soModuleAccesser* a) {
    float cy;
    const float* c = lbl_27_rodata_7B40;
    if (notifyEventAnimCmd__6WeaponFP9acAnimCmdP16soModuleAccesseri(self)) return 1;
    if (self->fn_27_24CC58(m->slot1()) == 0) return 0;
    if (m->slot2() <= -1 || m->slot2() >= 11) return 0;
    switch (m->slot2()) {
    case 6: {
        KineticEnergy* e = static_cast<KineticEnergy*>(__dynamic_cast(
            a->modules->kinetic->getEnergy(0), 0, __RTTI__21wnKineticEnergyNormal, __RTTI__15soKineticEnergy, 1));
        Vec2 v = unpack(e->getSpeed());
        float sum = v.x * v.x + v.y * v.y;
        float mag;
        if ((float)(double)nw4r::math::FAbs(sum) <= c[7]) mag = c[1];
        else mag = sum * rsqrtf(sum);
        Vec2 w = unpack(a->modules->vecmod->getSumSpeed(0xff, 0));
        double wy = w.y;
        double wx = w.x;
        float angB = (float)atan2(wx, wy);
        double vy = v.y;
        double vx = v.x;
        float angA = (float)atan2(vx, vy);
        float diff = angA - angB;
        a->modules->posture->s9();
        while (diff > c[9]) diff -= c[8];
        while (diff < c[10]) diff += c[8];
        float corr = c[1];
        if (c[1] != diff) {
            float r = angle__5Vec2fCFPC5Vec2f(&w, &v);
            float ar = (float)(double)nw4r::math::FAbs(r);
            float t;
            if (ar < c[11]) t = c[12] * (ar / c[9]);
            else t = c[13] * (ar / c[9]);
            if (diff < c[1]) t = -t;
            corr = r * t;
        }
        float ang = angA - corr;
        while (ang < c[10]) ang += c[8];
        while (ang > c[9]) ang -= c[8];
        cy = mag * (float)cos(ang);
        float sx = mag * (float)sin(ang);
        setSpeed(e, sx, cy);
        return 1;
    }
    default:
        break;
    }
    return 0;
}
}

void operator delete(void*);
// TODO: clean up, give variables real names
namespace jj_36A0A4 {
extern "C" void* __dt__18soEffectModuleImplFv(void*, short);

extern "C" void* fn_27_36A0A4(void* object, short deleting) {
    if (object != 0) {
        unsigned char* storage = static_cast<unsigned char*>(object);
        __dt__18soEffectModuleImplFv(storage + 0x14, -1);
        if (deleting > 0) {
            operator delete(object);
        }
    }
    return object;
}
}

