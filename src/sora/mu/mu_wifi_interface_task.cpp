#include <types.h>

#include <mu/wifi/mu_wifi_interface.h>
#include <nt/nt_net_report.h>
#include <nt/network.h>
#include <nt/nt_offline.h>
#include <nt/nt_report.h>
#include <string.h>
#include <nt/nt_match.h>
#include <ip/ip_switch.h>
#include <gm/gm_global.h>

extern u32 g_NtAIDBitmap;

extern "C" void fn_800CF22C(muWifiInterfaceTask* self);
extern "C" void fn_801473C4(muWifiInterfaceTask* self);
extern "C" void fn_80145D24(s32 arg0, void* arg1, s32 arg2, s32 arg3, s32 arg4);

struct muWifiInterfaceTaskFields {
    char pad0[0x44];
    s32 unk44;
    s8 unk48;
    char pad49[0x27];
    char unk70[0xF5A];
    u8 unkFCA;
    char padFCB[0x7D];
    s32 unk1048;
    s32 unk104C;
};

struct NetworkFields {
    char pad0[0x38];
    s32 unk38;
};

struct ntFriendInfoFields {
    char pad0[0x608];
    u8 unk608;
};

void muWifiInterfaceTask::connect(bool arg0) {
    // BYTES: declaration order picks the register assignment
    s32 offset;
    Network* network;
    s32 i;
    muWifiInterfaceTaskFields* fields = reinterpret_cast<muWifiInterfaceTaskFields*>(this);
    fields->unk48 = arg0;
    fields->unk44 = 1;
    fields->unk1048 = 0;
    fields->unk104C = 0;
    fn_801473C4(this);
    network = &g_Network;
    fields->unkFCA &= 0xFFFFFFEF;
    network->create(true);
    i = 0;
    offset = 0;
    do {
        // MATCH-ONLY: char* arithmetic keeps the loaded value as the left add operand
        ntFriendInfoFields* entry = reinterpret_cast<ntFriendInfoFields*>(reinterpret_cast<char*>(*reinterpret_cast<s32*>(ntFriendInfo::getInstance())) + offset);
        if (entry->unk608 != 0) {
            entry->unk608 = 2;
        }
        i += 1;
        offset += 0x89;
    } while (i < 0x40);
    fn_80145D24(reinterpret_cast<NetworkFields*>(network)->unk38, fields->unk70, 0x218, 0x138, 4);
    fn_800CF22C(this);
}

void netReport(const char* fmt, ...) {
}

// Function bodies below are taken from https://github.com/jj-link/brawl.

extern "C" s32 fn_800C9A4C(u32 arg0) {
    if ((arg0 >= 0x4E86U && arg0 <= 0x4E8DU) || (arg0 >= 0x4E8FU && arg0 <= 0x5207U)) {
        return 1;
    }
    if (arg0 == 0x4E85U || (arg0 >= 0x59D8U && arg0 <= 0x5DBFU)) {
        return 2;
    }
    if (arg0 == 0x4E8EU) {
        return 3;
    }
    if (arg0 == 0x7148U) {
        return 7;
    }
    if (arg0 == 0x7149U) {
        return 8;
    }
    if (arg0 == 0x4E84U || (arg0 >= 0xC350U && arg0 <= 0xEA5FU)) {
        return 4;
    }
    if ((arg0 - 0x10000U) == 0x3A2EU) {
        return 5;
    }
    return 6;
}

extern "C" void fn_80146574(Network* network);
extern "C" void fn_8014652C(Network* network);
extern "C" void fn_800CCF30(muWifiInterfaceTask* self, int arg1, int arg2, int arg3, int arg4);
extern "C" bool fn_8014592C(void* tr);
extern "C" u64 fn_80052250();

extern void* lbl_805A02A0;
extern void* lbl_805A03A8;
extern const muWifiErrorMsg lbl_80408488[];

void muWifiInterfaceTask::destroy() {
    if (g_muWifiInterfaceTask != NULL) {
        fn_80146574(&g_Network);
        g_muWifiInterfaceTask->unlink();
        delete g_muWifiInterfaceTask;
        g_muWifiInterfaceTask = NULL;
        lbl_805A02A0 = NULL;
    }
}

void muWifiInterfaceTask::exit() {
    exit();
}

muWifiInterfaceTask::~muWifiInterfaceTask() { }

u32 muWifiInterfaceTask::getErrorMsgID(int index) {
    return lbl_80408488[index].m_msgId;
}

bool muWifiInterfaceTask::haveErrorCode(int index) {
    return lbl_80408488[index].m_haveErrorCode;
}

u32 muWifiInterfaceTask::getBadNameErrorMsgID() {
    return 0xC;
}

u64 muWifiInterfaceTask::getUserID() {
    NetworkUserData* userData = g_Network.m_userData;
    if (userData != NULL) {
        return userData->m_userId;
    }
    return 0;
}

bool muWifiInterfaceTask::disconnect() {
    bool connected;
    if (g_muWifiInterfaceTask != NULL && g_muWifiInterfaceTask->unk40 == 0) {
        connected = true;
    } else {
        connected = false;
    }
    if (!connected) {
        return false;
    }
    netReport("muWifiInterfaceTask::disconnect\n");
    fn_8014652C(&g_Network);
    unk40 = 1;
    return true;
}

bool muWifiInterfaceTask::isMyPlayerArea(int index) {
    if (index < 0 || index >= 4) {
        return false;
    }
    u8 aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    muWifiPlayerArea* area = &m_playerAreas[index];
    return area->m_aid == aid && !area->unk5_b7;
}

u8 muWifiInterfaceTask::getTeamColor(int index) {
    int localPlayer = m_playerAreas[index].m_localPlayer;
    u8 aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    return m_aidInfos[aid].m_localPlayers[localPlayer].m_teamColor;
}

u8 muWifiInterfaceTask::getAssignedTeamColor(int index) {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        return 0;
    }
    return info->m_assignedPlayers[index].m_teamColor;
}

u8 muWifiInterfaceTask::getAssignedHomerunRule() {
    return m_assignedHomerunRule;
}

u8 muWifiInterfaceTask::getAssignedKumiteType() {
    return m_assignedKumiteType;
}

u8* muWifiInterfaceTask::getAssignedMeleeRule() {
    return m_assignedMeleeRule;
}

u8 muWifiInterfaceTask::getAssignedDragoon() {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        return 0;
    }
    return info->unk78_b2;
}

void muWifiInterfaceTask::setAppNetworkError(int error, int detail) {
    if (!unkFCB_b7) {
        m_appNetworkError[0] = error;
        m_appNetworkError[1] = detail;
    }
}

void muWifiInterfaceTask::connectMatchingDaiRantou(int arg) {
    netReport("大乱闘のクライアントとしてマッチングを開始しました。\n");
    fn_800CCF30(this, 4, 0, 3, arg);
}

void muWifiInterfaceTask::startMatchingBattle100(int arg) {
    netReport("百人組み手のサーバとしてマッチングを開始しました。\n");
    fn_800CCF30(this, arg, 1, 2, -1);
}

void muWifiInterfaceTask::connectMatchingBattle100(int arg) {
    netReport("百人組み手のクライアントとしてマッチングを開始しました。\n");
    fn_800CCF30(this, 4, 1, 3, arg);
}

void muWifiInterfaceTask::startMatchingHomerun(int arg) {
    netReport("ホームランコンテストのサーバとしてマッチングを開始しました。\n");
    fn_800CCF30(this, arg, 2, 2, -1);
}

void muWifiInterfaceTask::connectMatchingHomerun(int arg) {
    netReport("ホームランコンテストのクライアントとしてマッチングを開始しました。\n");
    fn_800CCF30(this, 4, 2, 3, arg);
}

void muWifiInterfaceTask::startMatchingBossBattle(int arg) {
    netReport("ボスバトルのサーバとしてマッチングを開始しました。\n");
    fn_800CCF30(this, arg, 3, 2, -1);
}

void muWifiInterfaceTask::connectMatchingBossBattle(int arg) {
    netReport("ボスバトルのクライアントとしてマッチングを開始しました。\n");
    fn_800CCF30(this, 4, 3, 3, arg);
}

void muWifiInterfaceTask::clearAppError() {
    m_appNetworkError[0] = 0;
    m_appNetworkError[1] = 0;
    fn_801473C4(this);
}

bool muWifiInterfaceTask::isEndMatchingEnd() {
    return gUnk805a03d8 == 3;
}

void muWifiInterfaceTask::resetLimitTime(int seconds) {
    m_limitTime = seconds * 60;
    netReport("制限時間を%d秒に再設定しました\n", seconds);
}

bool muWifiInterfaceTask::testSendAIDInfoDone() {
    return fn_8014592C(lbl_805A03A8);
}

void muWifiInterfaceTask::clearPrepareLoadStage() {
    for (int i = 0; i < 4; i++) {
        m_aidInfos[i].unk56_b6 = false;
    }
    netReport("ステージ準備完了の通知をクリアしました。\n");
}

int muWifiInterfaceTask::getFriendAIDIndexAID(int aid) {
    return m_aidInfos[aid].m_friendAIDIndex;
}

void muWifiInterfaceTask::clearShortComment(int aid) {
    memset(m_aidInfos[aid].m_shortComment, 0, sizeof(m_aidInfos[aid].m_shortComment));
}

s32 muWifiInterfaceTask::getMeleeRestTime(u8* flag) {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL || unkFCC_b24 < 3) {
        return -1;
    }
    if (flag != NULL) {
        *flag = info->unk56_b2;
    }
    return info->unk8;
}

u8 muWifiInterfaceTask::getRuleType() {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL || unkFCC_b24 < 3) {
        return 0xFF;
    }
    return info->unk3;
}

u8 muWifiInterfaceTask::getPlayerArea(int index, u8* localPlayer) {
    *localPlayer = m_playerAreas[index].m_localPlayer;
    return m_playerAreas[index].m_aid;
}

int muWifiInterfaceTask::getSelCharMeleeKind() {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        return 0;
    }
    return info->unk7D_b2;
}

bool muWifiInterfaceTask::getPlayerLive(int index) {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        return true;
    }
    return m_playerLive[index] != 0xFF;
}

void muWifiInterfaceTask::initPlayerLive() {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    for (int i = 0; i < 4; i++) {
        m_playerLive[i] = info->m_assignedPlayers[i].m_aid;
    }
}

void muWifiInterfaceTask::clearPlayerLive(u8 aid, u16 time) {
    for (int i = 0; i < 4; i++) {
        if (aid == m_playerLive[i]) {
            m_playerLive[i] = 0xFF;
            m_playerLiveTime[i] = time;
        }
    }
}

u64 muWifiInterfaceTask::getComposedCharaAcceptFlag() {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        netReport("getComposedCharaAcceptFlag: 親がいません\n");
        return fn_80052250();
    }
    return info->unk30;
}

muWifiAIDInfo* muWifiInterfaceTask::getSafeParentAIDInfo() {
    u8 parentAID = Network::getParentAID();
    if (parentAID == 0xFF) {
        u8 aid = Network::getMyAID();
        if (aid == 0xFF) {
            aid = 4;
        }
        return &m_aidInfos[aid];
    }
    muWifiAIDInfo* info = &m_aidInfos[parentAID];
    if (!info->unk78_b7) {
        u8 aid = Network::getMyAID();
        if (aid == 0xFF) {
            aid = 4;
        }
        return &m_aidInfos[aid];
    }
    return info;
}

u8 muWifiInterfaceTask::getMyFriendStatusIcon() {
    return ntFriendInfo::getInstance()->unk0->unk450[0xDA];
}

u8 muWifiInterfaceTask::getFriendStatusIcon(int index) {
    return ntFriendInfo::getInstance()->unk0->unk5B3[index].unk8[0];
}

u8 muWifiInterfaceTask::getFriendStatusWiFiStatus(int index) {
    return ntFriendInfo::getInstance()->unk0->unk5B3[index].unk8[0x4D];
}

u8 muWifiInterfaceTask::getFriendStatusRoundTripTime(int index) {
    return ntFriendInfo::getInstance()->unk0->unk350[index].unk2[1];
}

u64 muWifiInterfaceTask::getMyRFLID() {
    u64 id;
    ntFriendInfo* info = ntFriendInfo::getInstance();
    memcpy(&id, &info->unk0->unk450[0x129], 8);
    return id;
}

void muWifiInterfaceTask::setMyRFLID(u64 id) {
    memcpy(&ntFriendInfo::getInstance()->unk0->unk450[0x129], &id, 8);
    ntFriendInfo::getInstance()->setDirtyFlag(true);
}

u8 muWifiInterfaceTask::getFriendStatusEntryPlayers(int index) {
    u8 players = ntFriendInfo::getInstance()->unk0->unk5B3[index].unk8[0x4E];
    if (players == 0) {
        players++;
    }
    return players;
}

int muWifiInterfaceTask::getNetcond() {
    int condition = gUnk805a03d8;
    if (condition < 3) {
        return 0;
    }
    if (condition == 3) {
        return 1;
    }
    if (condition == 4) {
        return 2;
    }
    if (condition == 7) {
        return 3;
    }
    netReport("Unknown Network::Netcond %d\n", condition);
    return 0;
}

void muWifiInterfaceTask::setFriendFlag(int index, bool flag) {
    u8 aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    muWifiAIDInfo* info = &m_aidInfos[aid];
    info->unk59_b4 = (info->unk59_b4 & ~(1 << index)) | (flag << index);
}


int muWifiInterfaceTask::getNumNode() {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        return 1;
    }
    int count = 0;
    for (int i = 0; i < 4; i++) {
        if ((int)info->unk68[i] == 1) {
            count++;
        }
    }
    return count;
}

wchar_t* muWifiInterfaceTask::getMyFriendStatusName(wchar_t* name) {
    u8* destination;
    const u8* source = &ntFriendInfo::getInstance()->unk0->unk450[0xA0];
    destination = (u8*)name;
    int count = 5;
    memset(name, 0, 12);
    while (--count >= 0) {
        int character = source[0];
        destination[0] = character;
        character |= source[1];
        destination[1] = source[1];
        source += 2;
        destination += 2;
        if (character == 0) {
            break;
        }
    }
    return name;
}

wchar_t* muWifiInterfaceTask::getMyFriendStatusComment(wchar_t* name) {
    u8* destination;
    const u8* source = &ntFriendInfo::getInstance()->unk0->unk450[0xAA];
    destination = (u8*)name;
    int count = 20;
    memset(name, 0, 42);
    while (--count >= 0) {
        int character = source[0];
        destination[0] = character;
        character |= source[1];
        destination[1] = source[1];
        source += 2;
        destination += 2;
        if (character == 0) {
            break;
        }
    }
    return name;
}

wchar_t* muWifiInterfaceTask::getMyShortComment(int index, wchar_t* name) {
    u8* destination;
    const u8* source = &ntFriendInfo::getInstance()->unk0->unk450[index * 0x28];
    destination = (u8*)name;
    int count = 20;
    memset(name, 0, 42);
    while (--count >= 0) {
        int character = source[0];
        destination[0] = character;
        character |= source[1];
        destination[1] = source[1];
        source += 2;
        destination += 2;
        if (character == 0) {
            break;
        }
    }
    return name;
}

extern const char lbl_80457A10[0x38];

bool muWifiInterfaceTask::isPlayerAssignRecieved() {
    if (!((unkFC9 >> 4) & 1)) {
        return false;
    }
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        netReport(lbl_80457A10);
        NtMatching::setErrorStatus(12);
        return false;
    }
    if ((info->unk78_b3 >> 2) & 1) {
        u8 ownAid = Network::getMyAID();
        if (ownAid == 0xFF) { ownAid = 4; }
        m_aidInfos[ownAid].unk3 = info->unk3;
    }
    return (info->unk78_b3 >> 2) & 1;
}

bool muWifiInterfaceTask::isEqualServerGameMode() {
    u8 index = unkFD8;
    if (index >= 0x40) { return false; }
    u8 status = getFriendStatusWiFiStatus(index);
    switch (status) {
    case 19:
    case 23:
    case 25:
        return unk6CData == 0;
    case 20:
    case 26:
        return unk6CData == 1;
    case 21:
    case 27:
        return unk6CData == 2;
    default:
        netReport("isEqualServerGameMode:gameMode:%d  wifiStatus:%d\n", unk6CData, status);
        return false;
    }
}

bool muWifiInterfaceTask::needKeepAlive() {
    if (ipSwitch::getInstance()->m_source == 1) {
        return false;
    }
    if (g_GameGlobal->m_selCharData->unk6 != 0) {
        muWifiAIDInfo* info;
        u8 aid = Network::getParentAID();
        if (aid == 0xFF) {
            info = NULL;
        } else {
            info = &m_aidInfos[aid];
        }
        if (info == NULL) {
            return false;
        }
        if (info->unk55 == 4) {
            return false;
        }
    }
    return true;
}

extern const char lbl_80457AB0[0x64];
extern const char lbl_80457A48[0x2C];
extern const char lbl_80457A74[0x3C];

u8 muWifiInterfaceTask::getAssignedStageKind(u8* substage, u32* music) {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        return 0xFF;
    }
    if (!((info->unk78_b3 >> 2) & 1)) {
        return 0xFF;
    }
    *substage = info->unk10DData;
    *music = info->unk110;
    netReport(lbl_80457AB0, info->unk10C, *substage, *music);
    return info->unk10C;
}

void muWifiInterfaceTask::clearGlobalAIDStatusLive() {
    muWifiAIDInfo* info;
    Network::getMyAID();
    for (u8 aid = 0; aid < 5; aid++) {
        info = &m_aidInfos[aid];
        for (int player = 0; player < 4; player++) {
            info->unk68[player] = 0;
        }
    }
}

u8 muWifiInterfaceTask::getAssignedPlayerKind(int index) {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    u8* kind;
    if (info == NULL) {
        netReport(lbl_80457A48);
        kind = NULL;
    } else if (!((info->unk78_b3 >> 2) & 1)) {
        netReport(lbl_80457A74);
        kind = NULL;
    } else {
        kind = &info->unkE4[index].unk0;
    }
    if (kind == NULL) {
        return 3;
    }
    return *kind;
}

u8 muWifiInterfaceTask::getGlobalLiveAIDBitmap() {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        return 0;
    }
    u8 bitmap = 0;
    for (u32 index = 0; index < 4; index++) {
        if ((int)getSafeParentAIDInfo()->unk68[(u8)index] == 1) {
            bitmap |= 1 << (u8)index;
        }
    }
    return bitmap;
}

u8 muWifiInterfaceTask::getGlobalConnectAIDBitmap() {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        return 0;
    }
    u8 bitmap = 0;
    for (u32 index = 0; index < 4; index++) {
        if (getSafeParentAIDInfo()->unk68[(u8)index] != 0) {
            bitmap |= 1 << (u8)index;
        }
    }
    return bitmap;
}

u8 muWifiInterfaceTask::getRuleMasterAID() {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        aid = Network::getMyAID();
        if (aid == 0xFF) {
            aid = 4;
        }
        info = &m_aidInfos[aid];
    }
    u8 master = info->unk79Data;
    bool live;
    if (master >= 4) {
        live = false;
    } else {
        live = getSafeParentAIDInfo()->unk68[master] == 1;
    }
    if (!live) {
        return 0xFF;
    }
    return info->unk79Data;
}

// HYPOTHESIS: these one-argument calls use the same exported names as sq_lib.cpp.
extern "C" int exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(int kind);
extern "C" int getNumCharColor__6muMenuFiii(int kind);
extern const char lbl_804599B8[0x20];
extern const char lbl_804599D8[0x38];

u8 muWifiInterfaceTask::getFreeCharColor(int character, int count) {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        netReport(lbl_804599B8);
        return 0;
    }
    __typeof__(info->unkE4[0])* players = info->unkE4;
    int selection;
    for (u32 color = 0; (int)(u8)color <
         ((selection = exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(character)) == 0x28 ?
          1 : getNumCharColor__6muMenuFiii(selection)); color++) {
        int player;
        for (player = 0; player < count; player++) {
            if (character == players[player].unk3 && (u8)color == players[player].unk4) {
                break;
            }
        }
        if (player == count) {
            return color;
        }
    }
    netReport(lbl_804599D8, character);
    return 0;
}

// muWifiInterfaceTask::exchangeDWCErrorCode (the map name); src/sora/mu/mu_wifi_interface_task.cpp calls it by this stand-in name.
extern "C" s32 fn_800C9A4C(u32 arg0);

extern s32 lbl_805A03E8;
extern s32 lbl_805A03F0;

int muWifiInterfaceTask::_getNetworkError(int* code) {
    *code = 0;
    if (m_appNetworkError[0] != 0) {
        return m_appNetworkError[0];
    }
    if (&g_Network == NULL) {
        return 0;
    }
    int error = lbl_805A03E8;
    if (error == 0) {
        return 0;
    }
    if (error == 1) {
        *code = lbl_805A03F0;
        return fn_800C9A4C(*code);
    }
    if (!unkFCB_b7) {
        NtBase::setMatchingStatus(10);
    }
    if (error == 7) {
        *code = lbl_805A03F0;
        return fn_800C9A4C(*code);
    }
    switch (error) {
    case 2:
    case 3: return 13;
    case 8: return 9;
    case 4: return 10;
    case 5: return 10;
    case 6: return 13;
    case 9: return 11;
    case 10: return 12;
    case 11: return 11;
    case 12: return 11;
    case 13: return 11;
    case 14: return 13;
    default: return 0;
    }
}

// muWifiInterfaceTask::startMatchingCommon (the map name); src/sora/mu/mu_wifi_interface_task.cpp calls it by this stand-in name.
extern "C" void fn_800CCF30(muWifiInterfaceTask* self, int arg1, int arg2, int arg3, int arg4);

extern const char lbl_80457DC8[0x30];

void muWifiInterfaceTask::startMatchingDaiRantou(int arg) {
    netReport(lbl_80457DC8);
    __typeof__(unk1000[0])* rule = &unk1000[1];
    memset(rule, 0, sizeof(*rule));
    rule->unk1 = 0;
    rule->unk2 = 2;
    rule->unk3 = 2;
    rule->unk4 = 0;
    rule->unk5 = 10;
    rule->unk6 = 1;
    rule->unk7 = 0;
    rule->unk9 = 0;
    rule->unk8 = 0;
    rule->unkC = 0;
    rule->unkA = 0;
    unk1000[0] = unk1000[1];
    fn_800CCF30(this, arg, 0, 2, -1);
}

int muWifiInterfaceTask::getFriendAIDIndex(int index) {
    u8 aid = m_playerAreas[index].m_aid;
    bool live;
    if (aid >= 4) {
        live = false;
    } else {
        live = getSafeParentAIDInfo()->unk68[aid] == 1;
    }
    if (!live) {
        return -1;
    }
    return m_aidInfos[aid].m_friendAIDIndex;
}

wchar_t* muWifiInterfaceTask::getShortComment(unsigned int aid, int index, wchar_t* comment) {
    if (!(g_NtAIDBitmap & (1 << aid))) {
        *comment = 0;
        return comment;
    }
    muWifiAIDInfo* info = &m_aidInfos[aid];
    u8 ownAid = Network::getMyAID();
    if (ownAid == 0xFF) {
        ownAid = 4;
    }
    const void* comments;
    if (aid == ownAid) {
        comments = ntFriendInfo::getInstance()->unk0->unk450;
    } else {
        comments = info->m_shortComment;
    }
    u8* destination = (u8*)comment;
    const u8* source = (const u8*)comments + index * 0x28;
    int count = 20;
    memset(comment, 0, 42);
    while (--count >= 0) {
        int character = source[0];
        destination[0] = character;
        character |= source[1];
        destination[1] = source[1];
        source += 2;
        destination += 2;
        if (character == 0) {
            break;
        }
    }
    return comment;
}

u8 muWifiInterfaceTask::getAssignedPlayerContNum(int index) {
    muWifiAIDInfo* info = getSafeParentAIDInfo();
    int count;
    for (count = 0; count < 4; count++) {
        if (info->unkE4[count].unk0 != 0) {
            break;
        }
    }
    if (index >= count) {
        return 0;
    }
    return index + 1;
}
