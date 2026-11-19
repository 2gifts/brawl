#include <so/damage/so_damage_module_actor.h>
#include <so/so_module_accesser.h>
#include <types.h>

soDamageModuleActor::~soDamageModuleActor() { }

void soDamageModuleActor::activate(float damage) {
    soDamageModuleImpl::activate(damage);
    clearDamageLog();
}

void soDamageModuleActor::toDamageFlyRollStatus(float angle, float speed) {
    clearDamageLog();
    getTransactor()->setupDamageFlyRollStatus(angle, speed, m_moduleAccesser, &m_damageLog);
}

bool soDamageModuleActor::isCheckGroundDamage() {
    return getTransactor()->isCheckGroundDamage(m_moduleAccesser);
}

void soDamageModuleActor::clearDamageLog() {
    m_damageLog.m_reaction = 0.0f;
    m_damageLog.m_level = soDamage::Level_FlyRoll;
    m_damageLog.m_height = -1;
    m_damageLog.m_speed = Vec2f(0.0f, 0.0f);
    m_damageLog.m_angle = 0.0f;
    m_damageLog.m_lr = 1.0f;
    m_damageLog.m_frame = 0.0f;
    m_damageLog.m_hitStopFrame = 0;
    m_damageLog.m_attribute = (soCollisionAttackData::Attribute)0;
    m_damageLog.m_damageAdd = 0.0f;
    m_damageLog.m_attackerTaskId = -1;
    m_damageLog.m_attackerTeamNo = -1;
    m_damageLog.m_hitStopDelay = 1.0f;
    m_damageLog.m_isDamageAir = false;
    m_damageLog.m_unk26 = false;
    m_damageLog.m_isCollisionAbsolute = false;
    m_damageLog.m_isMeteor = false;
    m_damageLog.m_isAttackDirect = false;
    m_damageLog.m_isVector365 = false;
}

soDamageLog* soDamageModuleActor::getDamageLog() {
    return &m_damageLog;
}

#include <so/so_value_accesser.h>
// TODO: clean up, give variables real names
namespace jj_615E0 {
struct Receiver {
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
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
    virtual void invoke(unsigned int, unsigned int, unsigned int);
};

struct Base {
    unsigned char padding[8];
};

struct Object : Base {
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
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
    virtual Receiver* getReceiver();

    unsigned char paddingC[0x30];
    unsigned int value;
};

extern "C" void fn_27_615E0(Object* self, unsigned int arg1, unsigned int arg2) {
    Receiver* receiver = self->getReceiver();
    receiver->invoke(self->value, arg1, arg2);
}
}

// TODO: clean up, give variables real names
namespace jj_6164C {
extern "C" s32 preProcessCheckDamage__18soDamageTransactorFP16soModuleAccesserP8soDamage(void) {
    return 0x0;
}
}

// TODO: clean up, give variables real names
namespace jj_61654 {
class Obj1 {
public:
    virtual void slot0();
    virtual void* slot1();
};

class Obj3 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
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
    virtual void slot20(void*, void*, void*);
};

struct Head {
    char pad[8];
};

class Self : public Head {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
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
    virtual Obj3* slot39();

    char pad[0x3C - 0xC];
    void* unk3C;         // 0x3C
    Obj1* obj1;          // 0x40
    char pad2[0xBC - 0x44];
    char unkBC[0x101 - 0xBC];
    u8 flags;            // 0x101
};

extern "C" void fn_27_61654(Self* self) {
    void* temp = self->obj1->slot1();
    self->flags = self->flags & ~0x20;
    Obj3* result = self->slot39();
    result->slot20(self->unk3C, temp, (void*)self->unkBC);
}
}

// TODO: clean up, give variables real names
namespace jj_616D8 {
struct Inner {
    virtual void v0();
    virtual s32 getCount();
};

struct Handler {
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
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void finish(s32, s32, void*);
    virtual void v18();
    virtual void v19();
    virtual s32 start(s32, s32, void*);
    virtual void v21();
    virtual void v22();
};

struct Base {
    u8 pad[8];
};

struct Main : Base {
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
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v29();
    virtual void v30();
    virtual void v31();
    virtual void v32();
    virtual void v33();
    virtual void v34();
    virtual void v35();
    virtual void v36();
    virtual void v37();
    virtual void v38();
    virtual Handler* getHandler();

    u8 pad0[0x3C - 0xC];
    s32 field3C;
    Inner* inner;
    u8 pad44[0xBC - 0x44];
    u8 sub[0x101 - 0xBC];
    u8 flags;
};

extern "C" s32 fn_27_616D8(Main* self) {
    s32 count = self->inner->getCount();
    self->flags |= 0x20;
    s32 result = self->getHandler()->start(self->field3C, count, self->sub);
    self->getHandler()->finish(self->field3C, count, self->sub);
    return result;
}
}

// TODO: clean up, give variables real names
namespace jj_61860 {
struct Target {
    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual void unused3();
    virtual void unused4();
    virtual void unused5();
    virtual void unused6();
    virtual void unused7();
    virtual void unused8();
    virtual void unused9();
    virtual void unused10();
    virtual void unused11();
    virtual void unused12();
    virtual void unused13();
    virtual void unused14();
    virtual void unused15();
    virtual void unused16();
    virtual void unused17();
    virtual void unused18();
    virtual void unused19();
    virtual void unused20();
    virtual void unused21();
    virtual void unused22();
    virtual void unused23();
    virtual void unused24();
    virtual void unused25();
    virtual void invoke(unsigned int, unsigned int, unsigned int, unsigned int);
};

struct Base {
    char padding[8];
};

struct Object : Base {
    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual void unused3();
    virtual void unused4();
    virtual void unused5();
    virtual void unused6();
    virtual void unused7();
    virtual void unused8();
    virtual void unused9();
    virtual void unused10();
    virtual void unused11();
    virtual void unused12();
    virtual void unused13();
    virtual void unused14();
    virtual void unused15();
    virtual void unused16();
    virtual void unused17();
    virtual void unused18();
    virtual void unused19();
    virtual void unused20();
    virtual void unused21();
    virtual void unused22();
    virtual void unused23();
    virtual void unused24();
    virtual void unused25();
    virtual void unused26();
    virtual void unused27();
    virtual void unused28();
    virtual void unused29();
    virtual void unused30();
    virtual void unused31();
    virtual void unused32();
    virtual void unused33();
    virtual void unused34();
    virtual void unused35();
    virtual void unused36();
    virtual void unused37();
    virtual void unused38();
    virtual Target* getTarget();

    char paddingC[0x30];
    unsigned int value;
};

extern "C" void fn_27_61860(Object* object, unsigned int a,
                            unsigned int b, unsigned int c) {
    object->getTarget()->invoke(object->value, a, b, c);
}
}

// TODO: clean up, give variables real names
namespace jj_619B0 {
struct Module {
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
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
    virtual void invoke(void*, int);
};

struct ObjectBase {
    char padding[8];

    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
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
    virtual Module* getModule();
};

struct Object : ObjectBase {
    char padding[0x30];
    void* value;
};

extern "C" void fn_27_619B0(Object* self, int argument) {
    Module* module = self->getModule();
    module->invoke(self->value, argument);
}
}

// TODO: clean up, give variables real names
namespace jj_61A0C {
extern "C" {
extern const float lbl_27_rodata_8B4;
float getWeightReactionMul__18soDamageTransactorFP16soModuleAccesser() {
    return lbl_27_rodata_8B4;
}
}
}

// TODO: clean up, give variables real names
namespace jj_61A18 {
extern const float lbl_27_rodata_8B4;

struct Interface {
    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual void unused3();
    virtual void unused4();
    virtual void unused5();
    virtual void unused6();
    virtual void unused7();
    virtual void unused8();
    virtual void unused9();
    virtual void unused10();
    virtual void unused11();
    virtual void unused12();
    virtual void unused13();
    virtual void unused14();
    virtual void unused15();
    virtual void unused16();
    virtual void unused17();
    virtual void unused18();
    virtual void unused19();
    virtual void unused20();
    virtual void unused21();
    virtual void unused22();
    virtual void unused23();
    virtual void unused24();
    virtual void unused25();
    virtual void unused26();
    virtual void unused27();
    virtual void unused28();
    virtual float invoke(int, int, int, int);
};

struct ObjectPrefix {
    char unused[8];
};

struct Object : ObjectPrefix {
    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual void unused3();
    virtual void unused4();
    virtual void unused5();
    virtual void unused6();
    virtual void unused7();
    virtual void unused8();
    virtual void unused9();
    virtual void unused10();
    virtual void unused11();
    virtual void unused12();
    virtual void unused13();
    virtual void unused14();
    virtual void unused15();
    virtual void unused16();
    virtual void unused17();
    virtual void unused18();
    virtual void unused19();
    virtual void unused20();
    virtual void unused21();
    virtual void unused22();
    virtual void unused23();
    virtual void unused24();
    virtual void unused25();
    virtual void unused26();
    virtual void unused27();
    virtual void unused28();
    virtual void unused29();
    virtual void unused30();
    virtual void unused31();
    virtual void unused32();
    virtual void unused33();
    virtual void unused34();
    virtual void unused35();
    virtual void unused36();
    virtual void unused37();
    virtual void unused38();
    virtual Interface* getInterface();

    char padding[0x30];
    int value;
};

extern "C" float fn_27_61A18(Object* self, int a, int b, int c) {
    float result = lbl_27_rodata_8B4;
    Interface* interface = self->getInterface();
    result *= interface->invoke(self->value, a, b, c);
    return result;
}
}

// TODO: clean up, give variables real names
namespace jj_61AB4 {
extern const float lbl_27_rodata_8B0;

struct Elem {
    char pad[0xC];
    float m_f;
};

class Arr {
public:
    virtual void f0();
    virtual Elem* at(int);
    virtual void f2();
    virtual int size();
};

struct Base {
    char pad[8];
};

class Owner : public Base {
public:
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f5();
    virtual void f6();
    virtual void f7();
    virtual void f8();
    virtual void f9();
    virtual void f10();
    virtual void f11();
    virtual void f12();
    virtual void f13();
    virtual void f14();
    virtual void f15();
    virtual void f16();
    virtual void f17();
    virtual void f18();
    virtual void f19();
    virtual void f20();
    virtual int check(Elem*);

    char pad2[0x34];
    Arr* arr;
};

extern "C" int fn_27_61AB4(Owner* self) {
    for (int i = 0; i < self->arr->size(); i++) {
        Elem* e = self->arr->at(i);
        if (e->m_f >= lbl_27_rodata_8B0) {
            if (self->check(e) == 0)
                return 1;
        }
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_61B78 {
typedef float f32;
typedef int s32;
extern const float lbl_27_rodata_8B0;

struct Item {
    char pad0[4];
    float w;
    char pad1[0x14];
    unsigned int id;
};

struct Sub {
    virtual void v0();
    virtual Item* get(int i);
    virtual void v2();
    virtual int size();
};

struct Ret {
    char pad[0x28];
    unsigned int val;
};

struct Q {
    virtual void q0();
    virtual void q1();
    virtual void q2();
    virtual void q3();
    virtual void q4();
    virtual void q5();
    virtual void q6();
    virtual void q7();
    virtual void q8();
    virtual void q9();
    virtual void q10();
    virtual Ret* f13(int x);
};

struct Z {
    char pad[0x54];
    Q* q;
};

struct X {
    char pad[0xD8];
    Z* z;
};

struct Outer {
    char pad0[0x3C];
    X* x;
    Sub* sub;
};

extern "C" f32 fn_27_61B78(Outer* self) {
    f32 sum = lbl_27_rodata_8B0;
    for (int i = 0; i < self->sub->size(); i++) {
        Item* it = self->sub->get(i);
        Ret* r = self->x->z->q->f13(0);
        if (it->id != r->val) {
            sum += it->w;
        }
    }
    return sum;
}
}

// TODO: clean up, give variables real names
namespace jj_61C48 {

extern const float lbl_27_rodata_8B0;

struct Item {
    char pad0[0x4];
    float f4;
    char pad8[0x4];
    float fc;
    char pad10[0xC];
    u32 u1c;
    char pad20[0x4C];
    u32 u6c;
    char pad70[0x2C];
    u8 u9c;
};

class ItemList {
public:
    virtual void v0();
    virtual Item* get(int index);
    virtual void v2();
    virtual int size();
};

struct Ret {
    char pad[0x28];
    u32 val;
};

class ObjC {
public:
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
    virtual Ret* f13(int x);
};

struct ObjB {
    char pad[0x54];
    ObjC* c;
};

struct ObjA {
    char pad[0xD8];
    ObjB* b;
};

struct Outer {
    char pad0[0x3C];
    ObjA* a;
    ItemList* list;
};

extern "C" int fn_27_61C48(Outer* self) {
    for (int i = 0; i < self->list->size(); i++) {
        Item* it = self->list->get(i);
        if (it->fc >= lbl_27_rodata_8B0) {
            if ((it->u6c & 0x1F) == 7) {
                return 1;
            }
            if (it->u9c == 1) {
                return 1;
            }
            Ret* r = self->a->b->c->f13(0);
            if (it->u1c != r->val) {
                if (it->f4 >= soValueAccesser::getConstantFloat((soModuleAccesser*)self->a, 0x7F1, 0)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_61D74 {

extern const float lbl_27_rodata_8B0;

struct Item {
    char pad0[0x4];
    float f4;
    char pad8[0x4];
    float fc;
    char pad10[0xC];
    u32 u1c;
    char pad20[0x4C];
    u32 u6c;
    char pad70[0x4];
    u32 pad74high : 22;
    u32 flag74 : 1;
    u32 pad74low : 9;
    char pad78[0x24];
    u8 u9c;
};

class ItemList {
public:
    virtual void isNull();
    virtual Item* __dt__19soDamageModuleActorFv(int index);
    virtual void activate__19soDamageModuleActorFf();
    virtual int deactivate__18soDamageModuleImplFv();
    virtual void sleep__18soDamageModuleImplFb();
    virtual void update__18soDamageModuleImplFv();
    virtual void initDamage__18soDamageModuleImplFf();
    virtual void initInfo__18soDamageModuleImplFv();
    virtual void setInfo__18soDamageModuleImplFv();
    virtual void storeDamage__18soDamageModuleImplFP8soDamagef();
    virtual void check__18soDamageModuleImplFb();
    virtual void getDamageLr__18soDamageModuleImplFffPvPvii();
};

struct Ret {
    char pad[0x28];
    u32 val;
};

class Class_1250 {
public:
    virtual void __dt__18soMotionModuleImplFv();
    virtual void fn_27_14068();
    virtual void fn_27_140C0();
    virtual void fn_27_1412C();
    virtual void fn_27_14134();
    virtual void fn_27_14370();
    virtual void fn_27_14A28();
    virtual void fn_27_14A30();
    virtual void fn_27_14A38();
    virtual void fn_27_14A48();
    virtual void fn_27_14A68();
    virtual Ret* fn_27_14AD8(int);
};

struct ObjB {
    char pad[0x54];
    Class_1250* c;
};

struct ObjA {
    char pad[0xD8];
    ObjB* b;
};

struct Outer {
    char pad0[0x3C];
    ObjA* a;
    ItemList* list;
};

extern "C" int fn_27_61D74(Outer* self, u8* arg1) {
    *arg1 = 0;
    for (int i = 0; i < self->list->deactivate__18soDamageModuleImplFv(); i++) {
        Item* it = self->list->__dt__19soDamageModuleActorFv(i);
        if (it->fc >= lbl_27_rodata_8B0) {
            if ((it->u6c & 0x1F) == 7) {
                return 1;
            }
            if (it->u9c == 1) {
                *arg1 = 1;
                return 1;
            }
            Ret* r = self->a->b->c->fn_27_14AD8(0);
            if (it->u1c != r->val) {
                if (it->f4 >= soValueAccesser::getConstantFloat((soModuleAccesser*)self->a, 0x7F1, 0)) {
                    return 1;
                }
            } else if (!it->flag74) {
                *arg1 = 1;
                return 1;
            }
        }
    }
    return 0;
}
}

