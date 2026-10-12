#include <types.h>
#include <ft/ft_owner.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

// HYPOTHESIS: ftOwnerData fields: a counter at 0x64 (coins), then the controller state block at 0x78 (kind and four stick floats)
struct ftOwnerControllerState {
    u8 unk0[0x64];
    s32 unk64;
    u8 unk68[0x10];
    s32 controllerKind;
    f32 unk7C;
    f32 unk80;
    f32 unk84;
    f32 unk88;
};

// HYPOTHESIS: the button arguments are read as two floats each (x at offset 0, y at offset 4)
struct ftOwnerButtonsFloats {
    f32 unk0;
    f32 unk4;
};

bool ftOwner::sameCheckController(int kind, u64* buttons0, u64* buttons1) {
    bool result = false;
    ftOwnerButtonsFloats* stick0 = reinterpret_cast<ftOwnerButtonsFloats*>(buttons0);
    ftOwnerButtonsFloats* stick1 = reinterpret_cast<ftOwnerButtonsFloats*>(buttons1);
    bool stick1XSame = false;
    bool stick0YSame = false;
    bool kindSame = false;
    ftOwnerControllerState* state = reinterpret_cast<ftOwnerControllerState*>(m_data);

    if ((u32)state->controllerKind == (u32)kind && state->unk7C == stick0->unk0) {
        kindSame = true;
    }
    if (kindSame && state->unk80 == stick0->unk4) {
        stick0YSame = true;
    }
    if (stick0YSame && state->unk84 == stick1->unk0) {
        stick1XSame = true;
    }
    if (stick1XSame && state->unk88 == stick1->unk4) {
        result = true;
    }
    return result;
}

void ftOwner::addGenerateCoin(int coins)
{
    if (unkA == 1) {
        ftOwnerControllerState* data = reinterpret_cast<ftOwnerControllerState*>(m_data);
        s32 room = 0x7FFFFFFF - data->unk64;
        if (room < coins) {
            coins = room;
        }
        data->unk64 = data->unk64 + coins;
    }
}
