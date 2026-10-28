#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_805639C0[0xa30] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_805643F0[0xd0] __attribute__((aligned(8))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
#pragma auto_inline off

extern void fn_803D8E00(void *self, int mode);

void fn_803D80B0(int *arg0) {
    *arg0 &= ~4;
    fn_803D8E00(arg0, 0);
}

extern void fn_803D8E00(void* self, int mode);

void fn_803D80C4(u32* self) {
    *self |= 0x4;
    fn_803D8E00(self, 1);
}

int fn_803D8988(const unsigned int *p)
{
    return (p[0] >> 2) & 1;
}
