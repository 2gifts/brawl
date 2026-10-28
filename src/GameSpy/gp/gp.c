#include <types.h>

// Function bodies below are taken from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
#pragma auto_inline off

extern void fn_8035E00C(void* arg);

void fn_8036FD90(void** arg0) {
    fn_8035E00C(*arg0);
    *arg0 = 0;
}

int fn_803703DC(int* a, int b) {
    return *a % b;
}

int fn_803703F0(int *arg0, int *arg1) {
    return *arg0 - *arg1;
}
