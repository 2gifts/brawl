#include <types.h>
#include <ft/ft_owner.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

struct ftOwnerDataFields {
    char unk0[0x64];
    s32 unk64;
};

void ftOwner::addGenerateCoin(int coins)
{
    if (unkA == 1) {
        ftOwnerDataFields* data = reinterpret_cast<ftOwnerDataFields*>(m_data);
        s32 room = 0x7FFFFFFF - data->unk64;
        if (room < coins) {
            coins = room;
        }
        data->unk64 = data->unk64 + coins;
    }
}
