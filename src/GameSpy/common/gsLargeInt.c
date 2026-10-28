#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_805349A8[0x98] __attribute__((aligned(8))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
#pragma auto_inline off

u32 fn_8035EC44(s32 *arg0, s32 *arg1, u32 *arg2)
{
    s32 *p0;
    s32 *p1;
    s32 n0;
    s32 n1;
    u32 size;

    n0 = *arg0;
    n1 = *arg1;
    p0 = arg0 + n0;
    while (n0 > 0 && *p0 == 0) {
        p0--;
        n0--;
    }

    p1 = arg1 + n1;
    while (n1 > 0 && *p1 == 0) {
        p1--;
        n1--;
    }

    if (n0 > n1) {
        n1 = n0;
    }

    *arg2 = (u32)n1;
    size = 1;
    while (size < (u32)n1) {
        size <<= 1;
    }

    *arg2 = size;
    return size <= 64u;
}

int fn_8035ECE0(u32* a, u32 an, u32* b, u32 bn)
{
    u32* p = a + an;
    u32* q;
    while (p[-1] == 0 && an != 0) {
        p--;
        an--;
    }
    q = b + bn;
    while (q[-1] == 0 && bn != 0) {
        q--;
        bn--;
    }
    if (an < bn) return -1;
    if (an > bn) return 1;
    q = b + an;
    p = a + an;
    for (; an != 0; an--) {
        u32 y = q[-1];
        u32 x = p[-1];
        if (x < y) return -1;
        if (x > y) return 1;
        q--;
        p--;
    }
    return 0;
}
