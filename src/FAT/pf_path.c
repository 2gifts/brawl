#include <types.h>

// Function bodies below are taken from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
#pragma auto_inline off

int fn_803E72B4(int a, int b, int c);

int fn_803E6CDC(int a, int b) {
    return fn_803E72B4(a, b, 1) == 0;
}

int fn_803E5F58(int, int, int, int);

int fn_803E7224(int p1, int p2, int p3, int p4) {
    return fn_803E5F58(p1, p2, p3, 0);
}
