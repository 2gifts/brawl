#include <ad/ad_keep_manager.h>
#include <gf/gf_scene.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <mt/mt_prng.h>
#include <revolution/OS/OSTime.h>
#include <sq/sq_single_boss.h>
#include <sr/sr_common.h>
#include <types.h>

// HYPOTHESIS: muMenu::exchangeGmCharacterKindToMuCharKind is called with one argument; the mangled name has three.
extern "C" int exchangeGmCharacterKindToMuCharKind__6muMenuFiii(int kind);

sqSingleBoss* sqSingleBoss::create() {
    return new (Heaps::GlobalMode) sqSingleBoss("sqSingleBoss");
}

void sqSingleBoss::setScore(int chara, int flag, u32 score) {
    u8 kind = exchangeGmCharacterKindToMuCharKind__6muMenuFiii(chara);
    if (kind >= 0x23) {
        return;
    }
    gmGlobalRecord::CharaNumData* data = g_GameGlobal->getGlobalRecordCharaNumDatap(kind);
    if (data->unk9C_15 == 0) {
        if (flag != 0) {
            data->unk9C_15 = 1;
            data->unkA8 = score;
        } else if (data->unkA8 < score) {
            data->unkA8 = score;
        }
    } else if (flag != 0 && data->unkA8 > score) {
        data->unkA8 = score;
    }
    if (flag != 0) {
        int level = unk11 + 1;
        if (data->unk9C_4 < level) {
            data->unk9C_4 = level;
        }
        checkClearGet();
    }
}

void sqSingleBoss::setScore2Play(int chara1, int p2, int chara2, int p4, int flag, u32 score) {
    gmGlobalRecord* rec = g_GameGlobal->m_record;
    u8 kind1 = exchangeGmCharacterKindToMuCharKind__6muMenuFiii(chara1);
    u8 kind2 = exchangeGmCharacterKindToMuCharKind__6muMenuFiii(chara2);
    int updated = 0;
    if (rec->unk34D4_15 == 0) {
        if (flag != 0) {
            rec->unk34D4_15 = 1;
            updated = 1;
        } else if (rec->unk34DC < score) {
            updated = 1;
        }
    } else if (flag != 0 && rec->unk34DC > score) {
        updated = 1;
    }
    if (flag != 0) {
        int level = unk11 + 1;
        if (rec->unk34D4_7 < level) {
            rec->unk34D4_7 = level;
        }
        checkClearGet();
    }
    if (updated != 0) {
        rec->unk34DC = score;
        rec->unk3528 = kind1;
        rec->unk352A = kind2;
        rec->unk3529 = p2;
        rec->unk352B = p4;
        rec->unk34D4_0 = 1;
    }
}

void sqSingleBoss::start() {
    if (gfSceneManager::getInstance()->unk31C == 0xFF) {
        unk8 = 0x10;
    } else {
        g_GameGlobal->m_advData->m_saveRequest = 0;
        unk8 = 0;
        g_GameGlobal->initSelCharData();
        g_GameGlobal->m_selCharData->unk2E_b7 = 1;
        u32 pad = gmGetMenuDecisionPad();
        if (g_GameGlobal->m_advData != 0) {
            g_GameGlobal->m_advData->m_padInfo[0].m_flag.m_isSet = false;
            g_GameGlobal->m_advData->m_padInfo[0].m_padNo = pad;
        }
        if (g_GameGlobal->m_advData != 0) {
            g_GameGlobal->m_advData->m_padInfo[1].m_flag.m_isSet = false;
            g_GameGlobal->m_advData->m_padInfo[1].m_padNo = -1;
        }
        srandi(OSGetTick());
    }
}

void sqSingleBoss::exit() {
    g_GameGlobal->m_advData->m_flags.m_unk0 = false;
    adKeepManager::getInstance()->useBossBattleEnd();
}

void sqSingleBoss::checkClearGet() {
    int ids[5] = {36, 90, 23, 103, 0};
    for (int i = 0; i < 5; ++i) {
        int id = ids[i];
        if (!isClearGetterOpen(id)) {
            bool open = false;
            switch (id) {
            case 36:
                if (unk11 == 0) open = true;
                break;
            case 90:
                if (unk11 == 1) open = true;
                break;
            case 23:
                if (unk11 == 2) open = true;
                break;
            case 103:
                if (unk11 == 3) open = true;
                break;
            case 0:
                if (unk11 == 4) open = true;
                break;
            }
            if (open) setClearGetterOpen(id);
        }
    }
}

void sqSingleBoss::makeStageTable() {
    u8 candidates[9];
    for (int i = 0; i < 9; ++i) {
        u8* next = candidates;
        int count = 0;
        for (int candidate = 0; candidate < 9; ++candidate) {
            bool used = false;
            for (int j = 0; j < i; ++j) {
                if (candidate == unk12[j]) {
                    used = true;
                    break;
                }
            }
            if (!used) {
                *next++ = candidate;
                ++count;
            }
        }
        unk12[i] = candidates[randi(count * 1000) % count];
    }
    for (int i = 0; i < 9; ++i) {
        if (unk12[i] > 3) ++unk12[i];
        if (unk12[i] == 3 && randi(99) % 2 != 0) ++unk12[i];
    }
    unk12[9] = 10;
}
