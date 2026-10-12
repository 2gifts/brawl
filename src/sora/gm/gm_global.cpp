#include <types.h>
#include <gm/gm_global.h>

// HYPOTHESIS: m_appData points at the frame counter block that updateGameFrame writes; unk4 is the frame count, unk8 the frame divisor with unk9 its reload value, and unkA the position within the current frame
struct GameFrameState {
    char unk0[0x4];
    u32 unk4;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    char unkB;
    f32 unkC;
    char unk10[0x4];
    s32 unk14;
    u8 unk18;
};

float GameGlobal::getGameFrame() const {
    GameFrameState* state = (GameFrameState*)m_appData;

    return (f32)state->unk4 + ((f32)state->unkA / (f32)state->unk8);
}

void GameGlobal::updateGameFrame() {
    GameFrameState* state = (GameFrameState*)m_appData;

    if (state->unk8 == 1) {
        state->unk4++;
        u8 temp = state->unk9;
        state->unk8 = temp;
        state->unkA = 0;
        state->unkC = 1.0f / (f32)temp;
        state->unk14++;
        state->unk18 = 0;
        return;
    }

    state->unkA++;
    if (state->unkA >= state->unk8) {
        state->unkA = 0;
        state->unk8 = state->unk9;
        state->unk4++;
    }
    state->unkC = 1.0f / (f32)state->unk8;

    state->unk18++;
    if (state->unk18 >= state->unk8) {
        state->unk18 = 0;
        state->unk14++;
    }
}
