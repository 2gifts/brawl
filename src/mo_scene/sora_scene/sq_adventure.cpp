#include <ad/ad_keep_manager.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_adventure.h>
#include <sr/sr_common.h>
#include <types.h>

sqAdventure* sqAdventure::create() {
    return new (Heaps::GlobalMode) sqAdventure("sqAdventure");
}

void srReportLessRelease(const char* format, ...) {
}

void srReportLessMastar(const char* format, ...) {
}

// HYPOTHESIS: command records contain an opcode and either an integer or a pointer.
extern const struct {
    s32 unk0;
    union {
        s32 unk4;
        const s32* unk4p;
    };
} lbl_1_rodata_1378[];

s32 sqAdventure::getStartStepId(s32 index) {
    if (index < 0 || index > 33) {
        return 0;
    }
    bool found = false;
    for (s32 i = 0; ; ++i) {
        if (found) {
            if (lbl_1_rodata_1378[i].unk0 == 0 ||
                lbl_1_rodata_1378[i].unk0 == 1 ||
                lbl_1_rodata_1378[i].unk0 == 3 ||
                lbl_1_rodata_1378[i].unk0 == 4 ||
                lbl_1_rodata_1378[i].unk0 == 27) {
                return *lbl_1_rodata_1378[i].unk4p;
            }
        } else if (lbl_1_rodata_1378[i].unk0 == 22) {
            if (index == lbl_1_rodata_1378[i].unk4 - 1) {
                found = true;
            }
        }
    }
}

void sqAdventure::exit() {
    if (unk24 != NULL) {
        delete unk24;
        unk24 = NULL;
    }
    if (unk28 != NULL) {
        delete unk28;
        unk28 = NULL;
    }
    g_GameGlobal->m_modeMelee->m_meleeInitData.m_0x5a_4 = false;
}

void sqAdventure::calculateClearPercent() {
    float percent = 0.0f;
    s32 clearNum;
    s32 totalNum;
    s32 stepId;
    u32 i = 0;
    for (; i < 34; ++i) {
        stepId = getStartStepId(i);
        percent += adKeepManager::getInstance()->setMapPointClearPercent(i, stepId, &clearNum, &totalNum);
    }
    gmAdvData* advData = g_GameGlobal->m_advData;
    if (g_GameGlobal->m_advData->m_globalFlags[31] & 1) {
        if (!((g_GameGlobal->m_advData->m_globalFlags[31] >> 2) & 1)) {
            if (advData->m_levels[8].m_kind == 4) {
                advData->m_levels[8].m_kind = 3;
            }
        }
        if (!((g_GameGlobal->m_advData->m_globalFlags[31] >> 6) & 1)) {
            if (advData->m_levels[13].m_kind == 4) {
                advData->m_levels[13].m_kind = 3;
            }
        }
        if (!((g_GameGlobal->m_advData->m_globalFlags[31] >> 4) & 1)) {
            if (advData->m_levels[17].m_kind == 4) {
                advData->m_levels[17].m_kind = 3;
            }
        }
    }
    advData = g_GameGlobal->m_advData;
    if ((advData->m_globalFlags[29] >> 12) & 1) {
        percent += 1.0f;
    }
    u32 flags = advData->m_globalFlags[31];
    if ((flags >> 2) & 1) {
        percent += 1.0f;
    }
    if ((flags >> 4) & 1) {
        percent += 1.0f;
    }
    if ((flags >> 6) & 1) {
        percent += 1.0f;
    }
    advData->m_unk2A8 = percent;
}
