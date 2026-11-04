#include <if/if_adv_mngr.h>
#include <if/if_player.h>
#include <st/operator/st_operator_info_adventure.h>
#include <types.h>

extern "C" {
void fn_800E5174(IfPlayer* player, int mode);
}

// HYPOTHESIS: two (value, flag) pairs filled by the static initializer (same shape as other sora_melee units).
struct stOperatorInfoAdventurePair {
    u32 m_0;
    u32 m_4;
    stOperatorInfoAdventurePair(u32 first, u32 second) : m_0(first), m_4(second) { }
};
static stOperatorInfoAdventurePair g_stOperatorInfoAdventurePairA(0xff, 0);
static stOperatorInfoAdventurePair g_stOperatorInfoAdventurePairB(0xff, 1);

bool stOperatorInfoAdventure::isEnableOperation() {
    return m_isEnableOperation;
}

u32 stOperatorInfoAdventure::getEndType() {
    if (m_11d) {
        return 3;
    }
    return 1;
}

u32 stOperatorInfoAdventure::getErrorLevel() {
    return 0;
}

void stOperatorInfoAdventure::setPlayerCursorStatus(u8 idx, u8 status) {
    // MATCH-ONLY: the index is widened before the float table store.
    int index = idx;
    if (status == 0 && status != m_playersCursorStatus[index] && m_playersNameCursorStatus[index] == 0) {
        if (g_IfAdvMngr) {
            IfPlayer* player = g_IfAdvMngr->m_players[index];
            if (player->m_headCursorMode) {
                fn_800E5174(player, 0);
            }
        }
        m_98[index] = 0.0f;
    }
    m_playersCursorStatus[index] = status;
}

void stOperatorInfoAdventure::setPlayerNameCursorStatus(u8 idx, u8 status) {
    // MATCH-ONLY: the index is widened before the float table store.
    int index = idx;
    if (status == 0 && status != m_playersNameCursorStatus[index] && m_playersCursorStatus[index] == 0) {
        if (g_IfAdvMngr) {
            IfPlayer* player = g_IfAdvMngr->m_players[index];
            if (player->m_headCursorMode) {
                fn_800E5174(player, 0);
            }
        }
        m_98[index] = 0.0f;
    }
    m_playersNameCursorStatus[index] = status;
}

void stOperatorInfoAdventure::setPlayerLoupeStatus(u8 idx, u8 status) {
    // MATCH-ONLY: the index is widened before the table lookups.
    int index = idx;
    if (status == 0 && m_playersLoupeStatus[index] == 1 && g_IfAdvMngr) {
        g_IfAdvMngr->m_players[index]->setLoupeVisible(false);
        g_IfAdvMngr->m_players[index]->unk2B0 = 0;
    }
    m_playersLoupeStatus[index] = status;
}

void stOperatorInfoAdventure::renderDebug() {
    gfTask::renderDebug();
}

void stOperatorInfoAdventure::stopOperatorRequest() {
    m_isStopRequest = true;
}
