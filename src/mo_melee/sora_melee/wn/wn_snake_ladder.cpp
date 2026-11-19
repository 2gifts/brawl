#include <wn/snake/wn_snake_ladder.h>

void wnSnakeLadder::processFixCamera() {
    Weapon::processFixCamera();
}

// TODO: clean up, give variables real names
namespace jj_3B6C3C {
extern "C" {

void __dl__FPv(void*);

void* __dt__26wnSnakeLadderParamAccesserFv(void* a, int b) {
    if (a != 0 && b > 0)
        __dl__FPv(a);
    return a;
}

}
}

// TODO: clean up, give variables real names
namespace jj_3B6C7C {
extern "C" void* fn_27_3B6C7C(void* self) {
    return reinterpret_cast<u8*>(self) + 0x7f8;
}
}

// TODO: clean up, give variables real names
namespace jj_3B9268 {
extern "C" void fn_27_3B69C8(void*);

extern "C" void fn_27_3B9268(void* self) {
    fn_27_3B69C8(reinterpret_cast<u8*>(self) - 0x40);
}
}

// TODO: clean up, give variables real names
namespace jj_3B9270 {
extern "C" void fn_27_3B69C8(void*);

extern "C" void fn_27_3B9270(void* self) {
    fn_27_3B69C8(reinterpret_cast<u8*>(self) - 0x90);
}
}

// TODO: clean up, give variables real names
namespace jj_3B9278 {
extern "C" void fn_27_3B69C8(void*);

extern "C" void fn_27_3B9278(void* self) {
    fn_27_3B69C8(reinterpret_cast<u8*>(self) - 0x88);
}
}

// TODO: clean up, give variables real names
namespace jj_3B9290 {
extern "C" void fn_27_3B91BC(void*);

extern "C" void fn_27_3B9290(void* self) {
    fn_27_3B91BC(reinterpret_cast<u8*>(self) - 0x88);
}
}

