#include <gf/gf_model.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <snd/snd_system.h>
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

// The panel of the page: comes in, goes out.
static MuAnimIdxData sPanelAnims[2] = {
    { 0.0f, 19.0f, 0, 0, 0x9 },
    { 20.0f, 39.0f, 0, 0, 0x9 },
};
// The bar at the top: comes in, goes out, and the arrows of the next and the previous page flash.
static MuAnimIdxData sHelpAnims[4] = {
    { 0.0f, 9.0f, 0, 0, 0x11 },
    { 10.0f, 19.0f, 0, 0, 0x11 },
    { 20.0f, 23.0f, 0, 0, 0x11 },
    { 30.0f, 33.0f, 0, 0, 0x11 },
};

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

// The last page: the rankings of four players only come with name tags.
// HYPOTHESIS: why the last page depends on the name tags.
static inline int getLastPage() {
    bool hasNames = g_GameGlobal->getNumName() > 0;
    return hasNames + muProcDiagram::Page_3p;
}

// Shows the buttons of the controller of the screen on the page arrows.
static inline void updatePageButtonTex(muProcDiagram* proc) {
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(proc->getFixedControllerID(), &status);
    float frame = muProcDiagram::sPadTypeFrame[status.m_controllerType];
    proc->m_objs[muProcDiagram::Obj_ArrowPrev]->setFrameTex(frame);
    proc->m_objs[muProcDiagram::Obj_ArrowNext]->setFrameTex(frame);
}

// The bar names the page shown, the previous page (left) and the next page (right).
static inline void printPageNames(muProcDiagram* proc) {
    int page = proc->m_page;
    proc->m_msg->printIndex(muProcDiagram::Msg_Page, muProcDiagram::sPageMsg[page].m_title);
    if (page > 0) {
        proc->m_msg->printIndex(muProcDiagram::Msg_PrevPage, muProcDiagram::sPageMsg[page - 1].m_name);
    } else {
        proc->m_msg->printf(muProcDiagram::Msg_PrevPage, "");
    }
    if (page < getLastPage()) {
        proc->m_msg->printIndex(muProcDiagram::Msg_NextPage, muProcDiagram::sPageMsg[page + 1].m_name);
    } else {
        proc->m_msg->printf(muProcDiagram::Msg_NextPage, "");
    }
}

void muProcDiagram::init(nw4r::g3d::ScnGroup* group, int from) {
    m_unk66C_0 = 0;
    m_group = group;
    m_page = Page_1p;
    m_panelState = Panel_Hidden;
    m_helpState = Help_Hidden;
    m_panel->changeProcess1(0x22, 0);
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Panel]->m_sceneModel));
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Bar]));
    m_objs[Obj_Unk137]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    m_objs[Obj_ArrowPrev]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    m_objs[Obj_ArrowNext]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    updatePageButtonTex(this);
    m_unk664 = 0;

    // BYTES: the original reads m_msgData before it makes the messages.
    void* msgData = m_msgData;
    m_msg = MuMsg::create(3, Heaps::MenuInstance, 0x2B);
    m_msg->allocMsgBuf(0x100, 0x6C);
    m_msg->setMsgData(msgData);
    for (int i = 0; i < Msg_PageWndNum; i++) {
        m_msg->attachScnMdlSimple(sBarMsgBone[i][0],
                                  reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Bar]->m_sceneModel),
                                  sBarMsgBone[i][1], 1.0f / 16.0f);
    }
    m_msg->setFontType(Msg_PrevPage, 1);
    m_msg->setFontType(Msg_NextPage, 1);

    // Each page gets the message windows after the ones of the pages before it
    m_pages[Page_1p]->setConf(group, m_msg, Msg_PageWndNum);
    int msgWnd = Msg_PageWndNum + m_pages[Page_1p]->getUseNumOfMsgWnd();
    m_pages[Page_2p]->setConf(group, m_msg, msgWnd);
    msgWnd += m_pages[Page_2p]->getUseNumOfMsgWnd();
    m_pages[Page_3p]->setConf(group, m_msg, msgWnd);
    msgWnd += m_pages[Page_3p]->getUseNumOfMsgWnd();
    m_pages[Page_4p]->setConf(group, m_msg, msgWnd);

    m_unk67C = 0;
    m_objs[Obj_Panel]->setFrameTex(sPageFrame[m_page]);
    m_objs[Obj_Panel]->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
    setHelpArrowVisible(true);
    printPageNames(this);
    setPanelState(Panel_Come);
    setHelpState(Help_Come);
    m_pages[m_page]->begin(0);
}

const u8 muProcDiagram::sBarMsgBone[Msg_PageWndNum][2] = { { 0, 1 }, { 1, 0 }, { 2, 2 } };
const u8 muProcDiagram::sPageFrame[Page_Num] = { 3, 4, 5, 6 };

void muProcDiagramPageBase::setConf(nw4r::g3d::ScnGroup* group, MuMsg* msg, int msgWnd) {
    m_group = group;
}

int muProcDiagramPageBase::getUseNumOfMsgWnd() {
    return 0;
}

void muProcDiagramPageBase::begin(int side) { }

// Shows the arrows to the pages there are next to the page shown (visible) or hides both.
void muProcDiagram::setHelpArrowVisible(bool visible) {
    MuObject* barObj = m_objs[Obj_Bar];
    bool prev = false;
    bool next = false;
    reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_mdls[Mdl_Bar])->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_ArrowPrev]->m_sceneModel));
    reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_mdls[Mdl_Bar])->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_ArrowNext]->m_sceneModel));
    barObj->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    barObj->setFrameVisible(0.0f);
    if (visible) {
        if (m_page > 0) {
            reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_mdls[Mdl_Bar])->PushBack(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_ArrowPrev]->m_sceneModel), "pos01");
            prev = true;
        }
        if (m_page < getLastPage()) {
            reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_mdls[Mdl_Bar])->PushBack(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_ArrowNext]->m_sceneModel), "pos02");
            next = true;
        }
        float frame;
        if (prev) {
            if (next) {
                frame = 2.0f;
            } else {
                frame = 3.0f;
            }
        } else if (next) {
            frame = 1.0f;
        } else {
            frame = 0.0f;
        }
        barObj->setFrameVisible(frame);
    }
}

int muProcDiagram::proc() {
    int ret = 0;
    switch (m_unk664) {
    case 0:
        if (m_panelState != Panel_Come) {
            m_unk664 = 2;
        }
        break;
    case 1:
        if (m_panelState != Panel_Come) {
            m_unk664 = 2;
        }
        break;
    case 2: {
        u32 pad = padUpdate();
        // B on the first page leaves the screen
        bool stay;
        if (m_page <= 0 && (pad & Pad_Cancel)) {
            stay = false;
        } else {
            stay = true;
        }
        if (!stay) {
            setPanelState(Panel_Leave);
            setHelpState(Help_Leave);
            m_unk664 = 3;
            for (int i = 0; i < Page_Num; i++) {
                m_pages[i]->finish();
            }
            m_result = 0x10;
            ret = 1;
        } else {
            movePageMain();
            if (!m_pages[m_page]->isPageChanging()) {
                m_pages[m_page]->controlProc();
            }
        }
        break;
    }
    case 3:
        if (m_panelState == Panel_Hidden) {
            ret = 3;
        }
        break;
    }

    switch (m_panelState) {
    case Panel_Hidden:
        break;
    case Panel_Come:
        if (m_objs[Obj_Panel]->m_modelAnim->m_anmObjChrRes->GetFrame() >= 19.0f) {
            setPanelState(Panel_Shown);
        }
        break;
    case Panel_Shown:
        updatePageButtonTex(this);
        break;
    case Panel_Leave:
        if (m_objs[Obj_Panel]->m_modelAnim->m_anmObjChrRes->GetFrame() >= 39.0f) {
            setPanelState(Panel_Hidden);
        }
        break;
    }
    helpMain();
    for (int i = 0; i < Page_Num; i++) {
        m_pages[i]->main();
    }
    return ret;
}

void muProcDiagramPageBase::finish() { }

bool muProcDiagramPageBase::isPageChanging() {
    return false;
}

// The base page only goes back (B)
bool muProcDiagramPageBase::controlProc() {
    return !(padUpdate() & Pad_Cancel);
}

void muProcDiagramPageBase::main() { }

const u8 muProcDiagram::sPadTypeFrame[4] = { 0, 2, 3, 1 };

void muProcDiagram::setPanelState(int state) {
    if (state == m_panelState) {
        return;
    }
    switch (state) {
    case Panel_Hidden:
        m_objs[Obj_Panel]->m_animIdxData = NULL;
        m_objs[Obj_Panel]->setFrameNode(39.0f);
        m_objs[Obj_Panel]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
        m_objs[Obj_Panel]->setFrameTexSrt(39.0f);
        m_objs[Obj_Panel]->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(0.0f);
        break;
    case Panel_Come:
        m_objs[Obj_Panel]->setAnimIdx(&sPanelAnims[0], false);
        break;
    case Panel_Shown:
        m_objs[Obj_Panel]->m_animIdxData = NULL;
        m_objs[Obj_Panel]->setFrameNode(19.0f);
        m_objs[Obj_Panel]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
        m_objs[Obj_Panel]->setFrameTexSrt(19.0f);
        m_objs[Obj_Panel]->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(0.0f);
        break;
    case Panel_Leave:
        m_objs[Obj_Panel]->setAnimIdx(&sPanelAnims[1], false);
        break;
    }
    m_panelState = state;
}

// When the animation of the bar has stopped, the state after it
void muProcDiagram::helpMain() {
    switch (m_helpState) {
    case Help_Come:
        if (m_objs[Obj_Bar]->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
            setHelpState(Help_Shown);
        }
        break;
    case Help_Leave:
        if (m_objs[Obj_Bar]->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
            setHelpState(Help_Hidden);
        }
        break;
    case Help_TurnNext:
        if (m_objs[Obj_Bar]->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
            setHelpState(Help_Shown);
            setHelpArrowVisible(true);
        }
        break;
    case Help_TurnPrev:
        if (m_objs[Obj_Bar]->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
            setHelpState(Help_Shown);
            setHelpArrowVisible(true);
        }
        break;
    case Help_Unk6:
        if (m_objs[Obj_Bar]->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
            setHelpState(Help_Unk8);
        }
        break;
    case Help_Unk7:
        if (m_objs[Obj_Bar]->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
            setHelpState(Help_Shown);
        }
        break;
    case Help_Unk8:
        break;
    }
}

void muProcDiagram::setHelpState(int state) {
    if (state == m_helpState) {
        return;
    }
    MuObject* bar = m_objs[Obj_Bar];
    switch (state) {
    case Help_Hidden:
        bar->m_animIdxData = NULL;
        bar->setFrameNode(0.0f);
        bar->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
        break;
    case Help_Come:
        bar->setAnimIdx(&sHelpAnims[0], false);
        break;
    case Help_Shown:
        bar->m_animIdxData = NULL;
        bar->setFrameNode(9.0f);
        bar->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
        break;
    case Help_Leave:
        bar->setAnimIdx(&sHelpAnims[1], false);
        break;
    case Help_TurnNext:
        bar->setAnimIdx(&sHelpAnims[2], false);
        break;
    case Help_TurnPrev:
        bar->setAnimIdx(&sHelpAnims[3], false);
        break;
    case Help_Unk8:
        bar->m_animIdxData = NULL;
        bar->setFrameNode(44.0f);
        bar->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
        break;
    }
    m_helpState = state;
}

const u32 muProcDiagram::sPageButtons[4][2] = {
    { 0x520, 0xA40 },
    { 0x520, 0xA40 },
    { 0x200100, 0x400200 },
    { 0x20100, 0x10200 },
};

// 1: a pad pressed a button of the next page, 2: of the previous page, 0: none.
static inline int getPageMove() {
    for (int i = 0; i < 8; i++) {
        gfPadStatus status;
        g_gfPadSystem->getSysPadStatus(i, &status);
        if (status.m_error == 0) {
            if (status.m_buttonsPressedThisFrame.bits & muProcDiagram::sPageButtons[status.m_controllerType][0]) {
                return 1;
            }
            if (status.m_buttonsPressedThisFrame.bits & muProcDiagram::sPageButtons[status.m_controllerType][1]) {
                return 2;
            }
        }
    }
    return 0;
}

// Turns the page: the page shown leaves to one side and the new one comes from the other.
// HYPOTHESIS: the side argument of begin/post (1 left, 2 right, 0 none) follows the direction of the page change.
void muProcDiagram::movePageMain() {
    int oldPage = m_page;
    int move = getPageMove();
    if (move == 1) {
        if (m_page < getLastPage()) {
            g_sndSystem->playSE(static_cast<SndID>(0x23), -1, 0, 0, -1);
            m_page++;
            setHelpState(Help_TurnNext);
            m_objs[Obj_Panel]->setFrameTex(sPageFrame[m_page]);
            m_objs[Obj_Panel]->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
            printPageNames(this);
            m_pages[oldPage]->post(1);
            m_pages[m_page]->begin(2);
        }
    } else if (move == 2) {
        if (m_page > 0) {
            g_sndSystem->playSE(static_cast<SndID>(0x23), -1, 0, 0, -1);
            m_page--;
            setHelpState(Help_TurnPrev);
            m_objs[Obj_Panel]->setFrameTex(sPageFrame[m_page]);
            m_objs[Obj_Panel]->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
            printPageNames(this);
            m_pages[oldPage]->post(2);
            m_pages[m_page]->begin(1);
        }
    }
}

const muProcDiagram::PageMsg muProcDiagram::sPageMsg[Page_Num] = {
    { 0x52, 0x56 },
    { 0x53, 0x57 },
    { 0x54, 0x58 },
    { 0x55, 0x59 },
};

void muProcDiagramPageBase::post(int side) { }

void muProcDiagram::exit() {
    m_msg->detachMuObject(m_objs[Obj_Bar]);
    delete m_msg;
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Panel]->m_sceneModel));
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Bar]));
}

void muProcDiagram::playSE_CursorMove() { }
