#define MU_SELCHAR_UNIT_DECLS
#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <gm/gm_set_rule.h>
#include <memory.h>
#include <ms/ms_message.h>
#include <mt/mt_vector.h>
#include <mu/mu_msg.h>
#include <mu/menu.h>
#include <mu/mu_object.h>
#include <mu/mu_rule.h>
#include <mu/selchar/mu_selchar.h>
#include <mu/selchar/mu_selchar_coin.h>
#include <mu/selchar/mu_selchar_hand.h>
#include <mu/selchar/mu_selchar_panel_ctrl.h>
#include <mu/selchar/mu_selchar_player_area.h>
#include <types.h>
#include <snd/snd_system.h>
#include <string.h>
#include <gf/gf_pad_system.h>
#include <stdio.h>
#include <mu/wifi/mu_wifi_interface.h>
#include <mu/mu_notice_wnd.h>
#include <nt/network.h>

// The menu helpers take three integers; this unit passes only the first one (see TIPS.md).
extern "C" int exchangeMuSelchkindToMuCharKind__6muMenuFiii(int selchKind);
extern "C" int isCharUsable__6muMenuFiii(int charKind);
extern "C" int isCharRefered__6muMenuFiii(int charKind);
extern "C" void setCharRefered__6muMenuFiii(int charKind);

muSelCharCBCalcWorld::muSelCharCBCalcWorld() {
    m_calcWorldDone = false;
    m_next = nullptr;
}

int muSelCharTask::getSelCharType() {
    return g_GameGlobal->m_selCharData->m_selCharType;
}

// The handicap setting of the rules.
u8 muSelCharTask::getGlobalHandiType() {
    return g_GameGlobal->m_setRule->m_handicap;
}

void muSelCharTask::setHomerunRule(int rule) {
    m_homerunRule = rule;
}

void muSelCharTask::setKumiteType(int type) {
    m_kumiteType = type;
}

void muSelCharTask::setQueueMeleeDropRule(int rule) {
    m_queueMeleeDropRule = rule;
}

bool muSelCharTask::isTeamBattle() {
    return m_isTeamBattle;
}

muSelCharPlayerArea* muSelCharTask::getPlayerArea(int areaIdx) {
    return m_selCharPlayerAreas[areaIdx];
}

int muSelCharTask::getRuleNumStock() {
    return m_ruleNumStock;
}

int muSelCharTask::getHomerunRule() {
    return m_homerunRule;
}

int muSelCharTask::getKumiteType() {
    return m_kumiteType;
}

int muSelCharTask::getQueueMeleeDropRule() {
    return m_queueMeleeDropRule;
}

int muSelCharTask::getDifficulty() {
    return m_difficulty;
}

u8 muSelCharTask::isEntried2P() {
    return m_isEntried2P;
}

muSelCharCoin* muSelCharTask::getCoin(int areaIdx) {
    return m_selCharPlayerAreas[areaIdx]->m_selCharCoin;
}

muSelCharHand* muSelCharTask::getHand(int areaIdx) {
    return m_selCharPlayerAreas[areaIdx]->m_selCharHand;
}

int muSelCharTask::getRuleTaskState() {
    return m_ruleTaskState;
}

void muSelCharTask::setDifficulty(int difficulty) {
    m_difficulty = difficulty;
}

u8 muSelCharTask::isTeamBattleTouch() {
    return m_teamBattleTouch;
}

int muSelCharTask::getMaxDiff() {
    return m_maxDiff;
}

void muSelCharCBCalcWorld::ExecCallback_DRAW_XLU(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param, void* info) {
    if (m_next) {
        m_next->ExecCallback_DRAW_XLU(timing, obj, param, info);
    }
}

void muSelCharCBCalcWorld::ExecCallback_DRAW_OPA(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param, void* info) {
    if (m_next) {
        // BUG: the opaque draw is passed on as the translucent draw of the next callback (the call to DRAW_OPA does not match).
        m_next->ExecCallback_DRAW_XLU(timing, obj, param, info);
    }
}

void muSelCharCBCalcWorld::ExecCallback_CALC_VIEW(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param, void* info) {
    if (m_next) {
        m_next->ExecCallback_CALC_VIEW(timing, obj, param, info);
    }
}

void muSelCharCBCalcWorld::ExecCallback_CALC_MAT(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param, void* info) {
    if (m_next) {
        m_next->ExecCallback_CALC_MAT(timing, obj, param, info);
    }
}

void muSelCharCBCalcWorld::ExecCallback_CALC_WORLD(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param, void* info) {
    if (m_next) {
        m_next->ExecCallback_CALC_WORLD(timing, obj, param, info);
    }
    if (timing == nw4r::g3d::ScnObj::CALLBACK_TIMING_C) {
        m_calcWorldDone = true;
    }
}

// Which part of the screen a height is in.
int muSelCharTask::getScreenArea(Vec2f pos) {
    if (pos.m_y >= 16.0f) {
        return 0;
    }
    if (pos.m_y < -5.4f) {
        return 2;
    }
    return 1;
}

void muSelCharTask::setReadyButtonDecided() {
    m_objs[Obj_ReadyButton]->changeAnimN(3, 0x11, true, false);
}

void muSelCharTask::clickHomerunRule() {
    m_objs[Obj_HomerunRule]->changeAnimN(3, 1, true, false);
}

void muSelCharTask::clickKumiteRule() {
    m_objs[Obj_KumiteRule]->changeAnimN(3, 1, true, false);
}

// Clears the five lines of the high score.
void muSelCharTask::eraseAllHighScore() {
    for (int i = 0; i < 5; i++) {
        m_highScoreMsg->printf(i + 2, "");
    }
}

// Called out of line from the total high-score screen.
#pragma dont_inline on
void muSelCharTask::eraseHighScore(u32 msgIndex) {
    m_highScoreMsg->printf(msgIndex, "");
}
#pragma dont_inline reset

// Takes the picture of one of the two characters of a pair off the high score.
// Called out of line from the pair high-score screens.
#pragma dont_inline on
void muSelCharTask::eraseHighScorePairChar(int pairIdx) {
    static const int sPairCharObj[] = { 178, 179 };
    m_highScoreExpand->Remove(m_objs[sPairCharObj[pairIdx]]->m_sceneModel);
}
#pragma dont_inline reset

void muSelCharTask::addObj(int obj) {
    m_scnGroup->Insert(m_scnGroup->sceneItemsCount, m_objs[obj]->m_sceneModel);
}

void muSelCharTask::removeObj(int obj) {
    m_scnGroup->Remove(m_objs[obj]->m_sceneModel);
}

// The area of the player whose hand holds the number of a rule (-1: none).
int muSelCharTask::findRuleNumHoldPlayer() {
    for (int i = 0; i < m_numPlayerAreas; i++) {
        if (m_selCharPlayerAreas[i]->m_selCharHand->m_mode == 4) {
            return i;
        }
    }
    return -1;
}

// Lets the rules screen take the controls (or not).
void muSelCharTask::setRuleControlEnable(u8 enable) {
    m_ruleControlEnable = enable;
    if (m_ruleTask) {
        m_ruleTask->setCtrlFlg(enable ? 0xFF : 0);
    }
}

// The place of a character in the list of the screen (-1: not on the screen). The characters that share a picture (the two
// forms of a character, the Pokemon Trainer and his Pokemon) are found by the first of them.
int muSelCharTask::getCharListIdx(int charKind) {
    int i;
    int idx = -1;
    for (i = 0; i < m_charListNum; i++) {
        int kind = m_charList[i];
        switch (charKind) {
        case 3:
        case 4:
            if (kind == 3) {
                idx = i;
            }
            break;
        case 14:
        case 15:
            if (kind == 14) {
                idx = i;
            }
            break;
        case 27:
        case 28:
        case 29:
        case 30:
            if (kind == 27) {
                idx = i;
            }
            break;
        default:
            if (kind == charKind) {
                idx = i;
            }
            break;
        }
        if (idx >= 0) {
            break;
        }
    }
    return idx;
}

// The center of the picture of a character.
bool muSelCharTask::getFacePos(Vec2f* pos, int charKind) {
    Rect2D rect;
    if (!getFaceRect(&rect, charKind)) {
        return false;
    }
    pos->m_x = (rect.m_right + rect.m_left) / 2.0f;
    pos->m_y = (rect.m_down + rect.m_up) / 2.0f;
    return true;
}

// The corners of the picture of a character.
bool muSelCharTask::getFaceRect(Rect2D* rect, int charKind) {
    int idx = getCharListIdx(charKind);
    if (idx < 0) {
        return false;
    }
    m_objs[Obj_FaceBase + idx]->getRect3D(rect, "colL", "colR");
    return true;
}

// Puts the cursor (the number of a player on a coin) on the picture of a character.
void muSelCharTask::setCursor(int charKind, MuObject* cursor) {
    cursor->m_sceneModel->DetachFromParent();
    int idx = getCharListIdx(charKind);
    if (idx >= 0) {
        m_expands[Expand_FaceBase + idx]->PushBack(cursor->m_sceneModel, "pos00");
    }
}

void muSelCharTask::dispCanceled() {
    MuObject* obj = m_objs[2];
    obj->setFrameMatCol(15.0f);
    obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
}

void muSelCharTask::setReadyButtonPointed(u8 pointed) {
    if (m_readyButtonPointed != pointed) {
        if (m_readyState == 2) {
            m_objs[Obj_ReadyButton]->changeAnimN(pointed ? 2 : 1, 0x11, true, false);
        }
        m_readyButtonPointed = pointed;
    }
}

void muSelCharTask::setVisibleDiff(bool visible) {
    m_scnGroup->Remove(m_diffExpand);
    if (visible) {
        m_scnGroup->Insert(m_scnGroup->sceneItemsCount, m_diffExpand);
    }
}

void muSelCharTask::setVisibleStock(bool visible) {
    m_scnGroup->Remove(m_stockExpand);
    if (visible) {
        m_scnGroup->Insert(m_scnGroup->sceneItemsCount, m_stockExpand);
    }
}

// The level of the computers: the arrows only when there is more than one level.
void muSelCharTask::initDiffObj() {
    nw4r::g3d::ScnMdlExpand* expand = m_diffExpand;
    if (m_maxDiff > 1) {
        expand->PushBack(m_objs[Obj_DiffUp]->m_sceneModel, "pos00");
        expand->PushBack(m_objs[Obj_DiffDown]->m_sceneModel, "pos01");
    }
    m_objs[Obj_DiffLevel]->setFrameVisible(m_maxDiff);
    if (getSelCharType() == 7) {
        m_objs[Obj_DiffLevel]->setFrameTex(1.0f);
    }
}

void muSelCharTask::initStockObj() {
    nw4r::g3d::ScnMdlExpand* expand = m_stockExpand;
    expand->PushBack(m_objs[Obj_StockUp]->m_sceneModel, "pos00");
    expand->PushBack(m_objs[Obj_StockDown]->m_sceneModel, "pos01");
}

// HYPOTHESIS: the priorities of the coins are read and not used (the order is not changed any more).
void muSelCharTask::arrangeCoinPrio() {
    for (int i = 0; i < m_numPlayerAreas; i++) {
        if (m_selCharPlayerAreas[i]->m_playerKind != 0) {
            m_selCharPlayerAreas[i]->m_selCharCoin->getCoinDrawPrio();
        }
    }
}

// Whether a character is shown on the screen: 0 not at all, 1 as a picture that cannot be picked yet, 2 normally.
// HYPOTHESIS: the meaning of the values and of the flag (it skips the test of the unlocked characters).
int muSelCharTask::getCharApparState(int selchKind, bool refer) {
    int charKind = exchangeMuSelchkindToMuCharKind__6muMenuFiii(selchKind);
    if (!refer && !isCharUsable__6muMenuFiii(charKind)) {
        return 0;
    }
    if (!isCharRefered__6muMenuFiii(charKind)) {
        return 1;
    }
    return 2;
}

// The longest time of a battle in minutes (the shortest is 1 minute for kinds 1 and 0x12, 0 for no limit otherwise).
int muSelCharTask::getRuleTimeMax() {
    int max = 99;
    if (getSelCharType() == 0x12) {
        max = 60;
    }
    return max;
}

int muSelCharTask::getRuleTimeMin() {
    switch (getSelCharType()) {
    case 1:
    case 0x12:
        return 1;
    }
    return 0;
}

extern const int lbl_10_rodata_178[3];
extern "C" const muSelCharTypeData* getSelCharTypeData__13muSelCharTaskFi(int);

void muSelCharTask::updateCharHighScore(int line, int selchKind, int difficulty) {
    int type = getSelCharType();
    getSelCharTypeData__13muSelCharTaskFi(type);
    int msgIdx;
    if (type == 10) {
        msgIdx = 5;
    } else {
        msgIdx = lbl_10_rodata_178[line];
    }
    int style;
    int level;
    if (selchKind >= 0 && selchKind < 0x28) {
        int charKind = exchangeMuSelchkindToMuCharKind__6muMenuFiii(selchKind);
        int score = getCharHighScore(charKind, difficulty, &style, &level);
        dispHighScore(msgIdx, score, style);
    } else {
        eraseHighScore(msgIdx);
        level = -1;
    }
    if (getSelCharType() != 7) {
        dispHighScoreDiffLevel(level);
    }
}

static const muSelCharTypeData sSelCharTypeData[] = {
    { 0x0004, 1, 1, 0x10, 0, 0x0000, -1 },
    { 0x0004, 5, 5, 0x10, 0, 0x0000, 8262 },
    { 0x0004, 3, 3, 0x10, 0, 0x0000, 8262 },
    { 0x0004, 1, 1, 0x10, 0, 0x0000, -1 },
    { 0x00B4, 1, 1, 0x40, 1, 0x0000, 8240 },
    { 0x00B4, 2, 14, 0x40, 2, 0x0000, 8241 },
    { 0x00B4, 12, 13, 0x40, 3, 0x0000, 8243 },
    { 0x00B4, 6, 7, 0x40, 4, 0x0000, 8245 },
    { 0x00B4, 8, 9, 0x40, 5, 0x0000, 8246 },
    { 0x00B4, 10, 11, 0x40, 6, 0x0000, 8262 },
    { 0x00B4, 5, 5, 0x40, 12, 0x0000, 8244 },
    { 0x00B4, 3, 4, 0x40, 0, 0x0000, 8262 },
    { 0x0004, 1, 1, 0x30, 0, 0x0000, -1 },
    { 0x00B7, 6, 6, 0x40, 6, 0x0000, 8262 },
    { 0x00B7, 7, 7, 0x40, 5, 0x0000, 8246 },
    { 0x00B7, 5, 5, 0x40, 3, 0x0000, 8243 },
    { 0x00B7, 1, 1, 0x20, 0, 0x0000, 8247 },
    { 0x00B7, 2, 2, 0x20, 0, 0x0000, 8248 },
    { 0x00B7, 3, 3, 0x00, 0, 0x0000, 8262 },
};

const muSelCharTypeData* muSelCharTask::getSelCharTypeData(int selCharType) {
    return &sSelCharTypeData[selCharType];
}

// The high score of a kind of the select screen; the Multi-Man Brawl (kinds 9 and 13) has one for each of its kinds.
int muSelCharTask::getSelCharTypeHighScoreKind(int selCharType) {
    static const u8 sKumiteHighScoreKind[] = { 6, 7, 8, 9, 10, 11 };
    if (selCharType == 9 || selCharType == 13) {
        return sKumiteHighScoreKind[m_kumiteType];
    }
    return getSelCharTypeData(selCharType)->m_highScoreKind;
}

int muSelCharHand::getMode(int* arg) {
    if (arg) {
        *arg = m_modeArg;
    }
    return m_mode;
}

extern const u8 lbl_10_rodata_2C8[0x28];

void muSelCharTask::createWifiModel(nw4r::g3d::ResFile* resFile) {
    createObjResFile(lbl_10_rodata_2C8, 5, resFile, nullptr, false);
}

extern MuAnimIdxData lbl_10_data_610[5][2];

void muSelCharTask::dispDiffLevel(int level, bool enabled) {
    lbl_10_data_610[level][!enabled].m_flags = 1;
    m_objs[Obj_DiffLevel]->setAnimIdx(&lbl_10_data_610[level][!enabled], false);
}


void muSelCharTask::ruleCall() {
    SndID sound = (SndID)0x202B;
    switch (unk5D4) {
    case 0:
        sound = (SndID)0x202B;
        break;
    case 1:
        sound = (SndID)0x202D;
        break;
    case 2:
        sound = (SndID)0x202C;
        break;
    }
    g_sndSystem->playSEwithFixEffect(sound, 1.0f, 1.0f);
}

muSelCharTask* muSelCharTask::create(void* data) {
    muSelCharTask* task = new ((HeapType)0x2A) muSelCharTask;
    task->initProc(data);
    return task;
}

void muSelCharTask::sendSystemAllCharKind() {
    for (int area = 0; area < m_numPlayerAreas; area++) {
        m_selCharPlayerAreas[area]->sendSystemCharKind();
    }
}

int muSelCharTask::getRuleDispType() {
    int dispType = (getSelCharTypeData(getSelCharType())->m_unk4 >> 5) & 7;
    if (m_unk5D0) {
        dispType = 3;
    }
    if (getSelCharType() == 12 && isWifiGameMaster()) {
        dispType = 0;
    }
    return dispType;
}

int muSelCharTask::getCollKindPlayerArea(Vec2f* pos, int* areaIdx, int* arg, float* rect) {
    for (int i = 0; i < m_numPlayerAreas; i++) {
        int collKind = m_selCharPlayerAreas[i]->getCollKind(pos, arg, rect);
        if (collKind) {
            *areaIdx = i;
            return collKind;
        }
    }
    return 0;
}

void muSelCharTask::setRuleNumHoldCursorVisible(bool visible) {
    MuObject* obj = m_objs[Obj_RuleNumHold];
    m_scnGroup->Remove(obj->m_sceneModel);
    if (visible) {
        m_scnGroup->Insert(m_scnGroup->sceneItemsCount, obj->m_sceneModel);
        obj->setFrameMatCol(0.0f);
        obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    }
}

void muSelCharTask::setFaceIconMotion(int faceIdx, int motion) {
    static MuAnimNameData sMotions[] = {
        { 10.0f, 10.0f, 0, NULL, 0x11 },
        { 0.0f, 9.0f, 0, NULL, 1 },
        { 10.0f, 30.0f, 0, NULL, 0x10 },
        { 10.0f, 20.0f, 0, NULL, 1 },
    };
    MuObject* obj = m_objs[Obj_FaceBase + faceIdx];
    char animName[64];
    const nw4r::g3d::ResMdlData* mdl = obj->m_resMdl.ptr();
    strcpy(animName, mdl->m_toName != 0 ? reinterpret_cast<const char*>(mdl) + mdl->m_toName : NULL);
    strcat(animName, "__0");
    sMotions[motion].m_name = animName;
    obj->setAnimName(&sMotions[motion], false);
    obj->m_scnMdl->SetPriorityDrawOpa(0x49);
    obj->m_scnMdl->SetPriorityDrawXlu(0x49);
}

int muSelCharTask::findFreeWiiControllerNo() {
    gfPadStatus status;
    for (int controller = 4; controller < 8; controller++) {
        g_gfPadSystem->getSysPadStatus(controller, &status);
        if (status.m_error == 0 && status.m_controllerType != 0) {
            int area;
            for (area = 0; area < m_numPlayerAreas; area++) {
                if (m_selCharPlayerAreas[area]->m_controllerNo == controller) {
                    break;
                }
            }
            if (area >= m_numPlayerAreas) {
                return controller;
            }
        }
    }
    return -1;
}

void muSelCharTask::dispWifiPrompt(u32 message) {
    MuObject* obj = m_objs[187];
    m_highScoreMsg->printIndex(7, message, NULL);
    m_scnGroup->Remove(obj->m_sceneModel);
    m_scnGroup->Insert(m_scnGroup->sceneItemsCount, obj->m_sceneModel);
    obj->setFrameNode(0.0f);
    obj->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    g_sndSystem->playSE((SndID)0x1EF3, -1, 0, 0, -1);
}

int muSelCharTask::getCollCharKind(Vec2f* pos) {
    for (int faceIdx = 0; faceIdx < m_charListNum; faceIdx++) {
        Rect2D rect;
        m_objs[Obj_FaceBase + faceIdx]->getRect3D(&rect, "colL", "colR");
        if (rect.m_left <= pos->m_x && rect.m_right >= pos->m_x &&
            rect.m_down <= pos->m_y && rect.m_up >= pos->m_y) {
            return m_charList[faceIdx];
        }
    }
    return 0x28;
}

void muSelCharTask::dispHighScoreDiffLevel(int level) {
    MuObject* obj = m_objs[177];
    nw4r::g3d::ScnMdlExpand* expand;
    const char* node;
    if (getSelCharType() == 7) {
        expand = m_highScoreExpand;
        node = "pos04";
    } else {
        expand = m_diffExpand;
        node = "pos02";
    }
    expand->Remove(obj->m_sceneModel);
    if (level >= 0) {
        obj->setFrameVisible(level + 1);
        expand->PushBack(obj->m_sceneModel, node);
    }
}

extern char lbl_10_data_6E8[8];

// Called out of line from the pair high-score screens.
#pragma dont_inline on
void muSelCharTask::setHighScoreStyle(int style) {
    MuObject* object;
    nw4r::g3d::ScnMdlExpand* expand = m_highScoreExpand;
    object = m_objs[176];
    m_scnGroup->Remove(expand);
    if (style != 0) {
        float frame = (float)style - 1.0f;
        object->setFrameVisible(frame);
        object->setFrameNode(frame);
        m_scnGroup->Insert(m_scnGroup->sceneItemsCount, expand);
    }
}
#pragma dont_inline reset

void muSelCharTask::dispNumStock(int count, bool enabled) {
    int index;
    nw4r::g3d::ScnMdlExpand* expand = m_stockExpand;
    lbl_10_data_610[count - 1][!enabled].m_flags = 3;
    m_objs[167]->setAnimIdx(&lbl_10_data_610[count - 1][!enabled], false);
    for (index = 0; index < 5; index++) {
        expand->Remove(m_objs[168 + index]->m_sceneModel);
    }
    char node[8];
    for (index = 0; index < count; index++) {
        sprintf(node, lbl_10_data_6E8, index + 2);
        expand->PushBack(m_objs[168 + index]->m_sceneModel, node);
    }
}

int muSelCharTask::strcatHighScoreUnit(char* text, int capacity, int kind) {
    int message = -1;
    switch (kind) {
    case 2:
        if (g_GameGlobal->getLanguage() == 1) message = 26;
        else message = 25;
        break;
    case 3: message = 27; break;
    case 5: message = 28; break;
    case 6: message = 29; break;
    case 4: message = 30; break;
    }
    int length = strlen(text);
    if (message >= 0) {
        int size;
        const char* unit = m_highScoreMsg->getMsgData(message, &size);
        memcpy(text + length, unit, size);
        length += size;
        text[length] = 0;
        length++;
    }
    return length;
}

void muSelCharTask::sendWifiGameRule(bool force, bool send) {
    __typeof__(unk637) rule;
    memset(&rule, 0, sizeof(rule));
    rule.unk0_6 = unk5D4;
    rule.unk0_4 = m_handiMode;
    rule.unk0_2 = m_isTeamBattle != 0;
    rule.unk2 = unk5D8;
    rule.unk3 = m_ruleNumStock;
    rule.unk1_5 = m_kumiteType;
    rule.unk1_4 = m_homerunRule;
    if (force || memcmp(&rule, &unk637, sizeof(rule)) != 0) {
        g_muWifiInterfaceTask->setSelCharGameRule(&rule, send);
        unk637 = rule;
    }
}

int muSelCharTask::isPushedUnusedControllerStartButton() {
    bool used[8];
    memset(used, 0, sizeof(used));
    muSelCharPlayerArea* player;
    for (int area = 0; area < m_numPlayerAreas; area++) {
        player = getPlayerArea(area);
        int controller = player->m_controllerNo;
        if (controller >= 0) {
            used[controller] = true;
        }
        controller = player->unk426Flag ? player->unk42C : -1;
        if (controller >= 0) {
            used[controller] = true;
        }
    }
    for (int controller = 0; controller < 8; controller++) {
        if (!used[controller]) {
            gfPadStatus status;
            g_gfPadSystem->getSysPadStatus(controller, &status);
            if (status.m_error == 0 && (status.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_START)) {
                return controller;
            }
        }
    }
    return -1;
}

bool muSelCharTask::canWifiEntry() {
    int area;
    bool canEntry = false;
    int players = 0;
    int available = 0;
    muWifiInterfaceTask* wifi = g_muWifiInterfaceTask;
    if (getSelCharType() == 12) {
        for (area = 0; area < m_numPlayerAreas; area++) {
            muSelCharPlayerArea* player = m_selCharPlayerAreas[area];
            switch (player->_0) {
            case 0:
                if (player->m_playerKind != 2) {
                    players++;
                } else {
                    available++;
                }
                break;
            case 1:
                int state = player->getWifiStateByWifiCond();
                if (state == 0 && wifi->testRequestApproval(area)) {
                    available++;
                }
                if (state == 1 || state == 2) {
                    players++;
                }
                break;
            case 2:
            default:
                break;
            }
        }
        if (available > 0 && players < 3) {
            canEntry = true;
        }
    }
    return canEntry;
}

extern const u32 lbl_10_rodata_4F8[6];

void muSelCharTask::dispKumiteRule(int type) {
    int motion = 0;
    m_highScoreMsg->printIndex(1, lbl_10_rodata_4F8[type], NULL);
    int selCharType = getSelCharType();
    int group;
    if (isSelCharTypeIn(selCharType, 0, 4)) {
        group = 0;
    } else if (isSelCharTypeIn(selCharType, 4, 12)) {
        group = 1;
    } else {
        bool inGroup = isSelCharTypeIn(selCharType, 12, 16);
        group = 3;
        if (inGroup) {
            group = 2;
        }
    }
    if (group == 2 && !isWifiGameMaster()) {
        motion = 4;
    }
    m_objs[Obj_KumiteRule]->changeAnimN(motion, 0x10, false, false);
}

void muSelCharTask::wifiEntryPlayer() {
    muSelCharPlayerArea* player;
    int controller;
    int area;
    int entryArea = -1;
    bool replaceCpu = false;
    if (canWifiEntry()) {
        controller = isPushedUnusedControllerStartButton();
        if (controller >= 0) {
            for (area = 0; area < m_numPlayerAreas; area++) {
                player = getPlayerArea(area);
                if (player->_0 == 1 && player->getWifiStateByWifiCond() == 0) {
                    entryArea = area;
                    break;
                }
            }
            if (entryArea < 0) {
                for (area = 0; area < m_numPlayerAreas; area++) {
                    player = getPlayerArea(area);
                    if (player->_0 == 0 && player->m_playerKind == 2) {
                        entryArea = area;
                        replaceCpu = true;
                        break;
                    }
                }
            }
            if (entryArea >= 0) {
                player = m_selCharPlayerAreas[entryArea];
                if (replaceCpu) {
                    player->wifiExitCpuPlayer();
                }
                player->wifiRequestPlayerEntry(controller);
            }
        }
    }
}

void muSelCharTask::processDefault() {
    u8 selected;
    int i;
    unk64C--;
    if (isSelCharTypeOnline(getSelCharType())) wifiMain();
    int ruleState = m_ruleTaskState;
    if (ruleState != 0) mainRule();
    faceIconMain();
    for (i = 0; i < m_numPlayerAreas; i++) m_selCharPlayerAreas[i]->main();
    switch (unk40) {
    case 0:
        if (ruleState == 0) selectingMain();
        break;
    case 1:
        if (--unk5F8 <= 0) {
            setToGlobal();
            if (isSelCharTypeOnline(getSelCharType())) g_muWifiInterfaceTask->noticeEndOfCharSelect();
            setState(2, 0, 0);
        }
        break;
    case 3:
        unk40 = 4;
        break;
    case 6: {
        int result;
        if (unk60C->m_step != 3) result = 0;
        else {
            selected = unk60C->m_selected;
            closeWifiDlg();
            result = 2;
            if (selected) result = 1;
        }
        switch (result) {
        case 1: setState(4, 0, 0); break;
        case 2: setState(0, 0, 0); break;
        }
        break;
    }
    case 7: {
        int result;
        if (unk60C->m_step != 3) result = 0;
        else {
            selected = unk60C->m_selected;
            closeWifiDlg();
            result = 2;
            if (selected) result = 1;
        }
        if (result != 0) {
            if (unk644 == 3) setState(4, 0, 0);
            else setState(5, 0, 0);
        }
        break;
    }
    case 8: {
        u8 aid;
        int player;
        if (g_muWifiInterfaceTask->isCancelApprovalCalled(&aid, &player) && aid == unk635Byte && player == unk636Byte) {
            closeWifiDlg();
            setState(0, 0, 0);
        } else {
            int result;
            if (unk60C->m_step != 3) result = 0;
            else {
                selected = unk60C->m_selected;
                closeWifiDlg();
                result = 2;
                if (selected) result = 1;
            }
            if (result != 0) {
                g_muWifiInterfaceTask->answerRequestedEntry(result == 1, unk635Byte, unk636Byte);
                setState(0, 0, 0);
            }
        }
        break;
    }
    }
}

void muSelCharTask::setRuleNumStock(int count) {
    u8 aid;
    m_ruleNumStock = count;
    int type = getSelCharType();
    int group;
    if (isSelCharTypeIn(type, 0, 4)) group = 0;
    else if (isSelCharTypeIn(type, 4, 12)) group = 1;
    else {
        bool inGroup = isSelCharTypeIn(type, 12, 16);
        group = 3;
        if (inGroup) group = 2;
    }
    if (group == 2) {
        muWifiInterfaceTask& wifi = *g_muWifiInterfaceTask;
        int master = wifi.unkFCA_b5;
        if (!master) {
            aid = Network::getMyAID();
            if (aid == 0xFF) aid = 4;
            u8 masterAid = wifi.getRuleMasterAID();
            master = masterAid == aid;
        }
        if (master) sendWifiGameRule(false, true);
    }
}

extern const u8 lbl_10_rodata_4F0[4][2];

void muSelCharTask::appearHomerunRule() {
    int rule = m_homerunRule;
    int disabled = 0;
    int type = getSelCharType();
    int group;
    if (isSelCharTypeIn(type, 0, 4)) group = 0;
    else if (isSelCharTypeIn(type, 4, 12)) group = 1;
    else {
        bool inGroup = isSelCharTypeIn(type, 12, 16);
        group = 3;
        if (inGroup) group = 2;
    }
    if (group == 2 && !isWifiGameMaster()) disabled = 1;
    int motion;
    if (rule == 0) motion = lbl_10_rodata_4F0[disabled][0];
    else motion = lbl_10_rodata_4F0[disabled][1];
    m_objs[Obj_HomerunRule]->changeAnimN(motion, 0x10, false, false);
    MuObject* object = m_objs[Obj_HomerunRule];
    object->changeAnimN(2, 1, true, false);
    m_scnGroup->Insert(m_scnGroup->sceneItemsCount, object->m_sceneModel);
}

extern const u8 lbl_10_rodata_118[8];

int muSelCharTask::bButtonHoldProc() {
    float frame = 0.0f;
    int player = -1;
    int count = 0;
    for (int area = 0; area < m_numPlayerAreas; area++) {
        int held = m_selCharPlayerAreas[area]->m_selCharHand->m_bButtonCnt;
        if (held > count) {
            count = held;
            player = area;
        }
    }
    if (player >= 0) {
        int controller = m_selCharPlayerAreas[player]->getControllerKind();
        if (controller < 4) frame = lbl_10_rodata_118[controller];
    }
    MuObject* object = m_objs[3];
    m_scnGroup->Remove(object->m_sceneModel);
    if (count > 0) {
        m_scnGroup->Insert(m_scnGroup->sceneItemsCount, object->m_sceneModel);
        object->setFrameVisible(frame);
        object->setFrameNode(count);
    }
    if (count >= 30) return player;
    return -1;
}

extern const u8 lbl_10_rodata_4E0[4];
extern const u8 lbl_10_rodata_4E8[4][2];

void muSelCharTask::setReadyState(int state) {
    MuObject* object = m_objs[Obj_ReadyButton];
    m_scnGroup->Remove(object->m_sceneModel);
    if (state != 0) {
        m_scnGroup->Insert(m_scnGroup->sceneItemsCount, object->m_sceneModel);
        object->changeAnimN(state != 1, 0x11, true, false);
    }
    u32 motion = lbl_10_rodata_4E0[state];
    object = m_objs[1];
    int type = getSelCharType();
    int group;
    if (isSelCharTypeIn(type, 0, 4)) group = 0;
    else if (isSelCharTypeIn(type, 4, 12)) group = 1;
    else {
        bool inGroup = isSelCharTypeIn(type, 12, 16);
        group = 3;
        if (inGroup) group = 2;
    }
    int animation;
    bool play;
    if (motion <= 1) {
        animation = lbl_10_rodata_4E8[group][0];
        play = motion == 1;
    } else {
        animation = lbl_10_rodata_4E8[group][1];
        play = true;
    }
    object->changeAnimN(animation, 0x18, play, false);
    m_readyState = state;
}

extern const u8 lbl_10_rodata_4F0[4][2];
extern const u8 lbl_10_rodata_4E8[4][2];

void muSelCharTask::assignTeam() {
    int count = 0;
    for (int area = 0; area < m_numPlayerAreas; area++) {
        if (m_selCharPlayerAreas[area]->m_playerKind != 0) {
            count++;
        }
    }
    int* assignment;
    int countAssigned;
    int team;
    int teamSize;
    for (teamSize = count - 1; teamSize > 0; teamSize--) {
        if (count % teamSize == 0) {
            break;
        }
    }
    if (teamSize <= 0) {
        teamSize = 1;
    }
    int teams[4];
    assignment = teams;
    countAssigned = 0;
    for (team = 0; team < 3 && countAssigned < 4; team++) {
        for (int member = 0; member < teamSize && countAssigned < 4;) {
            *assignment = team;
            member++;
            assignment++;
            countAssigned++;
        }
    }
    int* assignedTeam = teams;
    for (int area = 0; area < m_numPlayerAreas; area++) {
        muSelCharPlayerArea* player = m_selCharPlayerAreas[area];
        if (player->m_playerKind != 0) {
            player->setTeamColor(*assignedTeam, player->m_teamSet);
            assignedTeam++;
        }
    }
}

void muSelCharTask::dispHomerunRule(int rule) {
    int mode = 0;
    int selCharType = getSelCharType();
    int group;
    if (isSelCharTypeIn(selCharType, 0, 4)) {
        group = 0;
    } else if (isSelCharTypeIn(selCharType, 4, 12)) {
        group = 1;
    } else {
        bool inGroup = isSelCharTypeIn(selCharType, 12, 16);
        group = 3;
        if (inGroup) {
            group = 2;
        }
    }
    if (group == 2 && !isWifiGameMaster()) {
        mode = 1;
    }
    m_objs[Obj_HomerunRule]->changeAnimN(rule == 0 ? lbl_10_rodata_4F0[mode][0] : lbl_10_rodata_4F0[mode][1], 0x10, false, false);
}

bool muSelCharTask::isWifiMode() {
    int selCharType = getSelCharType();
    int group;
    if (isSelCharTypeIn(selCharType, 0, 4)) {
        group = 0;
    } else if (isSelCharTypeIn(selCharType, 4, 12)) {
        group = 1;
    } else {
        bool inGroup = isSelCharTypeIn(selCharType, 12, 16);
        group = 3;
        if (inGroup) {
            group = 2;
        }
    }
    if (group == 0) goto offline;
    selCharType = getSelCharType();
    if (isSelCharTypeIn(selCharType, 0, 4)) {
        group = 0;
    } else if (isSelCharTypeIn(selCharType, 4, 12)) {
        group = 1;
    } else {
        bool inGroup = isSelCharTypeIn(selCharType, 12, 16);
        group = 3;
        if (inGroup) {
            group = 2;
        }
    }
    if (group != 1) goto online;
offline:
    return false;
online:
    return true;
}

// HYPOTHESIS: kind 4 keeps the stock in the three bits of the select data. Otherwise rule byte 2
// chooses the stock count (1) or the time in minutes.
void muSelCharTask::setRuleNumToGlobal() {
    gmSelCharData* sel = g_GameGlobal->m_selCharData;
    gmSetRule* rule = g_GameGlobal->m_setRule;
    int type = sel->m_selCharType;
    if (type == 4) {
        sel->unk2E_b4 = m_ruleNumStock;
        return;
    }
    int ruleKind = reinterpret_cast<u8*>(rule)[2];
    if (ruleKind == 1) {
        rule->m_stockCount = m_ruleNumStock;
    } else {
        rule->m_timeMinutes = unk5D8;
    }
}

int muSelCharTask::isWifiGameMaster() {
    u8 aid;
    muWifiInterfaceTask& wifi = *g_muWifiInterfaceTask;
    int master = wifi.unkFCA_b5;
    if (!master) {
        aid = Network::getMyAID();
        if (aid == 0xFF) {
            aid = 4;
        }
        u8 masterAid = wifi.getRuleMasterAID();
        master = masterAid == aid;
    }
    return master;
}

extern const u8 lbl_10_rodata_2F4[16];
void ScnObj_EnableCallbackTiming(nw4r::g3d::ScnObj* obj, u32 timing);
void ScnObj_EnableCallbackExecOp(nw4r::g3d::ScnObj* obj, u32 op);

// Puts the world-matrix callback in front of the callback on each listed model.
void muSelCharTask::bindCalcWorldFlag() {
    nw4r::g3d::ScnMdl* model;
    for (int index = 0; index < 15; ++index) {
        int obj;
        if (index >= 0 && index < 15) {
            obj = lbl_10_rodata_2F4[index];
        } else {
            obj = Obj_Num;
        }
        MuObject* object = m_objs[obj];
        if (object) {
            model = object->getSceneModel();
            m_calcWorldCBs[index].m_next = model->m_callback;
            model->m_callback = &m_calcWorldCBs[index];
            ScnObj_EnableCallbackExecOp(model, 4);
            ScnObj_EnableCallbackTiming(model, 1);
        }
    }
}

int muSelCharTask::dispTargetTotalHighScore() {
    int style = 1;
    int index = 0;
    int allClear = 0;
    do {
        if (isCharHighScoreCleared(index) == 0) {
            break;
        }
        index++;
    } while (index < 0x23);
    if (index >= 0x23) {
        allClear = 1;
    }
    if (allClear != 0) {
        style = 2;
    }
    setHighScoreStyle(style);
    if (allClear != 0) {
        int sum = 0;
        for (int c = 0; c < 0x23; c++) {
            sum += getCharHighScore(c, m_difficulty, 0, 0);
        }
        dispHighScore(3, sum, 1);
    } else {
        eraseHighScore(3);
    }
    return allClear;
}

// HYPOTHESIS: byte flag, then each character as a word and its color as a byte.
struct muSelCharScorePair {
    u8 m_valid;
    char _pad1[3];
    int m_charKind0;
    u8 m_color0;
    char _pad2[3];
    int m_charKind1;
    u8 m_color1;
};

void muSelCharTask::dispOnlinePairHighScore() {
    setHighScoreStyle(3);
    muSelCharScorePair chars;
    int style;
    int level;
    int score = getOnlinePairHighScore(&style, &level, &chars);
    dispHighScore(4, score, style);
    dispHighScoreDiffLevel(level);
    int valid = chars.m_valid;
    if (valid != 0) {
        dispHighScorePairChar(0, chars.m_charKind0, chars.m_color0);
        dispHighScorePairChar(1, chars.m_charKind1, chars.m_color1);
    } else {
        eraseHighScorePairChar(0);
        eraseHighScorePairChar(1);
    }
}

void muSelCharTask::dispPairHighScore() {
    setHighScoreStyle(3);
    muSelCharScorePair chars;
    int style;
    int level;
    int score = getPairHighScore(m_difficulty, &style, &level, &chars);
    dispHighScore(4, score, style);
    int valid = chars.m_valid;
    if (valid != 0) {
        dispHighScorePairChar(0, chars.m_charKind0, chars.m_color0);
        dispHighScorePairChar(1, chars.m_charKind1, chars.m_color1);
    } else {
        eraseHighScorePairChar(0);
        eraseHighScorePairChar(1);
    }
    if (getSelCharType() != 7) {
        dispHighScoreDiffLevel(level);
    }
}

// Marks a character as seen and takes the "not seen" picture off its icon.
void muSelCharTask::setReferFlag(int selchKind) {
    int referred = isCharRefered__6muMenuFiii(exchangeMuSelchkindToMuCharKind__6muMenuFiii(selchKind));
    int state = 1;
    if (referred != 0) {
        state = 2;
    }
    if (state != 2) {
        setCharRefered__6muMenuFiii(exchangeMuSelchkindToMuCharKind__6muMenuFiii(selchKind));
        int idx = getCharListIdx(selchKind);
        // HYPOTHESIS: 0x70 is the first "not seen" picture; expand 1 holds those pictures.
        m_expands[1]->Remove(m_objs[idx + 0x70]->m_sceneModel);
    }
}

bool muSelCharTask::closeWifiDlg() {
    if (unk60C != nullptr) {
        unk60C->exit();
        unk60C = nullptr;
        setRuleControlEnable(1);
        if (m_ruleTaskState == 0) {
            for (int i = 0; i < m_numPlayerAreas; i++) {
                m_selCharPlayerAreas[i]->m_selCharHand->setBanVisible(false);
            }
            for (int i = 0; i < m_numPlayerAreas; i++) {
                m_selCharPlayerAreas[i]->m_selCharCoin->setBanVisible(false);
            }
        }
        return true;
    }
    return false;
}

void muSelCharTask::faceIconMain() {
    // BYTES: a cursor stepped by one pointer keeps the picture in r31 and the cursor in r30.
    MuObject* icon;
    muSelCharTask* cursor = this;
    int i = 0;
    while (i < m_charListNum) {
        icon = cursor->m_objs[Obj_FaceBase];
        if (icon->m_modelAnim->m_anmObjChrRes->GetUpdateRate() <= 0.0f) {
            icon->m_scnMdl->SetPriorityDrawOpa(i + 0x17);
            icon->m_scnMdl->SetPriorityDrawXlu(i + 0x17);
        }
        cursor = reinterpret_cast<muSelCharTask*>(reinterpret_cast<char*>(cursor) + 4);
        i++;
    }
}

sndSystem* sndSystem::getInstance() {
    return g_sndSystem;
}

MuSelchkind muSelCharPlayerArea::getCharKind() {
    return m_charKind;
}

int muSelCharPlayerArea::getDirRepeat() {
    return m_menuController.m_trigger;
}

int gfPadStatus::isButtonAOn() {
    return (m_buttonsCurrentFrame2.bits >> 8) & 1;
}

int gfPadStatus::isButtonAOnEdge() {
    return (m_buttonsPressedThisFrame.bits >> 8) & 1;
}

int gfPadStatus::isButtonAOnRepeat() {
    return (m_buttonsPressedThisFrame2.bits >> 8) & 1;
}

void muSelCharTask::removeCursor(MuObject* cursor) {
    cursor->m_sceneModel->DetachFromParent();
}

void muSelCharTask::readyStateMain() {
    if (m_readyState == 1) {
        MuObject* object = m_objs[Obj_ReadyButton];
        if (object->isNodeAnimFinished()) {
            m_readyState = 2;
            object->changeAnimN(m_readyButtonPointed ? 2 : 1, 0x11, true, false);
            object = m_objs[1];
            int selCharType = getSelCharType();
            int group;
            if (isSelCharTypeIn(selCharType, 0, 4)) {
                group = 0;
            } else if (isSelCharTypeIn(selCharType, 4, 12)) {
                group = 1;
            } else {
                bool inGroup = isSelCharTypeIn(selCharType, 12, 16);
                group = 3;
                if (inGroup) {
                    group = 2;
                }
            }
            object->changeAnimN(lbl_10_rodata_4E8[group][1], 0x18, true, false);
        }
    }
}

void muSelCharTask::setVisibleAllPlayersHandi(bool visible) {
    for (int i = 0; i < m_numPlayerAreas; i++) {
        m_selCharPlayerAreas[i]->setHandiVisible(visible);
    }
    if (!visible) {
        int i;
        int* modeArg = nullptr;
        int arg;
        modeArg = &arg;
        for (i = 0; i < m_numPlayerAreas; i++) {
            muSelCharHand* hand = getHand(i);
            if (modeArg != nullptr) {
                arg = hand->m_modeArg;
            }
            // HYPOTHESIS: mode 5 is the one that opens the handicap list.
            if (hand->m_mode == 5) {
                getHand(i)->setMode(0, 0);
                m_selCharPlayerAreas[arg]->closeHandiList();
            }
        }
    }
}

// HYPOTHESIS: the style of the total high score for each kind of high score, in rows of six bytes.
extern const u8 lbl_10_rodata_128[][6];

void muSelCharTask::dispVersusHighScore() {
    const u8* styles;
    int offset = getSelCharTypeHighScoreKind(getSelCharType()) * 6;
    styles = lbl_10_rodata_128[0];
    setHighScoreStyle(6);
    int c;
    int sum = 0;
    for (c = 0; c < 0x23; c++) {
        sum += getCharHighScore(c, m_difficulty, 0, 0);
    }
    dispHighScore(3, sum, offset[styles]);
}

extern "C" int exchangeSelchkind2SelCharNarrationSndID__6muMenuFiii(int kind);
extern "C" int exchangeMuSelchkind2MuStockchkind__6muMenuFiii(int kind);

// The original passes the area index along; the function itself takes nothing.
extern "C" void arrangeCoinPrio__13muSelCharTaskFv(muSelCharTask* task, int areaIdx);

void muSelCharTask::decideCharKind(int areaIdx, int charKind) {
    muSelCharCoin* coin;
    muSelCharPlayerArea* area = m_selCharPlayerAreas[areaIdx];
    coin = area->m_selCharCoin;
    area->dispCharPicEffect();
    setFaceIconMotion(getCharListIdx(charKind), 2);
    coin->dispEffect();
    if (charKind != 0x29) {
        g_sndSystem->playSE((SndID)exchangeSelchkind2SelCharNarrationSndID__6muMenuFiii(
                                area->exchangeCharKindDetail(area->m_charKind)), -1, 0, 0, -1);
    }
    if (area->m_playerKind == 1 && charKind != 0x29) {
        int controllerNo = area->m_controllerNo;
        if (controllerNo >= 0) {
            gfPadType::PadType kind = area->getControllerKind();
            if (kind != 0 && kind != 4) {
                unsigned int stock = exchangeMuSelchkind2MuStockchkind__6muMenuFiii(area->exchangeCharKindDetail(area->m_charKind));
                if (stock != -1) {
                    g_sndSystem->playSERem(controllerNo - 4, (SndID)stock, -1, 0);
                }
            }
        }
    }
    arrangeCoinPrio__13muSelCharTaskFv(this, areaIdx);
    area->sendSystemCharKind();
}

extern const int lbl_10_rodata_304[9];

int muSelCharTask::getFaceSize() {
    const int* tbl = lbl_10_rodata_304;
    int num = m_charListNum;
    int i;
    for (i = 0; i < 9; i++) {
        if (tbl[i] >= num) {
            break;
        }
    }
    return i;
}

extern "C" int fn_800C9F14(muWifiInterfaceTask* self, int index);
extern "C" int getTeamColor__19muWifiInterfaceTaskFi(muWifiInterfaceTask* self, int index);

bool muSelCharTask::canTeamBattle() {
    int selCharType = getSelCharType();
    if (!((getSelCharTypeData(selCharType)->m_unk4 >> 4) & 1)) {
        return false;
    }
    if (getSelCharTypeGroup(selCharType) == 2) {
        if (!isWifiGameMaster()) {
            return false;
        }
        muWifiInterfaceTask* wifi = g_muWifiInterfaceTask;
        for (int i = 0; i < m_numPlayerAreas; i++) {
            int state = fn_800C9F14(wifi, i);
            if ((state == 2 || state == 3) && getTeamColor__19muWifiInterfaceTaskFi(wifi, i) == 3) {
                return false;
            }
        }
    }
    return true;
}

muSelCharTask::~muSelCharTask() {
    if (unk60C != nullptr) {
        unk60C->exit();
    }
    if (unk614 != nullptr) {
        unk614->exit();
    }
    if (unk68 != nullptr) {
        unk68->exit();
    }
    if (m_ruleTask != nullptr) {
        m_ruleTask->exit();
    }
    if (m_panelCtrl != nullptr) {
        m_panelCtrl->detachMsg();
        m_panelCtrl->exit();
    }
    for (int i = 0; i < num_player_areas; i++) {
        if (m_selCharPlayerAreas[i] != nullptr) {
            delete m_selCharPlayerAreas[i];
        }
    }
    muSelCharPlayerArea::clearCharPicTexResArchive();
    for (int i = 0; i < 4; i++) {
        if (unk54[i] != nullptr) {
            unk54[i]->Destroy();
        }
    }
    for (int i = 0; i < 56; i++) {
        if (m_allExpands[i] != nullptr) {
            m_allExpands[i]->Destroy();
        }
    }
    for (int i = 0; i < Obj_Num; i++) {
        if (m_objs[i] != nullptr) {
            delete m_objs[i];
        }
    }
    if (m_highScoreMsg != nullptr) {
        delete m_highScoreMsg;
    }
}

int muSelCharTask::getCharHighScore(int charKind, int difficulty, int* style, int* level) {
    u8 selCharType = getSelCharType();
    gmGlobalRecord::CharaNumData::SelCharView* view = &g_GameGlobal->m_record->m_charaNumData[charKind].m_selCharView;
    int offset = getSelCharTypeHighScoreKind(selCharType) * 6;
    const u8* styles = lbl_10_rodata_128[0] + offset;
    bool flag = false;
    int lv = -1;
    int score;
    switch (selCharType) {
    case 4:
        lv = view->unk9C_b10 - 1;
        score = view->unk4;
        break;
    case 5:
        lv = view->unk9C_7 - 1;
        score = view->unk8;
        break;
    case 6:
    case 15:
        lv = view->unk9C_4 - 1;
        score = view->unkC;
        break;
    case 7:
        score = view->unkCC[difficulty].unk0_low;
        if (view->unkCC[difficulty].unk0_30 == 1) {
            flag = true;
        }
        break;
    case 8:
    case 14:
        score = view->unk10;
        break;
    case 9:
    case 13:
        switch (m_kumiteType) {
        case 0:
            score = view->unk14;
            if (view->unk9C_14) {
                flag = true;
            }
            break;
        case 1:
            score = view->unk18;
            if (view->unk9C_13) {
                flag = true;
            }
            break;
        case 2:
            score = view->unk1C;
            if (view->unk9C_12) {
                flag = true;
            }
            break;
        case 3:
            score = view->unk20;
            if (view->unk9C_11) {
                flag = true;
            }
            break;
        case 4:
            score = view->unk24;
            break;
        case 5:
            score = view->unk28;
            break;
        }
        break;
    case 10:
        score = view->unk2C;
        break;
    case 11:
    case 12:
        break;
    }
    if (style != nullptr) {
        int s0 = styles[0];
        if (styles[1] != 7 && (lv >= 0 || flag)) {
            s0 = styles[1];
        }
        *style = s0;
    }
    if (level != nullptr) {
        *level = lv;
    }
    return score;
}

extern "C" int fn_10_AA74(muSelCharTask* task);

int muSelCharTask::readyProc() {
    int decided = 0;
    int ready = fn_10_AA74(this);
    if (ready != 0) {
        if (m_readyState == 0) {
            setReadyState(1);
            g_sndSystem->playSE((SndID)0x1EEC, -1, 0, 0, -1);
        }
    } else {
        if (m_readyState != 0) {
            setReadyState(0);
        }
    }
    if (ready != 0 && unk64C < 0) {
        int i;
        for (i = 0; i < m_numPlayerAreas; i++) {
            int ctrlNo = m_selCharPlayerAreas[i]->m_controllerNo;
            if (ctrlNo >= 0) {
                gfPadStatus pad;
                g_gfPadSystem->getSysPadStatus(ctrlNo, &pad);
                if (pad.m_error == 0) {
                    decided = 0;
                    if (pad.m_buttonsPressedThisFrame.bits & 0x1000) {
                        decided = 1;
                    } else if (pad.m_buttonsPressedThisFrame.bits & 0x100) {
                        if (m_selCharPlayerAreas[i]->m_selCharHand->getCollKind(nullptr, nullptr) == 8) {
                            decided = 1;
                        }
                    }
                    if (decided != 0) {
                        setReadyButtonDecided();
                        g_sndSystem->playSE((SndID)1, -1, 0, 0, -1);
                        g_sndSystem->playSE((SndID)0x16, -1, 0, 0, -1);
                        break;
                    }
                }
            }
        }
        if (i < m_numPlayerAreas) {
            g_sndSystem->playSE((SndID)1, -1, 0, 0, -1);
            for (int j = 0; j < m_numPlayerAreas; j++) {
                m_selCharPlayerAreas[j]->rumbleController();
            }
            setState(1, i, 0);
        }
    }
    return decided;
}

#include <gf/gf_heap_manager.h>
#include <mt/mt_prng.h>
#include <new>
#include <gf/gf_pad_status.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <cstdio>
// TODO: clean up, give variables real names
namespace jj_1290 {

struct SelCharSelf {
    char pad000[0x5C8];
    u8 m5C8;
    char pad5C9[0x5CC - 0x5C9];
    s32 m5CC;
    char pad5D0[0x5D1 - 0x5D0];
    u8 m5D1;
    char pad5D2[0x5E0 - 0x5D2];
    s32 m5E0;
    char pad5E4[0x600 - 0x5E4];
    s32 m600;
    s32 m604;
    char pad608[0x620 - 0x608];
    s32 m620;
    char pad624[0x640 - 0x624];
    u8 m640;
    char pad641[0x650 - 0x641];
    s32 m650;
};

extern "C" {
extern void* g_muWifiInterfaceTask;
u8 getMyAID__7NetworkFv(void);
void fn_10_1BBC(SelCharSelf* self);
u8 getRuleMasterAID__19muWifiInterfaceTaskFv(void* task);
void getSelCharGameRule__19muWifiInterfaceTaskFPvb(void* task, u8* buf, int n);
s32 getSelCharMeleeKind__19muWifiInterfaceTaskFv(void* task);
}

extern "C" void fn_10_1290(SelCharSelf* self) {
    u8 buf[2];
    unsigned char* sel;
    int state;
    int result;
    int w;
    int r0_4;
    int r0_5;
    int cond;
    int flag;

    sel = (unsigned char*)g_GameGlobal->m_selCharData;
    if (g_muWifiInterfaceTask != 0) {
        getSelCharGameRule__19muWifiInterfaceTaskFPvb(g_muWifiInterfaceTask, buf, 1);
    }

    state = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
    if (state >= 0 && state < 4) result = 1; else result = 0;
    if (result != 0) {
        result = 0;
    } else {
        if (state >= 4 && state < 0xc) result = 1; else result = 0;
        if (result != 0) {
            result = 1;
        } else {
            if (state >= 0xc && state < 0x10) r0_4 = 1; else r0_4 = 0;
            result = 3;
            if (r0_4 != 0) result = 2;
        }
    }
    if (result == 2) {
        u8 v2;
        u8 cur2;
        unsigned char* task2 = (unsigned char*)g_muWifiInterfaceTask;
        int bit2 = (task2[0xFCA] >> 5) & 1;
        if (!bit2) {
            v2 = getMyAID__7NetworkFv();
            if (v2 == 0xFF) v2 = 4;
            cur2 = getRuleMasterAID__19muWifiInterfaceTaskFv(task2);
            bit2 = (cur2 == v2);
        }
        if (bit2 == 0) {
            if (getSelCharMeleeKind__19muWifiInterfaceTaskFv(g_muWifiInterfaceTask) == 1) self->m5C8 = 1; else self->m5C8 = 0;
            goto L1414;
        }
    }
    if (sel[0x33] == 1) self->m5C8 = 1; else self->m5C8 = 0;
L1414:

    {
        int state3 = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
        if (state3 >= 0 && state3 < 4) result = 1; else result = 0;
        if (result != 0) {
            result = 0;
        } else {
            if (state3 >= 4 && state3 < 0xc) result = 1; else result = 0;
            if (result != 0) {
                result = 1;
            } else {
                if (state3 >= 0xc && state3 < 0x10) r0_4 = 1; else r0_4 = 0;
                result = 3;
                if (r0_4 != 0) result = 2;
            }
        }
        if (result == 0) goto c3f;
        if (state3 >= 0 && state3 < 4) w = 1; else w = 0;
        if (w != 0) {
            w = 0;
        } else {
            if (state3 >= 4 && state3 < 0xc) w = 1; else w = 0;
            if (w != 0) {
                w = 1;
            } else {
                if (state3 >= 0xc && state3 < 0x10) r0_5 = 1; else r0_5 = 0;
                w = 3;
                if (r0_5 != 0) w = 2;
            }
        }
        if (w != 1) goto c3g;
c3f:
        cond = 0;
        goto c3e;
c3g:
        cond = 1;
c3e:;
        if (cond != 0) {
            unsigned char* task3 = (unsigned char*)g_muWifiInterfaceTask;
            int bit3 = (task3[0xFCA] >> 5) & 1;
            if (!bit3) {
                u8 v3 = getMyAID__7NetworkFv();
                if (v3 == 0xFF) v3 = 4;
                u8 cur3 = getRuleMasterAID__19muWifiInterfaceTaskFv(task3);
                bit3 = (cur3 == v3);
            }
            if (bit3 == 0) {
                self->m5CC = (buf[0] >> 4) & 3;
                goto L15C4;
            }
        }
        self->m5CC = ((unsigned char*)g_GameGlobal->m_setRule)[5];
L15C4:;
    }
    fn_10_1BBC(self);

    {
        int state4 = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
        if (state4 >= 0 && state4 < 4) result = 1; else result = 0;
        if (result != 0) {
            result = 0;
        } else {
            if (state4 >= 4 && state4 < 0xc) result = 1; else result = 0;
            if (result != 0) {
                result = 1;
            } else {
                if (state4 >= 0xc && state4 < 0x10) r0_4 = 1; else r0_4 = 0;
                result = 3;
                if (r0_4 != 0) result = 2;
            }
        }
        if (result == 0) goto c4f;
        if (state4 >= 0 && state4 < 4) w = 1; else w = 0;
        if (w != 0) {
            w = 0;
        } else {
            if (state4 >= 4 && state4 < 0xc) w = 1; else w = 0;
            if (w != 0) {
                w = 1;
            } else {
                if (state4 >= 0xc && state4 < 0x10) r0_5 = 1; else r0_5 = 0;
                w = 3;
                if (r0_5 != 0) w = 2;
            }
        }
        if (w != 1) goto c4g;
c4f:
        cond = 0;
        goto c4e;
c4g:
        cond = 1;
c4e:;
        if (cond == 0 && self->m5D1 != 0) self->m5CC = 0;
    }

    {
        int state5 = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
        if (state5 == 8 || state5 == 0xe) flag = 1; else flag = 0;
        if (flag != 0) {
            if (state5 >= 0 && state5 < 4) result = 1; else result = 0;
            if (result != 0) {
                result = 0;
            } else {
                if (state5 >= 4 && state5 < 0xc) result = 1; else result = 0;
                if (result != 0) {
                    result = 1;
                } else {
                    if (state5 >= 0xc && state5 < 0x10) r0_4 = 1; else r0_4 = 0;
                    result = 3;
                    if (r0_4 != 0) result = 2;
                }
            }
            if (result == 0) goto c5f;
            if (state5 >= 0 && state5 < 4) w = 1; else w = 0;
            if (w != 0) {
                w = 0;
            } else {
                if (state5 >= 4 && state5 < 0xc) w = 1; else w = 0;
                if (w != 0) {
                    w = 1;
                } else {
                    if (state5 >= 0xc && state5 < 0x10) r0_5 = 1; else r0_5 = 0;
                    w = 3;
                    if (r0_5 != 0) w = 2;
                }
            }
            if (w != 1) goto c5g;
c5f:
            cond = 0;
            goto c5e;
c5g:
            cond = 1;
c5e:;
            if (cond != 0) {
                unsigned char* task5 = (unsigned char*)g_muWifiInterfaceTask;
                int bit5 = (task5[0xFCA] >> 5) & 1;
                if (!bit5) {
                    u8 v5 = getMyAID__7NetworkFv();
                    if (v5 == 0xFF) v5 = 4;
                    u8 cur5 = getRuleMasterAID__19muWifiInterfaceTaskFv(task5);
                    bit5 = (cur5 == v5);
                }
                if (bit5 == 0) {
                    self->m600 = (buf[1] >> 4) & 1;
                    goto L18E4;
                }
            }
            self->m600 = sel[1];
        }
L18E4:;
    }

    {
        int state6 = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
        if (state6 == 9 || state6 == 0xd) flag = 1; else flag = 0;
        if (flag != 0) {
            if (state6 >= 0 && state6 < 4) result = 1; else result = 0;
            if (result != 0) {
                result = 0;
            } else {
                if (state6 >= 4 && state6 < 0xc) result = 1; else result = 0;
                if (result != 0) {
                    result = 1;
                } else {
                    if (state6 >= 0xc && state6 < 0x10) r0_4 = 1; else r0_4 = 0;
                    result = 3;
                    if (r0_4 != 0) result = 2;
                }
            }
            if (result == 0) goto c6f;
            if (state6 >= 0 && state6 < 4) w = 1; else w = 0;
            if (w != 0) {
                w = 0;
            } else {
                if (state6 >= 4 && state6 < 0xc) w = 1; else w = 0;
                if (w != 0) {
                    w = 1;
                } else {
                    if (state6 >= 0xc && state6 < 0x10) r0_5 = 1; else r0_5 = 0;
                    w = 3;
                    if (r0_5 != 0) w = 2;
                }
            }
            if (w != 1) goto c6g;
c6f:
            cond = 0;
            goto c6e;
c6g:
            cond = 1;
c6e:;
            if (cond != 0) {
                unsigned char* task6 = (unsigned char*)g_muWifiInterfaceTask;
                int bit6 = (task6[0xFCA] >> 5) & 1;
                if (!bit6) {
                    u8 v6 = getMyAID__7NetworkFv();
                    if (v6 == 0xFF) v6 = 4;
                    u8 cur6 = getRuleMasterAID__19muWifiInterfaceTaskFv(task6);
                    bit6 = (cur6 == v6);
                }
                if (bit6 == 0) {
                    self->m604 = (buf[1] >> 5) & 7;
                    goto L1AAC;
                }
            }
            self->m604 = sel[2];
        }
L1AAC:;
    }

    state = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
    if (state == 1) self->m620 = sel[3];

    state = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
    if (state == 0xf) {
        unsigned char* task8 = (unsigned char*)g_muWifiInterfaceTask;
        int bit8 = (task8[0xFCA] >> 5) & 1;
        if (!bit8) {
            u8 v8 = getMyAID__7NetworkFv();
            if (v8 == 0xFF) v8 = 4;
            u8 cur8 = getRuleMasterAID__19muWifiInterfaceTaskFv(task8);
            bit8 = (cur8 == v8);
        }
        if (bit8 == 0) {
            self->m5E0 = (buf[1] >> 1) & 7;
            goto L1B50;
        }
    }
    self->m5E0 = (sel[0x2e] >> 5) & 7;
L1B50:

    state = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
    if (state == 0xc) {
        if (sel[6] != 0) self->m640 = 1;
    }
    self->m650 = sel[8];
}
}

// TODO: clean up, give variables real names
namespace jj_2834 {

static inline int mode(int state) {
    int result;
    if (state >= 0 && state < 4) result = 1; else result = 0;
    if (result != 0) {
        result = 0;
    } else {
        if (state >= 4 && state < 0xc) result = 1; else result = 0;
        if (result != 0) {
            result = 1;
        } else {
            int r;
            if (state >= 0xc && state < 0x10) r = 1; else r = 0;
            result = 3;
            if (r != 0) result = 2;
        }
    }
    return result;
}

static inline int special(int state) {
    if (mode(state) == 0) goto no;
    if (mode(state) != 1) goto yes;
no:
    return 0;
yes:
    return 1;
}

static inline int allow(int state) {
    if (mode(state) == 1 || special(state) || state == 1)
        return 0;
    return 1;
}

extern "C" void fn_10_2834(void* arg0, int* arg1) {
    unsigned char* sel = (unsigned char*)g_GameGlobal->m_selCharData;
    arg1[0] = -1;
    arg1[1] = -1;
    arg1[2] = -1;
    arg1[3] = -1;
    int state = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
    if (mode(state) == 1) {
        if (sel[0] != 0)
            arg1[0] = sel[0] - 1;
        else
            arg1[0] = 0;
        arg1[1] = sel[0x11b] - 1;
        return;
    }

    unsigned char* entry = sel;
    int* out = arg1;
    for (int i = 0; i < *(int*)((char*)arg0 + 0x648); ++i) {
        if (i == 0) {
            int st = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
            if (special(st)) {
                arg1[0] = sel[0] - 1;
                goto next;
            }
        }
        *out = entry[0xbf] - 1;
next:
        entry += 0x5c;
        ++out;
    }

    state = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
    if (allow(state)) {
        gfPadStatus status;
        int* p = arg1;
        int j;
        for (j = 0; j < 4 && j < *(int*)((char*)arg0 + 0x648); ++p, ++j) {
            g_gfPadSystem->getSysPadStatus(j, &status);
            if (status.m_error == 0)
                *p = j;
        }

        int target;
        int i;
        int k = 0;
        p = arg1;
        for (i = 0; i < *(int*)((char*)arg0 + 0x648) && k < 4; ++p, ++i) {
            if (*p < 0) {
                int found = 0;
                while (k < 4 && !found) {
                    int cnt = *(int*)((char*)arg0 + 0x648);
                    target = k + 4;
                    int n;
                    for (n = 0; n < cnt; ++n) {
                        if (arg1[n] == target)
                            break;
                    }
                    if (n >= cnt) {
                        g_gfPadSystem->getSysPadStatus(target, &status);
                        if (status.m_error == 0) {
                            *p = target;
                            found = 1;
                        }
                    }
                    ++k;
                }
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_2DC0 {

extern "C" {
s32 fn_801AB6CC(void*, s32, s32);
s32 getFaceSize__13muSelCharTaskFv(void*);
s32 create__19muSelCharPlayerAreaFPv(void*);
void sendSystemCharKind__19muSelCharPlayerAreaFv(s32);
s32 getNodeID__8MuObjectFPCc(s32, char*);
void PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjUl(s32, s32, s32);
s32 getNumCharColor__6muMenuFiii(s32);
int sprintf(char*, const char*, ...);
extern mtRand lbl_805A0420;
extern char lbl_10_data_18[];
extern f32 lbl_10_rodata_38;
}

struct Params {
    s32 w10, w14, w18, w1c, w20, w24, w28, w2c, w30, w34, w38, w3c, w40, w44, w48, w4c, w50;
    u8 b54;
    u8 pad55[3];
    s32 w58, w5c, w60, w64, w68, w6c, w70, w74, w78, w7c, w80, w84, w88, w8c;
};

struct SelArrays {
    char pad[0x44];
    s32 objects[4];
    s32 models[4];
};

static inline s32 getMode(s32 st) {
    s32 r;
    if (st >= 0 && st < 4) r = 1; else r = 0;
    if (r != 0) {
        r = 0;
    } else {
        s32 a;
        if (st >= 4 && st < 0xc) a = 1; else a = 0;
        if (a != 0) {
            r = 1;
        } else {
            s32 b;
            if (st >= 0xc && st < 0x10) b = 1; else b = 0;
            r = 3;
            if (b != 0) r = 2;
        }
    }
    return r;
}

static inline s32 randomInt(s32 max) {
    return (s32)((f32)max * lbl_805A0420.mtRand::randf());
}

extern "C" void fn_10_2DC0(char* self, s32 arg1, s32* __restrict arg2, s32 arg3) {
    Params p;
    char buf[8];
    u8* sel;
    u8* player;
    s32 st;
    s32 r;
    s32 v;
    s32 model;
    void* mem;
    f32 f;

    sel = (u8*)g_GameGlobal->m_selCharData;

    p.w10 = 0; p.w14 = 0; p.w18 = 0; p.w1c = 0;
    p.w20 = 0; p.w24 = 0; p.w28 = 0; p.w2c = 0;
    p.w30 = 0; p.w34 = 0; p.w38 = 0; p.w3c = 0;
    p.w40 = 0; p.w44 = 0; p.w48 = 0; p.w4c = 1;
    p.w50 = 0; p.b54 = 0; p.w58 = -1; p.w5c = 0;
    p.w60 = 0; p.w64 = 0; p.w68 = 0; p.w6c = 0;
    p.w70 = 0; p.w74 = 0; p.w78 = 0; p.w7c = 0;
    p.w80 = 0; p.w84 = 0; p.w88 = 0; p.w8c = 0;

    mem = gfHeapManager::getMEMAllocator((Heaps::HeapType)0x2a);
    model = fn_801AB6CC(mem, 0, 0x28);

    s32 a3;
    s32 a2;
    s32 a1;
    s32 a0;
    a0 = arg2[0];
    a1 = arg2[2];
    a2 = arg2[5];
    a3 = arg2[7];
    p.w20 = model;
    p.w10 = a0;
    p.w14 = a1;
    p.w18 = a2;
    p.w1c = a3;
    p.w24 = *(s32*)(self + 0x6c);
    p.w2c = arg1;
    p.w30 = 0xc;
    p.w34 = 0x50;
    p.w70 = (s32)self;
    p.w74 = getFaceSize__13muSelCharTaskFv(self);
    p.w78 = 0x5a;
    p.w7c = arg1 + 0x4a;
    p.w80 = 0x62;
    p.w84 = arg1 + 0x63;
    p.w88 = arg2[11];
    p.w28 = *(s32*)(self + 0x430);

    p.w44 = *(s32*)(self + 0x70 + randomInt(*(s32*)(self + 0x138) - 1) * 4);

    if ((s32)((u8*)g_GameGlobal->m_selCharData)[4] == 17) {
        p.w48 = 0;
    } else {
        p.w48 = randomInt(getNumCharColor__6muMenuFiii(p.w44));
    }

    p.w4c = (s32)(lbl_10_rodata_38 * lbl_805A0420.mtRand::randf()) + 1;

    if (arg1 == 1 && (s32)((u8*)g_GameGlobal->m_selCharData)[4] == 0xa) {
        p.w6c = -1;
    } else {
        p.w6c = arg3;
    }

    player = sel + arg1 * 0x5c;
    st = ((u8*)g_GameGlobal->m_selCharData)[4];
    if (getMode(st) == 0 || getMode(st) == 1)
        r = 0;
    else
        r = 1;
    if (r != 0) {
        if (arg1 == 0) {
            v = 1;
        } else if ((s32)player[0xb9] == 0) {
            v = 1;
        } else {
            v = 0;
        }
    } else {
        s32 r0;
        if (st >= 0 && st < 4) r0 = 1; else r0 = 0;
        if (r0 != 0) {
            v = 0;
        } else {
            s32 a;
            if (st >= 4 && st < 0xc) a = 1; else a = 0;
            if (a != 0) {
                v = 1;
            } else {
                s32 b;
                if (st >= 0xc && st < 0x10) b = 1; else b = 0;
                v = 3;
                if (b != 0) v = 2;
            }
        }
        if (v == 1) {
            if (arg1 == 1 && st == 0xa) {
                v = 2;
            } else {
                v = 1;
            }
        } else {
            switch (player[0xb9]) {
            case 0:
                v = 1;
                break;
            case 1:
                v = 2;
                break;
            default:
                v = 0;
                break;
            }
        }
    }

    p.w38 = v;
    {
        s32 kind = sel[arg1 * 4 + 0xa];
        if (arg1 == 1 && (s32)((u8*)g_GameGlobal->m_selCharData)[4] == 0xa && kind == 0x28) {
            p.w3c = *(s32*)(self + 0x70 + randomInt(*(s32*)(self + 0x138) - 1) * 4);
            p.w40 = randomInt(getNumCharColor__6muMenuFiii(p.w3c));
        } else {
            p.w3c = kind;
            p.w40 = player[0xbd];
        }
    }
    p.w50 = player[0xc3];
    p.b54 = 0;
    if (p.w3c == 0x15 && p.w40 >= 6) {
        p.b54 = 1;
    }
    if (player[0xd0] == 0x78) {
        p.w58 = -1;
    } else {
        p.w58 = player[0xd0];
    }
    p.w5c = player[0xd1];
    if (*(u8*)(self + 0x5d0) != 0) {
        p.w60 = *(u16*)(player + 0xdc) / 10;
    } else {
        p.w60 = *(u16*)(player + 0xde) / 10;
    }
    p.w64 = player[0xd7];
    p.w68 = *(u16*)(sel + arg1 * 4 + 0xc);

    st = ((u8*)g_GameGlobal->m_selCharData)[4];
    switch (getMode(st)) {
    case 2:
    case 3:
        if (arg1 == 0) {
            p.w8c = 0;
        } else if (p.w38 == 1) {
            p.w8c = 0;
        } else {
            p.w8c = 1;
        }
        break;
    case 1:
        if (arg1 == 1) {
            if (st == 0xa) {
                p.w8c = 0;
            } else if (*(u8*)(self + 0x5fc) == 0) {
                p.w8c = 2;
            } else {
                p.w8c = 0;
            }
        }
        break;
    case 0:
        p.w8c = 0;
        break;
    }

    SelArrays* arrays = (SelArrays*)self;
    arrays->objects[arg1] = create__19muSelCharPlayerAreaFPv(&p.w10);
    sprintf(buf, lbl_10_data_18,
            *(s32*)(self + 0x648) == 2 ? arg1 + 5 : arg1 + 1);
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjUl(*(s32*)(self + 0x430), model, getNodeID__8MuObjectFPCc(*(s32*)(self + 0x168), buf));
    arrays->models[arg1] = model;
    if (p.w38 != 0) {
        sendSystemCharKind__19muSelCharPlayerAreaFv(arrays->objects[arg1]);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_4374 {


extern "C" {
int getSelCharTypeHighScoreKind__13muSelCharTaskFi(void*, int);
void setHighScoreStyle__13muSelCharTaskFi(void*, int);
int getCharHighScore__13muSelCharTaskFiiPiPi(void*, int, int, int, int);
int fn_10_B678(void*, int);
int fn_10_B79C(void*, int, int*, int*, unsigned char*);
int fn_10_BA4C(void*, int*, int*, unsigned char*);
void dispHighScoreDiffLevel__13muSelCharTaskFi(void*, int);
void fn_10_1046C(void*, int, int, unsigned char);
void eraseHighScorePairChar__13muSelCharTaskFi(void*, int);
void updateCharHighScore__13muSelCharTaskFiii(void*, int, int, int);
extern const unsigned char lbl_10_rodata_128[];
}

struct Info {
    unsigned char flag;
    int a;
    unsigned char b;
    int c;
    unsigned char d;
};

struct BoolWrap {
    bool b;
    BoolWrap(int n) { b = n != 0; }
};

extern "C" void fn_10_4374(muSelCharTask* self) {
    int b, a, d, c;
    Info out1, out2;
    int idx = getSelCharTypeHighScoreKind__13muSelCharTaskFi(self, ((unsigned char*)g_GameGlobal->m_selCharData)[4]);
    unsigned char storage[sizeof(BoolWrap)];
    BoolWrap* valid = new (storage) BoolWrap(idx);
    const unsigned char* entry = lbl_10_rodata_128 + idx * 6;
    int totalFlag = 0;
    unsigned char scoreType;
    int usePlayers = 1;
    int useScore = 1;

    if (valid->b) {
        int state = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
        int category;

        if (state >= 0 && state < 4)
            category = 1;
        else
            category = 0;

        if (category != 0) {
            category = 0;
        } else {
            if (state >= 4 && state < 12)
                category = 1;
            else
                category = 0;

            if (category != 0) {
                category = 1;
            } else {
                int result;
                if (state >= 12 && state < 16)
                    result = 1;
                else
                    result = 0;
                category = 3;
                if (result != 0)
                    category = 2;
            }
        }

        switch (category) {
        case 1:
            if (entry[4] && *((unsigned char*)self + 0x5fc)) {
                int special;
                if (state == 8 || state == 14)
                    special = 1;
                else
                    special = 0;

                if (special && *(int*)((char*)self + 0x600) == 1) {
                    const unsigned char* table;
                    int index = getSelCharTypeHighScoreKind__13muSelCharTaskFi(self, state) * 6;
                    table = lbl_10_rodata_128;
                    setHighScoreStyle__13muSelCharTaskFi(self, 6);
                    int total = 0;
                    int i = 0;
                    do {
                        total += getCharHighScore__13muSelCharTaskFiiPiPi(
                            self, i, *(int*)((char*)self + 0x5e0), 0, 0);
                        i++;
                    } while (i < 0x23);
                    self->dispHighScore(3, total, table[index]);
                } else {
                    setHighScoreStyle__13muSelCharTaskFi(self, 3);
                    int result = fn_10_B79C(
                        self, *(int*)((char*)self + 0x5e0),
                        &a, &b, &out1.flag);
                    self->dispHighScore(4, result, a);
                    if (out1.flag) {
                        fn_10_1046C(self, 0, out1.a, out1.b);
                        fn_10_1046C(self, 1, out1.c, out1.d);
                    } else {
                        eraseHighScorePairChar__13muSelCharTaskFi(self, 0);
                        eraseHighScorePairChar__13muSelCharTaskFi(self, 1);
                    }
                    if ((int)((unsigned char*)g_GameGlobal->m_selCharData)[4] != 7)
                        dispHighScoreDiffLevel__13muSelCharTaskFi(self, b);
                    usePlayers = 0;
                }
                useScore = 0;
            }
            break;

        case 2:
        case 3:
            if (entry[5] &&
                (state != 14 || *(int*)((char*)self + 0x600) != 1)) {
                setHighScoreStyle__13muSelCharTaskFi(self, 3);
                int result = fn_10_BA4C(self, &c, &d, &out2.flag);
                self->dispHighScore(4, result, c);
                dispHighScoreDiffLevel__13muSelCharTaskFi(self, d);
                if (out2.flag) {
                    fn_10_1046C(self, 0, out2.a, out2.b);
                    fn_10_1046C(self, 1, out2.c, out2.d);
                } else {
                    eraseHighScorePairChar__13muSelCharTaskFi(self, 0);
                    eraseHighScorePairChar__13muSelCharTaskFi(self, 1);
                }
                usePlayers = 0;
                useScore = 0;
            }
            break;
        }

        if ((int)((unsigned char*)g_GameGlobal->m_selCharData)[4] == 7)
            dispHighScoreDiffLevel__13muSelCharTaskFi(self, *(int*)((char*)self + 0x5e0));

        if (usePlayers) {
            updateCharHighScore__13muSelCharTaskFiii(
                self, 0,
                *(int*)((char*)self->m_selCharPlayerAreas[0] + 0x1b8),
                *(int*)((char*)self + 0x5e0));

            if ((int)((unsigned char*)g_GameGlobal->m_selCharData)[4] == 8 &&
                *(int*)((char*)self + 0x600) == 1) {
                updateCharHighScore__13muSelCharTaskFiii(
                    self, 1,
                    *(int*)((char*)self->m_selCharPlayerAreas[1] + 0x1b8),
                    *(int*)((char*)self + 0x5e0));
            }

            if (useScore) {
                if (entry[3]) {
                    if (entry[1] == 7) {
                        scoreType = entry[0];
                        totalFlag = 1;
                    } else {
                        int i = 0;
                        do {
                            if (fn_10_B678(self, i) == 0)
                                break;
                            i++;
                        } while (i < 0x23);
                        if (i >= 0x23)
                            totalFlag = 1;
                        scoreType = entry[1];
                    }
                } else {
                    totalFlag = 0;
                }

                int mode;
                if ((int)((unsigned char*)g_GameGlobal->m_selCharData)[4] == 10) {
                    mode = 4;
                } else {
                    mode = 1;
                    if (totalFlag)
                        mode = 2;
                }
                setHighScoreStyle__13muSelCharTaskFi(self, mode);

                if (totalFlag) {
                    int i;
                    int total;
                    total = 0;
                    i = 0;
                    do {
                        total += getCharHighScore__13muSelCharTaskFiiPiPi(
                            self, i, *(int*)((char*)self + 0x5e0), 0, 0);
                        i++;
                    } while (i < 0x23);
                    self->dispHighScore(3, total, scoreType);
                }
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_4A54 {
extern "C" void setRuleControlEnable__13muSelCharTaskFUc(void* self, int flag);
extern "C" void setBanVisible__13muSelCharHandFb(void* obj, int v);
extern "C" void setBanVisible__13muSelCharCoinFb(void* obj, int v);
extern "C" void resetBButtonCnt__13muSelCharHandFv(void* p);
extern "C" void fn_10_B010(void* self, int v);
extern "C" int closeWifiDlg__13muSelCharTaskFv(void* self);
extern "C" void stopBGM__9sndSystemFi(void* snd, int v);
extern "C" int fn_800BF048(int, char*, unsigned long, void*, void*, int, int, int, int, int, int, int);
extern "C" void getPrintIndexData__7MessageFPvUlPPcPUl(void*, unsigned long, char**, unsigned long*);
extern char* g_GameGlobal;
extern void* g_sndSystem;

struct BF {
    unsigned char pad[0x2E];
    unsigned char hi : 3;
    unsigned char mid : 3;
    unsigned char lo : 2;
};

extern "C" void setState__13muSelCharTaskFiii(char* self, int state, signed char flag, unsigned long arg3) {
    unsigned long len;
    char* text;

    *(int*)(self + 0x40) = state;
    switch (state) {
    case 1: {
        int v = 0;
        *(int*)(self + 0x5F8) = 2;
        switch (*(int*)(self + 0x650)) {
        case 0:
            v = 0;
            break;
        case 1:
            v = 1;
            break;
        case 2: {
            int b = *(unsigned char*)(*(char**)((char*)g_GameGlobal + 0x1C) + 7);
            if (b == 1 || b == 3) {
                v = 1;
            } else {
                v = 0;
            }
            break;
        }
        }
        if (v != 0) {
            stopBGM__9sndSystemFi(g_sndSystem, 0);
        }
        break;
    }
    case 6: {
        unsigned long n;
        char* txt;
        getPrintIndexData__7MessageFPvUlPPcPUl(*(void**)(self + 0x610), 0x1A, &text, &len);
        n = len;
        txt = text;
        closeWifiDlg__13muSelCharTaskFv(self);
        setRuleControlEnable__13muSelCharTaskFUc(self, 0);
        *(int*)(self + 0x60C) = fn_800BF048(1, txt, n, *(void**)(self + 0x608), *(void**)(self + 0x6C), 0x74, 0x1E, 0x2A, 0x2A, 0, 0, 0xF0);
        {
            void** cur = (void**)self;
            for (int i = 0; i < *(int*)(self + 0x648); i++) {
                setBanVisible__13muSelCharHandFb(*(void**)((char*)cur[0x11] + 0x1A8), 1);
                setBanVisible__13muSelCharCoinFb(*(void**)((char*)cur[0x11] + 0x1AC), 1);
                cur++;
            }
        }
        {
            void** cur = (void**)self;
            for (int i = 0; i < *(int*)(self + 0x648); i++) {
                resetBButtonCnt__13muSelCharHandFv(*(void**)((char*)cur[0x11] + 0x1A8));
                cur++;
            }
        }
        break;
    }
    case 8:
        fn_10_B010(self, arg3);
        break;
    }

    if ((unsigned)(state - 3) <= 1U) {
        char* g10 = *(char**)((char*)g_GameGlobal + 0x10);
        char* g1c = *(char**)((char*)g_GameGlobal + 0x1C);
        if (*(signed char*)(g10 + 4) == 4) {
            ((BF*)g10)->mid = *(int*)(self + 0x5DC);
        } else {
            int c = *(unsigned char*)(g1c + 2);
            if (c == 1) {
                *(signed char*)(g1c + 4) = (signed char)*(int*)(self + 0x5DC);
            } else {
                *(signed char*)(g1c + 3) = (signed char)*(int*)(self + 0x5D8);
            }
        }
    }
    if (state == 1 || (unsigned)(state - 3) <= 1U) {
        char* g10 = *(char**)((char*)g_GameGlobal + 0x10);
        *(signed char*)(g10 + 5) = flag;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_5368 {


struct WifiTask {
    char pad[0xFCA];
    u8 flags;
};

struct SelCharTaskState {
    char pad000[0x40];
    s32 value40;
    void* areas;
    char pad048[0x24];
    s32 value6C;
    char pad070[0x558];
    u8 value5C8;
    char pad5C9[3];
    s32 value5CC;
    char pad5D0[4];
    s32 value5D4;
    s32 value5D8;
    s32 value5DC;
    s32 value5E0;
    char pad5E4[0x1C];
    s32 value600;
    s32 value604;
    s32 value608;
    void* value60C;
    Message* message;
    s32 value614;
    s32 timer;
    u8 value61C;
    char pad61D[0x19];
    s8 value636;
    u8 value637[4];
    char pad63B;
    s32 value63C;
    u8 value640;
    char pad641[3];
    s32 value644;
    s32 value648;
};

struct SelCharBits {
    u8 a : 2;
    u8 b : 2;
    u8 c : 2;
    u8 pad0 : 2;
    u8 d : 3;
    u8 e : 1;
    u8 pad1 : 4;
};

struct MessageArgs {
    u8 flag;
    char pad1[3];
    s32 a;
    u8 b;
    char pad2[3];
    s32 c;
    u8 d;
};

extern "C" {
extern WifiTask* g_muWifiInterfaceTask;
extern const char lbl_10_data_4C[];

int fn_800CBCB0(void*, s32*);
void closeWifiDlg__13muSelCharTaskFv(void*);
void* getErrorMsgID__19muWifiInterfaceTaskFi(s32);
int haveErrorCode__19muWifiInterfaceTaskFi(s32);
void setRuleControlEnable__13muSelCharTaskFUc(void*, u8);
void* fn_800BF048(int, void*, int, void*, void*, int, int, int, int, int, int, int);
void setBanVisible__13muSelCharHandFb(void*, u8);
void setBanVisible__13muSelCharCoinFb(void*, u8);
void dispSubMsg__15muNoticeWndTaskFPCc(void*, void*);
int fn_800CCA9C(void*, s32*, s32*);
int isPermittedEntry__19muWifiInterfaceTaskFi(void*, int);
void setState__13muSelCharTaskFiii(void*, int, int, void*);
u8 getMyAID__7NetworkFv();
u8 getRuleMasterAID__19muWifiInterfaceTaskFv(void*);
void setCounter__16muWifiCntWndTaskFii(s32, u32, int);
void removeObj__13muSelCharTaskFi(void*, int);
int canWifiEntry__13muSelCharTaskFv(void*);
void addObj__13muSelCharTaskFi(void*, int);
int fn_800D4EF0(void*, s32*, u8*, s32*);
void fn_10_C7E0(void*);
int getSelCharGameRule__19muWifiInterfaceTaskFPvb(void*, u8*, int);
void dispKumiteRule__13muSelCharTaskFi(void*, int);
void eraseAllHighScore__13muSelCharTaskFv(void*);
void setHighScoreStyle__13muSelCharTaskFi(void*, int);
int fn_10_BA4C(void*, s32*, s32*, MessageArgs*);
void dispHighScoreDiffLevel__13muSelCharTaskFi(void*, int);
void fn_10_1046C(void*, int, s32, u8);
void eraseHighScorePairChar__13muSelCharTaskFi(void*, int);
void dispHomerunRule__13muSelCharTaskFi(void*, int);
void fn_10_4374(void*);
void dispDiffLevel__13muSelCharTaskFib(void*, int, int);
void fn_10_C4C8(void*, int);
int memcmp(const void*, const void*, unsigned long);
void setSelCharGameRule__19muWifiInterfaceTaskFPCvb(void*, void*, u32);
void setVisibleAllPlayersHandi__13muSelCharTaskFb(void*, int);
void dispWifiPrompt__13muSelCharTaskFUl(void*, int);
}

static inline void openWifiMessage(SelCharTaskState* self, char* const& textData, const u32& lengthData) {
    char* text;
    u32 length;
    length = lengthData;
    text = textData;
    closeWifiDlg__13muSelCharTaskFv(self);
    setRuleControlEnable__13muSelCharTaskFUc(self, 0);
    self->value60C = fn_800BF048(
        0, text, length, (void*)self->value608,
        (void*)self->value6C, 0x74, 0x1E, 0x2A, 0x2A, 0, 0, 0xF0);
}

static inline void setAreasBusy(SelCharTaskState* self) {
    void** area = (void**)self;
    for (int i = 0; i < self->value648; ++i) {
        setBanVisible__13muSelCharHandFb(*(void**)((char*)area[0x11] + 0x1A8), 1);
        setBanVisible__13muSelCharCoinFb(*(void**)((char*)area[0x11] + 0x1AC), 1);
        ++area;
    }
}

static inline bool hasMatchingPlayerCount() {
    WifiTask* task = g_muWifiInterfaceTask;
    bool indicator = (task->flags >> 5) & 1;
    if (!indicator) {
        u8 local = getMyAID__7NetworkFv();
        if (local == 0xFF)
            local = 4;
        u8 current = getRuleMasterAID__19muWifiInterfaceTaskFv(task);
        indicator = (current == local);
    }
    return indicator;
}

#define CLASSIFY_STATE() \
    state = ((u8*)g_GameGlobal->m_selCharData)[4]; \
    if (state >= 0 && state < 4) result = 1; else result = 0; \
    if (result != 0) { \
        result = 0; \
    } else { \
        if (state >= 4 && state < 0xC) result = 1; else result = 0; \
        if (result != 0) { \
            result = 1; \
        } else { \
            int inRange; \
            if (state >= 0xC && state < 0x10) inRange = 1; else inRange = 0; \
            result = 3; \
            if (inRange != 0) result = 2; \
        } \
    }

#define SYNC_PACKET() \
    CLASSIFY_STATE(); \
    if (result == 2) { \
        if (hasMatchingPlayerCount()) { \
            union { SelCharBits bits; u8 bytes[4]; } data; \
            memset(&data, 0, 4); \
            data.bits.a = self->value5D4; \
            data.bits.b = self->value5CC; \
            data.bits.c = (self->value5C8 != 0); \
            data.bytes[2] = self->value5D8; \
            data.bytes[3] = self->value5DC; \
            data.bits.d = self->value604; \
            data.bits.e = self->value600; \
            if (memcmp(&data, self->value637, 4) != 0) { \
                setSelCharGameRule__19muWifiInterfaceTaskFPCvb(g_muWifiInterfaceTask, &data, 1); \
                self->value637[0] = data.bytes[0]; \
                self->value637[1] = data.bytes[1]; \
                self->value637[2] = data.bytes[2]; \
                self->value637[3] = data.bytes[3]; \
            } \
        } \
    }

extern "C" void wifiMain__13muSelCharTaskFv(SelCharTaskState* self) {
    s32 value58[3];
    s32 value54;
    u32 value50;
    char* value4C;
    s32 value48;
    s32 value44;
    s32 value40;
    s32 value3C;
    u32 textLength;
    char* text;
    u32 value30;
    char* value2C;
    s32 value28;
    s32 value24;
    char buffer[0x80];
    int state;
    WifiTask* wifi;
    int result;
    MessageArgs args;
    char* savedText;
    u32 savedLength;

    wifi = g_muWifiInterfaceTask;
    if (self->value40 != 7) {
        result = fn_800CBCB0(wifi, &value54);
        if (result != 0) {
            closeWifiDlg__13muSelCharTaskFv(self);
            Message::getPrintIndexData(
                self->message, (u32)getErrorMsgID__19muWifiInterfaceTaskFi(result), &text, &textLength);

            savedLength = textLength;
            savedText = text;

            closeWifiDlg__13muSelCharTaskFv(self);
            setRuleControlEnable__13muSelCharTaskFUc(self, 0);
            self->value60C = fn_800BF048(
                0, savedText, savedLength, (void*)self->value608,
                (void*)self->value6C, 0x74, 0x1E, 0x2A, 0x2A, 0, 0, 0xF0);

            setAreasBusy(self);

            if (haveErrorCode__19muWifiInterfaceTaskFi(result) != 0) {
                Message::getPrintIndexData(self->message, 0x13, &value4C, &value50);
                memcpy(buffer, value4C, value50);
                value50 += sprintf(buffer + value50, lbl_10_data_4C, value54);
                dispSubMsg__15muNoticeWndTaskFPCc(self->value60C, buffer);
            }

            setState__13muSelCharTaskFiii(self, 7, 0, 0);
            return;
        }

        if (self->value640 != 0 && self->value644 != 2) {
            int selection = -1;
            if (self->value644 == 0) {
                switch (fn_800CCA9C(wifi, &value40, &value3C)) {
                case 1:
                    break;
                case 2:
                    self->value644 = 1;
                    break;
                case 6:
                    selection = 0x1B;
                    break;
                case 7:
                    selection = 0x1C;
                    break;
                }
            } else {
                switch (isPermittedEntry__19muWifiInterfaceTaskFi(wifi, 0)) {
                case 0:
                    break;
                case 1:
                    self->value644 = 2;
                    break;
                case 2:
                    selection = 0x1B;
                    break;
                }
            }

            if (selection >= 0) {
                closeWifiDlg__13muSelCharTaskFv(self);
                Message::getPrintIndexData(
                    self->message, selection, &value2C, &value30);

                openWifiMessage(self, value2C, value30);

                setAreasBusy(self);

                setState__13muSelCharTaskFiii(self, 7, 0, 0);
                self->value644 = 3;
                return;
            }
        }
    }

    if (self->value40 == 7 || self->value40 == 1)
        return;

    if (self->value61C != 0 && self->value40 != 1 && self->timer > 0) {
        --self->timer;
        if (self->timer <= 0x12C && self->timer % 60 == 0)
            g_sndSystem->playSE((SndID)0xA, -1, 0, 0, -1);

        setCounter__16muWifiCntWndTaskFii(self->value614, (self->timer + 0x3B) / 60, 0);
        if (self->timer <= 0) {
            setState__13muSelCharTaskFiii(self, 1, 0, 0);
            return;
        }
    }

    CLASSIFY_STATE();
    if (result != 2)
        return;

    if (self->value40 != 8) {
        if (fn_800D4EF0(wifi, value58, (u8*)self + 0x635, &value48) != 0) {
            closeWifiDlg__13muSelCharTaskFv(self);
            self->value636 = (u8)value48;
            setState__13muSelCharTaskFiii(self, 8, 0, value58);
        }
    }

    removeObj__13muSelCharTaskFi(self, 0xB8);
    if (canWifiEntry__13muSelCharTaskFv(self) != 0)
        addObj__13muSelCharTaskFi(self, 0xB8);

    if (hasMatchingPlayerCount())
        return;

    if (--self->value63C > 0)
        return;
    self->value63C = 0;

    if (getSelCharGameRule__19muWifiInterfaceTaskFPvb(wifi, (u8*)&value44, 0) == 0)
        return;

    state = ((u8*)g_GameGlobal->m_selCharData)[4];
    switch (state) {
    case 0xD:
        self->value604 = (((u8*)&value44)[1] >> 5) & 7;
        dispKumiteRule__13muSelCharTaskFi(self, self->value604);
        eraseAllHighScore__13muSelCharTaskFv(self);
        setHighScoreStyle__13muSelCharTaskFi(self, 3);
        result = fn_10_BA4C(self, &value24, &value28, &args);
        ((muSelCharTask*)self)->dispHighScore(4, result, value24);
        dispHighScoreDiffLevel__13muSelCharTaskFi(self, value28);

        if (args.flag != 0) {
            fn_10_1046C(self, 0, args.a, args.b);
            fn_10_1046C(self, 1, args.c, args.d);
        } else {
            eraseHighScorePairChar__13muSelCharTaskFi(self, 0);
            eraseHighScorePairChar__13muSelCharTaskFi(self, 1);
        }
        break;

    case 0xE:
        self->value600 = (((u8*)&value44)[1] >> 4) & 1;
        dispHomerunRule__13muSelCharTaskFi(self, self->value600);
        eraseHighScorePairChar__13muSelCharTaskFi(self, 0);
        eraseHighScorePairChar__13muSelCharTaskFi(self, 1);
        eraseAllHighScore__13muSelCharTaskFv(self);
        fn_10_4374(self);
        break;

    case 0xF:
        self->value5E0 = (((u8*)&value44)[1] >> 1) & 7;
        dispDiffLevel__13muSelCharTaskFib(self, self->value5E0, 1);
        break;

    default:
        result = (((u8*)&value44)[0] >> 2) & 3;
        if ((result != 0) != self->value5C8)
            fn_10_C4C8(self, result != 0);

        self->value5D4 = (((u8*)&value44)[0] >> 6) & 3;
        SYNC_PACKET();

        self->value5D8 = ((u8*)&value44)[2];
        SYNC_PACKET();

        self->value5DC = ((u8*)&value44)[3];
        SYNC_PACKET();

        fn_10_C7E0(self);
        result = (((u8*)&value44)[0] >> 4) & 3;
        if (result != self->value5CC) {
            setVisibleAllPlayersHandi__13muSelCharTaskFb(self, result == 2);
            self->value5CC = (((u8*)&value44)[0] >> 4) & 3;
        }
        break;
    }

    dispWifiPrompt__13muSelCharTaskFUl(self, 0x20);
    self->value63C = 0xB4;
}
}

// TODO: clean up, give variables real names
namespace jj_6914 {

struct Cursor;
struct TokenData { char pad[0x1b8]; int kind; };
struct Token { char pad[0x10]; Cursor* cursor; TokenData* data; };
struct Cursor {
    char pad[0x84];
    float x, y, z;
    float oldX, oldY, oldZ;
    char pad9c[4];
    Token* token;
    char pada4[0x28];
    float scale;
};
struct Player {
    char pad[0x1a8];
    Cursor* cursor;
    Token* token;
    int index;
    int state;
    char pad1b8[0x24];
    int controller;
};
struct Menu { char pad[0x44]; Player* players[1]; };

extern float lbl_10_rodata_74;
extern float lbl_10_rodata_0;
extern "C" int getControllerKind__19muSelCharPlayerAreaFv(void*);
extern "C" void setControllerNo__19muSelCharPlayerAreaFi(void*, int);
extern "C" int findFreeWiiControllerNo__13muSelCharTaskFv(void*);
extern "C" int getCoinNo__13muSelCharCoinFv(void*);
extern "C" void updateMtx__13muSelCharHandFv(void*);
extern "C" void setHand__13muSelCharCoinFP13muSelCharHand(void*, void*);
extern "C" void getFacePos__13muSelCharTaskFP5Vec2fi(void*, float*, int);
extern "C" void setPos__13muSelCharCoinFRC5Vec2f(void*, float*);
extern "C" void decideCharKind__13muSelCharTaskFii(void*, int, int);
extern "C" void fn_10_95B8(void*, int, int);
extern "C" int getPlayerNo__13muSelCharHandFv(void*);
extern "C" int getCursorObj__13muSelCharCoinFv(void*);
extern "C" void removeCursor__13muSelCharTaskFP8MuObject(void*, int);

extern "C" void fn_10_6914(Menu* self, int port) {
    Player** slot = &self->players[port];
    Cursor* cursor;
    Player* player = *slot;
    cursor = player->cursor;
    gfPadStatus status;
    float pos[2];
    if (player->controller >= 0) {
        if (getControllerKind__19muSelCharPlayerAreaFv(player) == 4) {
            int available = findFreeWiiControllerNo__13muSelCharTaskFv(self);
            if (available >= 0) {
                setControllerNo__19muSelCharPlayerAreaFi(player, available);
                return;
            }
            setControllerNo__19muSelCharPlayerAreaFi(player, -1);
            if (player->state != 0) {
                Token* token = cursor->token;
                if (token) {
                    int kind;
                    int owner = getCoinNo__13muSelCharCoinFv(token);
                    if (port != owner || player->state == 2) {
                        cursor->token = 0;
                        cursor->scale = lbl_10_rodata_74;
                        updateMtx__13muSelCharHandFv(cursor);
                        setHand__13muSelCharCoinFP13muSelCharHand(token, 0);
                        kind = token->data->kind;
                        if (kind == 0x28) {
                            kind = 0x29;
                            getFacePos__13muSelCharTaskFP5Vec2fi(self, pos, 0x29);
                            setPos__13muSelCharCoinFRC5Vec2f(token, pos);
                        }
                        decideCharKind__13muSelCharTaskFii(self, owner, kind);
                    }
                }
                if (player->state == 1) {
                    fn_10_95B8(self, port, 0);
                    slot = (Player**)self->players[player->index]->token;
                    player = (Player*)((Token*)slot)->cursor;
                    if (player && port != getPlayerNo__13muSelCharHandFv(player)) {
                        ((Cursor*)player)->token = 0;
                        ((Cursor*)player)->scale = lbl_10_rodata_74;
                        updateMtx__13muSelCharHandFv(player);
                        setHand__13muSelCharCoinFP13muSelCharHand(slot, 0);
                    }
                    cursor->token = (Token*)slot;
                    if (slot == 0) {
                        cursor->scale = lbl_10_rodata_74;
                        updateMtx__13muSelCharHandFv(cursor);
                    } else {
                        cursor->scale = lbl_10_rodata_0;
                        updateMtx__13muSelCharHandFv(cursor);
                    }
                    setHand__13muSelCharCoinFP13muSelCharHand(slot, cursor);
                    removeCursor__13muSelCharTaskFP8MuObject(self, getCursorObj__13muSelCharCoinFv(slot));
                }
            }
            return;
        } else if (getControllerKind__19muSelCharPlayerAreaFv(player) != 0) {
            g_gfPadSystem->getSysPadStatus(port, &status);
            if (status.m_error == 0) {
                setControllerNo__19muSelCharPlayerAreaFi(player, port);
                return;
            }
        }
    }
    if (player->controller < 0) {
        g_gfPadSystem->getSysPadStatus(port, &status);
        if (status.m_error == 0) {
            setControllerNo__19muSelCharPlayerAreaFi(player, port);
            Cursor* c = (*slot)->cursor;
            c->oldX = c->x;
            c->oldY = c->y;
            c->oldZ = c->z;
        } else {
            int available = findFreeWiiControllerNo__13muSelCharTaskFv(self);
            if (available >= 0)
                setControllerNo__19muSelCharPlayerAreaFi(player, available);
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_6C84 {


struct Vec2 {
    float x;
    float y;
};

extern "C" {
void main__13muSelCharCoinFv(void*);
Vec2 getPos__13muSelCharCoinFv(void*);
int getScreenArea__13muSelCharCoinFv(void*);
int getCollCharKind__13muSelCharTaskFP5Vec2f(void*, Vec2*);
void fn_10_1469C(void*, int);
int getCharListIdx__13muSelCharTaskFi(void*, int);
void setFaceIconMotion__13muSelCharTaskFii(void*, int, int);
void setCursor__13muSelCharTaskFiP8MuObject(void*, int, int);
void setReferFlag__13muSelCharTaskFi(void*, int);
int exchangeCharKindDetail__19muSelCharPlayerAreaF11MuSelchkind(void*, int);
void fn_10_FBD4(void*, int, int);
int getSelCharTypeHighScoreKind__13muSelCharTaskFi(void*, int);
void updateCharHighScore__13muSelCharTaskFiii(void*, int, int, int);
int getCursorObj__13muSelCharCoinFv(void*);
}

extern "C" void fn_10_6C84(void* self, int idx) {
    void* sub;
    void* area = *(void**)((u8*)self + idx * 4 + 0x44);
    sub = *(void**)((u8*)area + 0x1AC);
    main__13muSelCharCoinFv(sub);
    if (*(int*)((u8*)area + 0x1B4) != 0 &&
        *(int*)area == 0 &&
        *(int*)((u8*)sub + 0x18) != 1) {
        Vec2 pos;
        pos = getPos__13muSelCharCoinFv(sub);
        int n;
        if (getScreenArea__13muSelCharCoinFv(sub) == 1) {
            n = getCollCharKind__13muSelCharTaskFP5Vec2f(self, &pos);
        } else {
            n = 0x28;
        }
        if (n != *(int*)((u8*)area + 0x1B8)) {
            fn_10_1469C(area, n);
            if (n != 0x28) {
                setFaceIconMotion__13muSelCharTaskFii(self, getCharListIdx__13muSelCharTaskFi(self, n), 3);
            }
            setCursor__13muSelCharTaskFiP8MuObject(self, n, getCursorObj__13muSelCharCoinFv(sub));
            if (n != 0x29 && n != 0x28) {
                setReferFlag__13muSelCharTaskFi(self, n);
            }
            if (idx == 0 && (s32)((unsigned char*)g_GameGlobal->m_selCharData)[4] == 4) {
                void* first = *(void**)((u8*)self + 0x44);
                sub = *(void**)((u8*)area + 0x1BC);
                int result = exchangeCharKindDetail__19muSelCharPlayerAreaF11MuSelchkind(first, *(int*)((u8*)first + 0x1B8));
                fn_10_FBD4(self, result, (int)sub);
            }
            u8 flag;
            int r = getSelCharTypeHighScoreKind__13muSelCharTaskFi(self, (s32)((unsigned char*)g_GameGlobal->m_selCharData)[4]);
            if (&flag != NULL) {
                flag = r != 0;
            }
            int var;
            if (flag == 0) {
                var = 0;
            } else if (*(u8*)((u8*)self + 0x5FC) != 0) {
                s32 k = (s32)((unsigned char*)g_GameGlobal->m_selCharData)[4];
                int ok;
                if (k == 8 || k == 0xE) {
                    ok = 1;
                } else {
                    ok = 0;
                }
                if (ok != 0 && (u32)idx <= 1 && *(int*)((u8*)self + 0x600) == 1) {
                    var = 1;
                } else {
                    var = 0;
                }
            } else {
                var = (s32)(idx <= 0);
            }
            if (var != 0) {
                updateCharHighScore__13muSelCharTaskFiii(self, idx, n, *(int*)((u8*)self + 0x5E0));
            }
            g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_6FFC {

extern "C" {
s32 isPushedUnusedControllerStartButton__13muSelCharTaskFv(void);
void changeEntryModeToNormalMode__19muSelCharPlayerAreaFi(void* a, s32 b);
void fn_10_C4C8(void* self, u8 v);
void appearHomerunRule__13muSelCharTaskFv(void* self);
void eraseAllHighScore__13muSelCharTaskFv(void* self);
void setHighScoreStyle__13muSelCharTaskFi(void* self, s32 v);
s32 fn_10_B79C(void* self, s32 a, s32* b, s32* c, u8* d);
void fn_10_1046C(void* self, s32 idx, s32 a, u8 b);
void eraseHighScorePairChar__13muSelCharTaskFi(void* self, s32 idx);
void dispHighScoreDiffLevel__13muSelCharTaskFi(void* self, s32 v);
void fn_10_771C(void* self, s32 slot);
void fn_10_95B8(void* self, s32 slot, s32 v);
s32 exchangeCharKindDetail__19muSelCharPlayerAreaF11MuSelchkind(void* a, s32 b);
void fn_10_FBD4(void* self, s32 a, s32 b);
s32 isColorIncButtonPushed__19muSelCharPlayerAreaFv(void* a);
s32 isColorDecButtonPushed__19muSelCharPlayerAreaFv(void* a);
s32 getCoinNo__13muSelCharCoinFv(void* a);
s32 getScreenArea__13muSelCharHandFv(void* a);
void fn_10_16FB8(void* a);
void fn_10_17300(void* a);
void incCharColorNo__19muSelCharPlayerAreaFv(void* a);
void decCharColorNo__19muSelCharPlayerAreaFv(void* a);
void sendSystemCharKind__19muSelCharPlayerAreaFv(void* a);
void fn_800CA5F8(void* wifi, s32 a, s32 b);
void setMode__13muSelCharCoinFiP13muSelCharHand(void* a, s32 b, void* c);
void catchCoin__13muSelCharHandFP13muSelCharCoin(void* a, void* b);
void takeCoin__13muSelCharHandFP13muSelCharCoin(void* a, void* b);
void fn_10_8500(void* self, s32 slot, gfPadStatus* p);
void fn_10_7A94(void* self, s32 slot, gfPadStatus* p);
void fn_10_883C(void* self, s32 slot, gfPadStatus* p);
void playSE__9sndSystemF5SndIDiiii(void* self, s32 id, s32 a, s32 b, s32 c, s32 d);
void getSysPadStatus__11gfPadSystemFiP11gfPadStatus(void* self, s32 idx, gfPadStatus* p);
}

struct SelOut {
    u8 flag;
    s32 a;
    u8 b;
    s32 c;
    u8 d;
    s32 e;
};

struct SelVec2 {
    f32 x;
    f32 y;
};

struct SelF {
    f32 v;
    f32 w;
};

static inline SelVec2 selGetPos(u8* h) {
    SelVec2 r;
    r.y = *(f32*)(h + 0x94);
    r.x = *(f32*)(h + 0x90);
    return r;
}

extern "C" void fn_10_6FFC(muSelCharTask* self, s32 slot) {
    gfPadStatus sp48;
    SelOut out;
    s32 spC;
    s32 sp8;
    u8* hand;
    s32 bit;
    s32 n;
    s32 r27;
    u8* a2;
    u8* other;
    u8* area;
    u8* var3;
    s32 r3v;
    s32 idx;
    s32 state;
    s32 cls;
    s32 flag;

    area = (u8*)self->m_selCharPlayerAreas[slot];
    hand = *(u8**)(area + 0x1A8);
    if (*(s32*)area == 2) {
        s32 t = isPushedUnusedControllerStartButton__13muSelCharTaskFv();
        if (t >= 0) {
            changeEntryModeToNormalMode__19muSelCharPlayerAreaFi(area, t);
            playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0x1EEF, -1, 0, 0, -1);
            playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0x26, -1, 0, 0, -1);
            *((u8*)self + 0x5FC) = 1;
            fn_10_C4C8(self, *((u8*)self + 0x5C8));
            s32 s0 = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
            s32 f0;
            if (s0 == 8 || s0 == 0xE) {
                f0 = 1;
            } else {
                f0 = 0;
            }
            if (f0 != 0) {
                appearHomerunRule__13muSelCharTaskFv(self);
            }
            eraseAllHighScore__13muSelCharTaskFv(self);
            setHighScoreStyle__13muSelCharTaskFi(self, 3);
            s32 r = fn_10_B79C(self, *(s32*)((u8*)self + 0x5E0), &sp8, &spC, &out.flag);
            self->dispHighScore(4, r, sp8);
            if (out.flag != 0) {
                fn_10_1046C(self, 0, out.a, out.b);
                fn_10_1046C(self, 1, out.c, out.d);
            } else {
                eraseHighScorePairChar__13muSelCharTaskFi(self, 0);
                eraseHighScorePairChar__13muSelCharTaskFi(self, 1);
            }
            s32 s1 = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
            if (s1 != 7) {
                dispHighScoreDiffLevel__13muSelCharTaskFi(self, spC);
            }
            s32 s2 = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
            if (s2 == 7) {
                dispHighScoreDiffLevel__13muSelCharTaskFi(self, *(s32*)((u8*)self + 0x5E0));
            }
        }
    }
    idx = *(s32*)(area + 0x1DC);
    if (idx >= 0) {
        getSysPadStatus__11gfPadSystemFiP11gfPadStatus(g_gfPadSystem, idx, &sp48);
        s32 hs = *(s32*)(hand + 0xA4);
        if (hs != 0 && hs != 4) {
            fn_10_771C(self, slot);
            return;
        }
        SelF fx;
        SelVec2 vb;
        SelVec2 va;
        SelF fy;
        f32 ty = *(f32*)(hand + 0x94);
        f32 tx = *(f32*)(hand + 0x90);
        va.x = tx;
        va.y = ty;
        ((u32*)&vb)[0] = ((u32*)&va)[0];
        ((u32*)&vb)[1] = ((u32*)&va)[1];
        f32 by = vb.y;
        f32 bx = vb.x;
        fx.v = bx;
        fy.w = by;
        if (getScreenArea__13muSelCharHandFv(hand) != 2 && *(s32*)(area + 0x1B4) == 0 && *(u8**)(hand + 0xA0) != 0 && slot == getCoinNo__13muSelCharCoinFv(*(u8**)(hand + 0xA0))) {
            fn_10_95B8(self, slot, 1);
        }
        bit = (((u32*)&sp48)[3] >> 9) & 1;
        r27 = isColorIncButtonPushed__19muSelCharPlayerAreaFv(area);
        r3v = isColorDecButtonPushed__19muSelCharPlayerAreaFv(area);
        if ((r27 != 0 || r3v != 0) && (s32)((unsigned char*)g_GameGlobal->m_selCharData)[4] != 0x11) {
            if (*(u8**)(hand + 0xA0) != 0) {
                var3 = (u8*)self->m_selCharPlayerAreas[getCoinNo__13muSelCharCoinFv(*(u8**)(hand + 0xA0))];
            } else {
                var3 = area;
            }
            if (*((u8*)self + 0x5C8) != 0) {
                n = *(s32*)(var3 + 0x1B0);
                a2 = (u8*)self->m_selCharPlayerAreas[n];
                if (r27 != 0) {
                    fn_10_16FB8(a2);
                } else {
                    fn_10_17300(a2);
                }
                state = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
                if (state >= 0 && state < 4) {
                    cls = 1;
                } else {
                    cls = 0;
                }
                if (cls != 0) {
                    cls = 0;
                } else {
                    if (state >= 4 && state < 0xC) {
                        cls = 1;
                    } else {
                        cls = 0;
                    }
                    if (cls != 0) {
                        cls = 1;
                    } else {
                        if (state >= 0xC && state < 0x10) {
                            flag = 1;
                        } else {
                            flag = 0;
                        }
                        cls = 3;
                        if (flag != 0) {
                            cls = 2;
                        }
                    }
                }
                if (cls == 0) {
                    goto zero_lbl;
                }
                {
                    if (state >= 0 && state < 4) {
                        flag = 1;
                    } else {
                        flag = 0;
                    }
                    if (flag != 0) {
                        cls = 0;
                    } else {
                        if (state >= 4 && state < 0xC) {
                            flag = 1;
                        } else {
                            flag = 0;
                        }
                        if (flag != 0) {
                            cls = 1;
                        } else {
                            if (state >= 0xC && state < 0x10) {
                                flag = 1;
                            } else {
                                flag = 0;
                            }
                            cls = 3;
                            if (flag != 0) {
                                cls = 2;
                            }
                        }
                    }
                    if (cls != 1) {
                        goto one_lbl;
                    }
                }
zero_lbl:
                cls = 0;
                goto end_lbl;
one_lbl:
                cls = 1;
end_lbl:
                if (cls != 0) {
                    fn_800CA5F8(g_muWifiInterfaceTask, n, *(s32*)(a2 + 0x1C0));
                }
                sendSystemCharKind__19muSelCharPlayerAreaFv(a2);
            } else {
                a2 = (u8*)self->m_selCharPlayerAreas[*(s32*)(var3 + 0x1B0)];
                if (r27 != 0) {
                    incCharColorNo__19muSelCharPlayerAreaFv(a2);
                } else {
                    decCharColorNo__19muSelCharPlayerAreaFv(a2);
                }
                sendSystemCharKind__19muSelCharPlayerAreaFv(a2);
            }
            if (slot == 0 && (s32)((unsigned char*)g_GameGlobal->m_selCharData)[4] == 4) {
                s32 v1 = *(s32*)(area + 0x1BC);
                s32 v2 = exchangeCharKindDetail__19muSelCharPlayerAreaF11MuSelchkind(area, *(s32*)(area + 0x1B8));
                fn_10_FBD4(self, v2, v1);
            }
            playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0, -1, 0, 0, -1);
        }
        if (bit != 0) {
            other = *(u8**)((u8*)self->m_selCharPlayerAreas[slot] + 0x1AC);
            if (*(s32*)(area + 0x1B4) != 0) {
                if (*(s32*)(hand + 0xA0) == 0 && *(s32*)(other + 0x10) == 0 && *(s32*)(hand + 0xA4) == 0) {
                    if (getScreenArea__13muSelCharHandFv(hand) == 1) {
                        setMode__13muSelCharCoinFiP13muSelCharHand(other, 1, hand);
                        catchCoin__13muSelCharHandFP13muSelCharCoin(hand, other);
                    } else {
                        takeCoin__13muSelCharHandFP13muSelCharCoin(hand, other);
                        setMode__13muSelCharCoinFiP13muSelCharHand(other, 2, hand);
                    }
                    playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0, -1, 0, 0, -1);
                }
                return;
            }
        }
        if (getScreenArea__13muSelCharHandFv(hand) == 1) {
            fn_10_8500(self, slot, &sp48);
            return;
        }
        fn_10_7A94(self, slot, &sp48);
        fn_10_883C(self, slot, &sp48);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_95B8 {


extern "C" void setPlayerKind__19muSelCharPlayerAreaFi(void*, int);
extern "C" void setNameNo__19muSelCharPlayerAreaFi(void*, int);
extern "C" void addDisableNameList__19muSelCharPlayerAreaFi(void*, int);
extern "C" void delDisableNameList__19muSelCharPlayerAreaFi(void*, int);

extern "C" void fn_10_95B8(void* self, int index, int value, int selected) {
    char* item = *(char**)((char*)self + 0x44 + index * 4);
    setPlayerKind__19muSelCharPlayerAreaFi(item, value);

    int state = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
    int valid;
    if (state >= 0 && state < 4)
        valid = 1;
    else
        valid = 0;

    if (valid) {
        selected = *(int*)(item + 0x1c8);
        if (selected >= 0) {
            if (value == 1) {
                if (selected >= 0) {
                    char* other;
                    char* cursor = (char*)self;
                    int i = 0;
                    int count = *(int*)((char*)self + 0x648);

                    for (; i < count; cursor += 4, i++) {
                        if (index != i) {
                            other = *(char**)(cursor + 0x44);
                            if (*(int*)(other + 0x1b4) == 1 &&
                                *(int*)(other + 0x1c8) >= 0 &&
                                selected == *(int*)(other + 0x1c8)) {
                                setNameNo__19muSelCharPlayerAreaFi(item, -1);
                                selected = -1;
                                break;
                            }
                        }
                    }

                    if (selected >= 0) {
                        char* cursor = (char*)self;
                        int i = 0;
                        while (i < *(int*)((char*)self + 0x648)) {
                            if (i != index) {
                                char* other = *(char**)(cursor + 0x44);
                                int active;
                                if (*(int*)(other + 0x200) != 0 &&
                                    *(int*)(other + 0x1fc) == 0)
                                    active = 1;
                                else
                                    active = 0;
                                if (active)
                                    addDisableNameList__19muSelCharPlayerAreaFi(other, selected);
                            }
                            cursor += 4;
                            i++;
                        }
                    }
                }
            } else if (selected >= 0) {
                int i = 0;
                char* cursor = (char*)self;
                while (i < *(int*)((char*)self + 0x648)) {
                    if (i != index) {
                        char* other = *(char**)(cursor + 0x44);
                        int active;
                        if (*(int*)(other + 0x200) != 0 &&
                            *(int*)(other + 0x1fc) == 0)
                            active = 1;
                        else
                            active = 0;
                        if (active)
                            delDisableNameList__19muSelCharPlayerAreaFi(other, selected);
                    }
                    cursor += 4;
                    i++;
                }
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_AA74 {

extern "C" {
int getTeamColor__19muWifiInterfaceTaskFi(void*, int);

static inline int mode(int s) {
    int r;
    if (s >= 0 && s < 4) r = 1; else r = 0;
    if (r != 0) return 0;
    if (s >= 4 && s < 0xc) r = 1; else r = 0;
    if (r != 0) return 1;
    int q;
    if (s >= 0xc && s < 0x10) q = 1; else q = 0;
    r = 3;
    if (q != 0) r = 2;
    return r;
}

int fn_10_AA74(void* self) {
    char* base = (char*)self;
    int state = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
    if (mode(state) == 0) {
        int n = *(int*)(base + 0x648);
        int c = 0;
        for (int i = 0; i < n; i++) {
            char* p = ((char**)base)[i + 0x11];
            if (*(int*)(p + 0x1b4) != 0) c++;
        }
        if (c < 2) return 0;
    } else {
        if (*(int*)(*(char**)(base + 0x44) + 0x1b4) == 0) return 0;
    }
    char* c1 = base;
    char* p1;
    int n = *(int*)(base + 0x648);
    for (int i = 0; i < n; i++, c1 += 4) {
        p1 = *(char**)(c1 + 0x44);
        if (*(int*)(p1 + 0x1dc) >= 0) {
            int v = *(int*)(*(char**)(p1 + 0x1a8) + 0xa4);
            if (v != 0 && v != 3) return 0;
        }
    }
    {
        char* c = base;
        char* p;
        for (int i = 0; i < n; i++, c += 4) {
            p = *(char**)(c + 0x44);
            if (*(int*)(p + 0x1ac) != 0 && *(int*)(p + 0x1b4) != 0 && *(int*)p != 2 && *(int*)(*(char**)(p + 0x1a8) + 0xa0) != 0) return 0;
        }
    }
    {
        char* p;
        char* c = base;
        for (int i = 0; i < n; i++, c += 4) {
            p = *(char**)(c + 0x44);
            if (*(int*)p == 0 && *(int*)(p + 0x1b4) != 0 && *(int*)(p + 0x1b8) == 0x28) return 0;
        }
    }
    if (*(unsigned char*)(base + 0x5c8) != 0) {
        int f;
        if (mode(state) == 0 || mode(state) == 1)
            f = 0;
        else
            f = 1;
        if (f == 0) {
            int cnt[3] = {0, 0, 0};
            char* cursor = base;
            for (int i = 0; i < *(int*)(base + 0x648); cursor += 4, i++) {
                char* p = *(char**)(cursor + 0x44);
                if (*(int*)p == 1) {
                    int t = getTeamColor__19muWifiInterfaceTaskFi(g_muWifiInterfaceTask, i);
                    if (t >= 0 && t < 3) cnt[t]++;
                } else if (*(int*)(p + 0x1b4) != 0) {
                    cnt[*(int*)(p + 0x1c0)]++;
                }
            }
            int k = 0;
            if (cnt[0] > 0) k = 1;
            if (cnt[1] > 0) k++;
            if (cnt[2] > 0) k++;
            if (k <= 1) return 0;
        }
    }
    return 1;
}
}
}

// TODO: clean up, give variables real names
namespace jj_C060 {

extern "C" {
extern char lbl_10_rodata_248[];
extern char lbl_10_rodata_2B8[];
void createObjResFile__13muSelCharTaskFPCviPQ34nw4r3g3d7ResFilePQ34nw4r3g3d7ResFileb(void* self, void* entries, int count, void* resourceFile, void* otherResource, int flag);
}

struct RodataEntry {
    unsigned char first;
    unsigned char begin;
    unsigned char end;
    unsigned char argument;
};

extern "C" void fn_10_C060(char* self, void* resourceFile) {
    int begin;
    int first;
    RodataEntry* entry;
    void* allocator;
    int count;
    int j;
    int i;

    createObjResFile__13muSelCharTaskFPCviPQ34nw4r3g3d7ResFilePQ34nw4r3g3d7ResFileb(self, lbl_10_rodata_248, 0xE, resourceFile, NULL, 0);
    allocator = gfHeapManager::getMEMAllocator((Heaps::HeapType)0x2A);
    entry = (RodataEntry*)lbl_10_rodata_2B8;
    for (i = 0; i < 3; i++, entry++) {
        count = entry->end - entry->begin;
        if (count <= 0) {
            count = 1;
        }
        first = entry->first;
        j = 0;
        begin = *((unsigned char*)entry + 1);
        for (; j < count; j++) {
            void* model = nw4r::g3d::ScnMdl::Construct(
                allocator, NULL, entry->argument,
                *(void**)(*(char**)(self + (first + j) * 4 + 0x13C) + 0x10));
            *(void**)(self + (begin + j) * 4 + 0x42C) = model;
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_C154 {
struct MuObject {
    char pad[0x14];
    void* animation;
};

struct Entry {
    const char* name;
    unsigned char first, last, argument, pad;
};

struct Menu {
    char pad[0x13C];
    MuObject* objects[1];
};

extern "C" {
extern char lbl_10_data_558[];
extern const float lbl_10_rodata_2F0;

void* create__8MuObjectFPQ34nw4r3g3d7ResFilePCciPQ34nw4r3g3d7ResFileQ25Heaps8HeapType(
    void*, const char*, int, void*, int);
void changeAnimN__8MuObjectFPCc(void*, const char*);
void setUpdateRate__16gfModelAnimationFf(void*, float);
char* strcpy(char*, const char*);
char* strcat(char*, const char*);
}

extern "C" void createObjResFile__13muSelCharTaskFPCviPQ34nw4r3g3d7ResFilePQ34nw4r3g3d7ResFileb(Menu* self, Entry* entries, int count,
                           void* resourceFile, void* otherResource, int flag) {
    int num;
    int i = 0;
    int j;
    Entry* entry = entries;
    MuObject* object;
    const char* name;
    char animationName[0x40];

    while (i < count) {
        num = 1;
        if (entry->first < entry->last)
            num = entry->last - entry->first;

        j = 0;
        while (j < num) {
            object = (MuObject*)create__8MuObjectFPQ34nw4r3g3d7ResFilePCciPQ34nw4r3g3d7ResFileQ25Heaps8HeapType(
                resourceFile, entry->name,
                flag ? entry->argument + j : entry->argument,
                otherResource, 0x2A);
            name = entry->name;
            self->objects[entry->first + j] = object;
            strcpy(animationName, name);
            strcat(animationName, lbl_10_data_558);
            changeAnimN__8MuObjectFPCc(object, animationName);
            if (object->animation)
                setUpdateRate__16gfModelAnimationFf(object->animation, lbl_10_rodata_2F0);
            ++j;
        }
        ++i;
        ++entry;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_D0D8 {

struct SelCharState {
    char pad000[0x5d4];
    s32 m_5d4;
    s32 m_5d8;
    s32 m_5dc;
};

extern "C" {
int isWifiGameMaster__13muSelCharTaskFv(void* self);
void sendWifiGameRule__13muSelCharTaskFbb(SelCharState* self, s32 a, u32 b);
void fn_10_CEEC(SelCharState* self, s32 v);
}

extern "C" void fn_10_D0D8(SelCharState* self) {
    s32 idx;

    if (self->m_5d4 == 1) {
        int state;
        int result;
        s32 v;
        v = self->m_5dc + 1;
        if (v > 0x63) {
            v = 1;
        }
        self->m_5dc = v;
        idx = v;

        state = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
        if (state >= 0 && state < 4) result = 1; else result = 0;
        if (result != 0) {
            result = 0;
        } else {
            if (state >= 4 && state < 0xc) result = 1; else result = 0;
            if (result != 0) {
                result = 1;
            } else {
                int r0_4;
                if (state >= 0xc && state < 0x10) r0_4 = 1; else r0_4 = 0;
                result = 3;
                if (r0_4 != 0) result = 2;
            }
        }
        if (result == 2) {
            if (isWifiGameMaster__13muSelCharTaskFv(self) != 0) {
                sendWifiGameRule__13muSelCharTaskFbb(self, 0, 1);
            }
        }
    } else {
        int state;
        int result;
        int limit;
        int st;
        s32 v;
        limit = 0x63;
        st = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
        if (st == 0x12) limit = 0x3c;
        v = self->m_5d8 + 1;
        if (v > limit) {
            switch (st) {
            case 1:
            case 0x12:
                v = 1;
                break;
            default:
                v = 0;
                break;
            }
        }
        self->m_5d8 = v;
        idx = v;

        state = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
        if (state >= 0 && state < 4) result = 1; else result = 0;
        if (result != 0) {
            result = 0;
        } else {
            if (state >= 4 && state < 0xc) result = 1; else result = 0;
            if (result != 0) {
                result = 1;
            } else {
                int r0_5;
                if (state >= 0xc && state < 0x10) r0_5 = 1; else r0_5 = 0;
                result = 3;
                if (r0_5 != 0) result = 2;
            }
        }
        if (result == 2) {
            if (isWifiGameMaster__13muSelCharTaskFv(self) != 0) {
                sendWifiGameRule__13muSelCharTaskFbb(self, 0, 1);
            }
        }
    }
    fn_10_CEEC(self, idx);
}
}

// TODO: clean up, give variables real names
namespace jj_D30C {

struct SelCharState {
    char pad[0x5d4];
    s32 m_5d4;
    s32 m_5d8;
    s32 m_5dc;
};

extern "C" {
int isWifiGameMaster__13muSelCharTaskFv(void*);
void sendWifiGameRule__13muSelCharTaskFbb(SelCharState*, s32, u32);
void fn_10_CEEC(SelCharState*, s32);
}

static inline void updateIndex(s32 index, SelCharState* self) {
    fn_10_CEEC(self, index);
}

extern "C" void fn_10_D30C(SelCharState* arg) {
        int state2;
    s32 idx;
    SelCharState* self = arg;

    if (self->m_5d4 == 1) {
        int state;
        int result;
        s32 v;

        v = self->m_5dc - 1;
        if (v < 1)
            v = 0x63;
        idx = v;
        self->m_5dc = idx;

        state = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
        if (state >= 0 && state < 4)
            result = 1;
        else
            result = 0;

        if (result != 0) {
            result = 0;
        } else {
            if (state >= 4 && state < 0xc)
                result = 1;
            else
                result = 0;

            if (result != 0) {
                result = 1;
            } else {
                int r0_4;
                if (state >= 0xc && state < 0x10)
                    r0_4 = 1;
                else
                    r0_4 = 0;
                result = 3;
                if (r0_4 != 0)
                    result = 2;
            }
        }

        if (result == 2) {
            if (isWifiGameMaster__13muSelCharTaskFv(self) != 0)
                sendWifiGameRule__13muSelCharTaskFbb(self, 0, 1);
        }
    } else {
        int flag;
        int state1 = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
        s32 v;

        switch (state1) {
        case 1:
        case 0x12:
            flag = 1;
            break;
        default:
            flag = 0;
            break;
        }

        v = self->m_5d8 - 1;
        if (v < flag) {
            v = 0x63;
            if (state1 == 0x12)
                v = 0x3c;
        }
        idx = v;
        self->m_5d8 = idx;

        state2 = ((unsigned char*)g_GameGlobal->m_selCharData)[4];
        int result;
        if (state2 >= 0 && state2 < 4)
            result = 1;
        else
            result = 0;

        if (result != 0) {
            result = 0;
        } else {
            if (state2 >= 4 && state2 < 0xc)
                result = 1;
            else
                result = 0;

            if (result != 0) {
                result = 1;
            } else {
                int r0_4;
                if (state2 >= 0xc && state2 < 0x10)
                    r0_4 = 1;
                else
                    r0_4 = 0;
                result = 3;
                if (r0_4 != 0)
                    result = 2;
            }
        }

        if (result == 2) {
            if (isWifiGameMaster__13muSelCharTaskFv(self) != 0)
                sendWifiGameRule__13muSelCharTaskFbb(self, 0, 1);
        }
    }

    updateIndex(idx, self);
}
}

// TODO: clean up, give variables real names
namespace jj_FEDC {


extern "C" {
unsigned int getCharHighScore__13muSelCharTaskFiiPiPi(void*, int, int, int, int);
unsigned int strcatHighScoreUnit__13muSelCharTaskFPcii(void*, char*, unsigned int, int);
void printData__5MuMsgFUlPCvi(void*, unsigned long, char*, unsigned long);

extern char lbl_10_data_1C8[];
extern float lbl_10_rodata_454;
extern float lbl_10_rodata_510;

void dispHighScore__13muSelCharTaskFiii(
    void* self, int index, unsigned int score, int type
) {
    const char* fmt = lbl_10_data_1C8;
    char buf[64];
    int len = 0;
    int i;
    int a = 0, b = 0, c = 0, d = 0;

    if (index == 3 && type == 1) {
        for (i = 0; i < 35; i++) {
            unsigned int v = getCharHighScore__13muSelCharTaskFiiPiPi(
                self, i, *(int*)((char*)self + 0x5e0), 0, 0
            );
            unsigned int h, m, s, f;

            if (v < 21600000U) {
                h = v / 216000;
                v %= 216000;
                m = v / 3600;
                v %= 3600;
                s = v / 60;
                v %= 60;
                f = v * 100 / 60;
            } else {
                h = 99;
                m = 59;
                s = 59;
                f = 99;
            }

            a += h;
            b += m;
            c += s;
            d += f;
        }

        c += d / 100;
        d %= 100;
        b += c / 60;
        c %= 60;
        a += b / 60;
        b %= 60;

        if (a >= 100) {
            a = 99;
            b = 59;
            c = 59;
            d = 99;
        }

        if (a > 0)
            len = sprintf(buf, fmt + 0x540, a);

        if (a > 0)
            len += sprintf(buf + len, fmt + 0x548, b);
        else
            len += sprintf(buf + len, fmt + 0x540, b);

        sprintf(buf + len, fmt + 0x550, c, d);
    } else {
        switch (type) {
        case 1: {
            int m, s, f, h;
            if (score < 21600000U) {
                unsigned int v = score;
                h = v / 216000;
                v %= 216000;
                m = v / 3600;
                v %= 3600;
                s = v / 60;
                v %= 60;
                f = v * 100 / 60;
            } else {
                h = 99;
                m = 59;
                s = 59;
                f = 99;
            }

            if (h > 0)
                len = sprintf(buf, fmt + 0x540, h);

            if (h > 0)
                len += sprintf(buf + len, fmt + 0x548, m);
            else
                len += sprintf(buf + len, fmt + 0x540, m);

            sprintf(buf + len, fmt + 0x550, s, f);
            break;
        }

        case 2: {
            float v = (float)score / lbl_10_rodata_454;
            if (v > lbl_10_rodata_510)
                v = lbl_10_rodata_510;
            sprintf(buf, fmt + 0x55c, v);
            break;
        }

        case 4:
            if (score > 999)
                score = 999;
            sprintf(buf, fmt + 0x564, score);
            break;

        default:
            if (score > 999999999U)
                score = 999999999U;
            sprintf(buf, fmt + 0x564, score);
            break;
        }
    }

    printData__5MuMsgFUlPCvi(
        *(void**)((char*)self + 0x5c4),
        index,
        buf,
        strcatHighScoreUnit__13muSelCharTaskFPcii(self, buf, 64, type)
    );
}
}
}

// TODO: clean up, give variables real names
namespace jj_105D8 {
extern "C" {
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void* __ct__22muSelcharPanelCtrlTaskFP8MuObjectPv(void*, void*, void*);
void* __ct__10muRuleTaskFPQ34nw4r3g3d8ScnGroupP9gfArchiveP15muPanelCtrlTaskii(void*, void*, void*, void*, unsigned int, int);
void changeAnimN__8MuObjectFiUlbb(void*, int, int, int, int);
void setRuleNumToGlobal__13muSelCharTaskFv(void*);
void setCtrlFlg__10muRuleTaskFUc(void*, unsigned char);
void* getHand__13muSelCharTaskFi(void*, int);
void* getCoin__13muSelCharTaskFi(void*, int);
void setBanVisible__13muSelCharHandFb(void*, int);
void setBanVisible__13muSelCharCoinFb(void*, int);
extern const unsigned char lbl_10_rodata_520[];
}

struct Resource {
    char pad[0x10];
    void* data;
};

struct Manager {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void call(void*, void*);

    char pad[0xe0];
    void* data;
};

struct gfTask_base {
    char pad[0x3c];
};

class gfTask : public gfTask_base {
public:
    virtual void slot0();
    virtual void processBegin();
    virtual void slot2();
    virtual void processUpdate();
    virtual void processPreMapCorrection();
    virtual void processMapCorrection();
    virtual void processFixPosition();
    virtual void processPreCollision();
    virtual void processCollision();
    virtual void processCatch();
    virtual void processHit();
    virtual void processCamera();
    virtual void processFixCamera();
    virtual void processEffect();
    virtual void processGameProc();
    virtual void processEnd();
    virtual void renderPre();
    virtual void renderOpa();
    virtual void renderXlu();
    virtual void processDebug();
    virtual void renderDebug();
    virtual void init();
};

struct Self {
    char pad0[0x6c];
    Manager* manager;
    char pad1[0x358];
    Resource* resource;
    char pad2[0x258];
    void* other;
    gfTask* first;
    gfTask* second;
    int state;
    unsigned char flag;
    char pad3[0x13];
    int count;
};

extern "C" void fn_10_105D8(Self* self, int index) {
    Manager* c;
    void* b;
    gfTask* a;
    Resource* res;
    gfTask* first;
    gfTask* second;
    int i;
    unsigned int value;

    res = self->resource;
    self->manager->call(self->manager->data, res->data);
    changeAnimN__8MuObjectFiUlbb(res, 0, 0x11, 1, 0);

    first = (gfTask*)__nw__FUlQ25Heaps8HeapType(0x54, 0x2a);
    if (first)
        first = (gfTask*)__ct__22muSelcharPanelCtrlTaskFP8MuObjectPv(first, res, 0);
    first->init();
    self->first = first;
    setRuleNumToGlobal__13muSelCharTaskFv(self);

    value = lbl_10_rodata_520[index];
    a = self->first;
    b = self->other;
    c = self->manager;

    second = (gfTask*)__nw__FUlQ25Heaps8HeapType(0x29c, 0x2a);
    if (second)
        second = (gfTask*)__ct__10muRuleTaskFPQ34nw4r3g3d8ScnGroupP9gfArchiveP15muPanelCtrlTaskii(second, c, b, a, value, 0x6a);
    second->init();
    self->second = second;
    self->state = 1;
    setCtrlFlg__10muRuleTaskFUc(second, (unsigned char)(-(self->flag != 0)));

    for (i = 0; i < self->count; i++) {
        setBanVisible__13muSelCharHandFb(getHand__13muSelCharTaskFi(self, i), 1);
        setBanVisible__13muSelCharCoinFb(getCoin__13muSelCharTaskFi(self, i), 1);
    }
}
}

