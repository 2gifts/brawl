#include <types.h>

// Function bodies below are taken from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
#pragma auto_inline off

void* fn_803EB4EC(void* arg0, void* arg1) {
    if (arg0 != 0) {
        return 0;
    }
    return arg1;
}

s32 fn_803EB504(s32 arg0) {
    if (arg0 == 0) {
        return 0;
    }
    if (arg0 == 1) {
        return 1;
    }
    return -1;
}

s32 fn_803EAAE4(s32 arg0);
s32 fn_803EB504(s32 arg0);

s32 fn_803EBA18(s8 arg0) {
    s32 result = fn_803EAAE4(arg0);
    return fn_803EB504(result);
}
