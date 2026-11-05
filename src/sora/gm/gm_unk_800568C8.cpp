#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <gm/gm_hide_lib.h>
#include <types.h>

// gmAddFigure (the map name); src/sora/gm/gm_common.cpp calls it by this stand-in name.
extern "C" void fn_80052704(int id, int num);
// gmAddSeal (the map name); src/sora/gm/gm_common.cpp calls it by this stand-in name.
extern "C" void fn_80052AC0(int id, int num);
// gmSetBGMIDUseEnable (the map name); src/sora/ad/ad_keep_manager.cpp calls it by this stand-in name.
extern "C" void fn_8004FB48(u32 id);

extern const struct {
    struct {
        u16 unk0[5];
    } unk0;
    u16 unkA[3];
} lbl_80406E98;

void gmActionOpenClearGet(int condition, int kind, int reward) {
    switch (kind) {
    case 5: {
        u32* flags = (u32*)g_GameGlobal->getHideEtcAppFlag();
        int bit = (reward - 1) * 2;
        flags[bit / 32] |= 1 << (bit % 32);
        break;
    }
    case 3: {
        u32* flags = (u32*)g_GameGlobal->getHideEtcAppFlag();
        int bit = (reward + 2) * 2;
        flags[bit / 32] |= 1 << (bit % 32);
        break;
    }
    case 1:
        fn_80052704((u16)reward, 1);
        gmSetFlagInfoFigure((u16)reward, 1);
        break;
    case 0: {
        __typeof__(lbl_80406E98.unk0) seals = lbl_80406E98.unk0;
        int seal = seals.unk0[reward - 1];
        fn_80052AC0(seal, 1);
        gmSetFlagInfoSeal(seal, 1);
        break;
    }
    case 2:
        fn_8004FB48(reward);
        break;
    case 4:
        gmSetStageUseEnable((srStageKind)reward);
        break;
    }
}

bool gmCheckTargetBreakHideElement() {
    if (!(g_GameGlobal->getCharaAppFlag()[0] & 0x10000000)) {
        for (int level = 0; level < 5; ++level) {
            u32 count = 0;
            for (int character = 0; character < 35; ++character) {
                if (g_GameGlobal->getGlobalRecordCharaNumDatap(character)->unkCC[level].unk0_30 == 1) {
                    ++count;
                    if (count >= 30) break;
                }
            }
            if (count >= 30) {
                g_GameGlobal->getCharaAppFlag()[0] |= 0x8000000;
                return true;
            }
        }
    }
    if (!(g_GameGlobal->getCharaAppFlag()[0] & 0x10000)) {
        u32 cleared = 0;
        for (int level = 0; level < 5; ++level) {
            cleared = 0;
            for (int character = 0; character < 35; ++character) {
                if (g_GameGlobal->getGlobalRecordCharaNumDatap(character)->unkCC[level].unk0_30 == 1) {
                    cleared = 1;
                    break;
                }
            }
            if (((g_GameGlobal->m_record->unk34FC[level].unk0 >> 30) & 1) == 1) cleared = 1;
            if (!cleared) break;
        }
        if (cleared == 1) {
            g_GameGlobal->getCharaAppFlag()[0] |= 0x8000;
            return true;
        }
    }
    return false;
}

bool checkAllstar(int condition) {
    bool satisfied = false;
    switch (condition) {
    case 27:
    case 24:
        int count = 0;
        for (int character = 0; character < 35; ++character) {
            if (g_GameGlobal->getGlobalRecordCharaNumDatap(character)->unk9C_7) ++count;
        }
        switch (condition) {
        case 27:
            if (count >= 35) satisfied = true;
            break;
        case 24:
            if (count >= 10) satisfied = true;
            break;
        }
        break;
    }
    return satisfied;
}

bool checkBossBattle(int condition) {
    bool satisfied = false;
    switch (condition) {
    case 127:
    case 101:
    case 61:
        int count = 0;
        for (int character = 0; character < 35; ++character) {
            if (g_GameGlobal->getGlobalRecordCharaNumDatap(character)->unk9C_4) ++count;
        }
        switch (condition) {
        case 127:
            if (count >= 35) satisfied = true;
            break;
        case 101:
            if (count >= 10) satisfied = true;
            break;
        case 61:
            if (count >= 20) satisfied = true;
            break;
        }
        break;
    }
    return satisfied;
}

namespace {
void gmCheckClearGetCondition();
void gmCheckInfoCondition();
void gmCheckRareUP();
}

extern const struct {
    struct {
        u32 unk0;
        int unk4;
    } unk0[14];
} lbl_80406E28;

int gmCheckChallengerToVsMelee() {
    u32* flags = g_GameGlobal->getCharaAppFlag();
    u32 count = gmGetOfflineVsMeleeTotalCount();
    __typeof__(lbl_80406E28) entries = lbl_80406E28;
    for (int i = 13; i >= 0; --i) {
        if (count >= entries.unk0[i].unk0) {
            int kind = entries.unk0[i].unk4;
            int bit = kind * 3 + 1;
            if (!(flags[bit / 32] & (1 << (bit % 32)))) {
                bit = kind * 3;
                flags[bit / 32] |= 1 << (bit % 32);
                return kind;
            }
        }
    }
    return -1;
}

void gmCheckHideCondition() {
    gmCheckSoftList();
    gmCheckClearGetCondition();
    gmCheckInfoCondition();
    gmCheckRareUP();
}

extern const struct {
    struct {
        u16 unk0[49];
    } unk0;
    u16 unk62;
} lbl_80406FE8;

extern const struct {
    u16 unk0[10];
} lbl_8040704C;

bool checkSimple(int condition) {
    bool satisfied = false;
    switch (condition) {
    case 5:
    case 120:
        int count = 0;
        for (int character = 0; character < 35; ++character) {
            if (g_GameGlobal->getGlobalRecordCharaNumDatap(character)->unk9C_b10) {
                ++count;
            }
        }
        switch (condition) {
        case 5:
            if (count >= 35) satisfied = true;
            break;
        case 120:
            if (count >= 20) satisfied = true;
            break;
        }
        break;
    }
    return satisfied;
}

bool checkMeleeEtc(int condition) {
    bool satisfied = false;
    u32 kills = *g_GameGlobal->getGlobalRecordKillTotalCountDatap();
    u32 hours = *g_GameGlobal->getGlobalRecordVsMeleeTimeTotalDatap() / 60 / 60;
    u32 coins = 0;
    for (int character = 0; character < 35; ++character) {
        coins += g_GameGlobal->getGlobalRecordCharaNumDatap(character)->m_diagram.m_getCoin;
    }
    switch (condition) {
    case 26:
        if (kills >= 1000) satisfied = true;
        break;
    case 2:
        if (kills >= 2000) satisfied = true;
        break;
    case 122:
        if (kills >= 500) satisfied = true;
        break;
    case 79:
        if (hours >= 30) satisfied = true;
        break;
    case 115:
        if (hours >= 50) satisfied = true;
        break;
    case 39:
        if (hours >= 100) satisfied = true;
        break;
    case 81:
        if (g_GameGlobal->getGlobalRecordCharaNumDatap(12)->m_diagram.m_champNum >= 5) satisfied = true;
        break;
    case 72:
        if (*g_GameGlobal->getGlobalRecordVsCoinMeleeCountDatap() >= 10) satisfied = true;
        break;
    case 119:
        if (coins >= 1000) satisfied = true;
        break;
    case 91:
        if (coins >= 5000) satisfied = true;
        break;
    case 80:
        break;
    }
    return satisfied;
}

bool checkTraining(int condition) {
    bool satisfied = false;
    u16 maximum = 0;
    u16 total = 0;
    for (int character = 0; character < 35; ++character) {
        u16 count = g_GameGlobal->getGlobalRecordCharaNumDatap(character)->unkC8;
        if (count > maximum) maximum = count;
        total += count;
    }
    switch (condition) {
    case 108:
        if (maximum >= 10) satisfied = true;
        break;
    case 33:
        if (total >= 400) satisfied = true;
        break;
    }
    return satisfied;
}

bool checkInfo_advFigure(int condition) {
    bool satisfied = false;
    switch (condition) {
    case 58: {
        __typeof__(lbl_80406FE8.unk0) figures = lbl_80406FE8.unk0;
        satisfied = true;
        for (int index = 0; index < 49; ++index) {
            if (!gmCheckExistFigure(figures.unk0[index])) {
                satisfied = false;
                break;
            }
        }
        break;
    }
    case 59: {
        __typeof__(lbl_8040704C) figures = lbl_8040704C;
        satisfied = true;
        for (int index = 0; index < 10; ++index) {
            if (!gmCheckExistFigure(figures.unk0[index])) {
                satisfied = false;
                break;
            }
        }
        break;
    }
    }
    return satisfied;
}
