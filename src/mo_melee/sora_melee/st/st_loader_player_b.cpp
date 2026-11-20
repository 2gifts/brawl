#include <ft/ft_manager.h>
#include <st/loader/st_loader_player.h>
#include <gm/gm_global_mode_melee.h>
#include <types.h>

// TODO: clean up - the flag byte at 0x5D (_0x5b[2]) is tested by hand; name its bits in the header.
// HYPOTHESIS: lockEntity/unlockEntity hand the fighter's controller to or from the player (operation status 0 / 1).
void stLoaderPlayer::lockEntity() {
    if (isEntity()) {
        g_ftManager->setFighterOperationStatus(m_entryId, 0);
    }
}

void stLoaderPlayer::unlockEntity() {
    if (isEntity()) {
        g_ftManager->setFighterOperationStatus(m_entryId, 1);
    }
}

bool stLoaderPlayer::isEntry() {
    u32 flag = ((u8)_0x5b[2] >> 3) & 1;
    if (flag == 1) {
        return true;
    }
    gmPlayerInitData* initData = getWatchPlayerInitData();
    if (initData == NULL) {
        return false;
    }
    return initData->m_state != 3;
}

void stLoaderPlayer::setSuddenDeath() { }

void stLoaderPlayer::advanceLoadedState() {
    if (m_state == 3) {
        m_state = 2;
    }
}

bool stLoaderPlayer::checkPokemonTrainer() {
    gmPlayerInitData* initData = getWatchPlayerInitData();
    if (initData == NULL) {
        return false;
    }
    if (initData->m_characterKind == 0xC || initData->m_characterKind == 0x17) {
        _0x54[1] = 0;
        return true;
    }
    return false;
}

bool stLoaderPlayer::isLoading() {
    gmPlayerInitData* initData = getWatchPlayerInitData();
    if (initData == NULL) {
        return false;
    }
    u8 state = m_state;
    if (state == 0) {
        return false;
    }
    if (state == 3) {
        return false;
    }
    return state != 6;
}

bool stLoaderPlayer::isEmpty() {
    if (m_state == 0) {
        gmPlayerInitData* initData = getWatchPlayerInitData();
        if (initData->m_state == 3 || initData->m_characterKind == 0x3e) {
            return true;
        }
        if ((u8)_0x45[2] == 1) {
            return true;
        }
    }
    return false;
}
