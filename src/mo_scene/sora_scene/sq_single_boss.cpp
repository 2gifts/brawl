#include <ad/ad_keep_manager.h>
#include <gf/gf_scene.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <mt/mt_prng.h>
#include <sq/sq_single_boss.h>
#include <sr/sr_common.h>
#include <types.h>

sqSingleBoss* sqSingleBoss::create() {
    return new (Heaps::GlobalMode) sqSingleBoss("sqSingleBoss");
}

void sqSingleBoss::exit() {
    g_GameGlobal->m_advData->m_record.unk4924_7 = 0;
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
