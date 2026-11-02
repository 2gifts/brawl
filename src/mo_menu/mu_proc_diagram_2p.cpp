#include <gf/gf_heap_manager.h>
#include <memory.h>
#include <mu/menu.h>
#include <snd/snd_system.h>
#include <types.h>

#include <mu/mu_proc_diagram.h>

// qsort of the library of the program
extern "C" void qsort(void* base, unsigned long count, unsigned long size, int (*compare)(const void*, const void*));

// muMenu helpers of main.dol whose symbols take three ints; the callers set only the first (as the matched units do)
extern "C" bool isCharUsable__6muMenuFiii(int muCharKind);
extern "C" int exchangeMuCharKindToMuStockchkind__6muMenuFiii(int muCharKind);

// The characters with the higher value first, then in the order of the diagrams.
static int compare(const void* a, const void* b) {
    const muProcDiagram2p::Rank* rankA = static_cast<const muProcDiagram2p::Rank*>(a);
    const muProcDiagram2p::Rank* rankB = static_cast<const muProcDiagram2p::Rank*>(b);
    int diff = rankB->m_value - rankA->m_value;
    return diff != 0 ? diff : muDiagGetMuCharKindRawNo(rankA->m_chara) - muDiagGetMuCharKindRawNo(rankB->m_chara);
}

void muProcDiagram2p::setConf(nw4r::g3d::ScnGroup* group, MuMsg* msg, int msgWnd) {
    m_group = group;
    m_state = State_Hidden;
    m_msgWnd = msgWnd;
    m_msg = msg;
    m_kind = MuDiagData_TotalBeat;
    m_index = 0;
    msg->attachScnMdlSimple(msgWnd, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Page]->m_sceneModel), 5,
                            1.0f / 16.0f);
    m_msg->setFontColor(m_msgWnd, 0x37, 0x37, 0x37, 0xFF);
    m_msg->attachScnMdlSimple(m_msgWnd + Msg_Expl,
                              reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Page]->m_sceneModel), 6,
                              1.0f / 16.0f);
    m_msg->setFontColor(m_msgWnd + Msg_Expl, 0x37, 0x37, 0x37, 0xFF);
    for (int i = 0; i < RankNum; i++) {
        int wnd = i + m_msgWnd + Msg_Value;
        m_msg->attachScnMdlSimple(wnd, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Page]->m_sceneModel), i,
                                  1.0f / 16.0f);
        m_msg->setFontColor(wnd, 0xFF, 0xFF, 0xFF, 0xFF);
        m_msg->setHScale(wnd, 0.8f);
        m_msg->setAlignMode(wnd, MuMsg::AlignMode(2));
        m_msg->setFontWidthModeFixed(wnd, 18.0f);
    }

    // The five best characters of each record, from the characters that can be used
    m_ranks = static_cast<Rank(*)[RankNum]>(gfHeapManager::alloc(Heaps::MenuInstance, sizeof(*m_ranks) * KindNum));
    for (int kind = 0; kind < KindNum; kind++) {
        Rank ranks[Menu_Char_Sonic + 1];
        Rank* dst = m_ranks[kind];
        Rank* rank = ranks;
        int count = 0;
        for (int chara = 0; chara < Menu_Char_Sonic + 1; chara++) {
            if (isCharUsable__6muMenuFiii(chara)) {
                rank->m_chara = static_cast<MuCharKind>(chara);
                rank->m_value = muDiagGetDataChar(static_cast<MuDiagDataKind>(kind), static_cast<MuCharKind>(chara));
                rank++;
                count++;
            }
        }
        qsort(ranks, count, sizeof(Rank), compare);
        memcpy(dst, ranks, sizeof(*m_ranks));
    }
}

// The arrows show that there is a record before (2) and after (1) the one shown.
inline int muProcDiagram2p::getArrowVisible() {
    int visible = 0;
    if (m_kind > 0) {
        visible |= 2;
    }
    if (m_kind < KindNum - 1) {
        visible |= 1;
    }
    return visible;
}

inline void muProcDiagram2p::changeState(int state) {
    if (state == m_state) {
        return;
    }
    m_state = state;
    if (state == State_Hidden) {
        m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Page]));
        return;
    }
    if (state == State_Shown) {
        m_objs[Obj_Page]->setFrameVisible(getArrowVisible());
    }
    m_objs[Obj_Page]->setAnimIdx(&sStateAnims[sStateAnimIdx[state]], false);
}

// HYPOTHESIS: side is the side the page comes from (0 none, 1 and 2 the two directions of muProcDiagram::movePageMain).
void muProcDiagram2p::begin(int side) {
    m_group->Insert(m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Page]));
    m_objs[Obj_Page]->setAnimIdx(&sCursorAnims[0], false);
    updateDisp(false);
    int state;
    switch (side) {
    case 0:
        state = State_Come;
        break;
    case 1:
        state = State_ComeSide;
        break;
    case 2:
        state = State_Come;
        break;
    }
    changeState(state);
}

// HYPOTHESIS: side is the side the page leaves to, as in begin.
void muProcDiagram2p::post(int side) {
    int state;
    switch (side) {
    case 0:
        state = State_Leave;
        break;
    case 1:
        state = State_LeaveSide;
        break;
    case 2:
        state = State_Leave;
        break;
    }
    changeState(state);
}

void muProcDiagram2p::finish() {
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Page]));
    m_msg->detachMuObject(m_objs[Obj_Page]);
    gfHeapManager::free(m_ranks);
}

// When the animation of the page has stopped, the state after it
void muProcDiagram2p::main() {
    switch (m_state) {
    case State_Hidden:
    case State_Shown:
        break;
    case State_Leave:
    case State_LeaveSide:
        if (m_objs[Obj_Page]->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
            changeState(State_Hidden);
        }
        break;
    case State_Come:
    case State_ComeSide:
        if (m_objs[Obj_Page]->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
            changeState(State_Shown);
        }
        break;
    }
}

bool muProcDiagram2p::isPageChanging() {
    // BYTES: an unsigned compare (the states that are neither hidden nor shown; m_state is compared signed elsewhere).
    return static_cast<u32>(m_state) > State_Shown;
}

// Writes the title and the explanation of the record shown and the five best characters with their values.
void muProcDiagram2p::updateDisp(bool arrows) {
    m_msg->printIndex(m_msgWnd + Msg_Title, muDiagGetDataTitleMsgID(m_kind));
    m_msg->printIndex(m_msgWnd + Msg_Expl, muDiagGetDataExplMsgID(m_kind));
    // The largest value shown (99:59:59 for times)
    // HYPOTHESIS: unit kind 6 is the hours of the play time.
    int i;
    u32 max;
    switch (muDiagGetDataUnitKind(m_kind)) {
    case 2:
        max = 99999999;
        break;
    case 6:
        max = 5999999;
        break;
    default:
        max = 99999999;
        break;
    }
    Rank* rank = m_ranks[m_kind];
    for (i = 0; i < RankNum; i++, rank++) {
        int stock = exchangeMuCharKindToMuStockchkind__6muMenuFiii(rank->m_chara);
        m_objs[Obj_CharIcon + i]->setFrameTex(muMenu::exchangeGmCharacterKind2Something(stock) * 10 + 1);
        const int bufSize = 0x80;
        char buf[bufSize];
        int unitMsgId;
        int length = muDiagTransformDataValue(buf, &unitMsgId, m_kind, rank->m_value, max);
        if (unitMsgId != -1) {
            int unitLength;
            const char* unit = m_msg->getMsgData(unitMsgId, &unitLength);
            memcpy(buf + length, unit, unitLength);
            length += unitLength;
        }
        m_msg->printData(i + m_msgWnd + Msg_Value, buf, length);
    }
    // BYTES: the arrows are tested here as in getArrowVisible; calling it compiles to other registers.
    int visible = 0;
    if (arrows) {
        if (m_kind > 0) {
            visible |= 2;
        }
        if (m_kind < KindNum - 1) {
            visible |= 1;
        }
    }
    m_objs[Obj_Page]->setFrameVisible(visible);
}

// Up and Down go through the records.
bool muProcDiagram2p::controlProc() {
    MuDiagDataKind oldKind = m_kind;
    u32 pad = padUpdate();
    if ((pad & Pad_Down) && m_index < KindNum - 1) {
        m_index++;
        m_kind = sKindOrder[m_index];
        m_objs[Obj_Page]->setAnimIdx(&sCursorAnims[2], false);
    }
    if ((pad & Pad_Up) && m_index > 0) {
        m_index--;
        m_kind = sKindOrder[m_index];
        m_objs[Obj_Page]->setAnimIdx(&sCursorAnims[1], false);
    }
    if (oldKind != m_kind) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        updateDisp(true);
    }
    return true;
}

void muProcDiagram2p::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}

int muProcDiagram2p::getUseNumOfMsgWnd() {
    return MsgWndNum;
}

muProcDiagram2p::~muProcDiagram2p() { }

// HYPOTHESIS: the tables are static members defined after the functions: no function reaches them from a pooled base.
const u8 muProcDiagram2p::sStateAnimIdx[State_Num] = { 0, 1, 2, 3, 5, 4 };

MuAnimIdxData muProcDiagram2p::sStateAnims[State_Num] = {
    { 0.0f, 0.0f, 0, 0, 0x1 },
    { 9.0f, 9.0f, 0, 0, 0x1 },
    { 0.0f, 9.0f, 0, 0, 0x1 },
    { 10.0f, 19.0f, 0, 0, 0x1 },
    { 20.0f, 29.0f, 0, 0, 0x1 },
    { 30.0f, 39.0f, 0, 0, 0x1 },
};

MuAnimIdxData muProcDiagram2p::sCursorAnims[3] = {
    { 0.0f, 0.0f, 0, 0, 0x10 },
    { 1.0f, 5.0f, 0, 0, 0x10 },
    { 6.0f, 10.0f, 0, 0, 0x10 },
};

MuDiagDataKind muProcDiagram2p::sKindOrder[KindNum] = {
    MuDiagData_TotalBeat,    MuDiagData_TotalFall,    MuDiagData_Suicide,      MuDiagData_HitRate,
    MuDiagData_GiveDamage,   MuDiagData_TakeDamage,   MuDiagData_RecoverDamage, MuDiagData_MaxDamage,
    MuDiagData_RoundNum,     MuDiagData_ChampNum,     MuDiagData_KetsuNum,     MuDiagData_PlayTime,
    MuDiagData_UseRate,      MuDiagData_JoinAverage,  MuDiagData_WalkDistance, MuDiagData_JumpDistance,
    MuDiagData_FallDistance, MuDiagData_BlowDistance, MuDiagData_GetCoin,      MuDiagData_MakeDropCoin,
    MuDiagData_DropCoin,     MuDiagData_FallButtocks, MuDiagData_SwimDistance, MuDiagData_SwimFall,
};
