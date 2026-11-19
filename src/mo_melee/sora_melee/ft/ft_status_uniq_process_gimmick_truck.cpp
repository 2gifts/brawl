#include <ft/ft_status_uniq_process_gimmick.h>
#include <so/so_module_accesser.h>
#include <types.h>

void ftStatusUniqProcessGimmickTruck::notifyEvent(soGimmickEventArgs* eventInfo, soModuleAccesser* moduleAccesser) {
    if (eventInfo->m_kind <= 0x1B || eventInfo->m_kind >= 0x22) {
        ftStatusUniqProcessGimmick::notifyEvent(eventInfo, moduleAccesser);
    } else {
        switch (eventInfo->m_kind) {
        case Gimmick::Truck_Event_Start_Run:
            moduleAccesser->getWorkManageModule().onFlag(0x22000010);
            break;
        case Gimmick::Truck_Event_Pos: {
            soGimmickTruckEventArgs* args = static_cast<soGimmickTruckEventArgs*>(eventInfo);
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            moduleAccesser->getWorkManageModule().setFloat(args->m_pos.m_x - pos.m_x, 0x21000004);
            moduleAccesser->getWorkManageModule().setFloat(args->m_pos.m_y - pos.m_y, 0x21000005);
            pos.m_x = args->m_pos.m_x;
            pos.m_y = args->m_pos.m_y;
            moduleAccesser->getPostureModule().setPos(&pos);
            break;
        }
        case Gimmick::Truck_Event_Crash:
            moduleAccesser->getWorkManageModule().offFlag(0x22000010);
            moduleAccesser->getStatusModule().changeStatusRequest(0x49, moduleAccesser);
            break;
        }
    }
}

#include <so/so_value_accesser.h>
// TODO: clean up, give variables real names
namespace jj_17835C {

struct Vec3 {
    float x, y, z;
    Vec3() {}
    void set(float a, float b, float c) { y = b; x = a; z = c; }
    Vec3(const Vec3& v) : x(v.x), y(v.y), z(v.z) {}
};
class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual int slot4(unsigned long);
    virtual void slot5(int, unsigned long);
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13(float, unsigned long);
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18(unsigned long);
};
struct Position {
    virtual void p0();
    virtual void p1();
    virtual void p2();
    virtual void p3();
    virtual Vec3 get();
    virtual void set(const Vec3*);
};
struct Modules {
    char pad[0xc];
    Position* pos;
    char pad2[0x54];
    Object_vptr_0* work;
};
struct Actor {
    char pad[0xd8];
    Modules* modules;
};
struct VecData { int x, y, z; };
struct Out {
    int a, b;
    VecData pos;
    int padding[4];
};
extern "C" void initStatus__26ftStatusUniqProcessGimmickFP16soModuleAccesser(void*, void*);
extern "C" void presentEvent__26ftStatusUniqProcessGimmickFP18soGimmickEventArgsP16soModuleAccesser(void*, void*, void*);
extern "C" void initStatus__31ftStatusUniqProcessGimmickTruckFP16soModuleAccesser(void* self, Actor* actor) {
    initStatus__26ftStatusUniqProcessGimmickFP16soModuleAccesser(self, actor);
    Vec3 pos = actor->modules->pos->get();
    Out out;
    out.a = 0x1c;
    out.b = 0;
    out.pos = *(VecData*)&pos;
    presentEvent__26ftStatusUniqProcessGimmickFP18soGimmickEventArgsP16soModuleAccesser(self, &out, actor);
    pos.set(*(float*)&out.pos.x, *(float*)&out.pos.y, pos.z);
    actor->modules->pos->set(&pos);
    actor->modules->work->slot18(0x1200003c);
    float value = soValueAccesser::getConstantFloat((soModuleAccesser*)actor, 0xd2e, 0);
    actor->modules->work->slot13(value, 0x11000012);
}
}

// TODO: clean up, give variables real names
namespace jj_17846C {
typedef unsigned char u8;
typedef int s32;
typedef float f32;

extern "C" void presentEvent__26ftStatusUniqProcessGimmickFP18soGimmickEventArgsP16soModuleAccesser(s32 a, void* b, void* c);
extern const f32 lbl_27_rodata_2328;

class Sub5C {
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
    virtual f32 vf16(void* a);
    virtual void vf17();
    virtual f32 vf18();
};

class Mid {
public:
    u8 m_pad[0x5c];
    Sub5C* m_sub;
};

class Obj {
public:
    u8 m_pad[0xd8];
    Mid* m_mid;
};

struct Vec3 {
    f32 x, y, z;
};

struct Words {
    s32 x, y, z;
};

struct Params {
    s32 a, b;
    Vec3 v;
    f32 f, g;
};

extern "C" void execStatus__31ftStatusUniqProcessGimmickTruckFP16soModuleAccesser(s32 arg0, void* arg1) {
    Vec3 t;
    Params p;

    t.x = lbl_27_rodata_2328;
    t.y = lbl_27_rodata_2328;
    t.z = lbl_27_rodata_2328;
    p.a = 0x1d;
    p.b = 0;
    *(Words*)&p.v = *(Words*)&t;
    p.f = ((Obj*)arg1)->m_mid->m_sub->vf16(arg1);
    p.g = ((Obj*)arg1)->m_mid->m_sub->vf18();
    presentEvent__26ftStatusUniqProcessGimmickFP18soGimmickEventArgsP16soModuleAccesser(arg0, &p, arg1);
}
}

// TODO: clean up, give variables real names
namespace jj_178524 {
typedef unsigned char u8;
typedef int s32;
typedef float f32;

extern "C" void* __dynamic_cast(void*, long, const void*, const void*, short);
extern "C" char __RTTI__21soKineticEnergyNormal[];
extern "C" char __RTTI__15soKineticEnergy[];
extern const f32 lbl_27_rodata_2328;
extern "C" void presentEvent__26ftStatusUniqProcessGimmickFP18soGimmickEventArgsP16soModuleAccesser(s32, void*, void*);
extern "C" void exitStatus__26ftStatusUniqProcessGimmickFP16soModuleAccesseri(void*, void*, s32);

struct Access64 {
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
    virtual f32 get(int);
};

struct Access7C {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void* set(int);
};

struct Access58 {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void set(int);
};

struct Mid {
    char pad[0x58];
    Access58* a58;
    char pad2[8];
    Access64* a64;
    char pad3[0x14];
    Access7C* a7c;
};

struct Obj {
    char pad[0xd8];
    Mid* mid;
};

struct Vec3 {
    f32 x, y, z;
};

struct Words {
    s32 x, y, z;
};

struct Params {
    s32 a, b;
    Vec3 v;
    f32 f, g, h, i;
};

extern "C" void exitStatus__31ftStatusUniqProcessGimmickTruckFP16soModuleAccesseri(void* arg0, Obj* arg1, s32 arg2) {
    if (arg2 == 11) {
        Params p;
        Vec3 t;

        f32 hi = arg1->mid->a64->get(0x21000005);
        f32 lo = arg1->mid->a64->get(0x21000004);
        f32 vals[2];
        vals[0] = lo;
        vals[1] = hi;

        void* o = arg1->mid->a7c->set(3);
        char* res = (char*)__dynamic_cast(
            o, 0, __RTTI__21soKineticEnergyNormal, __RTTI__15soKineticEnergy, 1);

        *(f32*)(res + 8) = vals[0];
        *(f32*)(res + 12) = vals[1];
        *(u8*)(res + 5) |= 0x80;

        t.x = lbl_27_rodata_2328;
        t.y = lbl_27_rodata_2328;
        t.z = lbl_27_rodata_2328;

        p.a = 30;
        p.b = 0;
        *(Words*)&p.v = *(Words*)&t;
        presentEvent__26ftStatusUniqProcessGimmickFP18soGimmickEventArgsP16soModuleAccesser((s32)arg0, &p, arg1);
    }

    arg1->mid->a58->set(1);
    exitStatus__26ftStatusUniqProcessGimmickFP16soModuleAccesseri(arg0, arg1, arg2);
}
}

// TODO: clean up, give variables real names
namespace jj_17881C {
extern "C" {
void* getInstance__13soEventSystemFv();
void* getManager__13soEventSystemFl(void*, long);
void __dl__FPv(void*);
}

struct EvSys {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f5();
    virtual u32 f6(s32 id);
};

struct EvMgr {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3(s16 a, s16 b);
};

class soGimmickEventObserver;

template<class T>
struct soEventObserver {
    static char __vt[];
};

struct fn27Obj {
    void* unk0;
    void* vptr;
    s16 m_8;
    s16 m_A;
    s16 m_C;
};

extern "C" fn27Obj* __dt__31ftStatusUniqProcessGimmickTruckFv(fn27Obj* p, s32 flag) {
    if (p != nullptr) {
        if (p != nullptr) {
            if ((char*)p + 4 != nullptr && (char*)p + 4 != nullptr) {
                s16 id = p->m_8;
                p->vptr = soEventObserver<soGimmickEventObserver>::__vt;
                if (((EvSys*)getInstance__13soEventSystemFv())->f6(id) == 1) {
                    EvMgr* mgr = (EvMgr*)getManager__13soEventSystemFl(
                        getInstance__13soEventSystemFv(), p->m_8);
                    mgr->f3(p->m_A, p->m_C);
                }
                p->m_C = -1;
                p->m_8 = -1;
            }
        }
        if (flag > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

