#include <types.h>

// TODO: clean up, give variables real names
namespace jj_2544C8 {
extern "C" u8 fn_27_2544C8(void* self) {
    return *reinterpret_cast<u8*>(reinterpret_cast<u8*>(self) + 0x48);
}
}

// TODO: clean up, give variables real names
namespace jj_2544D0 {
struct X {
    char _0x40[0x40];
    bool m_isSomething;
};

extern "C" void fn_27_2544D0(X* obj) {
    obj->m_isSomething = true;
}
}

