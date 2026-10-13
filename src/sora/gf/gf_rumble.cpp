#include <types.h>

extern "C" void fn_8002C510(void *, s32, s32, s32, u8, void *, s32);

struct gfRumble {
    char unk0[0xF0];
    // HYPOTHESIS: slot tables are indexed by a byte offset (8 bytes per slot)
    u8 *unkF0;
    u8 *unkF4;
    u16 unkF8;
    f32 unkFC;

    void lbSetRumble(s32, s32, s32, s32, s32);
};

void gfRumble::lbSetRumble(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 level;
    s32 slotOffset;
    u8 *slots;
    u16 base;

    if (arg0 < 4) {
        level = arg3 != 0 ? arg3 : -2;
        slotOffset = arg2 * 8;
        slots = this->unkF0;
        fn_8002C510(this, arg0, arg1, level, (slots + slotOffset)[4], *(void **) (slots + slotOffset), arg4);
        return;
    }
    if (arg3 > 0x1E) {
        base = this->unkF8;
        arg3 = base + (s32) (this->unkFC * (f32) (arg3 - base));
    }
    level = arg3 != 0 ? arg3 : -2;
    slots = this->unkF4;
    slotOffset = arg2 * 8;
    fn_8002C510(this, arg0, arg1, level, (slots + slotOffset)[4], *(void **) (slots + slotOffset), arg4);
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_80495380[0x10] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0058[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A005C[0x4] __attribute__((aligned(4))) = { 0 };
