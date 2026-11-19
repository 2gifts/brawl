#include <so/reflect/so_reflect_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

soReflectModule::~soReflectModule() { }

soReflectModuleImpl::~soReflectModuleImpl() { }

void soReflectModuleImpl::activate() {
    resetInfo();
}

void soReflectModuleImpl::initInfo() {
    m_taskId = -1;
    unk3C = 0;
}

void soReflectModuleImpl::resetInfo() {
    m_taskId = -1;
    m_attackMul = 1.0f;
    m_speedMul = 1.0f;
    m_lifeMul = 0.0f;
    m_count = 0;
    m_noSpeedMul = false;
}

bool soReflectModuleImpl::isReflect() {
    return m_taskId != -1;
}

void soReflectModuleImpl::setNoSpeedMul(bool noSpeedMul) {
    m_noSpeedMul = noSpeedMul;
}

float soReflectModuleImpl::getLifeMul() {
    return m_lifeMul;
}

float soReflectModuleImpl::getSpeedMul() {
    return m_speedMul;
}

void soReflectModuleImpl::setAttackMul(float attackMul) {
    m_attackMul = attackMul;
}

float soReflectModuleImpl::getAttackMul() {
    return m_attackMul;
}

int soReflectModuleImpl::getTeam() {
    return m_team;
}

int soReflectModuleImpl::getTaskId() {
    return m_taskId;
}

void soReflectModuleImpl::deactivate() { }

#include <gf/gf_task.h>
#include <gf/gf_task_scheduler.h>
#include <so/event/so_event_system.h>
#include <so/so_instance_manager.h>
// TODO: clean up, give variables real names
namespace jj_63B94 {
extern "C" u8 lbl_27_data_71A0[];
extern "C" u8 lbl_27_data_7270[];
extern "C" u8 lbl_27_data_7284[];
extern "C" u8 lbl_27_data_7298[];
extern "C" const f32 lbl_27_rodata_8F0;
extern "C" const f32 lbl_27_rodata_8F4;

struct OwnerModule {
    u8 pad[0x80];
    void* object;
};

struct Owner {
    u8 pad[0xD8];
    OwnerModule* module;
};

struct VirtualObject {
    virtual void fn0();
    virtual void fn1();
    virtual void fn2();
    virtual void fn3();
    virtual void fn4();
    virtual void fn5();
    virtual void fn6();
    virtual s32 getID();
};

struct Observer {
    virtual void attach(s16, s32);
    s16 id;
    s16 type;
    s16 link;
};

struct MainObject {
    void* field0;
    u8 field4;
    u8 pad5[3];
    void* field8;
    Observer observer;
    Observer observer2;
    s32 field24;
    s32 field28;
    f32 field2C;
    f32 field30;
    f32 field34;
    u8 pad38[5];
    u8 field3D;
};

extern "C" MainObject* __ct__19soReflectModuleImplFP16soModuleAccesseri(
    MainObject* self, Owner* obj, s32 flag
) {
    self->field4 = 0;
    self->field8 = lbl_27_data_7298;
    self->field0 = lbl_27_data_7298 + 8;

    s32 id = ((VirtualObject*)((u8*)obj->module->object + 8))->getID();

    s32 secondID = -1;

    self->observer.id = -1;
    self->observer.type = 1;
    self->observer.link = -1;
    *(void**)&self->observer = lbl_27_data_7284;
    self->observer.attach(id, -1);

    if (flag != 0) {
        secondID = ((VirtualObject*)((u8*)obj->module->object + 8))->getID();
    }

    Observer* obs2 = &self->observer2;
    obs2->id = -1;
    obs2->type = 0x11;
    obs2->link = -1;
    *(void**)obs2 = lbl_27_data_7270;
    obs2->attach(secondID, -1);

    self->field8 = lbl_27_data_71A0;
    self->field0 = lbl_27_data_71A0 + 8;
    *(void**)&self->observer = lbl_27_data_71A0 + 0x4C;
    *(void**)&self->observer2 = lbl_27_data_71A0 + 0x60;
    self->field24 = -1;
    self->field28 = -1;
    self->field2C = lbl_27_rodata_8F0;
    self->field30 = lbl_27_rodata_8F0;
    self->field34 = lbl_27_rodata_8F4;
    self->field3D = 0;

    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_63E00 {
extern "C" {
extern char lbl_27_data_7310[];
void* getInstance__13soEventSystemFv();
void* getManager__13soEventSystemFl(void*, long);
void __dl__FPv(void*);
}

struct EventSystem {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f5();
    virtual u32 f6(s16);
};

struct EventManager {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3(s16, s16);
};

struct Obj {
    void* vptr;
    s16 m4;
    s16 m6;
    s16 m8;
};

extern "C" Obj* __dt__30soCollisionSearchEventObserverFv(Obj* p, s32 flag) {
    if (p != 0) {
        if (p != 0) {
            s16 id = p->m4;
            p->vptr = lbl_27_data_7310;

            if (((EventSystem*)getInstance__13soEventSystemFv())->f6(id) == 1) {
                ((EventManager*)getManager__13soEventSystemFl(
                    getInstance__13soEventSystemFv(), p->m4))->f3(p->m6, p->m8);
            }

            p->m8 = -1;
            p->m4 = -1;
        }

        if (flag > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_6405C {

extern "C" int getConstantInt__15soValueAccesserFP16soModuleAccesserUlUl(
    soModuleAccesser*, unsigned long, unsigned long);

extern "C" bool fn_27_6405C(void* self, soModuleAccesser* accesser) {
    unsigned int constant =
        getConstantInt__15soValueAccesserFP16soModuleAccesserUlUl(
            accesser, 0x55f5, 0);
    unsigned int value =
        *reinterpret_cast<unsigned int*>(
            static_cast<unsigned char*>(self) + 0x38);
    return value >= constant;
}
}

// TODO: clean up, give variables real names
namespace jj_640AC {
typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;
typedef float f32;

extern "C" void* __dynamic_cast(void*, long, const void*, const void*, short);
extern "C" void* getInstance__15gfTaskSchedulerFv();
extern "C" void* getTaskById__15gfTaskSchedulerFQ26gfTask8CategoryUl(void*, int, u32);
extern "C" char __RTTI__11StageObject[];
extern "C" char __RTTI__6gfTask[];
extern "C" void* getCollisionReflectorModule__23soExternalValueAccesserFP11StageObject(void*);

class soNullable {
public:
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void s5();
    virtual void s6();
    virtual void s7();
    virtual void s8();
    virtual void s9();
    virtual void s10();
    virtual void s11();
    virtual void s12();
    virtual void s13();
    virtual void s14();
    virtual void s15();
    virtual void s16(int, int);
    virtual void s17();
    virtual void s18();
    virtual void s19();
    virtual void s20();
    virtual void s21();
    virtual void s22();
    virtual f32 s23(int);
    virtual f32 s24(int);
    virtual f32 s25(int);
    virtual f32 s26(int);
    virtual void s27();
    virtual void s28();
    virtual void* s29();
    virtual void s30();
    virtual void s31();
    virtual void s32();
    virtual void s33();
    virtual void s34();
    virtual void s35();
    virtual void s36();
    virtual void s37();
    virtual void s38();
    virtual void s39();
    virtual void s40();
    virtual void s41();
    virtual void s42();
    virtual void s43();
    virtual void s44();
    virtual void s45();
    virtual void s46();
    virtual void s47();
    virtual void s48();
    virtual void s49();
    virtual void s50();
    virtual void s51();
    virtual void s52();
    virtual void s53();
    virtual void s54();
    virtual void s55();
    virtual void s56();
    virtual void s57();
    virtual void s58();
    virtual void s59();
    virtual void s60();
    virtual void s61();
    virtual void s62();
    virtual void s63();
    virtual void s64();
    virtual void s65();
    virtual void s66();
    virtual void s67(void*, int);
};

struct Arg2;

struct Fd8Obj {
    char pad[0x1C];
    soNullable* n1c;
    char pad2[0x24];
    soNullable* n44;
};

struct Arg2 {
    char pad[0xD8];
    Fd8Obj* fd8;
};

struct Arg1 {
    char pad[0x0C];
    s32 f0c;
    char pad2[0x11];
    u8 f21;
    u8 f22;
    u8 f23;
};

struct SelfBase {
    char pad[8];
};

class Self : public SelfBase {
public:
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void s5();
    virtual void s6();
    virtual void s7();
    virtual void s8();
    virtual void s9();
    virtual void s10();
    virtual void s11();
    virtual void s12();
    virtual void s13();
    virtual void s14();
    virtual s32 s15(Arg2*);

    char pad_data[0x18];
    s32 f24;
    void* f28;
    f32 f2c;
    f32 f30;
    f32 f34;
    s32 f38;
    char pad3[1];
    u8 f3d;
};

extern "C" void fn_27_640AC(Self* self, const Arg1* arg1, Arg2* arg2, f32 f) {
    if (arg1->f21 == 3) {
        if ((u32)self->f24 + 0x10000 == 0xFFFF) {
            void* task = getTaskById__15gfTaskSchedulerFQ26gfTask8CategoryUl(
                getInstance__15gfTaskSchedulerFv(), (int)arg1->f22,
                (u32)arg1->f0c);
            void* casted = __dynamic_cast(
                task, 0x3C, __RTTI__11StageObject, __RTTI__6gfTask, 0);
            soNullable* obj = (soNullable*)getCollisionReflectorModule__23soExternalValueAccesserFP11StageObject(casted);

            if (f < obj->s26((int)arg1->f23)) {
                if (self->s15(arg2) == 0) {
                    self->f24 = arg1->f0c;
                    self->f28 = obj->s29();
                    self->f2c = self->f2c * obj->s23((int)arg1->f23);
                    if (self->f3d == 0) {
                        self->f30 = obj->s24((int)arg1->f23);
                    }
                    self->f34 = obj->s25((int)arg1->f23);
                } else {
                    arg2->fd8->n1c->s67((void*)arg1, 0);
                }
            } else {
                arg2->fd8->n44->s16(1, 0);
                arg2->fd8->n1c->s67((void*)arg1, 0);
            }

            self->f38++;
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_64290 {

extern "C" void* __dynamic_cast(void*, long, const void*, const void*, short);
extern "C" void* getCollisionReflectorModule__23soExternalValueAccesserFP11StageObject(void*);
extern "C" char __RTTI__11StageObject[];
extern "C" char __RTTI__6gfTask[];

struct Args {
    char pad0[0xC];
    u32 unkC;
    char pad1[0x21 - 0x10];
    u8 unk21;
    u8 unk22;
    u8 unk23;
};

struct Ctx;

class Inner {
public:
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void s5();
    virtual void s6();
    virtual void s7();
    virtual void s8();
    virtual void s9();
    virtual void s10();
    virtual void s11();
    virtual void s12();
    virtual void s13();
    virtual void s14();
    virtual void s15();
    virtual void s16(const Args* args, int flag);
};

struct Mid {
    char pad[0x34];
    Inner* unk34;
};

struct Ctx {
    char pad[0xD8];
    Mid* unkD8;
};

class Obj {
public:
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void s5();
    virtual void s6();
    virtual void s7();
    virtual void s8();
    virtual void s9();
    virtual void s10();
    virtual void s11();
    virtual void s12();
    virtual void s13();
    virtual void s14();
    virtual void s15();
    virtual void s16();
    virtual void s17();
    virtual void s18();
    virtual void s19();
    virtual void s20();
    virtual void s21();
    virtual void s22();
    virtual void s23();
    virtual float s24(u8 p);
    virtual float s25(u8 p);
    virtual void s26();
    virtual void s27();
    virtual void s28();
    virtual void* s29();
};

struct Base8 {
    char pad[8];
};

class Self : public Base8 {
public:
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void s5();
    virtual void s6();
    virtual void s7();
    virtual void s8();
    virtual void s9();
    virtual void s10();
    virtual void s11();
    virtual void s12();
    virtual void s13();
    virtual void s14();
    virtual int s15(Ctx* ctx);
    char pad0[0x24 - 0xC];
    u32 unk24;
    void* unk28;
    char pad1[0x30 - 0x2C];
    float unk30;
    float unk34;
    s32 unk38;
};

extern "C" void fn_27_64290(Self* self, const Args* args, Ctx* ctx) {
    if (args->unk21 == 3 && self->unk24 == -1) {
        if (self->s15(ctx) == 0) {
            void* task = gfTaskScheduler::getInstance()->getTaskById((gfTask::Category)args->unk22, args->unkC);
            void* obj = __dynamic_cast(task, 0x3c, __RTTI__11StageObject, __RTTI__6gfTask, 0);
            Obj* p = (Obj*)getCollisionReflectorModule__23soExternalValueAccesserFP11StageObject(obj);
            self->unk24 = args->unkC;
            self->unk28 = p->s29();
            self->unk30 = p->s24(args->unk23);
            self->unk34 = p->s25(args->unk23);
        } else {
            ctx->unkD8->unk34->s16(args, 0);
        }
        self->unk38++;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_643C0 {
extern "C" {
extern char lbl_27_data_3788[];
extern char __RTTI__11soEventUnit[];
void* __dynamic_cast(void*, long, const void*, const void*, short);
}
struct Self {
    void* vt;
    s16 a;
    s16 b;
    s16 c;
};
class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual s16 slot6(Self*, s8);
    virtual void slot7();
    virtual void slot8();
};
extern "C" void addObserver__30soCollisionAttackEventObserverFsSc(Self* self, s32 arg1, s8 arg2) {
    soEventManager* mgr;
    u32 f31 = 0;
    s32 f30 = 0;
    s32 f4 = 0;
    s32 f5 = 0;
    s16 ret;
    s16 id;
    s16 a = self->a;
    if (a >= 0 && self->b >= 0) {
        f5 = 1;
    }
    if (f5 != 0 && self->c > -1) {
        f4 = 1;
    }
    if (f4 != 0) {
        if (soEventSystem::getInstance()->getInstanceManager().isContain(a) == 1) {
            f30 = 1;
        }
    }
    if (f30 != 0) {
        if (soEventSystem::getInstance()->getManager(self->a)->getObserverCapacity(self->b) == 1) {
            f31 = 1;
        }
    }
    if (f31 == 1) {
        id = self->a;
        if (soEventSystem::getInstance()->getInstanceManager().isContain(id) == 1) {
            soEventSystem::getInstance()->getManager(self->a)->eraseObserver(self->b, self->c);
        }
        self->c = -1;
        self->a = -1;
    }
    if (arg1 > -1 && self->b > -1) {
        if (soEventSystem::getInstance()->getInstanceManager().isContain(arg1) != 0) {
            if (soEventSystem::getInstance()->getManager(arg1)->isNullUnit(self->b) != 1) {
                mgr = soEventSystem::getInstance()->getManager(arg1);
                if (mgr->getObserverCapacity(self->b) == 0) {
                    ret = -1;
                } else {
                    soEventUnit* unit = mgr->getEventUnit(self->b);
                    Object_vptr_0* cast = (Object_vptr_0*)__dynamic_cast(unit, 0, lbl_27_data_3788, __RTTI__11soEventUnit, 0);
                    if (cast == 0) {
                        ret = -1;
                    } else {
                        ret = cast->slot6(self, arg2);
                    }
                }
                self->c = ret;
                if (ret > -1 && ret > -1) {
                    self->a = (s16)arg1;
                }
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_645FC {

extern "C" {
extern char lbl_27_data_4950[];
extern char __RTTI__11soEventUnit[];
void* __dynamic_cast(void*, long, const void*, const void*, short);
}

struct Self {
    void* vt;
    s16 a;
    s16 b;
    s16 c;
};

class Class_4950 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual s16 slot6(Self*, s8);
    virtual void slot7();
    virtual void slot8();
};

extern "C" void addObserver__30soCollisionSearchEventObserverFsSc(Self* self, s32 arg1, s8 arg2) {
    soEventManager* mgr;
    u32 f31 = 0;
    s32 f30 = 0;
    s32 f4 = 0;
    s32 f5 = 0;
    s16 ret;
    s16 id;
    s16 a = self->a;

    if (a >= 0 && self->b >= 0) {
        f5 = 1;
    }
    if (f5 != 0 && self->c > -1) {
        f4 = 1;
    }
    if (f4 != 0) {
        if (soEventSystem::getInstance()->getInstanceManager().isContain(a) == 1) {
            f30 = 1;
        }
    }
    if (f30 != 0) {
        if (soEventSystem::getInstance()->getManager(self->a)->getObserverCapacity(self->b) == 1) {
            f31 = 1;
        }
    }
    if (f31 == 1) {
        id = self->a;
        if (soEventSystem::getInstance()->getInstanceManager().isContain(id) == 1) {
            soEventSystem::getInstance()->getManager(self->a)->eraseObserver(self->b, self->c);
        }
        self->c = -1;
        self->a = -1;
    }
    if (arg1 > -1 && self->b > -1) {
        if (soEventSystem::getInstance()->getInstanceManager().isContain(arg1) != 0) {
            if (soEventSystem::getInstance()->getManager(arg1)->isNullUnit(self->b) != 1) {
                mgr = soEventSystem::getInstance()->getManager(arg1);
                if (mgr->getObserverCapacity(self->b) == 0) {
                    ret = -1;
                } else {
                    soEventUnit* unit = mgr->getEventUnit(self->b);
                    Class_4950* cast = (Class_4950*)__dynamic_cast(unit, 0, lbl_27_data_4950, __RTTI__11soEventUnit, 0);
                    if (cast == 0) {
                        ret = -1;
                    } else {
                        ret = cast->slot6(self, arg2);
                    }
                }
                self->c = ret;
                if (ret > -1 && ret > -1) {
                    self->a = (s16)arg1;
                }
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_64874 {
extern "C" {
extern char lbl_27_data_7310[];
void* getInstance__13soEventSystemFv();
void* getManager__13soEventSystemFl(void*, long);
void __dl__FPv(void*);
}

struct fn27Obj {
    void* vptr;
    s16 m_4;
    s16 m_6;
    s16 m_8;
};

struct fn27EventSystem {
    virtual void fn0();
    virtual void fn1();
    virtual void fn2();
    virtual void fn3();
    virtual void fn4();
    virtual void fn5();
    virtual bool fn6(s16);
};

struct fn27Mgr {
    virtual void fn0();
    virtual void fn1();
    virtual void fn2();
    virtual void fn3(s16, s16);
};

extern "C" fn27Obj* fn_27_64874(fn27Obj* p, s32 flag) {
    if (p != 0) {
        s16 id = p->m_4;
        p->vptr = lbl_27_data_7310;

        if (((fn27EventSystem*)getInstance__13soEventSystemFv())->fn6(id) == 1) {
            ((fn27Mgr*)getManager__13soEventSystemFl(
                getInstance__13soEventSystemFv(), p->m_4))->fn3(p->m_6, p->m_8);
        }

        p->m_8 = -1;
        p->m_4 = -1;

        if (flag > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

