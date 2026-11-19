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

// TODO: clean up, give variables real names
namespace jj_281DC {
extern "C" void fn_1_281DC(void) {}
}

// TODO: clean up, give variables real names
namespace jj_28BB4 {
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

struct Scene {
    u8 pad[0x18];
    s32 count;
    s32 index;
    s32 level;
    s32 stage;
};
struct Result {
    u8 pad[0x260];
    u32 value;
    u8 pad264[0x2a8];
    u32 value2;
};
struct Global {
    u8 pad[8];
    u8* mode;
    u8 padc[0xc];
    Result* result;
    u8 pad1c[0x20];
    u8* corps;
};
extern "C" {
extern Global* g_GameGlobal;
extern s32 lbl_1_data_7808[];
void gmSubCoin__FUl(s32);
void fn_1_2902C(Scene*);
}
static inline int lower(int x) { return x < 0 ? 0 : x; }
extern "C" void fn_1_28BB4(Scene* self) {
    u32 rem;
    Result* result = g_GameGlobal->result;
    u8* mode = g_GameGlobal->mode;
    rem = result->value % 10;
    u32 val = result->value - rem;
    if (val != 0) val = val / 20 * 10;
    if (rem < 9) ++rem;
    result->value = val + rem;
    if (self->count == 2) {
        u32 rem2 = result->value2 % 10;
        u32 val2 = result->value2 - rem2;
        if (val2 != 0) val2 = val2 / 20 * 10;
        if (rem2 < 9) ++rem2;
        result->value2 = val2 + rem2;
    }
    gmSubCoin__FUl(lbl_1_data_7808[self->index] * 10);
    for (int i = 0; i < self->count; ++i, mode += 0x5c) {
        mode[0x9c] = 1;
        *(u16*)(mode + 0xba) = 0;
    }
    u8* entry;
    u8* corps = g_GameGlobal->corps;
    for (int i = 0; i < corps[3]; ++i) {
        entry = corps + (corps[0x1c] + i) * 0x14;
        if (entry[0x24] >= 0x1d && entry[0x24] <= 0x22)
            entry[0x26] = 3;
        else
            entry[0x26] = 1;
        if (self->level < 3)
            entry[0x28] = lower(entry[0x28] - 10);
    }
    if (self->stage < 9) ++self->stage;
    if (self->level < 3) ++self->level;
    fn_1_2902C(self);
}
}

// TODO: clean up, give variables real names
namespace jj_2902C {

struct Param2902C {
    char _0[0x18];
    int m_unk18;
};

struct Fighters {
    char _0[3];
    u8 m_numFighters;
};

extern "C" void fn_1_2902C(Param2902C* p) {
    gmPlayerInitData* players = g_GameGlobal->m_modeMelee->m_playersInitData;
    Fighters* corps = reinterpret_cast<Fighters*>(g_GameGlobal->m_corps);
    players[0].m_startPointIdx = 0;
    players[1].m_startPointIdx = 1;
    players[2].m_startPointIdx = 2;
    players[3].m_startPointIdx = 3;
    if (p->m_unk18 == 1) {
        if (corps->m_numFighters != 1) {
            players[0].m_startPointIdx = 0;
            players[1].m_startPointIdx = 1;
            players[2].m_startPointIdx = 3;
            players[3].m_startPointIdx = 2;
        }
    } else if (corps->m_numFighters == 1) {
        players[0].m_startPointIdx = 0;
        players[1].m_startPointIdx = 2;
        players[2].m_startPointIdx = 1;
        players[3].m_startPointIdx = 3;
    } else {
        players[0].m_startPointIdx = 0;
        players[1].m_startPointIdx = 2;
        players[2].m_startPointIdx = 3;
        players[3].m_startPointIdx = 1;
    }
}
}

