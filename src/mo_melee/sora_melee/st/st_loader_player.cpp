#include <st/loader/st_loader_player.h>
#include <gm/gm_global_mode_melee.h>
#include <types.h>

// TODO: clean up - the ctor sets bits of the byte at 0x5D (_0x5b[2]) by hand; name the bit flags in the header once the
// loader player's flag byte is understood (0x80 cleared, 0x40 set, 0x0C cleared).
stLoaderPlayer::stLoaderPlayer(u32 playerId) : stLoader("StLoaderPlayer"), m_playerId(playerId), m_entryId(-1), m_slotId(-1) {
    _0x54[1] = 0;
    m_56 = 0;
    m_mode = Mode_Entry;
    _0x58[0] = 0;
    u32 flags = (u8)_0x5b[2];
    flags &= ~0x80;
    flags |= 0x40;
    flags &= ~0x0C;
    _0x5b[2] = flags;
}

stLoaderPlayer::~stLoaderPlayer() { }

gmPlayerInitData* stLoaderPlayer::getWatchPlayerInitData() {
    return &m_globalModeMelee->m_playersInitData[m_playerId];
}
