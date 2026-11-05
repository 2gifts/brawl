#include <cstdio>
#include <gf/gf_boot_task.h>
#include <gf/gf_scene.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <gm/gm_lib.h>
#include <gm/gm_sel_char_data.h>
#include <sc/sc_fig_get_demo.h>
#include <sq/sq_single_allstar.h>
#include <sr/sr_common.h>
#include <types.h>
#include <ut/ut_relocate.h>

// gmAddSeal (the map name); sq_single_simple.cpp calls it by this stand-in name.
extern "C" void fn_80052AC0(int id, int num);
// HYPOTHESIS: the native menu conversion accepts one character kind.
extern "C" int exchangeGmCharacterKindToMuCharKind__6muMenuFiii(int kind);
void addMaxCheck(u32* total, u32 add, u32 max);

// gmAddFigure (the map name); gm_common.cpp defines it by this stand-in name.
extern "C" void fn_80052704(u16 id, int num);

// HYPOTHESIS: All-Star stage table pointers (1P and 2P)
extern void* lbl_1_bss_1998[2][19];
// HYPOTHESIS: All-Star preload buffer address
extern u8* lbl_1_bss_1A30;
// HYPOTHESIS: All-Star preload buffer size and clear character kinds
extern u32 lbl_1_bss_1A34[3];
// HYPOTHESIS: cached scene manager pointer
extern gfSceneManager* lbl_1_bss_1A40;

// HYPOTHESIS: character kind to All-Star trophy ID mapping entry
struct AllstarFigureEntry {
    s32 charKind;
    s32 figId;
};

// HYPOTHESIS: All-Star trophy ID table
struct AllstarFigureTable {
    AllstarFigureEntry entries[37];
};

extern const AllstarFigureTable lbl_1_rodata_C28;

namespace {

void setupData(u8* buf, u32 size);

}

sqSingleAllstar* sqSingleAllstar::create() {
    sqSingleAllstar* seq = new (Heaps::GlobalMode) sqSingleAllstar("sqSingleAllstar");
    if (lbl_1_bss_1A30 == nullptr) {
        gfBootTask::PreLoadDataBuffer buf = gfBootTask::getPreLoadDataBuffer(0);
        lbl_1_bss_1A30 = buf.addr;
        lbl_1_bss_1A34[0] = buf.size;
        setupData(lbl_1_bss_1A30, lbl_1_bss_1A34[0]);
    }
    return seq;
}

void sqSingleAllstar::updateHiScore(bool clear) {
    gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
    gmResultInfo* result = g_GameGlobal->m_resultInfo;
    int* challenge;
    if (unk18 == 1) {
        u32 score = result->m_playersResultInfo[0].m_unk23C;
        gmGlobalRecord::CharaNumData* record = g_GameGlobal->getGlobalRecordCharaNumDatap(
            (u8)exchangeGmCharacterKindToMuCharKind__6muMenuFiii(melee->m_playersInitData[0].m_characterKind));
        if (record->unkA4 < score) record->unkA4 = score;
        if (clear) {
            int level = unk1C + 1;
            if (record->unk9C_7 < level) record->unk9C_7 = level;
        }
    } else {
        u8 kind1 = exchangeGmCharacterKindToMuCharKind__6muMenuFiii(melee->m_playersInitData[0].m_characterKind);
        u8 kind2 = exchangeGmCharacterKindToMuCharKind__6muMenuFiii(melee->m_playersInitData[1].m_characterKind);
        u32 score = 0;
        addMaxCheck(&score, result->m_playersResultInfo[0].m_unk23C, 0xFFFFFFFF);
        addMaxCheck(&score, result->m_playersResultInfo[1].m_unk23C, 0xFFFFFFFF);
        gmGlobalRecord* record = g_GameGlobal->m_record;
        if (record->unk34D8 < score) {
            record->unk34D4_1 = 1;
            record->unk34D8 = score;
            record->unk3524 = kind1;
            record->unk3525 = melee->m_playersInitData[0].m_colorNo;
            record->unk3526 = kind2;
            record->unk3527 = melee->m_playersInitData[1].m_colorNo;
        }
        if (clear) {
            int level = unk1C + 1;
            if (record->unk34D4_10 < level) record->unk34D4_10 = level;
        }
    }
    if (clear) {
        if (isClearGetterOpen(0x41) == 1) {
            if (unk1C == 1) fn_80052AC0(0x28E, 1);
        }
        int ids[6] = {0x2C, 0x22, 0x41, 0x5F, 0x3C, 0x62};
        challenge = ids;
        for (int i = 0; i < 6; i++, challenge++) {
            int id = *challenge;
            if (isClearGetterOpen(id) != 0) continue;
            int open = 0;
            switch (id) {
            case 0x2C: if (unk24 == 0) open = 1; break;
            case 0x22: if (unk1C == 0) open = 1; break;
            case 0x41: if (unk1C == 1) open = 1; break;
            case 0x5F: if (unk1C == 2) open = 1; break;
            case 0x3C: if (unk1C == 3) open = 1; break;
            case 0x62: if (unk1C == 4) open = 1; break;
            }
            if (open != 0) setClearGetterOpen(id);
        }
    }
}

void sqSingleAllstar::start() {
    unk2C = gfSceneManager::getInstance()->unk31C;
    if (lbl_1_bss_1A40 == nullptr) {
        lbl_1_bss_1A40 = gfSceneManager::getInstance();
    }
    if (unk2C == 0xFF) {
        unk8 = 0x17;
        unk2C = 0;
    } else {
        unk8 = 0;
        g_GameGlobal->initSelCharData();
        gmInitGlobalModeMelee(g_GameGlobal->m_modeMelee);
        g_GameGlobal->m_selCharData->unk2E_b7 = 1;
    }
}

namespace {

void setupData(u8* buf, u32 size) {
    utRelocate relocate(buf, size);
    for (int i = 0; i < 19; i++) {
        char name[64];
        std::sprintf(name, "allstarStage%dTbl", i + 1);
        lbl_1_bss_1998[0][i] = relocate.getPublicAddress(name);
    }
    for (int i = 0; i < 19; i++) {
        char name[64];
        std::sprintf(name, "allstarStage%dTbl2p", i + 1);
        lbl_1_bss_1998[1][i] = relocate.getPublicAddress(name);
    }
}

__declspec(force_export) void setAllstarGetFigure(int charKind1, int charKind2) {
    scFigureGetDemo* scene = static_cast<scFigureGetDemo*>(lbl_1_bss_1A40->searchScene("scFigureGetDemo"));
    AllstarFigureTable table = lbl_1_rodata_C28;
    s32 figId = 0;
    for (int i = 0; i < 2; i++) {
        int kind = (i == 0) ? charKind2 : charKind1;
        if (kind != -1) {
            switch (kind) {
            case 0x11:
            case 0x12:
                kind = 0x10;
                break;
            case 0x1D:
            case 0x1E:
            case 0x1F:
            case 0x20:
            case 0x21:
            case 0x22:
                kind = 0x1D;
                break;
            }
            for (int j = 0; j < 37; j++) {
                if (kind == table.entries[j].charKind) {
                    figId = table.entries[j].figId;
                    break;
                }
            }
            fn_80052704(figId, 1);
        }
    }
    scene->unkC = figId;
    for (int i = 0; i < 37; i++) {
        if (gmCheckExistFigure(table.entries[i].figId) == true) {
            s32 id = table.entries[i].figId;
            if (id != figId) {
                scene->unk10.pushBack(id);
            }
        }
    }
}

}
