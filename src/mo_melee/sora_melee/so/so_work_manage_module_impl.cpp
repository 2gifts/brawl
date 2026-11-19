#include <mt/mt_prng.h>
#include <so/work/so_work_manage_module_impl.h>

// A work variable ID packs which general work holds it (top nibble) and the variable number (low 24 bits).
// Flag IDs address a single bit: the low 8 bits are the bit number, 32 bits to a word.
#define WORK_OF(index) m_generalWorks[(index) >> 28]
#define WORK_VAR(index) ((index) & 0xFFFFFF)

// Clears every general work and releases the lock counters.
void soWorkManageModuleImpl::activate() {
    for (int i = 0; i < 3; i++) {
        if (m_generalWorks[i] != NULL)
            m_generalWorks[i]->clearWorkAll();
    }
    soLockable::reset();
}

void soWorkManageModuleImpl::setWork(u32 index, soGeneralWorkAbstract* generalWork) {
    m_generalWorks[index] = generalWork;
}

int soWorkManageModuleImpl::getInt(u32 index) {
    return WORK_OF(index)->getIntWork(WORK_VAR(index));
}

void soWorkManageModuleImpl::setInt(int value, u32 index) {
    WORK_OF(index)->setIntWork(value, WORK_VAR(index));
}

// Sets the variable to a random value in [minValue, maxValue]; does nothing for an empty range.
void soWorkManageModuleImpl::rndInt(int minValue, int maxValue, u32 index) {
    if (maxValue < minValue)
        return;
    int value;
    if (maxValue == minValue)
        value = maxValue;
    else
        value = minValue + randi(maxValue - minValue + 1);
    setInt(value, index);
}

void soWorkManageModuleImpl::incInt(u32 index) {
    WORK_OF(index)->incIntWork(WORK_VAR(index));
}

void soWorkManageModuleImpl::decInt(u32 index) {
    WORK_OF(index)->decIntWork(WORK_VAR(index));
}

void soWorkManageModuleImpl::addInt(int addValue, u32 index) {
    WORK_OF(index)->addIntWork(addValue, WORK_VAR(index));
}

void soWorkManageModuleImpl::subInt(int subtractValue, u32 index) {
    WORK_OF(index)->subIntWork(subtractValue, WORK_VAR(index));
}

// Counts the variable down by one while it is above the threshold; true on the step that reaches the threshold.
u32 soWorkManageModuleImpl::countDownInt(u32 index, int threshold) {
    int value = getInt(index);
    if (value > threshold) {
        setInt(value - 1, index);
        return value - 1 == threshold;
    }
    return 0;
}

float soWorkManageModuleImpl::getFloat(u32 index) {
    return WORK_OF(index)->getFloatWork(WORK_VAR(index));
}

void soWorkManageModuleImpl::setFloat(float value, u32 index) {
    WORK_OF(index)->setFloatWork(value, WORK_VAR(index));
}

// Sets the variable to a random value in [minValue, maxValue]; does nothing for an empty range.
void soWorkManageModuleImpl::rndFloat(float minValue, float maxValue, u32 index) {
    if (maxValue < minValue)
        return;
    float value;
    if (maxValue == minValue)
        value = maxValue;
    else
        value = minValue + (1.17549435e-38f + (maxValue - minValue)) * randf();
    setFloat(value, index);
}

void soWorkManageModuleImpl::addFloat(float addValue, u32 index) {
    WORK_OF(index)->addFloatWork(addValue, WORK_VAR(index));
}

void soWorkManageModuleImpl::subFloat(float subtractValue, u32 index) {
    WORK_OF(index)->subFloatWork(subtractValue, WORK_VAR(index));
}

bool soWorkManageModuleImpl::isFlag(u32 index) {
    return WORK_OF(index)->isFlag(1 << (index & 0x1F), (int)(index & 0xFF) / 32);
}

void soWorkManageModuleImpl::onFlag(u32 index) {
    WORK_OF(index)->onFlag(1 << (index & 0x1F), (int)(index & 0xFF) / 32);
}

void soWorkManageModuleImpl::offFlag(u32 index) {
    WORK_OF(index)->offFlag(1 << (index & 0x1F), (int)(index & 0xFF) / 32);
}

u32 soWorkManageModuleImpl::turnOffFlag(u32 index) {
    return WORK_OF(index)->turnOffFlag(1 << (index & 0x1F), (int)(index & 0xFF) / 32);
}

void soWorkManageModuleImpl::clearAll(u32 index) {
    m_generalWorks[index]->clearWorkAll();
}

// The module listens for anim cmd type 0x12 (work variable commands).
bool soWorkManageModuleImpl::isObserv(char kind) {
    return kind == 0x12;
}

void* soWorkManageModuleImpl::getParamAccesser() {
    return m_paramAccesser;
}

void soWorkManageModuleImpl::setFlag(bool on, u32 index) {
    if (on == true)
        onFlag(index);
    else
        offFlag(index);
}

#include <so/anim/so_anim_cmd_event_presenter.h>
// TODO: clean up, give variables real names
namespace jj_A1DA4 {


extern "C" u8 lbl_27_data_AC28[];
extern "C" u8 lbl_27_data_1550C[];
extern "C" u8 __vt__22soAnimCmdEventObserver[];

struct Module {
    u8 pad[0x80];
    void* object;
};

struct Owner {
    u8 pad[0xD8];
    Module* module;
};

struct Getter {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f5();
    virtual void f6();
    virtual s16 target(Owner*, s32, void*);
};

struct Self {
    void* vtable;
    u8 flag;
    u8 pad05[3];
    s32 f8;
    s32 fC;
    soAnimCmdEventObserver observer;
    s32 f1C;
    s32 f20;
    s32 f24;
    s32 field_28;
    s32 f2C;
};

extern "C" Self* __ct__22soWorkManageModuleImplFP16soModuleAccesserPv(Self* self, Owner* owner, s32 arg2) {
    self->flag = 0;
    self->f8 = 0;
    self->fC = 0;
    self->vtable = &lbl_27_data_AC28;

    s16 id = ((Getter*)((u8*)owner->module->object + 8))->target(owner, arg2, &lbl_27_data_AC28);

    self->observer.m_manageID = -1;
    self->observer.m_unitID = 5;
    self->observer.m_sendID = -1;
    *(void**)&self->observer = __vt__22soAnimCmdEventObserver;
    self->observer.addObserver((short)id, -1);

    self->field_28 = arg2;
    self->vtable = &lbl_27_data_1550C;
    *(void**)&self->observer = lbl_27_data_1550C + 0x68;
    self->f2C = 0;
    self->f1C = 0;
    self->f20 = 0;
    self->f24 = 0;
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_A1E80 {
class soAnimCmdEventObserver;

template <class T>
struct soEventObserver {
    static void __vt(int, int);
};

extern "C" void* getInstance__13soEventSystemFv();
extern "C" void* getManager__13soEventSystemFl(void*, long);
extern "C" void __dl__FPv(void*);

struct EvSys {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual u32 v6(s16 id);
};

struct EvMgr {
    virtual void a0();
    virtual void a1();
    virtual void a2();
    virtual void a3(s16 x, s16 y);
};

struct Obj {
    char pad0[0x10];
    void* vtbl; // 0x10
    s16 m4;     // 0x14
    s16 m6;     // 0x16
    s16 m8;     // 0x18
};

extern "C" Obj* __dt__22soWorkManageModuleImplFv(Obj* p, s32 flag) {
    if (p) {
        s32 t = (s32)p + 0x10;
        if ((t != 0) && (t != 0)) {
            s16 id = p->m4;
            p->vtbl = (void*)&soEventObserver<soAnimCmdEventObserver>::__vt;
            if (((EvSys*)getInstance__13soEventSystemFv())->v6(id) == 1) {
                EvMgr* mgr = (EvMgr*)getManager__13soEventSystemFl(getInstance__13soEventSystemFv(), p->m4);
                mgr->a3(p->m6, p->m8);
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
namespace jj_A34C4 {
extern "C" void fn_27_A23B4(void*);

extern "C" void fn_27_A34C4(void* self) {
    fn_27_A23B4(reinterpret_cast<u8*>(self) - 0x10);
}
}

// TODO: clean up, give variables real names
namespace jj_A34CC {
extern "C" void isObserv__22soWorkManageModuleImplFc(void*);

extern "C" void fn_27_A34CC(void* self) {
    isObserv__22soWorkManageModuleImplFc(reinterpret_cast<u8*>(self) - 0x10);
}
}

