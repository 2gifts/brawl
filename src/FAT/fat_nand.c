#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_80563798[0x1a8] __attribute__((aligned(8))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
#pragma auto_inline off

void fn_803C9168(int, int, int, int, int);

void fn_803C9ECC(int a, int b, int c) {
    fn_803C9168(a, b, c, 3, -10);
}

int nanddrv_format(int arg0) {
    return arg0 ? 0 : -20;
}

extern int fn_803CACCC(void* a, void* b, int c, int d, int* e, void* f);

int nanddrv_pread(void* a, void* b, int c, void* d, int* e) {
    *e = 0;
    if (a == 0 || b == 0 || d == 0 || e == 0) {
        return -20;
    }
    return fn_803CACCC(d, b, c, 0x200, e, a);
}
