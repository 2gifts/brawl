#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char gTRKBigEndian[0x4] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80599F78[0x19b0] __attribute__((aligned(8))) = { 0 };
