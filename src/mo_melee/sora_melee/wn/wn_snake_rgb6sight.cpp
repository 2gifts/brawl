#include <wn/snake/wn_snake_rgb6_sight.h>
#include <wn/wn_extend_param_accesser.h>

void wnSnakeRgb6Sight::renderDebug() {
    StageObject::renderDebug();
}

template class wnExtendParamAccesser<3999, 6, 23999, 0, 0, 0>;

// TODO: clean up, give variables real names
namespace jj_3B1720 {
extern "C" void __dl__FPv(void*);

extern "C" void* __dt__29wnSnakeRgb6SightParamAccesserFv(void* ptr, int count) {
    if (ptr != 0 && count > 0)
        __dl__FPv(ptr);
    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_3B1760 {
extern "C" void* fn_27_3B1760(void* self) {
    return reinterpret_cast<u8*>(self) + 0x7f8;
}
}

// TODO: clean up, give variables real names
namespace jj_3B22C8 {
extern "C" void* fn_27_3B22C8(void* self) {
    return reinterpret_cast<u8*>(self) + 0x104;
}
}

// TODO: clean up, give variables real names
namespace jj_3B2C90 {
#pragma optimization_level 2
#pragma scheduling off
#pragma opt_propagation off

struct Owner;
class Object_vptr_0 {
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
    virtual void slot19(int, Owner*);
};
struct Middle {
    char pad[0x64];
    Object_vptr_0* target;
};
struct Owner {
    char pad[0xD8];
    Middle* middle;
};
struct Object {
    char pad[0x60];
    Owner* owner;
};
extern "C" void enableOperate__16wnSnakeRgb6SightFv(Object* self) {
    Owner* owner = self->owner;
    int value = 0x12000003;
    owner->middle->target->slot19(value, owner);
}
}

// TODO: clean up, give variables real names
namespace jj_3B2CB4 {
#pragma optimization_level 2
#pragma scheduling off
#pragma opt_propagation off

struct Owner;
class Object_vptr_0 {
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
    virtual void slot18(int, Owner*);
};
struct Middle { char pad[0x64]; Object_vptr_0* target; };
struct Owner { char pad[0xD8]; Middle* middle; };
struct Object { char pad[0x60]; Owner* owner; };
extern "C" void unableOperate__16wnSnakeRgb6SightFv(Object* self) {
    Owner* owner = self->owner;
    int value = 0x12000003;
    owner->middle->target->slot18(value, owner);
}
}

// TODO: clean up, give variables real names
namespace jj_3B3248 {
extern "C" void fn_27_3B13C0(void*);

extern "C" void fn_27_3B3248(void* self) {
    fn_27_3B13C0(reinterpret_cast<u8*>(self) - 0x40);
}
}

// TODO: clean up, give variables real names
namespace jj_3B3250 {
extern "C" void fn_27_3B13C0(void*);

extern "C" void fn_27_3B3250(void* self) {
    fn_27_3B13C0(reinterpret_cast<u8*>(self) - 0x90);
}
}

// TODO: clean up, give variables real names
namespace jj_3B3258 {
extern "C" void fn_27_3B13C0(void*);

extern "C" void fn_27_3B3258(void* self) {
    fn_27_3B13C0(reinterpret_cast<u8*>(self) - 0x88);
}
}

// TODO: clean up, give variables real names
namespace jj_3B3270 {
extern "C" void fn_27_3B319C(void*);

extern "C" void fn_27_3B3270(void* self) {
    fn_27_3B319C(reinterpret_cast<u8*>(self) - 0x88);
}
}

