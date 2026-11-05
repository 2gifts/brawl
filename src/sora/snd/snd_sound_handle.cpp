#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A12C8[0x8] __attribute__((aligned(4))) = { 0 };

#include <snd/snd_doubutsugo.h>
#include <nw4r/snd/snd_SoundArchivePlayer.h>

// HYPOTHESIS: these data records hold callbacks tested before starting speech.
extern void (Doubutsugo::*lbl_804937B8)();
extern void (Doubutsugo::*lbl_804937C4)();

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

void Doubutsugo::SetVolume(f32 volume) {
    if (volume < 0.0f) {
        unkC4 = 0.0f;
    } else if (volume > 1.0f) {
        unkC4 = 1.0f;
    } else {
        unkC4 = volume;
    }
}

bool Doubutsugo::Setup(nw4r::snd::SoundArchivePlayer* player) {
    bool available = false;
    if (player != 0 && player->IsAvailable()) {
        Init();
        unk0 = player;
        available = true;
    }
    return available;
}

void Doubutsugo::Start(const void* script) {
    if (unk0 != 0) {
        if (lbl_804937C4) {
            unk3A = 0;
        }
        unk80 = static_cast<const s16*>(script) + 4;
        unk39 = 1;
        unk4C = 0;
        unk40 = 3;
        unk44 = 0;
        unk74 = 0;
        unk78 = 0;
        unk7C = 0;
        unk6C = 0;
        unk18 = 1.0f;
        unk2C = 1.0f;
        unk64 = 0;
        unk1C = 1.0f;
        unk30 = 1.0f;
        unk38 = 0;
    }
}

void Doubutsugo::StartDirect(const s16* words) {
    if (unk0 != 0) {
        for (int count = 0; count < 32; count++) {
            unk84[count] = *words;
            if (*words == 0xD3) {
                unk84[count] = 0xCB;
                unk84[count + 1] = 0xCB;
                unk84[count + 2] = 0xCB;
                unk84[count + 3] = 0xD3;
                break;
            }
            if (count == 31) {
                return;
            }
            words++;
        }
        if (lbl_804937B8) {
            unk3A = 0;
        }
        unk80 = unk84;
        unk39 = 1;
        unk4C = 0;
        unk40 = 3;
        unk44 = 0;
        unk74 = 0;
        unk78 = 0;
        unk7C = 0;
        unk6C = 0;
        unk18 = 1.0f;
        unk2C = 1.0f;
        unk64 = 0;
        unk1C = 1.0f;
        unk30 = 1.0f;
        unk38 = 0;
    }
}
