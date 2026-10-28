#include <types.h>

// Function bodies below are taken from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
#pragma auto_inline off

extern void fn_8035AED0(int, int, int, int, int, int);

void fn_8035AEB4(int a1, int a2, int a3, int a4, int a5) {
    fn_8035AED0(a1, a2, 4, a3, a4, a5);
}
