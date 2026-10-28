#include <types.h>

// Function bodies below are taken from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
#pragma auto_inline off

extern "C" unsigned int fn_8014E708(unsigned int x) {
    return ((x + 0x1D45U) / 7474U) & 0xFFFFU;
}
