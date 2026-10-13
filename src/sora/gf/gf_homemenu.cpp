#include <types.h>
#include <gf/gf_homemenu.h>

extern "C" void fn_80228154(s32 p1);
extern "C" void fn_80228138(s32 p1);

struct gfHomeMenuFields {
    char unk0[0x9C];
    s32 unk9C;
    char unkA0[0x10];
    union {
        struct {
            u32 unkB0Hi : 4;
            u32 unkB0Mode : 4;
            u32 unkB0Lo : 24;
        } unkB0;
        u8 unkB0Bytes[4];
    };
};

void gfHomeMenu::start() {
    gfHomeMenuFields* self = reinterpret_cast<gfHomeMenuFields*>(this);
    s32 var_r4 = 0;

    if ((((u8) self->unkB0Bytes[1] >> 4U) & 1) || (self->unk9C != 0)) {
        var_r4 = 1;
    }
    if (var_r4 != 0) {
        fn_80228154(0);
        fn_80228154(1);
        fn_80228154(2);
        fn_80228154(3);
    }
    self->unkB0.unkB0Mode = 1;
}

void gfHomeMenu::kill() {
    gfHomeMenuFields* self = reinterpret_cast<gfHomeMenuFields*>(this);

    fn_80228138(0);
    fn_80228138(1);
    fn_80228138(2);
    fn_80228138(3);
    self->unkB0.unkB0Mode = 2;
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_804953D0[0x48] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80495418[0xc0] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_804954D8[0x8] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_804954E0[0xf4] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_804955D4[0x1] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_804955D5[0x1] = { 0 };
unsigned char lbl_804955D8[0x2100] __attribute__((aligned(8))) = { 0 };
