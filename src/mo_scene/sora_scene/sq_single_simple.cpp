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

// TODO: clean up, give variables real names
namespace jj_2636C {
extern "C" {
extern void* g_GameGlobal;
extern u8 lbl_1_data_7548[];
extern f64 lbl_1_rodata_A28;
extern f64 lbl_1_rodata_A30;
void* getInstance__14gfSceneManagerFv(void);
void* searchScene__14gfSceneManagerFPCc(void*, const char*);
int exchangeGmCharacterKind2MuStockchkind__6muMenuFi(int);
void* memset(void*, int, unsigned long);
}

struct Entry {
    s32 kind;
    s32 val;
};

struct Data {
    s32 a;
    s32 b;
    s32 count1;
    Entry e1[3];
    s32 count2;
    Entry e2[2];
};

extern "C" void fn_1_2636C(void* arg0) {
    Data d;
    u8* r31;
    u8* r30;
    s32* p29;
    s32* p28;
    u8* r27;
    s32 cnt26;
    s32 cnt25;
    s32 r24;
    s32 res;
    f64 f31;
    f64 f30;

    r30 = *(u8**)((u8*)g_GameGlobal + 8);
    d.a = 0;
    d.b = 0;
    d.count1 = 1;
    d.count2 = 0;
    memset(d.e1, 0, 0x18);
    memset(d.e2, 0, 0x10);
    d.a = *(s32*)((u8*)arg0 + 0x14);
    d.b = 0;
    if (*(u16*)(r30 + 0x1A) == 0x38) {
        d.b = 2;
    }
    if (*(u8*)(r30 + 0x14) == 4) {
        u8* t = *(u8**)((u8*)g_GameGlobal + 0x3C);
        d.b = 1;
        d.count1 = 10;
        res = exchangeGmCharacterKind2MuStockchkind__6muMenuFi(t[0xA]);
        d.e1[0].kind = res;
        d.e1[0].val = 0;
    } else {
        f30 = lbl_1_rodata_A28;
        f31 = lbl_1_rodata_A30;
        p29 = &d.a;
        p28 = &d.a;
        r31 = r30 + 0x5C;
        r27 = r30 + 0x10F;
        cnt26 = 0;
        cnt25 = 0;
        r24 = 1;
        do {
            if (*(u8*)(r31 + 0x99) == 1) {
                s32 r3 = exchangeGmCharacterKind2MuStockchkind__6muMenuFi(*(u8*)(r31 + 0x98));
                if (*(u8*)(r30 + 0x13) == 1 && *(u8*)(r30 + 0xA3) == *(u8*)(r31 + 0xA3)) {
                    p29[10] = r3;
                    p29 += 2;
                    cnt25 += 1;
                } else {
                    p28[3] = r3;
                    r3 = 0;
                    if (*(f32*)(r31 + 0xD8) >= f30) {
                        r3 = 2;
                    }
                    if (*(f32*)(r31 + 0xD8) <= f31) {
                        r3 = 1;
                    }
                    if ((((u32)*r27 >> 4) & 1U) == 1U) {
                        r3 = 3;
                    }
                    p28[4] = r3;
                    p28 += 2;
                    cnt26 += 1;
                }
            }
            r24 += 1;
            r31 += 0x5C;
            r27 += 0x5C;
        } while (r24 < 4);
        d.count1 = cnt26;
        d.count2 = cnt25;
    }
    u8* sc = (u8*)searchScene__14gfSceneManagerFPCc(getInstance__14gfSceneManagerFv(), (const char*)lbl_1_data_7548);
    *(Data*)(sc + 0x35C) = d;
}
}

// TODO: clean up, give variables real names
namespace jj_27574 {

extern "C" u32* getCharaAppFlag__10GameGlobalFv(void*);
extern "C" u8* getGlobalRecordCharaNumDatap__10GameGlobalFi(void*, int);
extern "C" u32* getInfoAppFlag__10GameGlobalFv(void*);
extern "C" u32 gmCheckTargetBreakHideElement__Fv();

extern "C" void fn_1_27574(u8 a, u32 b, u32 c, int d) {
    if (!(getInfoAppFlag__10GameGlobalFv(g_GameGlobal)[5] & 0x100000) &&
        ((u32)((*((u8*)g_GameGlobal->m_selCharData + 0x2e) >> 2) & 7) == 1) && c >= 4) {
        getInfoAppFlag__10GameGlobalFv(g_GameGlobal)[5] |= 0x100000;
    }
    if (gmCheckTargetBreakHideElement__Fv() != 1) {
        if (!(getCharaAppFlag__10GameGlobalFv(g_GameGlobal)[0] & 0x80000000)) {
            int count = 0;
            for (int i = 0; i < 0x23; i++) {
                if ((*(u16*)(getGlobalRecordCharaNumDatap__10GameGlobalFi(g_GameGlobal, i) + 0x9c) >> 8) & 7)
                    count++;
            }
            if (count >= 10) {
                getCharaAppFlag__10GameGlobalFv(g_GameGlobal)[0] |= 0x40000000;
                return;
            }
        }
        if (!(getCharaAppFlag__10GameGlobalFv(g_GameGlobal)[0] & 0x02000000) &&
            (a == 2 || (u8)(a + 0xf2) <= 1) && c >= 2) {
            getCharaAppFlag__10GameGlobalFv(g_GameGlobal)[0] |= 0x01000000;
            return;
        }
        if (!(getCharaAppFlag__10GameGlobalFv(g_GameGlobal)[0] & 0x2000) && c >= 1 && b <= 0xa8c0) {
            getCharaAppFlag__10GameGlobalFv(g_GameGlobal)[0] |= 0x1000;
            return;
        }
        if (!(getCharaAppFlag__10GameGlobalFv(g_GameGlobal)[0] & 0x80) && d == 0) {
            getCharaAppFlag__10GameGlobalFv(g_GameGlobal)[0] |= 0x40;
            return;
        }
        if (!(getCharaAppFlag__10GameGlobalFv(g_GameGlobal)[1] & 0x20) &&
            ((getInfoAppFlag__10GameGlobalFv(g_GameGlobal)[4] >> 26) & 1) == 1) {
            getCharaAppFlag__10GameGlobalFv(g_GameGlobal)[1] |= 0x10;
            return;
        }
        if (!(getCharaAppFlag__10GameGlobalFv(g_GameGlobal)[0] & 0x10))
            getCharaAppFlag__10GameGlobalFv(g_GameGlobal)[0] |= 8;
    }
}
}

