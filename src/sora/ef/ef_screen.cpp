#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char g_efScreen[0x8] __attribute__((aligned(4))) = { 0 };
