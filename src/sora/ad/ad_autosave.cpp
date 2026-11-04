#include <types.h>
#include <ad/ad_autosave.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
adAutoSave* lbl_805A0468[2] __attribute__((aligned(4))) = { 0 };

adAutoSave::~adAutoSave() {
    lbl_805A0468[0] = 0;
}
