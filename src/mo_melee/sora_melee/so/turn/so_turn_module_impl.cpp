#define SO_TURN_MODULE_IMPL_FULL
#include <so/turn/so_turn_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/posture/so_posture_module_simple.h>
#include <types.h>

void soTurnModuleImpl::activate() {
    end();
}

void soTurnModuleImpl::set(const soTurnData* data, bool changeLr, bool isExtern, float lr) {
    m_state = 1;
    m_turnData = data;
    m_changeLr = changeLr;
    unk24 = 0;
    m_lrReversed = false;
    m_lr = lr;
    m_isExtern = isExtern;
}

bool soTurnModuleImpl::isObserv(char group) {
    return group == 0x20;
}

soTurnModule::~soTurnModule() { }

void soTurnModuleImpl::end() {
    m_state = 0;
    m_rotY = unk30 = 0.0f;
    Vec3f rot(0.0f, 0.0f, 0.0f);
    m_moduleAccesser->getPostureModule().setRot(&rot, 0);
    m_isExtern = 0;
}

#include <so/event/so_event_system.h>
// TODO: clean up, give variables real names
namespace jj_693F4 {
extern "C" {
extern char lbl_27_data_7AF0[], lbl_27_data_7AB0[], lbl_27_data_7A08[], lbl_27_data_7A10[];
extern const float lbl_27_rodata_930;
extern char __vt__22soAnimCmdEventObserver[], __RTTI__11soEventUnit[];
void* __dynamic_cast(void*, long, const void*, const void*, long);
}
class Q {
public:
 virtual void f0(); virtual void f1(); virtual void f2(); virtual void f3();
 virtual void f4(); virtual void f5(); virtual void f6(); virtual s16 get();
};
class Obs { public: virtual void attach(s32, s32); };
class Unit {
public:
 virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
 virtual void v4(); virtual void v5(); virtual void v6(); virtual s32 get();
};
struct Self {
 void* vt; Obs obs; s16 a,b,c; char pad[2]; void* vt2; s16 id, unit; s32 value;
 void* arg; s32 x,y; float f,g,h; u8 flags : 7; u8 other : 1;
};
inline s32 getResult(Self* self) {
 s16 id = self->unit;
 soEventManager* mgr = soEventSystem::getInstance()->getManager(self->id);
 if (mgr->getObserverCapacity(id) == 0) return 0;
 Unit* unit = (Unit*)__dynamic_cast(mgr->getEventUnit(id), 0, lbl_27_data_7A08, __RTTI__11soEventUnit, 0);
 if (unit == 0) return 0;
 return unit->get();
}
extern "C" void* __ct__16soTurnModuleImplFP16soModuleAccesser(Self* self, u8* arg) {
 s32 result = 0;
 s32 eid;
 self->vt = lbl_27_data_7AF0;
 u8* obj = *(u8**)(*(u8**)(arg + 0xD8) + 0x80);
 s16 r = ((Q*)(obj + 8))->get();
 self->a = -1; self->b = 5; self->c = -1;
 *(void**)&self->obs = __vt__22soAnimCmdEventObserver;
 self->obs.attach((s32)r, -1);
 obj = *(u8**)(*(u8**)(arg + 0xD8) + 0x80);
 eid = (s16)((Q*)(obj + 8))->get();
 self->vt2 = lbl_27_data_7AB0;
 self->id = eid; self->unit = 18; self->value = result;
 if (eid > 0 && eid > -1) {
  if (soEventSystem::getInstance()->getInstanceManager().isContain(eid)) {
   if (soEventSystem::getInstance()->getManager(self->id)->getObserverCapacity(self->unit) == 0)
    self->value = 0;
   else {
    result = getResult(self);
    self->value = result;
   }
  }
 }
 self->vt = lbl_27_data_7A10;
 *(void**)&self->obs = lbl_27_data_7A10 + 0x34;
 self->vt2 = lbl_27_data_7A10 + 0x48;
 self->arg = arg; self->x = 0; self->y = 0;
 self->f = lbl_27_rodata_930; self->g = lbl_27_rodata_930; self->h = lbl_27_rodata_930;
 self->flags = 0;
 return self;
}
}

// TODO: clean up, give variables real names
namespace jj_69644 {
extern "C" void __dl__FPv(void*);

extern "C" void* fn_27_69644(void* ptr, int size) {
    if (ptr != 0 && size > 0)
        __dl__FPv(ptr);
    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_69684 {
extern "C" {
void* getInstance__13soEventSystemFv();
void* getManager__13soEventSystemFl(void*, long);
void __dl__FPv(void*);
}

class soAnimCmdEventObserver;

template <class T>
struct soEventObserver {
    static void __vt(int, int);
};

struct EvSys {
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void s5();
    virtual u32 s8(s32 id);
};

struct EvMgr {
    virtual void d0();
    virtual void d1();
    virtual void d2();
    virtual void f5(s16 a, s16 b);
};

struct CtlObj {
    void* pad0;
    void* vt;
    s16 m8;
    s16 mA;
    s16 mC;
};

extern "C" void* __dt__16soTurnModuleImplFv(void* p, s32 flag) {
    if (p != nullptr) {
        CtlObj* c = (CtlObj*)p;
        if ((char*)p + 4 != nullptr && (char*)c + 4 != nullptr) {
            s16 id = c->m8;
            c->vt = (void*)soEventObserver<soAnimCmdEventObserver>::__vt;
            u32 r = ((EvSys*)getInstance__13soEventSystemFv())->s8(id);
            if (r == 1) {
                EvMgr* mgr = (EvMgr*)getManager__13soEventSystemFl(
                    getInstance__13soEventSystemFv(), c->m8);
                mgr->f5(c->mA, c->mC);
            }
            c->mC = -1;
            c->m8 = -1;
        }
        if (flag > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_69EA4 {
typedef unsigned char u8;
typedef unsigned int u32;

extern "C" u32 isTurn__16soTurnModuleImplFv(u8* p) {
    u32 v = (p[0x34] >> 5) & 7;
    return v != 0;
}
}

// TODO: clean up, give variables real names
namespace jj_69EBC {
extern "C" void fn_27_69EBC(void) {}
}

// TODO: clean up, give variables real names
namespace jj_69ED4 {
typedef unsigned long u32;
struct ArgData { int type; u32 data; };
struct Arg {
 const ArgData* p; unsigned char null;
 Arg(const ArgData* q):p(q),null(0){}
 int type() const { return null==1?-1:p->type; }
 u32 value() const { return null==1?0:p->data; }
 bool boolean() const { return null==1?false:p->data!=0; }
};
struct List {
 virtual void v0();
 virtual void v1();
 virtual const ArgData* at(int) const;
 virtual int size() const;
 char data[12];
 Arg get(int i) const { Arg a(at(i)); return a; }
 int count() const { return size(); }
};
struct Cmd {
 virtual void v0();
 virtual signed char group();
 virtual signed char type();
 virtual void v3();
 virtual void v4();
 virtual List args();
};
struct Obj {
 virtual void v0();
 virtual void v1();
 virtual void set(void*,bool,int);
 virtual void reset();
 virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
 virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
 virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
 virtual void v16(); virtual void v17(); virtual void v18();
 virtual int valid(signed char);
};
struct Sub {
 virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
 virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
 virtual void v8(); virtual void act();
};
struct Modules { char pad[12]; Sub* sub; };
struct Access { char pad[0xd8]; Modules* modules; };
extern "C" void* getConstantIndefinite__15soValueAccesserFP16soModuleAccesserUlUl(Access*,u32,u32);
extern "C" bool notifyEventAnimCmd__16soTurnModuleImplFP9acAnimCmdP16soModuleAccesserl(Obj* self,Cmd* cmd,Access* acc) {
 if (!self->valid(cmd->group())) return 0;
 if (cmd->type()<=-1 || cmd->type()>=2) return 0;
 switch(cmd->type()) {
 case 0: {
 const List args=cmd->args();
 void* value;
 if(args.get(0).type()==5)
 value=getConstantIndefinite__15soValueAccesserFP16soModuleAccesserUlUl(acc,args.get(0).value(),0);
 else return 1;
 bool flag=true;
 if(args.count()>1) {
 if(args.get(1).type()==3) flag=args.get(1).boolean();
 else return 1;
 }
 acc->modules->sub->act();
 self->set(value,flag,0);
 return 1;
 }
 case 1:self->reset();return 1;
 }
 return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_6A1D4 {
struct Fn6A1D4Obj {
    u8 pad[0x34];
    u8 f0 : 6;
    u8 f1 : 1;
    u8 f2 : 1;
};

extern "C" {
void setOmitIntermediate__16soTurnModuleImplFb(Fn6A1D4Obj* self, u32 flag) {
    self->f1 = flag;
}
}
}

// TODO: clean up, give variables real names
namespace jj_6A1E4 {
extern "C" void* getEndFrame__16soTurnModuleImplFv(void* self) {
    void* p = *(void**)((char*)self + 0x20);
    return *(void**)((char*)p + 0x4);
}
}

// TODO: clean up, give variables real names
namespace jj_6A1F0 {
extern "C" {
extern const float lbl_27_rodata_934;

struct FnObj {
    char pad[0x28];
    float f28;
    float f2c;
};

float getRYLr__16soTurnModuleImplFv(FnObj* p)
{
    float r = lbl_27_rodata_934 * p->f28;
    return r + p->f2c;
}
}
}

// TODO: clean up, give variables real names
namespace jj_6A20C {
extern "C" f32 getRY__16soTurnModuleImplFv(void* self) {
    return *reinterpret_cast<f32*>(reinterpret_cast<u8*>(self) + 0x2c);
}
}

// TODO: clean up, give variables real names
namespace jj_6A214 {
struct S {
    unsigned char pad[0x34];
    unsigned char a : 5;
    unsigned char flag : 1;
    unsigned char b : 2;
};

extern "C" bool isExtern__16soTurnModuleImplFv(S* p)
{
    return p->flag;
}
}

// TODO: clean up, give variables real names
namespace jj_6A22C {
extern "C" void notifyEventAnimCmd__16soTurnModuleImplFP9acAnimCmdP16soModuleAccesserl(void*);

extern "C" void fn_27_6A22C(void* self) {
    notifyEventAnimCmd__16soTurnModuleImplFP9acAnimCmdP16soModuleAccesserl(reinterpret_cast<u8*>(self) - 0x4);
}
}

// TODO: clean up, give variables real names
namespace jj_6A234 {
extern "C" void isObserv__16soTurnModuleImplFc(void*);

extern "C" void fn_27_6A234(void* self) {
    isObserv__16soTurnModuleImplFc(reinterpret_cast<u8*>(self) - 0x4);
}
}

// TODO: clean up, give variables real names
namespace jj_6A23C {
extern "C" void __dt__16soTurnModuleImplFv(void*);

extern "C" void fn_27_6A23C(void* self) {
    __dt__16soTurnModuleImplFv(reinterpret_cast<u8*>(self) - 0x10);
}
}

