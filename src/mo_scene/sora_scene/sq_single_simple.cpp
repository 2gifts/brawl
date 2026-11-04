#include <gf/gf_application.h>
#include <gf/gf_boot_task.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <mu/menu.h>
#include <sc/sc_fig_get_demo.h>
#include <sq/sq_single_simple.h>
#include <types.h>

// HYPOTHESIS: one row of the simple-mode stage table.
struct SqStageRow {
    s32 m_type;
    char _4[0x10];
    s8 m_kind;
    u8 m_flag;
    char _16[2];
    float m_value;
    char _1C[0x34];
};
static_assert(sizeof(SqStageRow) == 0x50, "Class is wrong size!");

extern const double lbl_1_rodata_A28;

// gmAddSeal (the map name); gm_common.cpp defines it by this stand-in name.
extern "C" void fn_80052AC0(int id, int num);
extern "C" int exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(int kind);
extern "C" int exchangeGmCharacterKindToMuCharKind__6muMenuFiii(int kind);

static u8* lbl_1_bss_198C;
static u32 lbl_1_bss_1990;
static gfSceneManager* lbl_1_bss_1994;
static u32 lbl_1_data_7298[5] = {10, 20, 30, 50, 100};

namespace {
void setupData(u8* data, u32 size);
void decisionStage(int kind);
}

int checkStageType(SqStageRow* rows) {
    int result = 0;
    switch (rows->m_type) {
    case 0:
        result = 1;
        break;
    case 1:
        result = 2;
        break;
    case 2:
        result = 4;
        break;
    default:
        break;
    }
    for (u8 i = 0; i < 3; i++) {
        SqStageRow* row = &rows[i];
        if (row->m_value >= lbl_1_rodata_A28) {
            result = 3;
            break;
        }
        if (row->m_flag == 1) {
            result = 5;
            break;
        }
    }
    return result;
}

int checkStageHideChar(SqStageRow* rows) {
    int result = 1;
    for (int i = 1; i < 4; i++) {
        int kind = rows[i - 1].m_kind;
        switch (kind) {
        case Character_PokemonTrainer:
            kind = Character_PokeLizardon_Trainer;
            break;
        case Character_SamusSZeroSuit:
            kind = Character_Samus;
            break;
        case Character_ZeldaShiek:
            kind = Character_Zelda;
            break;
        }
        if (kind != -1) {
            if (gmCheckChKindUseEnable(static_cast<gmCharacterKind>(kind)) == 0) {
                result = 0;
            }
        }
    }
    return result;
}

sqSingleSimple* sqSingleSimple::create() {
    sqSingleSimple* seq = new (Heaps::GlobalMode) sqSingleSimple("sqSingleSimple");
    if (lbl_1_bss_198C == 0) {
        gfBootTask::PreLoadDataBuffer buf = gfBootTask::getPreLoadDataBuffer(1);
        u8* data = buf.addr;
        u32 size = buf.size;
        lbl_1_bss_198C = data;
        lbl_1_bss_1990 = size;
        setupData(data, size);
    }
    return seq;
}

void sqSingleSimple::exit() {
    g_GameGlobal->m_advData->m_flags.m_unk1 = 0;
}

void sqSingleSimple::start() {
    unk20 = gfSceneManager::getInstance()->unk31C;
    if (lbl_1_bss_1994 == 0) {
        lbl_1_bss_1994 = gfSceneManager::getInstance();
    }
    if (unk20 == 0xff) {
        unk8 = 0x18;
        unk10 = 0;
        unk20 = 0;
    } else {
        unk8 = 0;
        unk10 = 0;
        unk2F = g_gfApplication->unk110Value != 0;
        g_GameGlobal->initSelCharData();
        g_GameGlobal->m_selCharData->unk2E_b7 = 1;
    }
}


void sqSingleSimple::setSimpleSetting() {
    gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
    gmSelCharData* sel = g_GameGlobal->m_selCharData;
    gmResultInfo* result = g_GameGlobal->m_resultInfo;
    unk32 = sel->unk2E_b7;
    u8 mid = sel->unk2E_b4;
    unk14 = 0;
    unk31 = mid;
    unk18 = 0;
    unk1CValue = 0;
    unk24 = 0;
    unk28 = 0;
    unk2C = 1;
    unk2D = 1;
    gmInitGlobalModeMelee(melee);
    melee->m_meleeInitData.m_gameRule = 1;
    melee->m_meleeInitData.m_isTeams = 0;
    melee->m_meleeInitData.m_timeLimitFrames = 0x4650;
    melee->m_meleeInitData.m_level = unk32;
    melee->m_meleeInitData.m_scoreDisplay = true;
    melee->m_meleeInitData.m_0x3_2 = false;
    melee->m_meleeInitData.m_globalOffenseRatio = 0x2D;
    melee->m_meleeInitData.m_globalDefenseRatio = 0x5A;
    melee->m_playersInitData[0].m_state = 0;
    u8 kind = sel->m_playersInitData[0].m_characterKind;
    melee->m_playersInitData[0].m_characterKind = kind;
    melee->m_playersInitData[0].m_teamNo = 0;
    u8 color = sel->m_playersInitData[0].m_colorFileNo;
    melee->m_playersInitData[0].m_colorNo = color;
    int selchk = exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(kind);
    melee->m_playersInitData[0].m_colorFileNo = selchk == Selch_SelectNone
        ? 0
        : muMenu::getFighterColorFileNo(selchk, color);
    melee->m_playersInitData[0].m_nameIndex = sel->m_playersInitData[0].m_nameIndex;
    melee->m_playersInitData[0].m_controllerNo = sel->m_playersInitData[0].m_controllerNo;
    melee->m_playersInitData[0].m_playerId = 0;
    result->m_playersResultInfo[0].m_unk234 = 0;
    result->m_playersResultInfo[0].m_unk23C = 0;
    result->m_playersResultInfo[0].m_unk238 = 0;
    result->m_unk1368 = 0;
    result->m_unk136C = 0;
    result->m_unk1370 = 0;
    u8 kind2 = melee->m_playersInitData[0].m_characterKind;
    unk33 = kind2;
    decisionStage(kind2);
    g_GameGlobal->m_advData->m_flags.m_unk1 = 0;
}

void sqSingleSimple::setStageContinueSetting() {
    u32 n;
    u32 tens;
    u32 mod;
    gmResultInfo* result;
    gmSelCharData* sel;
    result = g_GameGlobal->m_resultInfo;
    sel = g_GameGlobal->m_selCharData;
    n = result->m_playersResultInfo[0].m_unk23C;
    mod = n % 10;
    tens = n - mod;
    if (tens != 0) {
        tens = tens / 20 * 10;
    }
    if (mod < 9) {
        mod++;
    }
    result->m_playersResultInfo[0].m_unk23C = tens + mod;
    gmSubCoin(lbl_1_data_7298[unk32] * 10);
    u8 stocks = sel->unk2E_b4;
    unk31 = stocks;
    gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
    melee->m_playersInitData[0].m_stockCount = stocks;
    melee->m_playersInitData[0].m_startDamage = 0;
    melee->m_playersInitData[0].m_startPointIdx = 0;
    melee->m_playersInitData[1].m_startPointIdx = 1;
    melee->m_playersInitData[2].m_startPointIdx = 2;
    melee->m_playersInitData[3].m_startPointIdx = 3;
    if (unk28 < 9) {
        unk28++;
    }
    if (unk24 >= 3) {
        return;
    }
    unk24++;
    if (melee->m_meleeInitData.m_gameMode == Game_Mode_Adventure) {
        return;
    }
    if (melee->m_meleeInitData.m_corpsKind == 0) {
        for (int i = 1; i < 4; i++) {
            if (melee->m_playersInitData[i].m_state == 1
                && (melee->m_meleeInitData.m_isTeams != 1
                    || (u8)melee->m_playersInitData[0].m_teamNo != (u8)melee->m_playersInitData[i].m_teamNo)) {
                int rank = (int)melee->m_playersInitData[i].m_cpuRank - 10;
                melee->m_playersInitData[i].m_cpuRank = rank < 0 ? 0 : rank;
            }
        }
    } else {
        gmGlobalCorps* corps = g_GameGlobal->m_corps;
        int left = (int)(u8)corps->m_cpuRank - 10;
        corps->m_cpuRank = left < 0 ? 0 : left;
    }
}

void sqSingleSimple::updateHiScore(int charKind, int flag, u32 score) {
    gmGlobalRecord::CharaNumData* data =
        g_GameGlobal->getGlobalRecordCharaNumDatap((u8)exchangeGmCharacterKindToMuCharKind__6muMenuFiii(charKind));
    if (data->unkA0 < score) {
        data->unkA0 = score;
    }
    if (flag == 0) {
        return;
    }
    if (data->unk9C_b10 < unk32 + 1) {
        data->unk9C_b10 = unk32 + 1;
    }
    if (isClearGetterOpen(0x29) == 1) {
        if (unk32 == 0) {
            fn_80052AC0(0x78, 1);
        }
    }
    int ids[5] = {0x29, 0x2D, 0x25, 0x3E, 7};
    for (int i = 0; i < 5; i++) {
        int id = ids[i];
        if (isClearGetterOpen(id) != 0) {
            continue;
        }
        int open = 0;
        switch (id) {
        case 0x29:
            if (unk32 == 0) {
                open = 1;
            }
            break;
        case 0x2D:
            if (unk32 == 1u) {
                open = 1;
            }
            break;
        case 0x25:
            if (unk32 == 2u) {
                open = 1;
            }
            break;
        case 0x3E:
            if (unk32 == 3u) {
                open = 1;
            }
            break;
        case 7:
            if (unk32 == 4u) {
                open = 1;
            }
            break;
        }
        if (open != 0) {
            setClearGetterOpen(id);
        }
    }
}

// gmAddFigure (the map name); sq_single_allstar.cpp calls it by this stand-in name.
extern "C" void fn_80052704(u16 id, int num);

extern struct {
    struct {
        int unk0;
        int unk4;
        u8 unk8;
        u8 unk9[3];
    } unk0[35];
    u8 unk1A4[4];
} lbl_1_data_72B0;

extern const struct {
    struct {
        int unk0;
        int unk4;
    } unk0[39];
} lbl_1_rodata_AC8;

namespace {

__declspec(force_export) void setHitTbl(int kind) {
    switch (kind) {
    case 0x10:
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
    case 0x48:
        kind = 0x1D;
        break;
    case 3:
    case 4:
    case 0x49:
        kind = 3;
        break;
    case 0xE:
    case 0xF:
    case 0x4A:
        kind = 0xE;
        break;
    }
    for (int i = 0; i < 35; i++) {
        if (kind == lbl_1_data_72B0.unk0[i].unk0) {
            lbl_1_data_72B0.unk0[i].unk8 = 1;
            return;
        }
    }
}

__declspec(force_export) void setSimpleGetFigure(int kind) {
    scFigureGetDemo* scene = static_cast<scFigureGetDemo*>(lbl_1_bss_1994->searchScene("scFigureGetDemo"));
    switch (kind) {
    case 0x11:
    case 0x12:
        kind = 0x10;
        break;
    case 0x1E:
        kind = 0x1D;
        break;
    case 0x20:
        kind = 0x1F;
        break;
    case 0x22:
        kind = 0x21;
        break;
    }
    __typeof__(lbl_1_rodata_AC8) table = lbl_1_rodata_AC8;
    int figure = 0;
    for (int i = 0; i < 39; i++) {
        if (kind == table.unk0[i].unk0) {
            figure = table.unk0[i].unk4;
            break;
        }
    }
    fn_80052704(figure, 1);
    switch (figure) {
    case 0x43:
    case 0x45:
    case 0x47:
        figure = 0x41;
        fn_80052704(0x41, 1);
        break;
    }
    scene->unkC = figure;
    for (int i = 0; i < 39; i++) {
        if (gmCheckExistFigure(table.unk0[i].unk4) == true) {
            int id = table.unk0[i].unk4;
            if (id != 0x43 && id != 0x45 && id != 0x47 && id != figure) {
                scene->unk10.pushBack(id);
            }
        }
    }
    if (gmCheckExistFigure(0x41) == true && figure != 0x41) {
        scene->unk10.pushBack(0x41);
    }
}

__declspec(force_export) void setupChallenger() {
    int colorFile;
    int color;
    gmSelCharData* sel;
    gmPlayerInitData* player;
    player = g_GameGlobal->unkC;
    sel = g_GameGlobal->m_selCharData;
    gmInitPlayerDataDefault(player, 0);
    player->m_state = 0;
    u8 kind = sel->m_playersInitData[0].m_characterKind;
    player->m_characterKind = kind;
    player->m_teamNo = 0;
    color = (u8)sel->m_playersInitData[0].m_colorFileNo;
    player->m_colorNo = color;
    int selection = exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(kind);
    if (selection == Selch_SelectNone) {
        colorFile = 0;
    } else {
        colorFile = muMenu::getFighterColorFileNo(selection, color);
    }
    player->m_colorFileNo = colorFile;
    player->m_controllerNo = sel->m_playersInitData[0].m_controllerNo;
    u8 name = sel->m_playersInitData[0].m_nameIndex;
    player->m_nameIndex = name;
    if (name != 120) {
        g_GameGlobal->getNameUTF16(name, player->m_name);
    }
    g_GameGlobal->m_setRule->m_ctrlNamePlayerNo = (u8)player->m_controllerNo - 1;
    g_GameGlobal->m_advData->m_flags.m_unk1 = 0;
}

}
