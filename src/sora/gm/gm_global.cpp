#include <types.h>
#include <gm/gm_global.h>

// HYPOTHESIS: m_appData points at the frame counter block that updateGameFrame writes; unk4 is the frame count and unkA/unk8 the position within the current frame
struct GameFrameState {
    char unk0[0x4];
    u32 unk4;
    u8 unk8;
    u8 unk9;
    u8 unkA;
};

float GameGlobal::getGameFrame() const {
    GameFrameState* state = (GameFrameState*)m_appData;

    return (f32)state->unk4 + ((f32)state->unkA / (f32)state->unk8);
}
