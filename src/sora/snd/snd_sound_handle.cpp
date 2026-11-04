#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A12C8[0x8] __attribute__((aligned(4))) = { 0 };

#include <snd/snd_doubutsugo.h>

void Doubutsugo::Init() {
    unk3C = 0;
    unk40 = 3;
    unk44 = 0;
    if (m_source != 0) {
        m_source->vf18(0);
    }
    unk48 = 0x12;
    unk4C = 0;
    unk54 = 0;
    unk58 = 0;
    unk5C = 0;
    unk60 = 0;
    unk64 = 0;
    unk68 = 0x216;
    unk6C = 0;
    unk70 = 0;
    unk39 = 0;
    unk74 = 0;
    unk78 = 0;
    unk7C = 0;
    unk38 = 0;
    unkC4 = 0.7f;
    unk14 = 1.0f;
    unk28 = 1.0f;
}
