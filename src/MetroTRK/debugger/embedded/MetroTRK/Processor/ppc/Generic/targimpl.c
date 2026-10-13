#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char TRK_saved_exceptionID[0x2] __attribute__((aligned(8))) = { 0 };
unsigned char gTRKState[0xa4] __attribute__((aligned(4))) = { 0 };
unsigned char gTRKCPUState[0x430] __attribute__((aligned(8))) = { 0 };
unsigned char gTRKSaveState[0x94] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_8059BEB4[0x14] __attribute__((aligned(4))) = { 0 };
