#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A1060[0x2] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1064[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1068[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A106C[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1070[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1074[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1078[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1080[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1084[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1088[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A108C[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1090[0x4] __attribute__((aligned(4))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
#pragma auto_inline off

void fn_8037ED38(s32, s32, s32, s32, s32, s32, s32, s32);

void fn_8037ED14(s32 a, s32 b, s32 c, s32 d, s32 e) {
    fn_8037ED38(a, 0, b, 0, c, 0, d, e);
}

void fn_8037F1A4(void *p);

void fn_8037EFEC(void *p) {
    if (p != 0) {
        fn_8037F1A4(p);
    }
}

extern void fn_8037F1B4(int arg);

void fn_8037EFFC(int arg) {
    if (arg) {
        fn_8037F1B4(arg);
    }
}
