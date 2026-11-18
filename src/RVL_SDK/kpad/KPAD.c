#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_804F9FC0[0x14a0] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_804FB460[0x40] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0CA0[0x4] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_805A0CA4[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0CA8[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0CB0[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0CB4[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0CB8[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0CBC[0x4] __attribute__((aligned(4))) = { 0 };

// TODO: clean up, give variables real names
extern int lbl_8059ED5C;
extern int lbl_8059ED60;

void fn_8022555C(char a, char b) {
    lbl_8059ED5C = a;
    lbl_8059ED60 = b;
}

extern unsigned char lbl_804F9FC0[];

void fn_80225570(int idx, float a, float b) {
    unsigned char* p = lbl_804F9FC0 + idx * 0x528;
    *(float*)(p + 0x9c) = a;
    *(float*)(p + 0xa0) = b;
}

void fn_80228138(int arg0) {
    int offset = arg0 * 0x528;
    ((unsigned char *)lbl_804F9FC0 + offset)[0x520] = 0;
}

void fn_80228154(int arg0) {
    int offset = arg0 * 0x528;
    ((unsigned char *)lbl_804F9FC0 + offset)[0x520] = 1;
}

