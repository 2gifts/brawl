#include <types.h>

// Function bodies below are taken from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
#pragma auto_inline off

void fn_80265AE0(int *arg0, signed char *arg1)
{
    int flag = 0;
    int v = (*arg0 >> 11) + 0x80;

    if (v < 0x100 && v > -1) {
        flag = 1;
    }

    *arg1 = flag ? v : (v < 0 ? 0 : 255);
}

void fn_80265E6C(s32 *arg0, s8 *arg1) {
    s32 value;
    s32 valid;

    valid = 0;
    value = *arg0 >> 11;
    if (value < 128 && value > -129) {
        valid = 1;
    }
    *arg1 = valid ? value : (value > 0 ? 127 : -128);
}
