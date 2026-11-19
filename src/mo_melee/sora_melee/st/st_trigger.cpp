#include <st/st_trigger.h>
#include <types.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

    void fn_27_235514(stObsTriggerSquareWindCB* windCB, grGimmickWindData2nd* wnd, s32 p2);


    extern char lbl_27_data_4BCF8[];
    void* __dynamic_cast(void* ptr, long vtblOffset, const void* target, const void* source, int isRef);

} // extern "C"

stTrigger::~stTrigger() {
    if (m_obsTriggerCB != nullptr) {
        delete m_obsTriggerCB;
    }
    if (unk20 != nullptr) {
        delete unk20;
    }
    if (unk24 != nullptr) {
        delete unk24;
    }
    if (unk28 != nullptr) {
        delete unk28;
    }
    m_obsTriggerCB = nullptr;
    unk20 = nullptr;
    unk24 = nullptr;
    unk28 = nullptr;
}

void stTrigger::setWindParam(grGimmickWindData2nd* wnd, s32 p2) {
    if (dynamic_cast<stObsTriggerSquareWindCB*>(m_obsTriggerCB) != nullptr) {
        fn_27_235514(dynamic_cast<stObsTriggerSquareWindCB*>(m_obsTriggerCB), wnd, p2);
    }
}


void stTrigger::setValidCallback(gfCallBack* cb) {
    // HYPOTHESIS: slot 0x20 is a gfCallBack list head; the header names it m_yakumono, but setObserveYakumono stores at 0x1C
    gfCallBack* last = *(gfCallBack**)((u8*)this + 0x20);
    if (last != NULL) {
        while (*(gfCallBack**)last != NULL) {
            last = *(gfCallBack**)last;
        }
        *(gfCallBack**)last = cb;
        return;
    }
    *(gfCallBack**)((u8*)this + 0x20) = (gfCallBack*)__dynamic_cast(cb, 4, lbl_27_data_4BCF8, &__RTTI__10gfCallBack, 0);
}

void stTrigger::setKeyAreaTrigger(stGimmickAreaData* areaData, Vec2f pos) {
    stObsTriggerKeyArea* cb = new (Heaps::StageInstance) stObsTriggerKeyArea(&m_isValid, true);
    cb->createArea(areaData, pos);
    if (m_obsTriggerCB != nullptr) {
        delete m_obsTriggerCB;
    }
    m_obsTriggerCB = cb;
}

void stTrigger::setTruckAreaTrigger(stSimpleAreaData* areaData) {
    stObsTriggerTruckArea* cb = new (Heaps::StageInstance) stObsTriggerTruckArea(&m_isValid, true);
    stGimmickAreaData gimmickAreaData;
    cb->setDefaultGimmickAreaData(areaData, &gimmickAreaData);
    gimmickAreaData.m_group = gfArea::Stage_Group_Space_Normal;
    gimmickAreaData.m_checkTargetGroup = 1 << gfArea::Stage_Group_Gimmick_Normal;
    cb->createArea(&gimmickAreaData, Vec2f(0.0f, 0.0f));
    if (m_obsTriggerCB != nullptr) {
        delete m_obsTriggerCB;
    }
    m_obsTriggerCB = cb;
}

void stTrigger::setFrameTrigger(float frame) {
    stObsTriggerFrameCB* cb = new (Heaps::StageInstance) stObsTriggerFrameCB(&m_isValid, frame);
    if (m_obsTriggerCB != nullptr) {
        delete m_obsTriggerCB;
    }
    m_obsTriggerCB = cb;
}

void stTrigger::setValidEdgeCallback(gfCallBack* cb) {
    gfCallBack* last = unk24;
    if (last != nullptr) {
        while (last->m_next != nullptr) {
            last = last->m_next;
        }
        last->m_next = cb;
    } else {
        unk24 = dynamic_cast<stTriggerCB*>(cb);
    }
}

void stTrigger::setUnValidEdgeCallback(gfCallBack* cb) {
    gfCallBack* last = unk28;
    if (last != nullptr) {
        while (last->m_next != nullptr) {
            last = last->m_next;
        }
        last->m_next = cb;
    } else {
        unk28 = dynamic_cast<stTriggerCB*>(cb);
    }
}

void stTrigger::setBeltConveyorParam(grGimmickBeltConveyorData* data) {
    if (dynamic_cast<stObsTriggerSquareBeltConveyorCB*>(m_obsTriggerCB) != nullptr) {
        dynamic_cast<stObsTriggerSquareBeltConveyorCB*>(m_obsTriggerCB)->setParam(data);
    }
}

void stTrigger::setBeltConveyorParam(stEarthBeltData* data) {
    if (dynamic_cast<stObsTriggerTriBeltConveyorCB*>(m_obsTriggerCB) != nullptr) {
        dynamic_cast<stObsTriggerTriBeltConveyorCB*>(m_obsTriggerCB)->setParam(data);
    }
}

void stTrigger::setPressureParam(grGimmickPressureAreaData* data) {
    if (dynamic_cast<stObsTriggerSquareBeltConveyorCB*>(m_obsTriggerCB) != nullptr) {
        dynamic_cast<stObsTriggerSquareBeltConveyorCB*>(m_obsTriggerCB)->setParam(data);
    }
}

void stTrigger::setWaterParam(grGimmickWaterData* data) {
    if (dynamic_cast<stObsTriggerSquareWaterCB*>(m_obsTriggerCB) != nullptr) {
        dynamic_cast<stObsTriggerSquareWaterCB*>(m_obsTriggerCB)->setParam(data);
    }
}

stObsTriggerTriBeltConveyorCB* stTrigger::setBeltConveyorTrigger(stEarthBeltData* data) {
    stObsTriggerTriBeltConveyorCB* cb = new (Heaps::StageInstance) stObsTriggerTriBeltConveyorCB(&m_isValid, data);
    if (m_obsTriggerCB != nullptr) {
        delete m_obsTriggerCB;
    }
    m_obsTriggerCB = cb;
    return cb;
}

stObsTriggerSquareBeltConveyorCB* stTrigger::setPressureAreaTrigger(grGimmickPressureAreaData* data) {
    stObsTriggerSquareBeltConveyorCB* cb = new (Heaps::StageInstance) stObsTriggerSquareBeltConveyorCB(&m_isValid, data);
    if (m_obsTriggerCB != nullptr) {
        delete m_obsTriggerCB;
    }
    m_obsTriggerCB = cb;
    return cb;
}

void stTrigger::process() {
    if (m_isValid == true) {
        if (unk14 == 0 || unk14 == 3) {
            unk14 = 1;
        } else {
            unk14 = 2;
        }
    } else {
        if (unk14 == 1 || unk14 == 2) {
            unk14 = 3;
        } else {
            unk14 = 0;
        }
    }
    switch (unk14) {
    case 1:
        if (unk24 != nullptr) {
            gfCallBack* callback = unk24;
            callback->userProc();
            for (callback = callback->m_next; callback != nullptr; callback = callback->m_next) {
                callback->userProc();
            }
        }
        if (unk20 != nullptr) {
            gfCallBack* callback = unk20;
            callback->userProc();
            for (callback = callback->m_next; callback != nullptr; callback = callback->m_next) {
                callback->userProc();
            }
        }
        break;
    case 2:
        if (unk20 != nullptr) {
            gfCallBack* callback = unk20;
            callback->userProc();
            for (callback = callback->m_next; callback != nullptr; callback = callback->m_next) {
                callback->userProc();
            }
        }
        break;
    case 3:
        if (unk28 != nullptr) {
            gfCallBack* callback = unk28;
            callback->userProc();
            for (callback = callback->m_next; callback != nullptr; callback = callback->m_next) {
                callback->userProc();
            }
        }
        break;
    }
}

// TODO: clean up, give variables real names
namespace jj_233640 {
extern "C" void setObserveYakumono__9stTriggerFP8Yakumono(void* self, u32 value) {
    *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x1c) = value;
}
}

// TODO: clean up, give variables real names
namespace jj_233648 {
extern "C" s32 fn_27_233648(void) {
    return 0x0;
}
}

// TODO: clean up, give variables real names
namespace jj_233650 {
extern "C" s32 fn_27_233650(void) {
    return 0x0;
}
}

// TODO: clean up, give variables real names
namespace jj_233658 {
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void* fn_27_2350AC(void* obj, void* p, void* arg);

struct grGimmickWindData;

struct TriggerAction {
    int value;
    virtual void unused();
    virtual void fn(int del);
};

struct stTrigger {
    char pad0[0x18];
    TriggerAction* action;
    char pad1[0x10];
};

extern "C" void* setWindTrigger__9stTriggerFP17grGimmickWindData(
    stTrigger* self, grGimmickWindData* data)
{
    void* obj = __nw__FUlQ25Heaps8HeapType(0x40, 0xF);

    if (obj)
        obj = fn_27_2350AC(obj, (char*)self + 0x2C, data);

    if (self->action && self->action)
        self->action->fn(1);

    self->action = (TriggerAction*)obj;
    return obj;
}
}

// TODO: clean up, give variables real names
namespace jj_2336E8 {
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void* __ct__24stObsTriggerSquareWindCBFPbP20grGimmickWindData2nd(void* obj, void* p, int arg);

struct Sub {
    int hdr;
    virtual void vf0();
    virtual void vf1(int del);
};

struct Owner {
    char pad[0x18];
    Sub* sub;
};

extern "C" {
void* fn_27_2336E8(Owner* self, int arg) {
    void* obj = __nw__FUlQ25Heaps8HeapType(0x40, 0xf);
    if (obj) {
        obj = __ct__24stObsTriggerSquareWindCBFPbP20grGimmickWindData2nd(obj, (char*)self + 0x2c, arg);
    }
    if (self->sub && self->sub) {
        self->sub->vf1(1);
    }
    self->sub = (Sub*)obj;
    return obj;
}
}
}

// TODO: clean up, give variables real names
namespace jj_233778 {
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);

struct grGimmickBeltConveyorData;

struct TriggerAction {
    int value;
    virtual void unused();
    virtual ~TriggerAction();
};

extern "C" TriggerAction* fn_27_2356A4(
    void*, void*, grGimmickBeltConveyorData*);

struct stTrigger {
    char pad0[0x18];
    TriggerAction* action;
    char pad1[0x10];
};

extern "C" TriggerAction*
setBeltConveyorTrigger__9stTriggerFP25grGimmickBeltConveyorData(
    stTrigger* self, grGimmickBeltConveyorData* data)
{
    TriggerAction* result =
        (TriggerAction*)__nw__FUlQ25Heaps8HeapType(0x40, 0xF);

    if (result)
        result = fn_27_2356A4(result, (char*)self + 0x2C, data);

    if (self->action)
        delete self->action;

    self->action = result;
    return result;
}
}

// TODO: clean up, give variables real names
namespace jj_233ED8 {
extern "C" void fn_27_233ED8(void) {}
}

// TODO: clean up, give variables real names
namespace jj_233EDC {
extern "C" void fn_27_233EDC(void) {}
}

// TODO: clean up, give variables real names
namespace jj_233EE0 {
extern "C" void fn_27_233EE0(void) {}
}

// TODO: clean up, give variables real names
namespace jj_2340A0 {
extern "C" void* __dynamic_cast(void*, long, const void*, const void*, short);
extern "C" char __RTTI__20stActTriggerEnemyPop[];
extern "C" char lbl_27_data_4BCF8[];
extern "C" char __RTTI__10gfCallBack[];

struct Node { Node* next; int pad; int type; };
struct Object { char pad[0x20]; Node* first; Node* second; };

extern "C" int fn_27_2340A0(Object* self) {
    Node* node = self->first;
    if (node) {
        while (node) {
            if (node->type == 9 && !*(unsigned char*)((char*)__dynamic_cast(node, 4, __RTTI__20stActTriggerEnemyPop, lbl_27_data_4BCF8, 0) + 0xb1))
                return 0;
            node = (Node*)__dynamic_cast(node->next, 4, lbl_27_data_4BCF8, __RTTI__10gfCallBack, 0);
        }
    }
    node = self->second;
    if (node) {
        while (node) {
            if (node->type == 9 && !*(unsigned char*)((char*)__dynamic_cast(node, 4, __RTTI__20stActTriggerEnemyPop, lbl_27_data_4BCF8, 0) + 0xb1))
                return 0;
            node = (Node*)__dynamic_cast(node->next, 4, lbl_27_data_4BCF8, __RTTI__10gfCallBack, 0);
        }
    }
    return 1;
}
}

// TODO: clean up, give variables real names
namespace jj_2341C4 {
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void* __ct__31stObsTriggerPlayerIncludeAreaCBFPbP6Rect2Db(void*, void*, float*, unsigned int);

struct Object_vptr_4_base { char pad[0x4]; };
class Object_vptr_4 : public Object_vptr_4_base {
public:
    virtual void slot0();
    virtual void slot1(int del);
};

struct Self {
    char pad[0x18];
    Object_vptr_4* obj;
};

struct Source {
    char pad0;
    unsigned char c1;
    char pad[2];
    float f4;
    float f8;
    float fc;
    float f10;
};

extern "C" void fn_27_2341C4(Self* self, const Source* src) {
    float d;
    float c;
    float b;
    float a;
    a = src->f4;
    b = src->fc;
    c = src->f10;
    d = src->f8;
    float arr[4];
    arr[0] = a;
    arr[1] = b;
    arr[2] = c;
    arr[3] = d;
    Object_vptr_4* obj = (Object_vptr_4*)__nw__FUlQ25Heaps8HeapType(0x28, 0xf);
    if (obj) {
        obj = (Object_vptr_4*)__ct__31stObsTriggerPlayerIncludeAreaCBFPbP6Rect2Db(obj, (char*)self + 0x2c, arr, src->c1 != 0);
    }
    if (self->obj && self->obj) {
        self->obj->slot1(1);
    }
    self->obj = obj;
}
}

// TODO: clean up, give variables real names
namespace jj_234414 {
struct Object_vptr_4_base {
    char pad[4];
};

class Object_vptr_4 : public Object_vptr_4_base {
public:
    virtual void slot0();
    virtual ~Object_vptr_4();
};

struct Vec2 {
    float x, y;
};

struct Params {
    u8 type, flag;
    u16 pad;
    float a, b, c, d;
};

struct Rect {
    float a, b, c, d;
};

struct Owner {
    char pad[0x18];
    Object_vptr_4* child;
    char pad2[0x10];
    char sub[1];
};

extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" Object_vptr_4* __ct__31stObsTriggerPlayerIncludeAreaCBFPbP6Rect2Db(void*, void*, Rect*, bool);

extern "C" void fn_27_234414(Owner* self, const Vec2* a, const Vec2* b, u8 flag) {
    float by;
    float bx;
    float ay;
    float ax;
    ax = a->x;
    ay = a->y;
    bx = b->x;
    by = b->y;
    Params p;
    p.type = 1;
    p.flag = flag;
    p.pad = 0;
    p.a = ax;
    p.b = ay;
    p.c = bx;
    p.d = by;
    Rect r = { ax, bx, by, ay };
    Object_vptr_4* result =
        (Object_vptr_4*)__nw__FUlQ25Heaps8HeapType(0x28, 0xf);
    if (result) {
        result = __ct__31stObsTriggerPlayerIncludeAreaCBFPbP6Rect2Db(result, self->sub, &r, p.flag != 0);
    }
    if (self->child) {
        delete self->child;
    }
    self->child = result;
}
}

// TODO: clean up, give variables real names
namespace jj_234660 {
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);

struct Object {
    unsigned int data;
    virtual void unused();
    virtual ~Object();
};

extern "C" Object* fn_40_DBE8(Object*, void*, unsigned int);

struct Owner {
    char pad[0x18];
    Object* object;
    char pad1[0x10];
};

extern "C" void fn_27_234660(Owner* self, unsigned int value) {
    Object* object = (Object*)__nw__FUlQ25Heaps8HeapType(0x14, 0xf);
    if (object)
        object = fn_40_DBE8(object, (char*)self + 0x2c, value);

    if (self->object)
        delete self->object;

    self->object = object;
}
}

// TODO: clean up, give variables real names
namespace jj_2346EC {
extern "C" {
void setObserveFlag__9stTriggerFb(u8* self, u8 val) {
    if (self[0x2f] == 0) {
        self[0x2c] = val;
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_234700 {
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void __ct__14stObsTriggerCBFPbb(void*, void*, unsigned char);
extern "C" bool fn_27_234B2C(void*, void*);
extern "C" char lbl_27_data_4C410[];

struct Trigger {
    unsigned short id;
    unsigned char pad;
    unsigned char kind;
};

struct Data {
    unsigned int head;
    Trigger entries[5];
    unsigned char flag0;
    unsigned char flag1;
};

struct Object {
    unsigned int head;
    void* vtable;
    unsigned int field8;
    unsigned int fieldC;
    void* entries[5];
    unsigned int count;
    bool flag0;
    bool flag1;
};

struct Holder {
    char pad[0x18];
    Object* object;
    char pad1C[0x10];
    char context[1];
};

class Dispatch {
public:
    unsigned int head;
    virtual void v0();
    virtual void destroy(int);
};

inline void init(Object* obj, void* context, bool a, bool b) {
    __ct__14stObsTriggerCBFPbb(obj, context, 0);
    obj->vtable = lbl_27_data_4C410;
    obj->count = 0;
    obj->flag0 = a;
    obj->flag1 = b;
    obj->entries[0] = 0;
    obj->entries[1] = 0;
    obj->entries[2] = 0;
    obj->entries[3] = 0;
    obj->entries[4] = 0;
}

extern "C" void fn_27_234700(Holder* self, Data* data) {
    Object* obj = (Object*)__nw__FUlQ25Heaps8HeapType(0x2C, 15);
    if (obj) {
        init(obj, self->context, data->flag0, data->flag1);
    }

    for (int i = 0; i < 5; ++i) {
        if (data->entries[i].id) {
            fn_27_234B2C(obj, &data->entries[i]);
        }
    }

    if (self->object) {
        if (self->object) {
            ((Dispatch*)self->object)->destroy(1);
        }
    }
    self->object = obj;
}
}

