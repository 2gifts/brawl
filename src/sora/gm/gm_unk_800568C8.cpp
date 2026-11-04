#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <gm/gm_hide_lib.h>
#include <types.h>

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
