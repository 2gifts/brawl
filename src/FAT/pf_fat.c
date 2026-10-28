#include <types.h>

// Function bodies below are taken from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
#pragma auto_inline off

extern void* fn_803E12B4(int, int, int, int, int);

void* fn_803E1414(int arg0, int arg1, int arg2, int arg3) {
    void* result = fn_803E12B4(arg0, arg1, arg2 != 0, 0, arg3);

    if (result != 0) {
        return result;
    }
    return 0;
}
