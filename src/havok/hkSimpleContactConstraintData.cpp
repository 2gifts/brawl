#include <types.h>

// Function bodies below are taken from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
#pragma auto_inline off

extern "C" int fn_802DE230(void) {
    return 0x01000000;
}

extern "C" void* fn_802DE25C(void* a, void* b)
{
    return b;
}
