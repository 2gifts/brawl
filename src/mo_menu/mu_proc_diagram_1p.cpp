#include <gf/gf_heap_manager.h>
#include <gm/gm_global.h>
#include <mu/menu.h>
#include <snd/snd_system.h>
#include <types.h>

#include <mu/mu_proc_diagram.h>

// qsort of the library of the program
extern "C" void qsort(void* base, unsigned long count, unsigned long size, int (*compare)(const void*, const void*));

// muMenu helpers of main.dol whose symbols take three ints; the callers set only the first (as the matched units do)
extern "C" bool isCharUsable__6muMenuFiii(int muCharKind);
extern "C" int exchangeMuCharKindToMuStockchkind__6muMenuFiii(int muCharKind);

static int compare(const void* a, const void* b);

void muProcDiagram1p::setConf(nw4r::g3d::ScnGroup* group, MuMsg* msg, int msgWnd) {
    m_group = group;
    m_state = State_Hidden;
    m_msg = msg;
    m_msgWnd = msgWnd;
    m_column = 0;
    m_row = 0;
    initMsg();

    // The characters that can be used
    m_table = static_cast<Table*>(gfHeapManager::alloc(Heaps::MenuInstance, sizeof(Table)));
    u8 count = 0;
    for (int chara = 0; chara < CharaNum; chara++) {
        if (isCharUsable__6muMenuFiii(chara)) {
            m_table->m_charas[count++] = static_cast<MuCharKind>(chara);
        }
    }
    m_table->m_charaNum = count;
    makeRankTbl();
}

// Each row gets the KOs its character made of the characters listed; the rows are sorted by them.
void muProcDiagram1p::makeRankTbl() {
    int i;
    u32 total;
    MuCharKind chara;
    int j;
    for (i = 0; i < m_table->m_charaNum; i++) {
        m_table->m_rows[i].m_chara = m_table->m_charas[i];
        total = 0;
        m_table->m_rows[i].m_no = i;
        for (j = 0; j < m_table->m_charaNum; j++) {
            chara = m_table->m_charas[j];
            total += g_GameGlobal->getGlobalRecordCharaNumDatap(m_table->m_charas[i])->m_beatCount[chara];
        }
        m_table->m_rows[i].m_total = total;
    }
    if (m_table->m_charaNum > 1) {
        qsort(m_table->m_rows, m_table->m_charaNum, sizeof(Row), compare);
    }
}

// The rows with the higher total first, then in the order of the characters.
static int compare(const void* a, const void* b) {
    const muProcDiagram1p::Row* rowA = static_cast<const muProcDiagram1p::Row*>(a);
    const muProcDiagram1p::Row* rowB = static_cast<const muProcDiagram1p::Row*>(b);
    int diff = rowB->m_total - rowA->m_total;
    if (diff != 0) {
        return diff;
    }
    return rowA->m_no - rowB->m_no;
}

// The arrows show that there are rows before (2) and after (1) the ones shown.
inline int muProcDiagram1p::getRowArrowVisible() {
    int visible = 0;
    if (m_row > 0) {
        visible |= 2;
    }
    if (m_row + RowNum < m_table->m_charaNum) {
        visible |= 1;
    }
    return visible;
}

// The arrows show that there are columns before (1) and after (2) the ones shown.
inline int muProcDiagram1p::getColumnArrowVisible() {
    int visible = 0;
    if (m_column > 0) {
        visible |= 1;
    }
    if (m_column + ColumnNum < m_table->m_charaNum) {
        visible |= 2;
    }
    return visible;
}

inline void muProcDiagram1p::changeState(int state) {
    if (state == m_state) {
        return;
    }
    m_state = state;
    if (state == State_Hidden) {
        m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Row]));
        m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Column]));
        return;
    }
    if (state == State_Shown) {
        m_objs[Obj_RowArrow]->setFrameVisible(getRowArrowVisible());
        m_objs[Obj_ColumnArrow]->setFrameVisible(getColumnArrowVisible());
    }
    MuAnimIdxData* anim = &sStateAnims[sStateAnimIdx[state]];
    m_objs[Obj_RowArrow]->setAnimIdx(anim, false);
    m_objs[Obj_ColumnArrow]->setAnimIdx(anim, false);
}

// HYPOTHESIS: side is the side the page comes from, as in muProcDiagram2p::begin.
void muProcDiagram1p::begin(int side) {
    m_group->Insert(m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Row]));
    m_group->Insert(m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Column]));
    updateDisp(false);
    m_objs[Obj_RowArrow]->setAnimIdx(&sCursorAnims[0], false);
    m_objs[Obj_ColumnArrow]->setAnimIdx(&sCursorAnims[0], false);
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

// The message windows of the rows (rank and total) and of the values of the rows in the columns.
void muProcDiagram1p::initMsg() {
    for (int row = 0; row < RowNum; row++) {
        int wnd = m_msgWnd + row;
        m_msg->attachScnMdlSimple(
            wnd, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Row + row]->m_sceneModel), 0, 1.0f / 16.0f);
        m_msg->setFontType(wnd, 1);
        m_msg->setFontColor(wnd, 0x60, 0x70, 0x85, 0xFF);
        m_msg->setHScale(wnd, 0.8f);
        m_msg->setEdgeEnable(wnd, true);
        m_msg->setEdge(wnd, 0xFF, 0xFF, 0xFF, 2.0f);

        wnd = row + m_msgWnd + Msg_Total;
        m_msg->attachScnMdlSimple(
            wnd, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Row + row]->m_sceneModel), 1, 1.0f / 16.0f);
        m_msg->setFontColor(wnd, 0xFF, 0xFF, 0xFF, 0xFF);
        m_msg->setHScale(wnd, 0.8f);
        m_msg->setAlignMode(wnd, MuMsg::AlignMode(2));
        m_msg->setFontWidthModeFixed(wnd, 20.0f);

        for (int column = 0; column < ColumnNum; column++) {
            int valueWnd = m_msgWnd + (column + (row * ColumnNum + Msg_Value));
            m_msg->attachScnMdlSimple(
                valueWnd, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Column + column]->m_sceneModel), row,
                1.0f / 16.0f);
            m_msg->setFontColor(valueWnd, 0xE5, 0xE5, 0xE5, 0xFF);
            m_msg->setHScale(valueWnd, 0.75f);
            m_msg->getHScale(valueWnd, 0.9f);
            m_msg->setAlignMode(valueWnd, MuMsg::AlignMode(2));
            m_msg->setFontWidthModeFixed(valueWnd, 18.0f);
        }
    }
}

// HYPOTHESIS: side is the side the page leaves to, as in begin.
void muProcDiagram1p::post(int side) {
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

void muProcDiagram1p::finish() {
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Row]));
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Column]));
    for (int i = 0; i < RowNum; i++) {
        m_msg->detachMuObject(m_objs[Obj_Row + i]);
    }
    for (int i = 0; i < ColumnNum; i++) {
        m_msg->detachMuObject(m_objs[Obj_Column + i]);
    }
    gfHeapManager::free(m_table);
}

// When the animation of the page has stopped, the state after it
void muProcDiagram1p::main() {
    switch (m_state) {
    case State_Hidden:
    case State_Shown:
        break;
    case State_Leave:
    case State_LeaveSide:
        if (m_objs[Obj_RowArrow]->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
            changeState(State_Hidden);
        }
        break;
    case State_Come:
    case State_ComeSide:
        if (m_objs[Obj_RowArrow]->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
            changeState(State_Shown);
        }
        break;
    }
}

bool muProcDiagram1p::isPageChanging() {
    if (m_state == State_Hidden || m_state == State_Shown) {
        return false;
    }
    return true;
}

// HYPOTHESIS: the tables are static members defined between the functions, where the original's .rodata and .data have
// them (before the literals of updateDisp and the format of updateDispLine).
const u8 muProcDiagram1p::sStateAnimIdx[State_Num] = { 0, 1, 2, 3, 5, 4 };

MuAnimIdxData muProcDiagram1p::sStateAnims[State_Num] = {
    { 0.0f, 0.0f, 0, 0, 0x1 },
    { 9.0f, 9.0f, 0, 0, 0x1 },
    { 0.0f, 9.0f, 0, 0, 0x1 },
    { 10.0f, 19.0f, 0, 0, 0x1 },
    { 20.0f, 29.0f, 0, 0, 0x1 },
    { 30.0f, 39.0f, 0, 0, 0x1 },
};

MuAnimIdxData muProcDiagram1p::sCursorAnims[3] = {
    { 0.0f, 0.0f, 0, 0, 0x10 },
    { 1.0f, 5.0f, 0, 0, 0x10 },
    { 6.0f, 10.0f, 0, 0, 0x10 },
};

// The icons of the columns, the rows and the arrows.
void muProcDiagram1p::updateDisp(bool arrows) {
    int i;
    int column = m_column;
    for (i = 0; i < ColumnNum && column < m_table->m_charaNum; i++, column++) {
        int stock = exchangeMuCharKindToMuStockchkind__6muMenuFiii(m_table->m_charas[column]);
        m_objs[Obj_Column + i]->setFrameTex(muMenu::exchangeGmCharacterKind2Something(stock) * 10 + 1);
    }
    int row = m_row;
    for (i = 0; i < RowNum && row < m_table->m_charaNum; i++, row++) {
        updateDispLine(i, row, &m_table->m_rows[row]);
    }
    // BYTES: the arrows are tested here as in getRowArrowVisible/getColumnArrowVisible; calling them compiles to
    // other registers.
    int visible = 0;
    if (arrows) {
        if (m_row > 0) {
            visible |= 2;
        }
        if (m_row + RowNum < m_table->m_charaNum) {
            visible |= 1;
        }
    }
    m_objs[Obj_RowArrow]->setFrameVisible(visible);
    visible = 0;
    if (arrows) {
        if (m_column > 0) {
            visible |= 1;
        }
        if (m_column + ColumnNum < m_table->m_charaNum) {
            visible |= 2;
        }
    }
    m_objs[Obj_ColumnArrow]->setFrameVisible(visible);
}

// One row: its rank, its total, its character and its KOs of the characters of the columns shown.
void muProcDiagram1p::updateDispLine(int line, int row, Row* data) {
    int i;
    int column;
    int wnd = m_msgWnd + line;
    float width;
    float scale;
    if (row + 1 < 10) {
        width = 24.0f;
        scale = 1.0f;
    } else {
        width = 20.0f;
        scale = 0.7f;
    }
    m_msg->setFontWidthModeFixed(wnd, width);
    m_msg->setHScale(wnd, scale);
    m_msg->printf(wnd, "%d", row + 1);

    u32 total = data->m_total;
    if (total > 99999999) {
        total = 99999999;
    }
    m_msg->printf(line + m_msgWnd + Msg_Total, "%d", total);

    // The colour of the five first ranks
    float color;
    if (row < 5) {
        color = row + 1;
    } else {
        color = 5.0f;
    }
    m_objs[Obj_Row + line]->setFrameMatCol(color);
    int stock = exchangeMuCharKindToMuStockchkind__6muMenuFiii(data->m_chara);
    m_objs[Obj_Row + line]->setFrameTex(muMenu::exchangeGmCharacterKind2Something(stock) * 10 + 1);

    column = m_column;
    int valueWnd = line * ColumnNum + Msg_Value;
    for (i = 0; i < ColumnNum && column < m_table->m_charaNum; i++, column++, valueWnd++) {
        MuCharKind chara = m_table->m_charas[column];
        u32 count = g_GameGlobal->getGlobalRecordCharaNumDatap(data->m_chara)->m_beatCount[chara];
        if (count > 99999) {
            count = 99999;
        }
        m_msg->printf(m_msgWnd + valueWnd, "%d", count);
    }
}

// Up and Down scroll the rows, Left and Right the columns.
bool muProcDiagram1p::controlProc() {
    u8 oldColumn = m_column;
    u8 oldRow = m_row;
    u32 pad = padUpdate();
    if ((pad & Pad_Down) && m_row + RowNum < m_table->m_charaNum) {
        m_row++;
        m_objs[Obj_RowArrow]->setAnimIdx(&sCursorAnims[2], false);
    }
    if ((pad & Pad_Up) && m_row > 0) {
        m_row--;
        m_objs[Obj_RowArrow]->setAnimIdx(&sCursorAnims[1], false);
    }
    if ((pad & Pad_Right) && m_column + ColumnNum < m_table->m_charaNum) {
        m_column++;
        m_objs[Obj_ColumnArrow]->setAnimIdx(&sCursorAnims[1], false);
    }
    if ((pad & Pad_Left) && m_column > 0) {
        m_column--;
        m_objs[Obj_ColumnArrow]->setAnimIdx(&sCursorAnims[2], false);
    }
    if (oldColumn != m_column || oldRow != m_row) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        updateDisp(true);
    }
    return true;
}

void muProcDiagram1p::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}

int muProcDiagram1p::getUseNumOfMsgWnd() {
    return MsgWndNum;
}

muProcDiagram1p::~muProcDiagram1p() { }
