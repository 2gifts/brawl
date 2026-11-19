#include <types.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

// TODO: clean up, give variables real names
namespace jj_2544DC {
typedef unsigned char u8;

extern "C" float randf__Fv();
extern "C" const float lbl_27_rodata_4108[4];

struct SoWork {
    char pad[0x40];
    u8 unk40;
    char pad2[0xb];
    float unk4C;
    float unk50;
};

extern "C" void fn_27_2544DC(SoWork* self) {
    float* c = (float*)lbl_27_rodata_4108;
    self->unk40 = 1;
    self->unk4C = c[1] + c[2] * (c[3] * randf__Fv());
    self->unk50 = c[1] + c[2] * (c[3] * randf__Fv());
}
}

