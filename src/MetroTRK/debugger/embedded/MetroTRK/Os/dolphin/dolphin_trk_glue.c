#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char TRK_Use_BBA[0x1] __attribute__((aligned(8))) = { 0 };
