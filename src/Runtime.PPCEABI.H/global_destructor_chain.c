#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char atexit_funcs[0x300] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char atexit_curr_func[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char __global_destructor_chain[0x4] __attribute__((aligned(4))) = { 0 };
