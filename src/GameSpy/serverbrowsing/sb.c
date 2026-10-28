#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A1120[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1124[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1128[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1130[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1134[0x4] __attribute__((aligned(4))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
#pragma auto_inline off

u8 fn_80387608(u8 *arg0, u32 arg1, u8 *arg2, u32 arg3, u8 *arg4, u32 *arg5) {
    u32 count;
    u32 mask;
    u32 v;

    if (arg1 == 0) {
        return 0;
    }

    count = 0;
    mask = 1;
    while (mask < arg1) {
        mask = (mask << 1) + 1;
    }

    do {
        *arg4 = arg0[*arg4] + arg2[(*arg5)++];
        if (*arg5 >= arg3) {
            *arg5 = 0;
            *arg4 = *arg4 + arg3;
        }

        count++;
        v = mask & *arg4;
        if (count > 11) {
            v %= arg1;
        }
    } while (v > arg1);

    return (u8)v;
}

extern void fn_80389610(int, int, int, int, int, int, int, int);

void fn_80389754(int a1, int a2, int a3, int a4, int a5, int a6, int a7) {
    fn_80389610(a1, a2, a3, a4, a5, a6, 0x80, a7);
}

s32 fn_8038A298(const u8 *arg0, s32 arg1) {
    s32 i = 0;
    while (arg1 > 0) {
        if ((s8)*arg0 == 0)
            return i + 1;
        i++;
        arg0++;
        arg1--;
    }
    return -1;
}
