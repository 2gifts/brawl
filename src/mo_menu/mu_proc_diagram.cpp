#include <gm/gm_global.h>
#include <memory.h>
#include <stdio.h>
#include <types.h>

#include <mu/mu_proc_diagram.h>

// muMenu helper of main.dol whose symbol takes three ints; the callers set only the first (as the matched units do)
extern "C" bool isCharUsable__6muMenuFiii(int muCharKind);

// HYPOTHESIS: GameGlobal tests whether the language of the game is 0 (main.dol 0x8004D9E8, lbz 0x82A == 0); its map name is
// unclear (isLanguage / isLanguageJp). The diagrams then give distances in metres, otherwise in feet.
extern "C" int fn_8004D9E8(GameGlobal* global);

static u32 muDiagGetTotalBeatChar(MuCharKind kind);
static u32 muDiagGetTotalBeatName(int nameNo);
static u32 muDiagGetTotalFallChar(MuCharKind kind);
static u32 muDiagGetTotalFallName(int nameNo);
static u32 muDiagGetSuicideChar(MuCharKind kind);
static u32 muDiagGetSuicideName(int nameNo);
static u32 muDiagGetHitRateChar(MuCharKind kind);
static u32 muDiagGetHitRateName(int nameNo);
static u32 muDiagGetGiveDamageChar(MuCharKind kind);
static u32 muDiagGetGiveDamageName(int nameNo);
static u32 muDiagGetTakeDamageChar(MuCharKind kind);
static u32 muDiagGetTakeDamageName(int nameNo);
static u32 muDiagGetRecoverDamageChar(MuCharKind kind);
static u32 muDiagGetRecoverDamageName(int nameNo);
static u32 muDiagGetMaxDamageChar(MuCharKind kind);
static u32 muDiagGetMaxDamageName(int nameNo);
static u32 muDiagGetRoundNumChar(MuCharKind kind);
static u32 muDiagGetRoundNumName(int nameNo);
static u32 muDiagGetChampNumChar(MuCharKind kind);
static u32 muDiagGetChampNumName(int nameNo);
static u32 muDiagGetKetsuNumChar(MuCharKind kind);
static u32 muDiagGetKetsuNumName(int nameNo);
static u32 muDiagGetPlayTimeChar(MuCharKind kind);
static u32 muDiagGetPlayTimeName(int nameNo);
static u32 muDiagGetUseRateChar(MuCharKind kind);
static u32 muDiagGetUseRateName(int nameNo);
static u32 muDiagGetJoinAverageChar(MuCharKind kind);
static u32 muDiagGetJoinAverageName(int nameNo);
static u32 muDiagGetWalkDistanceChar(MuCharKind kind);
static u32 muDiagGetWalkDistanceName(int nameNo);
static u32 muDiagGetJumpDistanceChar(MuCharKind kind);
static u32 muDiagGetJumpDistanceName(int nameNo);
static u32 muDiagGetFallDistanceChar(MuCharKind kind);
static u32 muDiagGetFallDistanceName(int nameNo);
static u32 muDiagGetBlowDistanceChar(MuCharKind kind);
static u32 muDiagGetBlowDistanceName(int nameNo);
static u32 muDiagGetGetCoinChar(MuCharKind kind);
static u32 muDiagGetGetCoinName(int nameNo);
static u32 muDiagGetMakeDropCoinChar(MuCharKind kind);
static u32 muDiagGetMakeDropCoinName(int nameNo);
static u32 muDiagGetDropCoinChar(MuCharKind kind);
static u32 muDiagGetDropCoinName(int nameNo);
static u32 muDiagGetFallButtocksChar(MuCharKind kind);
static u32 muDiagGetFallButtocksName(int nameNo);
static u32 muDiagGetSwimDistanceChar(MuCharKind kind);
static u32 muDiagGetSwimDistanceName(int nameNo);
static u32 muDiagGetSwimFallChar(MuCharKind kind);
static u32 muDiagGetSwimFallName(int nameNo);

static const muDiagDataInfo sDataInfo[MuDiagData_Num] = {
    { muDiagGetTotalBeatChar, muDiagGetTotalBeatName, 1, MuDiagValue_Number, 0x5A, 0x75 },
    { muDiagGetTotalFallChar, muDiagGetTotalFallName, 1, MuDiagValue_Number, 0x5B, 0x76 },
    { muDiagGetSuicideChar, muDiagGetSuicideName, 1, MuDiagValue_Number, 0x5C, 0x77 },
    { muDiagGetHitRateChar, muDiagGetHitRateName, 2, MuDiagValue_Hundredths, 0x5D, 0x78 },
    { muDiagGetGiveDamageChar, muDiagGetGiveDamageName, 2, MuDiagValue_Number, 0x5E, 0x79 },
    { muDiagGetTakeDamageChar, muDiagGetTakeDamageName, 2, MuDiagValue_Number, 0x5F, 0x7A },
    { muDiagGetRecoverDamageChar, muDiagGetRecoverDamageName, 2, MuDiagValue_Number, 0x60, 0x7B },
    { muDiagGetMaxDamageChar, muDiagGetMaxDamageName, 2, MuDiagValue_Number, 0x61, 0x7C },
    { muDiagGetRoundNumChar, muDiagGetRoundNumName, 1, MuDiagValue_Number, 0x62, 0x7D },
    { muDiagGetChampNumChar, muDiagGetChampNumName, 1, MuDiagValue_Number, 0x63, 0x7E },
    { muDiagGetKetsuNumChar, muDiagGetKetsuNumName, 1, MuDiagValue_Number, 0x64, 0x7F },
    { muDiagGetPlayTimeChar, muDiagGetPlayTimeName, 6, MuDiagValue_MinSec, 0x65, 0x80 },
    { muDiagGetUseRateChar, muDiagGetUseRateName, 2, MuDiagValue_Hundredths, 0x66, 0x81 },
    { muDiagGetJoinAverageChar, muDiagGetJoinAverageName, 3, MuDiagValue_Hundredths, 0x67, 0x82 },
    { muDiagGetWalkDistanceChar, muDiagGetWalkDistanceName, 4, MuDiagValue_Distance, 0x68, 0x83 },
    { muDiagGetJumpDistanceChar, muDiagGetJumpDistanceName, 4, MuDiagValue_Distance, 0x69, 0x84 },
    { muDiagGetFallDistanceChar, muDiagGetFallDistanceName, 4, MuDiagValue_Distance, 0x6A, 0x85 },
    { muDiagGetBlowDistanceChar, muDiagGetBlowDistanceName, 4, MuDiagValue_Distance, 0x6B, 0x86 },
    { muDiagGetGetCoinChar, muDiagGetGetCoinName, 5, MuDiagValue_Number, 0x6C, 0x87 },
    { muDiagGetMakeDropCoinChar, muDiagGetMakeDropCoinName, 5, MuDiagValue_Number, 0x6D, 0x88 },
    { muDiagGetDropCoinChar, muDiagGetDropCoinName, 5, MuDiagValue_Number, 0x6E, 0x89 },
    { muDiagGetFallButtocksChar, muDiagGetFallButtocksName, 1, MuDiagValue_Number, 0x6F, 0x8A },
    { muDiagGetSwimDistanceChar, muDiagGetSwimDistanceName, 4, MuDiagValue_Distance, 0x70, 0x8B },
    { muDiagGetSwimFallChar, muDiagGetSwimFallName, 1, MuDiagValue_Number, 0x71, 0x8C },
    { NULL, NULL, 0, 4, 0x72, 0x8D },
    { NULL, NULL, 0, 4, 0x73, 0x8E },
    { NULL, NULL, 0, 4, 0x74, 0x8F },
};

// The message of each unit (MuDiagDataInfo::m_unitKind) written after a value; -1 is none.
// HYPOTHESIS: 1 counts, 2 percent, 3 players, 4 metres, 5 coins, 6 minutes (from the kinds that use them).
static const u32 sUnitMsgId[7] = { 0xFFFFFFFF, 0x90, 0x92, 0x91, 0x94, 0x93, 0xFFFFFFFF };

static const u8 sCharKindOrder[Menu_Char_Sonic + 1] = {
    Menu_Char_Mario, Menu_Char_Donkey, Menu_Char_Link, Menu_Char_Samus, Menu_Char_Yoshi,
    Menu_Char_Kirby, Menu_Char_Fox, Menu_Char_Pikachu, Menu_Char_Luigi, Menu_Char_Captain,
    Menu_Char_Ness, Menu_Char_Koopa, Menu_Char_Peach, Menu_Char_Zelda, Menu_Char_Popo,
    Menu_Char_Marth, Menu_Char_GameWatch, Menu_Char_Falco, Menu_Char_Ganon, Menu_Char_Wario,
    Menu_Char_MetaKnight, Menu_Char_Pit, Menu_Char_Pikmin, Menu_Char_Lucas, Menu_Char_Diddy,
    Menu_Char_PokeTrainer, Menu_Char_Dedede, Menu_Char_Lucario, Menu_Char_Ike, Menu_Char_Robot,
    Menu_Char_Purin, Menu_Char_ToonLink, Menu_Char_Wolf, Menu_Char_Snake, Menu_Char_Sonic,
};

MuCharKind muDiagGetMuCharKindRaw(int no) {
    return static_cast<MuCharKind>(sCharKindOrder[no]);
}

int muDiagGetMuCharKindRawNo(MuCharKind kind) {
    int no;
    for (no = 0; no < Menu_Char_Sonic + 1; no++) {
        if (kind == sCharKindOrder[no]) {
            break;
        }
    }
    return no;
}

// The rates are percentages with two decimals (written by MuDiagValue_Hundredths)
static u32 muDiagGetHitRateChar(MuCharKind kind) {
    gmDiagramResult* diagram = &g_GameGlobal->getGlobalRecordCharaNumDatap(kind)->m_diagram;
    float rate;
    if (diagram->m_totalFall != 0) {
        rate = 100.0f * (100.0f * (static_cast<float>(diagram->m_totalBeat) / static_cast<float>(diagram->m_totalFall)));
    } else {
        rate = 0.0f;
    }
    return rate;
}

static u32 muDiagGetHitRateName(int nameNo) {
    gmDiagramResult* diagram = &g_GameGlobal->getGlobalRecordNameDatap(nameNo)->m_diagram;
    float rate;
    if (diagram->m_totalFall != 0) {
        rate = 100.0f * (100.0f * (static_cast<float>(diagram->m_totalBeat) / static_cast<float>(diagram->m_totalFall)));
    } else {
        rate = 0.0f;
    }
    return rate;
}

static u32 muDiagGetUseRateChar(MuCharKind kind) {
    float total = 0.0f;
    for (int i = 0; i < Menu_Char_Sonic + 1; i++) {
        total += g_GameGlobal->getGlobalRecordCharaNumDatap(i)->m_diagram.m_playTime;
    }
    float rate;
    if (total > 0.0f) {
        rate = 100.0f * (100.0f * (g_GameGlobal->getGlobalRecordCharaNumDatap(kind)->m_diagram.m_playTime / total));
    } else {
        rate = 0.0f;
    }
    return rate;
}

static u32 muDiagGetUseRateName(int nameNo) {
    float total = 0.0f;
    for (int i = 0; i < MuDiag_NameNum; i++) {
        total += g_GameGlobal->getGlobalRecordNameDatap(i)->m_diagram.m_playTime;
    }
    float rate;
    if (total > 0.0f) {
        rate = 100.0f * (100.0f * (g_GameGlobal->getGlobalRecordNameDatap(nameNo)->m_diagram.m_playTime / total));
    } else {
        rate = 0.0f;
    }
    return rate;
}

// The average number of players of the matches, clamped to 0.00-4.00
static u32 muDiagGetJoinAverageChar(MuCharKind kind) {
    gmDiagramResult* diagram = &g_GameGlobal->getGlobalRecordCharaNumDatap(kind)->m_diagram;
    float average;
    if (diagram->m_roundNum != 0) {
        average = 100.0f * (static_cast<float>(diagram->m_joinTotal) / static_cast<float>(diagram->m_roundNum));
    } else {
        average = 0.0f;
    }
    if (average > 400.0f) {
        average = 400.0f;
    }
    if (average < 0.0f) {
        average = 0.0f;
    }
    return average;
}

static u32 muDiagGetJoinAverageName(int nameNo) {
    gmDiagramResult* diagram = &g_GameGlobal->getGlobalRecordNameDatap(nameNo)->m_diagram;
    float average;
    if (diagram->m_roundNum != 0) {
        average = 100.0f * (static_cast<float>(diagram->m_joinTotal) / static_cast<float>(diagram->m_roundNum));
    } else {
        average = 0.0f;
    }
    if (average > 400.0f) {
        average = 400.0f;
    }
    if (average < 0.0f) {
        average = 0.0f;
    }
    return average;
}

static u32 muDiagGetSuicideChar(MuCharKind kind) {
    return g_GameGlobal->getGlobalRecordCharaNumDatap(kind)->m_diagram.m_suicide;
}

static u32 muDiagGetSuicideName(int nameNo) {
    return g_GameGlobal->getGlobalRecordNameDatap(nameNo)->m_diagram.m_suicide;
}

static u32 muDiagGetTotalBeatChar(MuCharKind kind) {
    u32 count = 0;
    for (int i = 0; i < Menu_Char_Sonic + 1; i++) {
        if (isCharUsable__6muMenuFiii(i)) {
            count += g_GameGlobal->getGlobalRecordCharaNumDatap(kind)->m_beatCount[i];
        }
    }
    return count;
}

static u32 muDiagGetTotalBeatName(int nameNo) {
    return g_GameGlobal->getGlobalRecordNameDatap(nameNo)->m_beatTotal;
}

static u32 muDiagGetTotalFallChar(MuCharKind kind) {
    return g_GameGlobal->getGlobalRecordCharaNumDatap(kind)->m_fallCount;
}

static u32 muDiagGetTotalFallName(int nameNo) {
    return g_GameGlobal->getGlobalRecordNameDatap(nameNo)->m_fallTotal;
}

#define MU_DIAG_GETTERS(NAME, FIELD)                                                \
    static u32 muDiagGet##NAME##Char(MuCharKind kind) {                             \
        return g_GameGlobal->getGlobalRecordCharaNumDatap(kind)->m_diagram.FIELD;   \
    }                                                                               \
    static u32 muDiagGet##NAME##Name(int nameNo) {                                  \
        return g_GameGlobal->getGlobalRecordNameDatap(nameNo)->m_diagram.FIELD;     \
    }

MU_DIAG_GETTERS(GiveDamage, m_giveDamage)
MU_DIAG_GETTERS(TakeDamage, m_takeDamage)
MU_DIAG_GETTERS(RecoverDamage, m_recoverDamage)
MU_DIAG_GETTERS(MaxDamage, m_maxDamage)
MU_DIAG_GETTERS(RoundNum, m_roundNum)
MU_DIAG_GETTERS(ChampNum, m_champNum)
MU_DIAG_GETTERS(KetsuNum, m_ketsuNum)

// minutes
static u32 muDiagGetPlayTimeChar(MuCharKind kind) {
    return g_GameGlobal->getGlobalRecordCharaNumDatap(kind)->m_diagram.m_playTime / 60;
}

static u32 muDiagGetPlayTimeName(int nameNo) {
    return g_GameGlobal->getGlobalRecordNameDatap(nameNo)->m_diagram.m_playTime / 60;
}

MU_DIAG_GETTERS(WalkDistance, m_walkDistance)
MU_DIAG_GETTERS(JumpDistance, m_jumpDistance)
MU_DIAG_GETTERS(FallDistance, m_fallDistance)
MU_DIAG_GETTERS(BlowDistance, m_blowDistance)
MU_DIAG_GETTERS(GetCoin, m_getCoin)
MU_DIAG_GETTERS(MakeDropCoin, m_makeDropCoin)
MU_DIAG_GETTERS(DropCoin, m_dropCoin)
MU_DIAG_GETTERS(FallButtocks, m_fallButtocks)
MU_DIAG_GETTERS(SwimDistance, m_swimDistance)
MU_DIAG_GETTERS(SwimFall, m_swimFall)

u32 muDiagGetDataChar(MuDiagDataKind kind, MuCharKind chara) {
    if (sDataInfo[kind].m_getChar != nullptr) {
        return sDataInfo[kind].m_getChar(chara);
    }
    return 0;
}

u32 muDiagGetDataName(MuDiagDataKind kind, int nameNo) {
    if (sDataInfo[kind].m_getName != nullptr) {
        return sDataInfo[kind].m_getName(nameNo);
    }
    return 0;
}

u8 muDiagGetDataUnitKind(MuDiagDataKind kind) {
    return sDataInfo[kind].m_unitKind;
}

u32 muDiagGetDataTitleMsgID(MuDiagDataKind kind) {
    return sDataInfo[kind].m_titleMsgId;
}

u32 muDiagGetDataExplMsgID(MuDiagDataKind kind) {
    return sDataInfo[kind].m_explMsgId;
}

// Writes a value of the record kind to buf and gives the message of its unit (-1 for none). Distances are recorded in
// tenths of the unit of the records and written in metres (kilometres from 1000 m) or in feet (miles from one mile).
// HYPOTHESIS: the parameter roles; max is the largest value the screen shows.
void muDiagTransformDataValue(char* buf, u32* unitMsgId, MuDiagDataKind kind, u32 value, u32 max) {
    int unitKind = sDataInfo[kind].m_unitKind;
    if (value > max) {
        value = max;
    }
    u32 msgId = sUnitMsgId[unitKind];
    switch (sDataInfo[kind].m_valueKind) {    case MuDiagValue_Hundredths:
        sprintf(buf, "%.2f", value / 100.0f);
        break;
    case MuDiagValue_MinSec:
        sprintf(buf, "%d:%02d", value / 60, value % 60);
        break;
    case MuDiagValue_Distance:
        value *= 10;
        if (fn_8004D9E8(g_GameGlobal)) {
            if (value >= 100000) {
                msgId = 0x95;
                value /= 100000;
            } else {
                value /= 100;
            }
        } else if (value > 160934) {
            msgId = 0x95;
            value /= 160934;
        } else {
            value = value / 30.4788;
        }
        sprintf(buf, "%d", value);
        break;
    case MuDiagValue_Number:
        sprintf(buf, "%d", value);
        break;
    }
    *unitMsgId = msgId;
}

muProcDiagram::muProcDiagram(MuObject** objs, nw4r::g3d::G3dObj** mdls, void* msgData) : muProcMenu(objs, mdls) {
    m_itemCount = 0;
    m_msgData = msgData;
    m_pages[Page_1p] = new (Heaps::MenuInstance) muProcDiagram1p(m_objs, m_mdls);
    m_pages[Page_2p] = new (Heaps::MenuInstance) muProcDiagram2p(m_objs, m_mdls);
    m_pages[Page_3p] = new (Heaps::MenuInstance) muProcDiagram3p(m_objs, m_mdls);
    m_pages[Page_4p] = new (Heaps::MenuInstance) muProcDiagram4p(m_objs, m_mdls);
}

muProcDiagram::~muProcDiagram() {
    for (int i = 0; i < Page_Num; i++) {
        delete m_pages[i];
    }
}

void muProcDiagramPageBase::setConf(nw4r::g3d::ScnGroup* group, MuMsg* msg, int msgWnd) {
    m_group = group;
}

int muProcDiagramPageBase::getUseNumOfMsgWnd() {
    return 0;
}

void muProcDiagramPageBase::begin() { }

void muProcDiagramPageBase::finish() { }

bool muProcDiagramPageBase::isPageChanging() {
    return false;
}

// The base page only goes back (B)
bool muProcDiagramPageBase::controlProc() {
    return !(padUpdate() & Pad_Cancel);
}

void muProcDiagramPageBase::main() { }

void muProcDiagramPageBase::post() { }

void muProcDiagram::exit() {
    m_msg->detachMuObject(m_objs[Obj_MsgWnd]);
    delete m_msg;
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Unk111]->m_sceneModel));
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Unk13]));
}

void muProcDiagram::playSE_CursorMove() { }
