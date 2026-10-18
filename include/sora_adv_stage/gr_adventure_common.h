#pragma once

// Helpers shared by the Subspace Emissary gimmick units.

#include <ft/ft_manager.h>
#include <gf/gf_task.h>
#include <gm/gm_global.h>
#include <gr/ground.h>
#include <types.h>

// Ground::isCollisionStatusOwnerTask (sora_melee): the symbol takes an int pointer (the header declares another type)
extern "C" bool isCollisionStatusOwnerTask__6GroundFP12grCollStatusPi(Ground* ground, grCollStatus* collStatus, int* flags);

// HYPOTHESIS: an unnamed sora_melee float (several stage units read it the same way).
extern "C" float lbl_27_data_547E0;

// The player number of the fighter that owns a task (-1 when it is no player), as grYakumono::getPlayerNumber does it.
static inline s8 advGetPlayerNumber(int taskId) {
    ftManager* manager = g_ftManager;
    int entryId = manager->getEntryIdFromTaskId(taskId, NULL);
    if (entryId == -1) {
        return -1;
    }
    int playerNo = manager->getPlayerNo(entryId);
    if (playerNo >= 4) {
        return -1;
    }
    return playerNo;
}

// HYPOTHESIS: the difficulty level of the adventure data (byte 0x5FD) less the bias byte 0x4916, clamped to 0..14
// (gmAdvData is only partly described).
static inline u8 advGetDifficultyLevel() {
    u8* advData = reinterpret_cast<u8*>(g_GameGlobal->m_advData);
    int level = advData[0x5FD] - advData[0x4916];
    int minLevel = advData[0x5FD] - 5;
    if (level < minLevel) {
        level = minLevel;
    }
    if (level < 0) {
        level = 0;
    }
    if (level > 14) {
        level = 14;
    }
    return level;
}
