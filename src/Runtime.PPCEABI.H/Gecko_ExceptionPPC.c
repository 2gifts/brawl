#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char fragmentinfo[0xc] __attribute__((aligned(8))) = { 0 };
