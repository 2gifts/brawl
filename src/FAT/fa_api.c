#include <types.h>
#include <revolution/FA/FA.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_80597FB0[0x68] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80598018[0x68] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80598080[0x18] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A1268[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A126C[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1270[0x8] __attribute__((aligned(4))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
#pragma auto_inline off

extern FAHandle* fn_803EB644(const char*, const char*);

FAHandle* FAFopen(const char* filepath, const char* mode) {
    FAHandle* handle = fn_803EB644(filepath, mode);
    if (handle == NULL)
        return NULL;
    return handle;
}

extern int fn_803EB8B4(char);

int fn_803EC094(char arg0) {
    int result = fn_803EB8B4(arg0);
    if (result != 0 && result != 1) {
        result = -1;
    }
    return result;
}
