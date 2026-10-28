#include <types.h>
#include <FAT/pf2.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A1260[0x8] __attribute__((aligned(4))) = { 0 };

// Function bodies below are derived from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own.
#pragma auto_inline off

void PF_enterCritical(void* unk) {
    PF_CRITICAL* critical = unk;
    u32 context;

    if (critical->count == 0) {
        fn_803EC3D4(critical->resource);
        fn_803EC27C(&critical->owner);
    } else {
        fn_803EC27C(&context);
        if (critical->owner != context) {
            fn_803EC3D4(critical->resource);
            critical->owner = context;
        }
    }
    critical->count++;
}

void PF_exitCritical(void* unk) {
    PF_CRITICAL* critical = unk;

    critical->count--;
    if (critical->count != 0) {
        return;
    }
    critical->owner = 0;
    fn_803EC418(critical->resource);
}
