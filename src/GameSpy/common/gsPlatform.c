#include <types.h>
#include <revolution/OS/OSInterrupt.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_80534638[0x178] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_805347B0[0x18] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_805347C8[0x40] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A1038[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A103C[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1040[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1044[0x4] __attribute__((aligned(4))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
#pragma auto_inline off

extern int SOInetAtoN(int a, int* b);

int fn_8035C620(int arg) {
    int local;
    int result = SOInetAtoN(arg, &local);
    if (result == 0) {
        return -1;
    }
    return local;
}

s32 fn_8035D200(s32 *arg0)
{
    BOOL enabled = OSDisableInterrupts();
    s32 old = *arg0;
    s32 result = old - 1;
    *arg0 = result;
    OSRestoreInterrupts(enabled);
    return result;
}
