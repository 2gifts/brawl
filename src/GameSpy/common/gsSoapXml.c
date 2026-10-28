#include <types.h>

// Function bodies below are taken from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
#pragma auto_inline off

void fn_8035E00C(void* p);

void fn_80362DD4(void** arg0) {
    fn_8035E00C(*arg0);
    fn_8035E00C(arg0);
}
