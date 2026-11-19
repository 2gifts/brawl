#include <so/ground/so_ground_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/slope/so_slope_module_impl.h>
#include <so/so_module_accesser.h>

// Slope module: tilts the fighter's top (and left/right/part) joints to the angle of the ground it stands on.

// Resets the module: no slope status, no queued change, flat ground.
void soSlopeModuleImpl::activate() {
    setStatus(0, 0, false);
    changeStatus();
    m_slopeAngle = 0.0f;
    m_topAngle[0] = m_topAngle[1] = 0;
    m_leftAngle[0] = m_leftAngle[1] = 0;
    m_rightAngle[0] = m_rightAngle[1] = 0;
    m_partAngle = 0;
    m_0x6C = 0xFFFF;
    setTopAngleUnlimit(false);
    m_statusParamArrayVector.clear();
}

// Queues a status change; it is applied by changeStatus(). Statuses the module was told to ignore are dropped.
void soSlopeModuleImpl::setStatus(u32 statusMask, int angle, bool isPart) {
    soSlopeStatusParam param;
    param.m_status.m_mask = statusMask & ~m_invalidStatus.m_mask;
    param.m_0x4 = angle;
    param.m_0x8 = isPart;
    if (m_statusParamArrayVector.isEmpty() == false)
        m_statusParamArrayVector.clear();
    m_statusParamArrayVector.push(param);
}

// Applies the queued status: parts that become active start at the queued angle.
void soSlopeModuleImpl::changeStatus() {
    if (m_statusParamArrayVector.isEmpty() == false) {
        const soSlopeStatusParam& param = m_statusParamArrayVector.at(0);
        u32 prevStatus = m_status.m_mask;
        m_status.m_mask = param.m_status.m_mask;
        if (param.m_0x8 == true) {
            m_partAngle = param.m_0x4;
        } else {
            if ((m_status.m_mask & soSlopeStatusParam::STATUS_MASK_TOP) != (prevStatus & soSlopeStatusParam::STATUS_MASK_TOP))
                m_topAngle[0] = m_topAngle[1] = param.m_0x4;
            if ((m_status.m_mask & soSlopeStatusParam::STATUS_MASK_L) != (prevStatus & soSlopeStatusParam::STATUS_MASK_L))
                m_leftAngle[0] = m_leftAngle[1] = param.m_0x4;
            if ((m_status.m_mask & soSlopeStatusParam::STATUS_MASK_R) != (prevStatus & soSlopeStatusParam::STATUS_MASK_R))
                m_rightAngle[0] = m_rightAngle[1] = param.m_0x4;
            m_partAngle = 0;
        }
        m_statusParamArrayVector.clear();
    }
}

// Tilts the model's top (or part) node to the slope while the fighter stands on the ground.
void soSlopeModuleImpl::updateModelTopAngle() {
    if (m_moduleAccesser->getSituationModule().getKind() != 0)
        return;
    if (m_moduleAccesser->getGroundModule().isTouch(8, 0) != true)
        return;
    if (m_slopeAngle == 0.0f)
        return;
    int node;
    if (m_status.m_mask & soSlopeStatusParam::STATUS_MASK_PART)
        node = m_partNode;
    else
        node = 0;
    Vec3f rotate = m_moduleAccesser->getModelModule().getNodeRotate(node);
    // HYPOTHESIS: byte 0x7c selects tilting around X (scaled by the model's facing) instead of around Z.
    if (m_0x7C == 0)
        rotate.m_x = m_slopeAngle * m_moduleAccesser->getPostureModule().getRotYLr() / 57.29578f;
    else
        rotate.m_z = -m_slopeAngle;
    m_moduleAccesser->getModelModule().setNodeRotate(node, &rotate);
}

void soSlopeModuleImpl::setTopAngleUnlimit(bool isUnlimit) {
    m_isTopAngleUnlimit = isUnlimit;
}

bool soSlopeModuleImpl::isObserv(char kind) {
    return kind == 0x18;
}

// HYPOTHESIS: the status data's fourth word starts with flags; the third-from-last bit marks statuses that follow the
// ground slope. The SDK declares soStatusData as an empty struct, so this view stands in for the real fields.
struct soSlopeStatusDataView {
    u32 m_0;
    u32 m_4;
    u32 m_8;
    u32 : 29;
    u32 m_followsSlope : 1;
    u32 : 2;
};

// A status that does not use the slope clears the slope and the unlimited-top-angle flag.
void soSlopeModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    if (reinterpret_cast<soSlopeStatusDataView*>(statusData)->m_followsSlope == 0) {
        setStatus(0, 0, false);
        m_partAngle = 0;
        setTopAngleUnlimit(false);
    }
}

void soSlopeModuleImpl::setInvalidStatus(u32 statusMask) {
    m_invalidStatus.m_mask = statusMask;
}

void soSlopeModuleImpl::setPartNode(int partNode) {
    m_partNode = partNode;
}

soSlopeStatusParam::StatusMask soSlopeModuleImpl::getStatus() {
    return m_status.m_mask;
}

#include <so/status/so_status_event_presenter.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
// TODO: clean up, give variables real names
namespace jj_ACAC4 {
typedef unsigned char u8;
typedef short s16;
typedef signed char s8;
struct Object_vptr_4_base { char pad[4]; };
class Object_vptr_4 : public Object_vptr_4_base { public: virtual void slot0(); };
struct Object_vptr_10_base { char pad[0x10]; };
class Object_vptr_10 : public Object_vptr_10_base { public: virtual void slot0(); };
struct Getter {
 virtual void f0(); virtual void f1(); virtual void f2(); virtual void f3();
 virtual void f4(); virtual void f5(); virtual void f6(); virtual s16 get();
};
struct Module { u8 pad[0x80]; void* object; };
struct Owner { u8 pad[0xd8]; Module* module; };
extern "C" u8 lbl_27_data_A418[];
extern "C" u8 lbl_27_data_16058[];
extern "C" u8 lbl_27_data_16100[];
extern "C" void* __vt__21soStatusEventObserver[];
extern "C" void* __vt__22soAnimCmdEventObserver[];
extern "C" void fn_27_AE448(void*);
extern "C" void fn_27_ACEE4(void*);
extern "C" void __construct_array(void*, void (*)(void*), void (*)(void*), unsigned long, int);
extern "C" void* __ct__17soSlopeModuleImplFP16soModuleAccesserfiib(void* self, Owner* owner, int a2, int a3, s8 a4, float f) {
 u8* s = (u8*)self;
 *(void**)s = lbl_27_data_A418;
 s16 id = ((Getter*)((u8*)owner->module->object + 8))->get();
 soStatusEventObserver* status = (soStatusEventObserver*)(s + 4);
 status->m_manageID = -1;
 status->m_unitID = 4;
 status->m_sendID = -1;
 *(void***)status = __vt__21soStatusEventObserver;
 status->addObserver(id, -1);
 id = ((Getter*)((u8*)owner->module->object + 8))->get();
 soAnimCmdEventObserver* anim = (soAnimCmdEventObserver*)(s + 0x10);
 anim->m_manageID = -1;
 anim->m_unitID = 5;
 anim->m_sendID = -1;
 *(void***)anim = __vt__22soAnimCmdEventObserver;
 anim->addObserver(id, -1);
 *(void**)s = lbl_27_data_16058;
 *(void**)(s + 4) = lbl_27_data_16058 + 0x2c;
 *(void**)(s + 0x10) = lbl_27_data_16058 + 0x3c;
 *(Owner**)(s + 0x1c) = owner;
 *(int*)(s + 0x20) = a2;
 *(int*)(s + 0x24) = a3;
 *(int*)(s + 0x28) = 0;
 *(void**)(s + 0x34) = lbl_27_data_16100;
 *(void**)(s + 0x38) = lbl_27_data_16100 + 0x48;
 *(u32*)(s + 0x3c) &= 0x1ffffff;
 __construct_array(s + 0x40, fn_27_AE448, fn_27_ACEE4, 0xc, 1);
 *(float*)(s + 0x70) = f;
 *(int*)(s + 0x6c) = 0xffff;
 *(int*)(s + 0x74) = -1;
 *(int*)(s + 0x78) = 0;
 s[0x7c] = a4;
 return self;
}
}

// TODO: clean up, give variables real names
namespace jj_ACEE4 {
extern "C" void* fn_27_ACEE4(void* ptr, s32 size) {
    if (ptr != nullptr && size > 0) {
        operator delete(ptr);
    }
    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_ADF30 {
struct Value { int tag; int value; };

struct Ref {
    Value* p;
    u8 null;
    Ref(Value* v) : p(v), null(0) {}
    int valueOrZero() const { return null == 1 ? 0 : p->value; }
};

struct Args {
    virtual void slot0();
    virtual void slot1();
    virtual Value* at(int) const;
    virtual void slot3();
    virtual int isNull() const;
    virtual void slot5();
    virtual void advance();
    int data[3];
    Ref get(int index) const { return Ref(at(index)); }
    void shift() { advance(); }
    int size() const { return isNull(); }
};

struct Input {
    virtual void slot0();
    virtual s8 group();
    virtual s8 mode();
    virtual void slot3();
    virtual void slot4();
    virtual Args args();
    virtual void result(Ref*, int);
};

struct AnimationBase { char pad[8]; };
struct Animation : AnimationBase {
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
    virtual void slot44(); virtual void slot45(); virtual void slot46(); virtual void slot47();
    virtual void slot48(); virtual void slot49(); virtual void slot50(); virtual void slot51();
    virtual void slot52(); virtual void slot53(); virtual void slot54(); virtual void slot55();
    virtual void slot56(); virtual void slot57(); virtual void slot58(); virtual void slot59();
    virtual void slot60(); virtual void slot61();
    virtual void update(void*, float);
};

struct Modules { char pad[0x9c]; Animation* animation; };
struct ModuleOwner { char pad[0xd8]; Modules* modules; };

class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2(int, int, int);
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6(int);
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
    virtual int slot18(s8);
    char pad[0x18];
    ModuleOwner* owner;
    void* data20;
    void* data24;
};

struct Object_vptr_88_base { char pad[0x88]; };
class Object_vptr_88 : public Object_vptr_88_base {
public:
    virtual void slot0(); virtual void slot1(); virtual void slot2(); virtual void slot3();
    virtual void slot4(); virtual void slot5(); virtual void slot6();
};

extern "C" const float lbl_27_rodata_D78;
extern "C" const float lbl_27_rodata_DA0;

extern "C" int fn_27_ADF30(Object_vptr_0* self, Input* in) {
    if (!self->slot18(in->group())) {
        return 0;
    }
    if (in->mode() <= -1 || in->mode() >= 4) {
        return 0;
    }
    switch (in->mode()) {
    case 0: {
        Ref value(0);
        in->result(&value, 0);
        self->slot2(value.valueOrZero(), 0, 0);
        return 1;
    }
    case 1: {
        int flag;
        Args args = in->args();
        const Ref& first = args.get(0);
        args.shift();
        const Ref& second = args.get(0);
        args.shift();
        flag = 0;
        if (args.size() == 0) {
            flag = 1;
        }
        int firstValue = first.valueOrZero();
        self->slot2(firstValue, second.valueOrZero(), flag);
        return 1;
    }
    case 2: {
        Ref value(0);
        in->result(&value, 0);
        self->slot6(value.null == 1 ? 0 : value.p->value != 0);
        return 1;
    }
    case 3: {
        float value;
        Args args = in->args();
        const Ref& first = args.get(0);
        args.shift();
        const Ref& second = args.get(0);
        int flags = first.valueOrZero();
        value = second.null == 1
            ? lbl_27_rodata_D78
            : (float)second.p->value / lbl_27_rodata_DA0;
        if (flags & 2) {
            self->owner->modules->animation->update(self->data20, value);
        }
        if (flags & 4) {
            self->owner->modules->animation->update(self->data24, value);
        }
        return 1;
    }
    }
    return 0;
}
}

