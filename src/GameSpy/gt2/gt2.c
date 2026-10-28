#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_80534A40[0x1000] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80535A40[0x40] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A1050[0x8] __attribute__((aligned(4))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
#pragma auto_inline off

int fn_80377DDC(int a1, int a2, int a3, int a4, int a5, int a6);

int fn_80374878(int a1, int a2, int a3, int a4, int a5, int a6) {
    return fn_80377DDC(a1, a2, a3, a4, a5, 0);
}

void fn_803746D0(void* p, int val);

void fn_80374BC0(void* p) {
    fn_803746D0(p, 1);
}
