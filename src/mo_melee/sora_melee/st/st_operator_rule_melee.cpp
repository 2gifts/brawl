#include <st/operator/st_operator_rule_melee.h>
#include <ai/ai_mgr.h>
#include <ft/ft_manager.h>
#include <gf/gf_application.h>
#include <gm/gm_global.h>
#include <math.h>
#include <gm/gm_global_mode_melee.h>
#include <mu/wifi/mu_wifi_interface.h>
#include <types.h>

// TODO: the rule processing, result and corps members.

bool stOperatorRuleMelee::isEnd() {
    return m_state == 5;
}

void stOperatorRuleMelee::updateExtraInfo() { }

bool stOperatorRuleMelee::checkExtraRule() {
    return false;
}

void stOperatorRuleMelee::decisionExtraRule() { }

void stOperatorRuleMelee::calcScore() {
    setNormalResultInfo();
}

// The corps rule (HYPOTHESIS: corps kind 4) is set up from the global corps data.
void stOperatorRuleMelee::notifyEventSetupCorps() {
    if (m_condition->m_meleeInitData.m_corpsKind == 4) {
        initCorpsRule(g_GameGlobal->m_corps);
    }
}

static inline int getStock(gmPlayerResultInfo* info) {
    s8 stock = info->m_startStockCount;
    if (stock < 0) {
        return 1;
    }
    return stock;
}

// The stocks left of the human players with a controller (a negative count counts as one).
int stOperatorRuleMelee::getLocalHumanTotalStock() {
    int total = 0;
    for (int i = 0; i < m_numPlayers; i++) {
        gmPlayerResultInfo* info = &m_resultInfo->m_playersResultInfo[i];
        if (info->m_state == 0 && info->m_hasController) {
            total += getStock(info);
        }
    }
    return total;
}

int stOperatorRuleMelee::getPlayerStock(int playerNo) {
    return getStock(&m_resultInfo->m_playersResultInfo[m_condition->m_playersInitData[playerNo].m_playerNo]);
}

// Per team: the members, and the stocks left of the members still in (and the count of teams with stocks left).
void stOperatorRuleMelee::getTeamTotalStock(int* numTeams, int* teamStocks, int* teamMembers) {
    *numTeams = 0;
    teamStocks[0] = 0;
    teamMembers[0] = 0;
    teamStocks[1] = 0;
    teamMembers[1] = 0;
    teamStocks[2] = 0;
    teamMembers[2] = 0;
    for (int i = 0; i < m_numPlayers; i++) {
        gmPlayerResultInfo* info = &m_resultInfo->m_playersResultInfo[i];
        if (info->m_state != 3) {
            u8 team = info->m_teamNo;
            teamMembers[team]++;
            if (info->m_startStockCount != 0) {
                if (teamStocks[team] == 0) {
                    (*numTeams)++;
                }
                int stock = info->m_startStockCount;
                teamStocks[team] += stock < 1 ? 1 : stock;
            }
        }
    }
}

void stOperatorRuleMelee::getExistTeam(int* numTeams) {
    *numTeams = 0;
    u32 teams = 0;
    for (int i = 0; i < m_numPlayers; i++) {
        gmPlayerResultInfo* info = &m_resultInfo->m_playersResultInfo[i];
        if (info->m_state != 3) {
            u32 bit = 1 << info->m_teamNo;
            if (!(teams & bit)) {
                teams |= bit;
                (*numTeams)++;
            }
        }
    }
}

// A tie in the time-type modes goes to sudden death (not in corps matches).
bool stOperatorRuleMelee::isSuddenDeath() {
    switch (m_condition->m_meleeInitData.m_gameMode) {
    case 0:
    case 1:
    case 0xe:
    case 0x17:
    case 0x18:
    case 0x1c:
        if (m_condition->m_meleeInitData.m_corpsKind == 0) {
            if (m_resultInfo->m_isTeams == false) {
                return m_resultInfo->m_numWinners > 1;
            }
            return m_resultInfo->m_unk10 > 1;
        }
        break;
    }
    return false;
}

// HYPOTHESIS: the high score record of the solo modes 2 and 4: the time in seconds, the damage given (by both players when
// the second one is in) and the time in frames.
void stOperatorRuleMelee::calcHighScore() {
    u32 mode = m_condition->m_meleeInitData.m_gameMode;
    if (mode == 4 || mode == 2) {
        gmResultInfo* info = m_resultInfo;
        u32 frames = m_remainingFrameTime;
        u32 damage = info->m_playersResultInfo[0].m_giveDamage;
        if (info->m_playersResultInfo[1].m_state == 0) {
            damage += info->m_playersResultInfo[1].m_giveDamage;
        }
        info->m_playersResultInfo[0].m_unk240 = (int)frames / 60;
        info->m_playersResultInfo[0].m_unk244 = damage;
        *reinterpret_cast<u32*>(info->m_playersResultInfo[0]._248) = 0;
        *reinterpret_cast<u32*>(info->m_playersResultInfo[0]._248 + 4) = m_remainingFrameTime;
    }
}

// HYPOTHESIS: +0x224 is the network flag of the match; byte +0xF of each player's result goes to the Wi-Fi task's bytes at +0xFDF.
void stOperatorRuleMelee::networkWriteRanking() {
    if (*(reinterpret_cast<u8*>(this) + 0x224) != 0) {
        u8* rank = reinterpret_cast<u8*>(g_muWifiInterfaceTask) + 0xFDF;
        for (int i = 0; i < m_numPlayers; i++) {
            gmPlayerInitData* player = &m_condition->m_playersInitData[i];
            if (player->m_state != 3) {
                int playerNo = player->m_playerNo;
                if (playerNo >= 0 && playerNo < 4) {
                    rank[i] = m_resultInfo->m_playersResultInfo[playerNo].m_unkF;
                }
            }
        }
    }
}

// HYPOTHESIS: the pause/stall bits at the top of the application word at +0xEC.
struct stRuleMeleeAppFlagsView {
    u32 m_stop : 5;
    u32 unk : 27;
};

static inline stRuleMeleeAppFlagsView* getAppFlags() {
    return reinterpret_cast<stRuleMeleeAppFlagsView*>(reinterpret_cast<u8*>(g_gfApplication) + 0xEC);
}

// HYPOTHESIS: the init data flag byte at +0x1D (0x04: removed) and the CPU type and rank after it.
struct stRuleMeleePlayerFlagsView {
    char _0[0x1D];
    u8 unk80 : 5;
    u8 m_isRemoved : 1;
    u8 unk02 : 2;
    u8 m_cpuType;
    u8 m_cpuRank;
};

// HYPOTHESIS: flag 0x08 of the mode byte at +0xF.
struct stRuleMeleeModeFlagsView {
    char _0[0xF];
    u8 unkF0 : 4;
    u8 m_flag08 : 1;
    u8 unkF7 : 3;
};

static inline u8* byteAt(void* object, int offset) {
    return reinterpret_cast<u8*>(object) + offset;
}

// HYPOTHESIS: +0xF4 is the time step (60 over the application frame rate at +0xF8, a halfword), +0xD5 is set unless flag
// 0x08 of the mode byte +0xF is.
void stOperatorRuleMelee::startOperator() {
    stOperatorRule::startOperator();
    m_isStart = true;
    *reinterpret_cast<float*>(byteAt(this, 0xF4)) = 60.0f / *reinterpret_cast<u16*>(byteAt(g_gfApplication, 0xF8));
    *reinterpret_cast<bool*>(byteAt(this, 0xD5)) = reinterpret_cast<stRuleMeleeModeFlagsView*>(m_condition)->m_flag08 == 0;
}

// The frame counters advance by whole frames of the accumulated time step (paused games and HYPOTHESIS: the network
// stall flags at +0x75 and the application's +0xEC word stop them); they saturate at six hours.
void stOperatorRuleMelee::updateFrameCounter() {
    if (*byteAt(this, 0x75) != 0 && (getAppFlags()->m_stop & 4)) {
        return;
    }
    if (!m_isStart) {
        return;
    }
    if (!(*byteAt(m_condition, 0xC) & 1) && getAppFlags()->m_stop != 0) {
        return;
    }
    float* time = reinterpret_cast<float*>(byteAt(this, 0xEC));
    *time += *reinterpret_cast<float*>(byteAt(this, 0xF4));
    float whole = floor(*time);
    *time -= whole;
    int frames = whole;
    if (frames != 0) {
        if (m_remainingFrameTime > frames) {
            m_remainingFrameTime -= frames;
        } else {
            m_remainingFrameTime = 0;
        }
        m_framesElapsed += frames;
        m_framesElapsed = m_framesElapsed < 21599999 ? m_framesElapsed : 21599999;
        if (*byteAt(this, 0xD7) != 0) {
            m_frameCounter += frames;
            m_frameCounter = m_frameCounter < 21599999 ? m_frameCounter : 21599999;
        }
    }
}

// Removes the players still in that are not in the mask: as a CPU (mode 1) or leaving (state 3).
// HYPOTHESIS: flag 0x04 of +0x1D of the init data marks a removed player; +0x1E/+0x1F are its CPU type and rank.
void stOperatorRuleMelee::deletePlayer(u32 mode, u32 keepMask) {
    for (int i = 0; i < m_numPlayers; i++) {
        gmPlayerInitData* player = &m_condition->m_playersInitData[i];
        if (player->m_state == 0) {
            stRuleMeleePlayerFlagsView* flags = reinterpret_cast<stRuleMeleePlayerFlagsView*>(player);
            if (!flags->m_isRemoved && !(keepMask & (1 << i))) {
                flags->m_isRemoved = true;
                if (mode == 1) {
                    int entryId = g_ftManager->getEntryId(i);
                    if (entryId != -1) {
                        g_ftManager->setFighterOperationType(entryId, 1);
                        g_ftManager->setFighterOperationCpuType(entryId, flags->m_cpuType);
                        g_aiMgr->setCpuRank(i, flags->m_cpuRank);
                    }
                } else {
                    player->m_state = 3;
                }
            }
        }
    }
}

// HYPOTHESIS: +0x74 stops the bookkeeping (a replay or a network spectator).
void stOperatorRuleMelee::notifyEventKnockout(u32 playerIndex) {
    if (*byteAt(this, 0x74) == 0 && (int)playerIndex < m_numPlayers) {
        m_0x124[playerIndex] = m_framesElapsed;
        m_playersLastDeadSleepFrame[playerIndex] = 0;
    }
}

// Logs the KO in both players' results (the last 100 of each), then the common bookkeeping.
void stOperatorRuleMelee::notifyEventBeat(u32 playerIndex1, u32 playerIndex2) {
    if (*byteAt(this, 0x74) == 0) {
        gmPlayerResultInfo* winner = &m_resultInfo->m_playersResultInfo[m_condition->m_playersInitData[playerIndex1].m_playerNo];
        gmPlayerResultInfo* loser = &m_resultInfo->m_playersResultInfo[m_condition->m_playersInitData[playerIndex2].m_playerNo];
        int koIndex = m_0x15C[playerIndex1]++ % 100;
        winner->m_koTargets[koIndex] = loser->m_characterKind;
        winner->m_koCauses[koIndex] = loser->_3[0];
        int deadIndex = m_0x178[playerIndex2]++ % 100;
        loser->m_deadTargets[deadIndex] = winner->m_characterKind;
        loser->m_deadCauses[deadIndex] = winner->_3[0];
        stOperatorRule::notifyEventBeat(playerIndex1, playerIndex2);
    }
}

#include <ip/ip_switch.h>
#include <revolution/OS/OSError.h>
#include <snd/snd_system.h>
#include <if/if_mngr.h>
#include <st/stage.h>
// TODO: clean up, give variables real names
namespace jj_248C90 {
struct Self {
    char pad[0x3c]; char* f3c; u8 f40; char pad4[3]; void* f44;
    char pad_48[0x84]; int fcc; u8 bytes_d0[16]; int fe0, fe4, fe8; float fec;
    char pad_ec[8]; int ff8, ffc, f100, f104; int arr[6][7]; int f1b0, f1b4;
    char pad_b[0x24]; int f1dc; char pad_e0; u8 f1e1; char pad_e2[2];
    int f1e4, f1e8; char pad_f[0x38]; u8 f224, f225, f226; char pad_227;
    int f228, f22c, f230; u8 f234, f235; char pad_236[10]; int f240, f244;
    u8 f248; char pad_249[3]; int f24c, f250; char pad_254[4]; u8 f258;
};
struct GameGlobal { int pad[6]; int f18; };
extern "C" void __ct__14stOperatorRuleFv(void*);
extern "C" void addCallBack__13ipKeyRecorderFPQ213ipKeyRecorder8CallBack(void*, void*);
extern "C" char lbl_27_data_4F290[];
extern "C" u8 lbl_27_bss_59A0[];
extern "C" int lbl_27_bss_59A8;
extern "C" int lbl_27_data_4F050;
extern "C" const float lbl_27_rodata_4028;
extern "C" GameGlobal* g_GameGlobal;
extern "C" Self* fn_27_248C90(Self* self, int arg1) {
    int one = 1;
    int four = 4;
    __ct__14stOperatorRuleFv(self);
    float fl = lbl_27_rodata_4028;
    self->f3c = lbl_27_data_4F290;
    self->bytes_d0[0] = 0;
    self->bytes_d0[1] = 0;
    self->bytes_d0[2] = 0;
    self->bytes_d0[3] = 0;
    self->bytes_d0[4] = 0;
    self->bytes_d0[5] = 0;
    self->bytes_d0[6] = 0;
    self->bytes_d0[7] = one;
    self->bytes_d0[8] = 0;
    self->bytes_d0[9] = 0;
    self->bytes_d0[10] = 0;
    self->bytes_d0[11] = 0;
    self->bytes_d0[12] = 0;
    self->bytes_d0[13] = 0;
    self->bytes_d0[14] = 0;
    self->bytes_d0[15] = 0;
    self->fe0 = 0;
    self->fe4 = 0;
    self->fe8 = 0;
    self->fec = fl;
    self->ff8 = 0;
    self->ffc = 0;
    self->f100 = one;
    self->f104 = four;
    self->f1b4 = one;
    self->f1dc = 0;
    self->f1e1 = 0;
    self->f1e4 = 0;
    self->f1e8 = 0;
    self->f224 = (u8)arg1;
    self->f225 = 0;
    self->f226 = 0;
    self->f228 = 12;
    self->f22c = 0;
    self->f230 = 0;
    self->f234 = 0;
    self->f235 = one;
    self->f240 = lbl_27_data_4F050;
    self->f244 = 0;
    self->f248 = one;
    self->f24c = lbl_27_data_4F050;
    self->f250 = 0;
    self->f258 = 0;
    lbl_27_bss_59A8 = 0;
    if (ipSwitch::getInstance() != 0) {
        if (ipSwitch::getInstance()->m_keyRecorder != 0) {
            lbl_27_bss_59A0[4] = 0;
            lbl_27_bss_59A0[5] = 0;
            *(u16*)(lbl_27_bss_59A0 + 6) = 0;
            addCallBack__13ipKeyRecorderFPQ213ipKeyRecorder8CallBack(ipSwitch::getInstance()->m_keyRecorder, lbl_27_bss_59A0);
        }
    }
    self->f40 = 0;
    int v22b8 = 0x22b8;
    int constA = 0xff439eb2;
    for (int i = 0; i < 7; i++) {
        self->arr[0][i] = 0;
        self->arr[1][i] = 0;
        self->arr[2][i] = v22b8;
        self->arr[3][i] = 0;
        self->arr[4][i] = 0;
        self->arr[5][i] = constA;
    }
    self->f1b0 = constA;
    self->fcc = g_GameGlobal->f18;
    unsigned int key = ((u8)((u8*)self->f44)[8] >> 2) & 0x3F;
    switch (key) {
    case 3: break;
    case 5: self->bytes_d0[7] = 0;
    case 4: self->bytes_d0[3] = one; self->bytes_d0[6] = one; break;
    case 7:
        if (((u8*)self->f44)[0x14] == 4) { self->f100 = 3; self->f1b4 = 4; }
        self->bytes_d0[13] = one; break;
    case 14: self->f226 = one; break;
    case 24: self->f226 = one; self->f225 = one; break;
    case 28: self->f226 = one; self->f225 = one; break;
    case 8: case 9: case 10: self->bytes_d0[11] = one; self->bytes_d0[13] = one; break;
    case 16: case 18: self->f226 = one; self->bytes_d0[11] = one; self->bytes_d0[13] = one; break;
    default: break;
    }
    u8 c = ((u8*)self->f44)[0x14];
    if (c >= 5 && c <= 0xa) { self->f104 = 7; self->f1e1 = one; }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_248FA8 {
typedef unsigned char u8;
typedef int s32;
typedef unsigned short u16;

struct IpSwitchObj {
    u8 pad0[0x3C];
    void* unk3C;
};

extern "C" void* getInstance__8ipSwitchFv(void);
extern "C" void removeCallBack__13ipKeyRecorderFPQ213ipKeyRecorder8CallBack(s32, void*);
extern "C" void __dt__14stOperatorRuleFv(void*, int);
extern "C" void __dl__FPv(void*);
extern "C" u8 lbl_27_data_4F290[];
extern "C" u8 lbl_27_bss_59A0[];
extern "C" s32 lbl_27_bss_59A8;

extern "C" IpSwitchObj* fn_27_248FA8(IpSwitchObj* self, int deleting) {
    if (self != 0) {
        self->unk3C = lbl_27_data_4F290;
        if (getInstance__8ipSwitchFv() != 0) {
            void* inst = getInstance__8ipSwitchFv();
            if (*(s32*)((u8*)inst + 0x18) != 0) {
                lbl_27_bss_59A0[4] = 0;
                lbl_27_bss_59A0[5] = 0;
                *(u16*)(lbl_27_bss_59A0 + 6) = 0;
                removeCallBack__13ipKeyRecorderFPQ213ipKeyRecorder8CallBack(*(s32*)((u8*)getInstance__8ipSwitchFv() + 0x18), lbl_27_bss_59A0);
            }
        }
        lbl_27_bss_59A8 = 0;
        __dt__14stOperatorRuleFv(self, 0);
        if (deleting > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_2498DC {
typedef unsigned char u8;
typedef unsigned int u32;

struct Data {
    u8 pad0[8];
    u8 kind;
    u8 mode;
    u8 padA;
    unsigned char unused : 2;
    unsigned char flag : 1;
    unsigned char unused2 : 5;
    u8 padC[0x14];
    int value;
};

struct Obj {
    u8 pad0[0x2C];
    unsigned char flagsHigh : 6;
    unsigned char flagBit : 1;
    unsigned char flagsLow : 1;
    u8 pad2D[0x13];
    u8 active;
    u8 pad41[3];
    Data* data;
    u8 pad48[0x88];
    bool d0;
    bool d1;
    bool d2;
    bool d3;
    u8 padD4[2];
    bool d6;
    u8 padD7[5];
    bool dc;
    u8 padDD[3];
    int e0;
    u8 padE4[4];
    int e8;
    u8 padEC[4];
    int f0;
    u8 padF4[0x138];
    int state;
};

extern "C" void notifyEventStartStage__14stOperatorRuleFv(Obj*);
extern "C" void fn_27_24AC3C(Obj*);
extern "C" Obj* lbl_27_bss_59A8;

inline bool special(Data* p) {
    return (((u32)p->mode >> 5) & 7) == 2 &&
           (((u32)p->kind >> 2) & 63) == 7;
}

extern "C" void fn_27_2498DC(Obj* p) {
    notifyEventStartStage__14stOperatorRuleFv(p);
    fn_27_24AC3C(p);
    lbl_27_bss_59A8 = p;

    if (p->state == 1 && p->data->value == 0)
        p->d3 = true;

    p->d2 = p->data->flag;
    p->d1 = (p->d2 || (((u32)p->data->mode >> 5) & 7) == 1) ||
            special(p->data);
    p->dc = (((u32)p->data->mode >> 5) & 7) == 1 ||
            special(p->data);

    int value = p->data->value;
    if (value) {
        if (p->d3) {
            if (p->d6)
                p->e8 = value;
        } else {
            p->e0 = value;
            p->d0 = true;
            p->f0 = 302;
        }
    }

    p->flagBit = 0;
    p->active = true;
}
}

// TODO: clean up, give variables real names
namespace jj_249B14 {

class Class_CF8 {
public:
    virtual void __dt__27soModelModuleScnObjCallbackFv();
    virtual void ExecCallback_CALC_WORLD__Q34nw4r3g3d15IScnObjCallbackFQ44nw4r3g3d6ScnObj6TimingPQ34nw4r3g3d6ScnObjUlPv(int, int);
    virtual void ExecCallback_CALC_MAT__Q34nw4r3g3d15IScnObjCallbackFQ44nw4r3g3d6ScnObj6TimingPQ34nw4r3g3d6ScnObjUlPv();
    virtual void ExecCallback_CALC_VIEW__27soModelModuleScnObjCallbackFQ44nw4r3g3d6ScnObj6TimingPQ34nw4r3g3d6ScnObjUlPv();
    virtual void ExecCallback_DRAW_OPA__Q34nw4r3g3d15IScnObjCallbackFQ44nw4r3g3d6ScnObj6TimingPQ34nw4r3g3d6ScnObjUlPv(int);
};
struct PlayerRecord {
    unsigned char kind;
    char pad1[0x1b];
    unsigned char flags : 1;
    char pad2[0x3f];
};
struct PlayerData {
    char pad[0x98];
    PlayerRecord player[1];
};
struct EntryRecord {
    char pad[0x12];
    unsigned char active;
    char tail[1];
};
struct EntryData {
    char pad[4];
    unsigned char mode;
    char pad2[0xf];
    EntryRecord entries[1];
};
struct Work {
    char pad0[0x44];
    PlayerData* data;
    char pad48[0xc];
    Class_CF8* callback;
    char pad58[0x1c];
    unsigned char disabled;
    char pad75[0x5d];
    unsigned char flagD2;
    char padD3;
    unsigned char flagD4;
    char padD5[7];
    unsigned char flagDC;
    char padDD[7];
    int defaultSleep;
    char padE8[0x1c];
    int count;
    int sleep[7];
    int altSleep[7];
    int reasons[7];
    char pad15c[0x5c];
    int threshold;
    char pad1bc[8];
    int total;
    char pad1c8[4];
    int remaining;
    char pad1d0[0xc];
    EntryData* entryData;
    char pad1e0[0xc];
    int dead[7];
    int entries[7];
};
extern "C" int getScoreEntryId__9ftManagerCFi(ftManager*, int);
extern "C" void checkDispCorps__14stOperatorRuleFv(Work*);
extern "C" char lbl_27_data_4F0BC[];

extern "C" int notifyEventDead__19stOperatorRuleMeleeFiUli(Work* self, int entry, int player, int reason) {
    if (self->disabled)
        return 0;
    if (player < self->count) {
        ftManager* manager = g_ftManager;
        int no = manager->getPlayerNo(getScoreEntryId__9ftManagerCFi(manager, entry));
        int* sleep;
        if (!(self->data->player[no].flags)) {
            self->sleep[no] = self->defaultSleep;
            sleep = &self->sleep[no];
        } else {
            int value = self->altSleep[no];
            value = value ? value : self->defaultSleep;
            self->sleep[no] = value;
            sleep = &self->sleep[no];
        }
        OSReport(lbl_27_data_4F0BC, no, *sleep, sleep);
        if (self->data->player[player].kind < 0x2e)
            self->reasons[player] = reason;
        else
            self->reasons[player] = 0x2d;
    }
    if (self->callback)
        self->callback->ExecCallback_CALC_WORLD__Q34nw4r3g3d15IScnObjCallbackFQ44nw4r3g3d6ScnObj6TimingPQ34nw4r3g3d6ScnObjUlPv(entry, player);
    bool alive;
    if (self->flagD2)
        alive = false;
    else if (self->flagDC) {
        if (!g_ftManager->isValidEntryId(entry))
            alive = false;
        else
            alive = g_ftManager->getOwner(entry)->getStockCount() != 0;
    } else
        alive = true;
    if (!alive && self->flagD4 && self->threshold <= player) {
        if (self->total < 0x05f5e0ff)
            self->total++;
        self->remaining--;
        self->dead[player] = 1;
        if (self->callback && self->entries[player] >= 0)
            self->callback->ExecCallback_DRAW_OPA__Q34nw4r3g3d15IScnObjCallbackFQ44nw4r3g3d6ScnObj6TimingPQ34nw4r3g3d6ScnObjUlPv(self->entries[player]);
        if (self->entryData->mode == 1) {
            if (self->entries[player] >= 0) {
                self->entryData->entries[self->entries[player]].active = 0;
                self->entries[player] = -1;
            }
            checkDispCorps__14stOperatorRuleFv(self);
        }
    }
    return alive;
}
}

// TODO: clean up, give variables real names
namespace jj_249E78 {
extern "C" void fn_27_249E78(void *unused, s32 n, u8 *buf) {
    s32 j;
    s32 i;
    s32 k;
    u8 tmp[100];

    if (n >= 100) {
        k = n % 100;
        for (i = k, j = 0; i < 100; ++j, ++i) {
            tmp[j] = buf[i];
        }
        for (i = 0; i < k; ++i, ++j) {
            tmp[j] = buf[i];
        }
        for (i = 0; i < 100; ++i) {
            buf[i] = tmp[i];
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_24A314 {
extern "C" {
double floor(double);

float fn_27_24A314(double x) {
    return (float)floor(x);
}
}
}

// TODO: clean up, give variables real names
namespace jj_24A338 {


struct Entry {
    char pad0[0x25];
    unsigned char state;
    char pad26[4];
    unsigned char team;
    char pad2b[3];
    signed char value;
    char pad2f[0x27d];
};

struct Rules {
    char pad0[8];
    unsigned char mode;
    char pad9[0xa];
    unsigned char teams;
    unsigned char special;
};

struct Context {
    char pad0[0x44];
    Rules* rules;
    char pad48[0x84];
    Entry* entries;
    char padD0[1];
    unsigned char active;
    char padD2[6];
    unsigned char d8;
    unsigned char d9;
    unsigned char da;
    char padDB[0x25];
    int result;
    int count;
    char pad108[0x11d];
    unsigned char b225;
    unsigned char b226;
};

inline int getValue(Entry* e) {
    int n = 1;
    if (e->value >= 0)
        n = e->value;
    return n;
}

inline int getTeamBit(Entry* e) {
    return 1 << e->team;
}

inline int countActive(Context* self) {
    int total;
    int offset;
    total = 0;
    offset = 0;
    for (int i = 0; i < self->count; ++i, offset += 0x2ac) {
        Entry* e = (Entry*)((char*)self->entries + offset);
        if (e->state != 3 && e->value != 0)
            ++total;
    }
    return total;
}

extern "C" void getTeamTotalStock__19stOperatorRuleMeleeFPiPiPi(Context*, int*, int*, int*);

extern "C" int fn_27_24A338(Context* self) {
    if (!self->active)
        return 0;

    Rules* rules = self->rules;
    if (!rules->special) {
        unsigned int mode = (rules->mode >> 2) & 0x3f;
        if (mode == 7) {
            int count = self->count;
            int total = 0;
            int zeros = 0;

            for (int i = 0; i < count; ++i) {
                Entry* e = &self->entries[i];
                if (e->state == 0) {
                    total += getValue(e);
                    if (e->value == 0)
                        ++zeros;
                }
            }

            if (total == 0) {
                self->result = 3;
                return 1;
            }

            if (!rules->teams) {
                if (!self->d8) {
                    int total2 = 0;
                    for (int i = 0; i < count; ++i) {
                        Entry* e = &self->entries[i];
                        if (e->state == 1)
                            total2 += getValue(e);
                    }
                    if (total2 <= 0) {
                        self->result = 4;
                        return 1;
                    }
                }
            } else {
                if (self->da && zeros) {
                    self->result = 3;
                    return 1;
                }

                int a, b[3], c[3];
                getTeamTotalStock__19stOperatorRuleMeleeFPiPiPi(self, &a, b, c);
                if (b[0] == 0 || (self->d9 && b[0] < c[0])) {
                    self->result = 3;
                    return 1;
                }
                if (!self->d8 && b[1] <= 0 && b[2] <= 0) {
                    self->result = 4;
                    return 1;
                }
            }
            return 0;
        }

        if (mode == 2) {
            int total = 0;
            for (int i = 0; i < self->count; ++i) {
                Entry* e = &self->entries[i];
                if (e->state == 0)
                    total += getValue(e);
            }
            if (total == 0)
                return 1;
        }

        if (self->b226 && !self->b225) {
            if (!rules->teams) {
                if (g_muWifiInterfaceTask &&
                    (int)*((unsigned char*)g_muWifiInterfaceTask + 0xfd5) <= 1)
                    return 0;
            } else {
                int i;
                int mask;
                int offset;
                int total;
                total = 0;
                mask = 0;
                offset = 0;
                for (i = 0; i < self->count; ++i, offset += 0x2ac) {
                    Entry* e = (Entry*)((char*)self->entries + offset);
                    if (e->state != 3) {
                        int bit = getTeamBit(e);
                        if (!(mask & bit)) {
                            mask |= bit;
                            ++total;
                        }
                    }
                }
                if (total <= 1)
                    return 0;
            }
        }

        if (!rules->teams)
            return countActive(self) <= 1;

        int a, b[3], c[3];
        getTeamTotalStock__19stOperatorRuleMeleeFPiPiPi(self, &a, b, c);
        return a <= 1;
    }

    int players = 0;
    int total = 0;
    for (int i = 0; i < self->count; ++i) {
        Entry* e = &self->entries[i];
        if (e->state == 0) {
            ++players;
            total += getValue(e);
        }
    }

    if ((unsigned int)((rules->mode >> 2) & 0x3f) == 7) {
        if (total == 0) {
            self->result = 3;
            return 1;
        }
    } else {
        return total < players;
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_24AC3C {
#define FLD(T, p, off) (*(T*)((u8*)(p) + (off)))

struct FlagsIn { u8 a : 2; u8 bit : 1; u8 rest : 5; };
struct FlagsOut { u8 bit : 1; u8 rest : 7; };

extern "C" void* memset(void*, int, unsigned long);
extern "C" s32 randi__Fl(s32);

extern "C" void fn_27_24AC3C(u8* arg0) {
    s32 off;
    s32 r27;
    s32 m;
    s32 r26;
    s32 r;
    s32 q;
    s32 k;
    u8 b;
    u16 h;

    FLD(u8, FLD(void*, arg0, 0xCC), 0x0) = (u8)((FLD(u8, FLD(void*, arg0, 0x44), 0x8) >> 2) & 0x3F);
    FLD(u8, FLD(void*, arg0, 0xCC), 0x1) = (u8)((FLD(u8, FLD(void*, arg0, 0x44), 0x9) >> 5) & 7);
    FLD(u8, FLD(void*, arg0, 0xCC), 0x2) = FLD(u8, FLD(void*, arg0, 0x44), 0x13);
    FLD(u8, FLD(void*, arg0, 0xCC), 0x3) = FLD(u8, FLD(void*, arg0, 0x44), 0x17);
    FLD(s32, FLD(void*, arg0, 0xCC), 0x4) = (s32)(FLD(u32, FLD(void*, arg0, 0x44), 0x20) / 3600);
    FLD(s32, FLD(void*, arg0, 0xCC), 0x1374) = (s32)((FLD(u8, FLD(void*, arg0, 0x44), 0xE) >> 5) & 7);
    ((FlagsOut*)((u8*)FLD(void*, arg0, 0xCC) + 0xE))->bit = ((FlagsIn*)((u8*)FLD(void*, arg0, 0x44) + 0x62))->bit;
    h = FLD(u16, FLD(void*, arg0, 0x44), 0x1A);
    FLD(u16, FLD(void*, arg0, 0xCC), 0xC) = h;
    FLD(u8, FLD(void*, arg0, 0xCC), 0x1378) = 9;
    r27 = FLD(s32, FLD(void*, arg0, 0xCC), 0x25C);
    r26 = FLD(s32, FLD(void*, arg0, 0xCC), 0x260);
    memset((u8*)FLD(void*, arg0, 0xCC) + 0x24, 0, 0x12B4);
    memset((u8*)FLD(void*, arg0, 0xCC) + 0x12D8, 0, 0x90);
    FLD(s32, FLD(void*, arg0, 0xCC), 0x25C) = r27;
    FLD(s32, FLD(void*, arg0, 0xCC), 0x260) = r26;

    s32 i = 0;
    off = 0;
    for (; i < FLD(s32, arg0, 0x104); i++) {
        m = FLD(u8, (u8*)FLD(void*, arg0, 0x44) + off, 0x9B) * 0x2AC;
        r = randi__Fl(0xFFFFFF);
        FLD(s32, (u8*)FLD(void*, arg0, 0xCC) + m, 0x2A0) = r;
        FLD(s32, (u8*)FLD(void*, arg0, 0xCC) + m, 0x2A4) = 7 - i;
        off += 0x5C;
    }

    k = 0;
    off = 0;
    do {
        FLD(s32, (u8*)FLD(void*, arg0, 0xCC) + off, 0x12FC) = randi__Fl(0xFFFFFF);
        FLD(s32, (u8*)FLD(void*, arg0, 0xCC) + off, 0x1300) = 7 - k++;
        off += 0x30;
    } while (k < 3);
}
}

// TODO: clean up, give variables real names
namespace jj_24BF80 {
extern "C" double ceil(double x);

extern "C" float fn_27_24BF80(double x) {
    return ceil(x);
}
}

// TODO: clean up, give variables real names
namespace jj_24BFA4 {
typedef unsigned char u8;
struct E { u8 a[0x25]; u8 k; u8 b[0xc]; u8 v0; u8 v1; u8 c[0x2ac-0x34]; };
struct H { u8 a[0x1304]; u8 v0; u8 v1; u8 f; u8 pad[0x30-3]; };
extern "C" void fn_27_24BFA4(H* p) {
    if (((u8*)p)[0xf] > 1) {
        E* e = (E*)p;
        for (int i = 0; i < 7; i++, e++) {
            if (e->a[0x25] != 3 && e->v0 == 0) e->v0 = e->v1;
        }
    }
    if (((u8*)p)[0x10] > 1) {
        for (int i = 0; i < 3; i++) {
            if (p[0].f != 0 && p[0].v0 == 0) p[0].v0 = p[0].v1;
            p = (H*)((u8*)p + 0x30);
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_24C498 {

struct Stage_base { char pad[0x3c]; };
class Stage : public Stage_base {
public:
    virtual void processDefault();
    virtual void processBegin();
    virtual void processAnim();
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
    virtual ~Stage();
    virtual void createObj();
    virtual void createObjPokeTrainer();
    virtual void getPokeTrainerPointNum();
    virtual void getPokeTrainerPointData();
    virtual void getPokeTrainerPositionZ();
    virtual void getPokeTrainerDrawLayer();
    virtual void isAdventureStage();
    virtual void getItemPac();
    virtual void getItemGenPac();
    virtual void getItemPacEnemyFigure();
    virtual void getEnemyPac();
    virtual void getAdvRadarData();
    virtual void initializeStage();
    virtual void closeStage();
    virtual void renderDebugPositions();
    virtual void loading();
    virtual void process();
    virtual void updateStagePositions();
    virtual void debugCollision();
    virtual void getAIRange();
    virtual void getDefaultLightSetIndex();
    virtual void getZoneLightSetIndex();
    virtual void getScrollDir();
    virtual void clearCameraParam();
    virtual void initCameraParam();
    virtual void startLoadLocalData();
    virtual void isLoadLocalData();
    virtual void entryLocalData();
    virtual void removeLocalData();
    virtual void appearanceFighterLocal();
    virtual void getFighterStartPos();
    virtual void getFighterReStartPos();
    virtual void isReStartSamePoint();
    virtual void getPokeTrainerStartPos();
    virtual void getItemPosCount();
    virtual void getItemPos();
    virtual void getRandItemPos();
    virtual void getKirifudaPos();
    virtual void getKirifudaAngle();
    virtual void getKirifudaScale();
    virtual void getKirifudaModelType();
    virtual void getPokeTrainerPosCount();
    virtual void getPokeTrainerPos();
    virtual void getFighterDeadEffectSizeRate();
    virtual void getEnemyDeadEffectSizeRate();
    virtual void getEnableZ();
    virtual void getBgmID();
    virtual void getBgmID_2();
    virtual void getNowStepBgmID();
    virtual void getBgmOptionID();
    virtual void isBgmChange();
    virtual void getBgmChangeID();
    virtual void setBgmChange();
    virtual void getBgmVolume();
    virtual void getBgmPlayOffsetFrame();
    virtual void isNextStepBgmEqualNowStepBgmFromFlag();
    virtual void setFrameRuleTime();
    virtual void getFrameRuleTime();
    virtual void notifyTimmingGameStart();
    virtual void isEnd();
    virtual void isEventEnd();
    virtual void enableDevil();
    virtual void disableDevil();
    virtual void isDevil();
    virtual void setDevilScrool();
    virtual void getLucarioFinalTechniquePosition();
    virtual void startAppear();
    virtual void setAppearKind();
    virtual void endAppear();
    virtual void getAppearTask();
    virtual void forceStopAppear();
    virtual void getFinalTechniqColor();
    virtual void setMotionRatio();
    virtual void saveMotionRatio();
    virtual void restoreMotionRatio();
    virtual void setMotionSubRatio();
    virtual void saveMotionSubRatio();
    virtual void restoreMotionSubRatio();
    virtual void isCameraLocked();
    virtual void isBossBattleMode();
    virtual void isSimpleBossBattleMode();
    virtual void isAppear();
    virtual void isStartAppearTimming();
    virtual void getMadeinAiData();
    virtual void isBamperVector();
    virtual void getBamperVector();
    virtual void notifyEventInfoReady();
    virtual void notifyEventInfoGo();
    virtual void getDestroyBossParamCommon();
    virtual void stAdventureEventGetItem();
    virtual void setStageOutEffectInit();
    virtual void setStageInEffectInit();
    virtual void helperStarWarp();
    virtual void initializeFighterAttackRatio();
    virtual void startFighterEvent();
    virtual void getZoneState();
    virtual void getZonePos();
    virtual void getMagmaHeight();
    virtual void getAcidHeight();
    virtual void getIteamDropStatus();
    virtual void createWind2ndOnly();
    virtual void getWind2ndOnlyData();
    virtual void updateWind2ndOnly();
    virtual void setVision(unsigned char);
};

extern Stage* g_Stage;
extern "C" void setCenterState__8IfCenterFi(void*, int);
extern "C" void stopGroup__9sndSystemFii(sndSystem*, void*, int);

struct Object {
    char pad[0xac];
    void* sound;
    char pad2[0x2d];
    unsigned char flag;
};

extern "C" void fn_27_24C498(Object* self, int kind, int alternate) {
    sndSystem* system;
    int sound = 0;
    int voice = 0;
    switch (kind) {
    case 0:
        if (g_IfMngr) setCenterState__8IfCenterFi(g_IfMngr->m_ifCenter, 4);
        g_Stage->setVision(9);
        sound = 0x1fdc;
        break;
    case 1:
        if (g_IfMngr) setCenterState__8IfCenterFi(g_IfMngr->m_ifCenter, 7);
        g_Stage->setVision(12);
        sound = 0x1fd9;
        voice = 30;
        break;
    case 2:
        if (g_IfMngr) setCenterState__8IfCenterFi(g_IfMngr->m_ifCenter, 5);
        g_Stage->setVision(10);
        sound = 0x1fd7;
        voice = 30;
        break;
    case 3:
        g_Stage->setVision(11);
        if (g_IfMngr) setCenterState__8IfCenterFi(g_IfMngr->m_ifCenter, 8);
        sound = 0x1fd8;
        voice = 32;
        break;
    case 4:
        g_Stage->setVision(19);
        if (g_IfMngr) setCenterState__8IfCenterFi(g_IfMngr->m_ifCenter, 9);
        sound = 0x1fd6;
        voice = 30;
        break;
    case 5:
        break;
    }
    if (sound != 0) {
        stopGroup__9sndSystemFii(g_sndSystem, self->sound, 0);
        system = g_sndSystem;
        int id = 0x1fd2;
        if (!alternate) id = sound;
        system->playSE((SndID)id, -1, 0, 0, -1);
    }
    if (self->flag && voice != 0) {
        system = g_sndSystem;
        int id = 0x1a;
        if (!alternate) id = voice;
        system->playSE((SndID)id, -1, 0, 0, -1);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_24C6FC {
#define W(p, o) (*(s32*)((u8*)(p) + (o)))
#define B(p, o) (*(u8*)((u8*)(p) + (o)))
#define P(p, o) (*(void**)((u8*)(p) + (o)))

struct SubBits {
    u8 pad[0xf];
    u8 a : 5;
    u8 flag : 1;
    u8 b : 2;
};
struct DataBits {
    u8 pad[5];
    u8 top : 1;
    u8 rest : 7;
};

extern "C" {
void fn_27_24CA7C(void* self, int flag);

void initCorpsRule__19stOperatorRuleMeleeFP13gmGlobalCorps(void* self, u8* data) {
    s32 v = 1;
    s32 t;

    P(self, 0x1dc) = data;
    B(self, 0xd4) = 1;

    switch (B(data, 4)) {
    case 0: {
        if ((s32)B(data, 0) >= 1) v = B(data, 0);
        W(self, 0x1b8) = v;
        W(self, 0x1c4) = 0;
        W(self, 0x1c8) = B(P(self, 0x1dc), 3);
        W(self, 0x1cc) = 0;
        W(self, 0x1d0) = ((s32)B(P(self, 0x1dc), 2) >= 1) ? (s32)B(P(self, 0x1dc), 2) : 1;
        W(self, 0x1d4) = 0;
        W(self, 0x1d8) = B(P(self, 0x1dc), 1);
        B(self, 0x1e0) = 1;
        break;
    }
    case 1: {
        if ((s32)B(data, 0) >= 1) v = B(data, 0);
        W(self, 0x1b8) = v;
        W(self, 0x1c4) = 0;
        W(self, 0x1c8) = B(P(self, 0x1dc), 3);
        W(self, 0x1cc) = 0;
        W(self, 0x1d0) = ((s32)B(P(self, 0x1dc), 2) >= 1) ? (s32)B(P(self, 0x1dc), 2) : 1;
        u8* d = (u8*)P(self, 0x1dc);
        t = B(d, 0x1c);
        W(self, 0x1d4) = t;
        W(self, 0x1d8) = t + B(d, 3);
        *(bool*)((u8*)self + 0x1e0) = !((SubBits*)P(self, 0x44))->flag;
        break;
    }
    case 2: {
        if ((s32)B(data, 0) >= 1) v = B(data, 0);
        W(self, 0x1b8) = v;
        W(self, 0x1c4) = 0;
        W(self, 0x1c8) = B(P(self, 0x1dc), 3);
        W(self, 0x1cc) = 0;
        W(self, 0x1d0) = ((s32)B(P(self, 0x1dc), 2) >= 1) ? (s32)B(P(self, 0x1dc), 2) : 1;
        W(self, 0x1d4) = 0;
        W(self, 0x1d8) = B(P(self, 0x1dc), 1);
        B(self, 0x1e0) = 1;
        break;
    }
    }

    {
        s32 sum = W(self, 0x1b8) + W(self, 0x1d0);
        s32 r = 7;
        if (sum <= 7) r = sum;
        W(self, 0x1bc) = r;
        if (B(P(self, 0x1dc), 4) == 2) {
            W(self, 0x1c0) = W(self, 0x1b8) + 1;
        } else {
            s32 q = 4;
            if (r <= 4) q = r;
            W(self, 0x1c0) = q;
        }
    }

    W(self, 0x1ec) = 0x270f;
    W(self, 0x208) = -1;
    W(self, 0x1f0) = 0x270f;
    W(self, 0x20c) = -1;
    W(self, 0x1f4) = 0x270f;
    W(self, 0x210) = -1;
    W(self, 0x1f8) = 0x270f;
    W(self, 0x214) = -1;
    W(self, 0x1fc) = 0x270f;
    W(self, 0x218) = -1;
    W(self, 0x200) = 0x270f;
    W(self, 0x21c) = -1;
    W(self, 0x204) = 0x270f;
    W(self, 0x220) = -1;

    if (((DataBits*)P(self, 0x1dc))->top) fn_27_24CA7C(self, 1);
}
}
}

// TODO: clean up, give variables real names
namespace jj_24C91C {
struct Data { unsigned char a; unsigned char b; };
struct Base { char pad[0x3c]; };
#define V(n) virtual void n();
struct Obj : Base { V(a0)V(a1)V(a2)V(a3)V(a4)V(a5)V(a6)V(a7)V(a8)V(a9)
V(b0)V(b1)V(b2)V(b3)V(b4)V(b5)V(b6)V(b7)V(b8)V(b9)
V(c0)V(c1)V(c2)V(c3)V(c4)V(c5)V(c6)V(c7)V(c8)V(c9)
V(d0)V(d1)V(d2)V(d3)V(d4)V(d5)V(d6) virtual Data* get(); };
struct App { char pad[0xec]; unsigned flags; };
struct Mgr { Obj* o[7]; };
struct Self {
    char pad0[0xd4]; unsigned char en;
    char pad1[0x140-0xd5]; int v140[7];
    char pad2[0x1b8-0x15c]; int start;
    char pad3[0x1c4-0x1bc]; int lim; int cur;
    char pad4[0x1e1-0x1cc]; unsigned char flag;
    char pad5[0x1ec-0x1e2]; int cnt[7];
};
extern App* g_gfApplication;
extern Mgr* g_stLoaderManager;
extern "C" void fn_27_23E34C(Obj*);
extern "C" void fn_27_24CA7C(Self*, int);

extern "C" int fn_27_24C91C(Self* s) {
    if (s->en == 0 || (g_gfApplication->flags >> 27) != 0) return 0;
    if (s->cur != 0 && s->cur <= s->lim) return 1;
    fn_27_24CA7C(s, 0);
    for (int i = s->start; i < 7; i++) {
        int c = s->cnt[i];
        if (c != 0 && c < 0x1869F) {
            s->cnt[i] = c + 1;
            if (s->flag != 0 && s->cnt[i] == s->v140[i] + 10) {
                Obj* o = g_stLoaderManager->o[i];
                if (o != 0) {
                    Data* d = o->get();
                    if (d->a < 0x2e || d->a > 0x31) {
                        if (d->b != 3) {
                            d->b = 3;
                            s->v140[i] += s->cnt[i] + 0x3c;
                        }
                    } else fn_27_23E34C(o);
                }
            }
        }
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_24CA7C {
struct MainObj_base { char pad[0x3c]; };

class MainObj : public MainObj_base {
public:
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
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v29();
    virtual void v30();
    virtual void v31();
    virtual void v32();
    virtual void v33();
    virtual void v34();
    virtual void v35();
    virtual void v36();
    virtual void v37();
    virtual void v38();
    virtual void v39();
    virtual void v40();
    virtual void v41();
    virtual void v42();
    virtual void v43();
    virtual void v44();
    virtual void v45();
    virtual int v46(unsigned char* a, unsigned char* b);
    virtual int v47(unsigned char a, unsigned char b, void** c, int* d, unsigned char* e);
    virtual void v48(void* a, unsigned char b, unsigned char c, int d, int e);

    char pad_40[0x94];
    unsigned char m_d4;
    char pad_d5[0xF7];
    int m_1cc;
    int m_1d0;
    int m_1d4;
    char pad_1d8[8];
    unsigned char m_1e0;
    char pad_1e1[0xb];
    int m_1ec[7];
    int m_208[1];
};

struct OtherObj_base { char pad[0x3c]; };

class OtherObj : public OtherObj_base {
public:
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
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
};

extern "C" void fn_27_24CA7C(MainObj* self, int arg2) {
    if (self->m_d4 == 0) return;
    if (self->m_1e0 == 0) return;

    void* obj;
    int index;
    unsigned char byteA;
    unsigned char byte9;
    unsigned char byte8;

    while (self->m_1e0 != 0 && self->m_1cc < self->m_1d0) {
        if (!self->v46(&byteA, &byte9)) break;
        if (!self->v47(byteA, byte9, &obj, &index, &byte8)) break;
        self->v48(obj, byteA, byte9, index, arg2);
        self->m_208[index] = self->m_1d4;
        self->m_1ec[index] = 0;
        self->m_1cc++;
        self->m_1d4++;
        if (byte8 != 0) {
            reinterpret_cast<OtherObj*>(obj)->v24();
        }
        if (arg2 == 0) break;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_24CBD8 {
typedef unsigned char u8;
typedef int s32;

struct Fn27Obj {
    char pad[0x1d4];
    s32 m_1d4;
    s32 m_1d8;
    u8* m_1dc;
    u8 m_1e0;
};

extern "C" s32 fn_27_24CBD8(Fn27Obj* self, u8* out1, u8* out2) {
    if (self->m_1d4 < self->m_1d8) {
        if (self->m_1dc[4] == 0) {
            *out1 = self->m_1dc[10];
            *out2 = (self->m_1dc + self->m_1d4)[0x24];
        } else {
            *out1 = (self->m_1dc + self->m_1d4 * 0x14)[0x24];
            *out2 = (self->m_1dc + self->m_1d4 * 0x14)[0x25];
        }
        return 1;
    }
    self->m_1e0 = 0;
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_24CC58 {
typedef unsigned char u8;

struct Data {
    u8 a;
    u8 b;
};

struct Object_vptr_3c_base { char pad[0x3c]; };

class Object_vptr_3c : public Object_vptr_3c_base {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void processCamera();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void renderOpa();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void init();
    virtual void slot22();
    virtual void slot23();
    virtual void slot24();
    virtual void slot25();
    virtual void slot26();
    virtual void slot27();
    virtual void slot28();
    virtual void slot29();
    virtual void slot30();
    virtual void slot31();
    virtual void slot32();
    virtual void slot33();
    virtual void slot34();
    virtual void slot35();
    virtual void slot36();
    virtual Data* slot37();
};

struct Mgr { Object_vptr_3c* o[7]; };

struct Self {
    char pad0[0x140];
    int v140[7];
    char pad1[0x1b8 - 0x15c];
    int start;
    int lim;
    int cur;
    char pad2[0x1e1 - 0x1c4];
    u8 flag;
    char pad3[0x1ec - 0x1e2];
    int cnt[7];
};

extern Mgr* g_stLoaderManager;

extern "C" int fn_27_24CC58(Self* self, unsigned int arg1, int arg2, Object_vptr_3c** outObj, int* outIdx, u8* outFlag) {
    int i;
    Object_vptr_3c* obj;
    if (self->flag != 0) {
        *outObj = 0;
        if (arg1 < 0x2e) {
            i = self->cur - 1;
        } else {
            i = self->lim - 1;
        }
        while (i >= self->start) {
            if (self->v140[i] < self->cnt[i]) {
                obj = g_stLoaderManager->o[i];
                if (obj != 0) {
                    Data* d = obj->slot37();
                    if ((d->a >= 0x2e && d->a <= 0x31) || d->b == 3) {
                        *outObj = obj;
                        *outIdx = i;
                        *outFlag = 1;
                        return 1;
                    }
                }
            }
            i--;
        }
    } else {
        *outObj = 0;
        for (i = self->start; i < self->lim; i++) {
            if (self->v140[i] < self->cnt[i]) {
                obj = g_stLoaderManager->o[i];
                if (obj != 0) {
                    *outObj = obj;
                    *outIdx = i;
                    *outFlag = 1;
                    return 1;
                }
            }
        }
    }
    return *outObj != 0;
}
}

// TODO: clean up, give variables real names
namespace jj_24CDE4 {
struct Rec {
    u8 kind;
    u8 f01;
    u8 f02;
    u8 pad03;
    u8 f04;
    s8 color;
    u8 costume;
    u8 pad07;
    u8 f08;
    u8 pad09[2];
    u8 f0b;
    u8 pad0c[0x0e];
    u8 f1a;
    u8 f1b;
    u8 pad1c[2];
    u8 f1e;
    u8 f1f;
    u8 pad20[2];
    u16 f22;
    u8 pad24[0x10];
    f32 f34;
    f32 f38;
    u8 pad3c[4];
    f32 f40;
};

struct Object_vptr_3c_base { char pad[0x3c]; };
class Object_vptr_3c : public Object_vptr_3c_base {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void processCamera();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void renderOpa();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void init();
    virtual void slot22();
    virtual void slot23();
    virtual void slot24();
    virtual void slot25();
    virtual void slot26();
    virtual void slot27();
    virtual void slot28();
    virtual void slot29();
    virtual void slot30();
    virtual void slot31();
    virtual void slot32();
    virtual void slot33();
    virtual void slot34();
    virtual void slot35();
    virtual void slot36();
    virtual Rec* slot37();
};

struct Src {
    u8 pad00[4];
    u8 f04;
    u8 pad05;
    u8 f06;
    u8 pad07;
    u16 f08;
    u8 pad0a;
    u8 f0b;
    u8 f0c;
    u8 pad0d[3];
    f32 f10;
    f32 f14;
    f32 f18;
};

struct Elem {
    u8 pad00[0x26];
    u8 f26;
    u8 f27;
    u8 f28;
    u8 pad29[3];
    f32 f2c;
    f32 f30;
    f32 f34;
};

struct Menu {
    u8 pad0[0x1d4];
    s32 f1d4;
    s32 pad1d8;
    Src* f1dc;
    s32 pad1e0;
    s32 f1e4;
    s32 f1e8;
};

extern "C" int exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(int);
extern "C" int getFighterColorFileNo__6muMenuFiii(int, int);

extern "C" void fn_27_24CDE4(Menu* self, Object_vptr_3c* obj, s32 kind, s32 costume, s32 arg4, s32 flag)
{
    Rec* r = obj->slot37();
    r->kind = (u8)kind;
    r->f01 = 1;
    r->f02 = (u8)arg4;
    r->f0b = 1;

    s32 m = exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(kind);
    s32 color;
    if (m == 0x28) {
        color = 0;
    } else {
        color = getFighterColorFileNo__6muMenuFiii(m, costume);
    }
    r->color = (s8)color;
    r->costume = (u8)costume;
    r->f1b = (u8)(r->f1b | 0x80);

    if (self->f1dc->f04 == 0) {
        s32 t;
        u8 e;
        if (self->f1dc->f0b != 0) {
            e = self->f1dc->f0b;
        } else {
            t = self->f1e8;
            e = 7;
            self->f1e8 = t + 1;
            if (t & 1) {
                e = 0;
            }
        }
        r->f1e = e;
        r->f1f = self->f1dc->f0c;
        r->f1a = self->f1dc->f06;
        r->f22 = self->f1dc->f08;
        r->f34 = self->f1dc->f10;
        r->f38 = self->f1dc->f14;
        r->f40 = self->f1dc->f18;
        r->f04 = 1;
    } else {
        r->f1a = self->f1dc->f06;
        Elem* el = (Elem*)((u8*)self->f1dc + self->f1d4 * 0x14);
        u8 c = el->f26;
        r->f04 = ((s8)c >= 1) ? c : 1;
        r->f1e = el->f27;
        r->f1f = el->f28;
        r->f34 = el->f2c;
        r->f38 = el->f30;
        r->f40 = el->f34;
    }
    if (flag == 0) {
        r->f08 = 0x1e;
    }
    self->f1e4 = self->f1e4 + 1;
}
}

// TODO: clean up, give variables real names
namespace jj_24CF90 {
extern "C" {
extern int g_muWifiInterfaceTask;
extern void* g_aiMgr;
int getNetworkErrorLevel__19muWifiInterfaceTaskFv(void*);
u8 getMyAID__7NetworkFv();
int getPlayerLive__19muWifiInterfaceTaskFi(void*, int);
void setFighterOperationType__9ftManagerFiSc(ftManager*, int, int);
void setFighterOperationCpuType__9ftManagerFii(ftManager*, int, u8);
void setCpuRank__5aiMgrFiSc(void*, int, u8);
void setSyncCommand__13ipKeyRecorderFUli(void*, u8, u16);
}
struct Flags { u8 a : 5; u8 b : 1; u8 c : 2; };
inline void setInfo(u8* mgr, int value, u8 flag) {
    u8 idx = getMyAID__7NetworkFv();
    if (idx == 0xFF) idx = 4;
    u8* e = mgr + idx * 0x218;
    ((Flags*)(e + 0xC6))->b = flag;
    *(int*)(e + 0x78) = value;
}
inline int handleEntry(u8* ent, u8* self, int i) {
    int act;
    ent[0xB5] |= 4;
    if (self[0x225]) {
        act = g_ftManager->getEntryId(i);
        if (act != -1) {
            setFighterOperationType__9ftManagerFiSc(g_ftManager, act, 1);
            setFighterOperationCpuType__9ftManagerFii(g_ftManager, act, ent[0xB6]);
            setCpuRank__5aiMgrFiSc(g_aiMgr, i, ent[0xB7]);
        }
    } else {
        ent[0x99] = 3;
    }
    return 1;
}
extern "C" void fn_27_24CF90(u8* self) {
    u8* ent;
    int off;
    u8* mgr;
    int act;
    int bits;
    int i;
    mgr = (u8*)g_muWifiInterfaceTask;
    if (mgr != 0) {
        if (getNetworkErrorLevel__19muWifiInterfaceTaskFv(mgr) > 0) {
            self[0x224] = 0;
            return;
        }
        setInfo(mgr, self[0xD0] ? *(int*)(self + 0xE0) : -1, self[0x76]);
        if (self[0x226]) {
            act = 0;
            bits = 0;
            i = 0;
            off = 0;
            for (; i < *(int*)(self + 0x104); i++) {
                ent = *(u8**)(self + 0x44) + off;
                if (ent[0x99] || ((ent[0xD6] >> 7) & 1)) {
                    bits |= 1 << i;
                } else if (!((ent[0xB5] >> 2) & 1) && !getPlayerLive__19muWifiInterfaceTaskFi(mgr, i)) {
                    act = handleEntry(ent, self, i);
                } else {
                    bits |= 1 << i;
                }
                off += 0x5C;
            }
            if (act && ipSwitch::getInstance() && ipSwitch::getInstance()->m_keyRecorder) {
                int s = self[0x225] ? 1 : 2;
                setSyncCommand__13ipKeyRecorderFUli(ipSwitch::getInstance()->m_keyRecorder, (u8)s, (u16)bits);
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_24D18C {
extern "C" unsigned char lbl_8059E742;

extern "C" unsigned char getMyAID__7NetworkFv() {
    return lbl_8059E742;
}
}

// TODO: clean up, give variables real names
namespace jj_24D4B8 {
extern "C" {
extern char lbl_27_rodata_4028[];
void fn_27_249E78(void*, void*, void*);
unsigned int __cvt_fp2unsigned(float);
}

extern "C" void fn_27_24D4B8(void* self) {
    char* elem = (char*)0;
    float scale2 = *(float*)(lbl_27_rodata_4028 + 0x24);
    int offset;
    char* ptr = (char*)self;
    int i = 0;
    float scale1 = *(float*)(lbl_27_rodata_4028 + 0x20);
    float threshold = *(float*)(lbl_27_rodata_4028 + 0x1C);
    offset = 0;

    while (i < *(int*)((char*)self + 0x104)) {
        void* ref = *(void**)((char*)self + 0x44);
        elem = (char*)*(void**)((char*)self + 0xCC) + offset;
        float val = *(float*)((char*)ref + 0x50);

        if (threshold != val) {
            *(unsigned int*)(elem + 0x204) *= 2;
            *(unsigned int*)(elem + 0x208) *= 2;
            *(unsigned int*)(elem + 0x20C) *= 2;
            *(unsigned int*)(elem + 0x23C) *= 2;
            *(unsigned int*)(elem + 0x240) *= 2;
            *(float*)(elem + 0x2C8) *= scale1;
            *(float*)(elem + 0x2CC) *= scale1;

            ref = *(void**)((char*)self + 0x44);
            val = *(float*)((char*)ref + 0x50);

            if (threshold < val) {
                *(unsigned int*)(elem + 0x204) /= 3;
                *(unsigned int*)(elem + 0x208) /= 3;
                *(unsigned int*)(elem + 0x20C) /= 3;
                *(unsigned int*)(elem + 0x23C) /= 3;
                *(unsigned int*)(elem + 0x240) /= 3;
                *(float*)(elem + 0x2C8) *= scale2;
                *(float*)(elem + 0x2CC) *= scale2;
            }

            *(unsigned int*)(elem + 0x234) =
                __cvt_fp2unsigned(*(float*)(elem + 0x2C8));
            *(unsigned int*)(elem + 0x238) =
                __cvt_fp2unsigned(*(float*)(elem + 0x2CC));
        }

        if (i < 4) {
            fn_27_249E78(self, *(void**)(ptr + 0x15C), elem + 0x58);
            fn_27_249E78(self, *(void**)(ptr + 0x15C), elem + 0xBC);
            fn_27_249E78(self, *(void**)(ptr + 0x178), elem + 0x120);
            fn_27_249E78(self, *(void**)(ptr + 0x178), elem + 0x184);
        }

        offset += 0x2AC;
        ptr += 4;
        i++;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_24D6A0 {

extern "C" {
extern int lbl_27_data_4F050[];
extern const u8 lbl_27_rodata_4050[];
extern Stage* g_Stage;
}

struct Info {
    char pad0[8];
    u8 flags;
    char pad9[2];
    u8 flags2;
    char padc[0xe];
    u16 stage;
    char pad1c[0x7d];
    u8 type;
    u8 value;
    u8 index;
};

struct Status {
    char pad[0x2e];
    s8 active;
    char rest[0x27d];
};

struct Object {
    char pad0[0x44];
    Info* info;
    char pad48[0x2e];
    u8 disabled;
    char pad77[0x35];
    s32 sound;
    char padb0[0x1c];
    Status* status;
    char padd0[0xc];
    u8 flag;
    char paddd[0x158];
    u8 enabled;
    char pad236[2];
    u32 mask;
    u32 timer;
    s32 stateData;
    s32 state;
};

extern "C" void fn_27_24D6A0(Object* self) {
    Info* entry;
    int* data = lbl_27_data_4F050;
    u32 bit;
    s32 i;

    if (!self->enabled) {
        return;
    }
    switch (self->disabled) {
    case 0:
        break;
    default:
        return;
    }
    switch (self->state) {
    case 0:
        self->stateData = data[0x10c / 4];
        self->state = 0xbfd;
    case 0xbfd:
        switch ((self->info->flags >> 2) & 0x3f) {
        case 0:
        case 1:
        case 14:
        case 23:
        case 24:
        case 28:
        case 33: {
            u8 en = 0;
            if (self->flag && !((self->info->flags2 >> 5) & 1)) {
                en = 1;
            }
            self->enabled = en;
            break;
        }
        default:
            self->enabled = 0;
            break;
        }
        self->stateData = data[0x110 / 4];
        self->state = 0xc0e;
        return;
    case 0xc0e:
        self->mask = 0;
    case 0xc57:
    scan:
        for (i = 0; i < 4; i++) {
            entry = (Info*)((char*)self->info + i * 0x5c);
            if (entry->type != 3) {
                if (self->status[entry->index].active == 0) {
                    bit = 1 << i;
                    if (!(self->mask & bit)) {
                        self->mask |= bit;
                        if (entry->type == 0) {
                            g_sndSystem->playSE((SndID)(entry->value + 0x1fe8), self->sound, 0, 0, -1);
                            Stage* stage = g_Stage;
                            const u8* table = lbl_27_rodata_4050;
                            u8 vision[4] = { *table++, table[0], table[1], table[2] };
                            stage->setVision(vision[entry->value]);
                        } else {
                            g_sndSystem->playSE((SndID)0x1fec, self->sound, 0, 0, -1);
                        }
                        if (self->info->stage == 0x24) {
                            g_sndSystem->playSE((SndID)0x1d1e, -1, 0, 0, -1);
                        }
                        self->timer = 0;
                        goto delay1;
                    }
                } else {
                    goto clear;
                }
            }
            continue;
        inc41:
            self->timer++;
            self->stateData = data[0x114 / 4];
            self->state = 0xc41;
            return;
        case 0xc41:
        delay1:
            if (self->timer < 0x50) {
                goto inc41;
            }
            g_sndSystem->playSE((SndID)0x1fed, self->sound, 0, 0, -1);
            self->stateData = data[0x118 / 4];
            self->state = 0xc46;
            return;
        case 0xc46:
            self->timer = 0;
            goto check4c;
        inc4c:
            self->timer++;
            self->stateData = data[0x11c / 4];
            self->state = 0xc4c;
            return;
        case 0xc4c:
        check4c:
            if (self->timer < 0x50) {
                goto inc4c;
            }
            goto scan;
        clear:
            self->mask &= ~(1 << i);
        }
        self->stateData = data[0x120 / 4];
        self->state = 0xc57;
        return;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_24D9D8 {

struct SlotTask_base { char pad[0x3c]; };
class SlotTask : public SlotTask_base {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void processCamera();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void renderOpa();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void init();
    virtual void slot22();
    virtual void slot23();
};

extern "C" {
extern f32 lbl_27_rodata_4028;
extern s32 lbl_27_data_4F050[];
void setStockCount__7ftOwnerFi(void* owner, s32 arg);
void sharePlayerStock__6IfMngrFii(void* ifMngr, u8 a, u8 b);
s32 getEntryId__9ftManagerFi(void* mgr, s32 playerNo);
s32 isValidEntryId__9ftManagerFi(void* mgr, s32 entryId);
void* getOwner__9ftManagerFi(void* mgr, s32 entryId);
s32 getStockCount__7ftOwnerFv(void* owner);
}

extern "C" void fn_27_24D9D8(char* self) {
    char* e;
    s32* data = lbl_27_data_4F050;
    if (*(u8*)(self + 0x248) != 0) {
        switch (*(u8*)(self + 0x76)) {
        case 0:
            break;
        default:
            return;
        }
        {
            switch (*(s32*)(self + 0x250)) {
            case 0:
                *(s32*)(self + 0x24C) = data[0x1AC / 4];
                *(s32*)(self + 0x250) = 0xC64;
                /* fallthrough */
            case 0xC64: {
                char* base = *(char**)(self + 0x44);
                u32 kind = ((u32)*(u8*)(base + 0x8) >> 2) & 0x3F;
                switch (kind) {
                case 0:
                case 1:
                case 14:
                case 23:
                case 24:
                case 28:
                case 33: {
                    u8 flag = 0;
                    if ((((u32)*(u8*)(base + 0x9) >> 5) & 7) == 1 && *(u8*)(base + 0x13) == 1) {
                        flag = 1;
                    }
                    *(u8*)(self + 0x248) = flag;
                    break;
                }
                default:
                    *(u8*)(self + 0x248) = 0;
                    break;
                }
                *(s32*)(self + 0x24C) = data[0x1B0 / 4];
                *(s32*)(self + 0x250) = 0xC74;
                return;
            }
            case 0xC74:
            case 0xCEF: {
                f32 zero = lbl_27_rodata_4028;
                s32 off;
                char* objp = self;
                s32 i = 0;
                off = 0;
                for (; i < 4; i++, off += 0x5C, objp += 4) {
                    e = *(char**)(self + 0x44) + off;
                    if (*(u8*)(e + 0x99) == 0) {
                        if (*(u8*)(e + 0x9F) != 0) {
                            char* fighter = *(char**)(self + 0xCC) + *(u8*)(e + 0x9B) * 0x2AC;
                            if (*(s8*)(fighter + 0x2E) == 0) {
                                ipPadStatus pad;
                                ipSwitch::getInstance()->getPadStatus(i, pad);
                                if (pad.m_button & 0x10) {
                                    s32 team = *(u8*)(e + 0xA3);
                                    s32 best = 1;
                                    char* bestPtr = 0;
                                    s32 bestJ;
                                    for (s32 j = 0; j < 4; j++) {
                                        if (j != i) {
                                            char* p = *(char**)(self + 0x44) + j * 0x5C;
                                            char* q = p + 0x98;
                                            if (*(u8*)(p + 0xA3) == team) {
                                                char* f = *(char**)(self + 0xCC) + *(u8*)(q + 3) * 0x2AC;
                                                s8 v = *(s8*)(f + 0x2E);
                                                if (best < v) {
                                                    best = v;
                                                    bestPtr = q;
                                                    bestJ = j;
                                                }
                                            }
                                        }
                                    }
                                    if (bestPtr != 0) {
                                        g_sndSystem->playSE((SndID)0x1FD1, *(s32*)(self + 0xAC), 0, 0, -1);
                                        s32 entry = getEntryId__9ftManagerFi(g_ftManager, bestJ);
                                        if (isValidEntryId__9ftManagerFi(g_ftManager, entry) != 0) {
                                            void* owner1 = getOwner__9ftManagerFi(g_ftManager, entry);
                                            s32 stocks = getStockCount__7ftOwnerFv(owner1);
                                            if (stocks > 1) {
                                                s32 entry2 = getEntryId__9ftManagerFi(g_ftManager, i);
                                                if (isValidEntryId__9ftManagerFi(g_ftManager, entry2) != 0) {
                                                    void* owner2 = getOwner__9ftManagerFi(g_ftManager, entry2);
                                                    setStockCount__7ftOwnerFi(owner2, 1);
                                                    setStockCount__7ftOwnerFi(owner1, stocks - 1);
                                                    *(f32*)(*(char**)(objp + 0x58) + 0x48) = zero;
                                                    ((SlotTask*)*(char**)(objp + 0x58))->slot23();
                                                    if (g_IfMngr != 0) {
                                                        sharePlayerStock__6IfMngrFii(g_IfMngr, *(u8*)(bestPtr + 2), *(u8*)(e + 0x9A));
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                *(s32*)(self + 0x24C) = data[0x1B4 / 4];
                *(s32*)(self + 0x250) = 0xCEF;
                break;
            }
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_24DD38 {
extern "C" void fn_27_24DD38(void* self) {
    u8* p = (u8*)self;
    u8 selected = 0;
    for (int i = 0; i < 4; i++) {
        if (p[0x25 + i * 0x2AC] != 3 && p[0x33 + i * 0x2AC] == 0) {
            selected = i;
        }
    }
    p[0x1D] = selected;
    selected = 0;
    for (int i = 0; i < 3; i++) {
        if (p[0x1306 + i * 0x30] != 0 && p[0x1305 + i * 0x30] == 0) {
            selected = i;
        }
    }
    p[0x1E] = selected;
    if (p[2] == 1) {
        u8* current = p;
        u8 team;
        int candidate;
        for (int i = 0; i < 4; current += 0x2AC, i++) {
            if (current[0x25] == 3) continue;
            team = p[0x1E];
            if (team == current[0x2A]) {
                candidate = i;
                for (int j = 0; j < 4; j++) {
                    if (p[0x25 + j * 0x2AC] != 3 &&
                        team == p[0x2A + j * 0x2AC] &&
                        i != j &&
                        current[0x33] > p[0x33 + j * 0x2AC]) {
                        candidate = -1;
                        break;
                    }
                }
                if (candidate >= 0) {
                    p[0x1F] = candidate;
                    return;
                }
            }
        }
    } else {
        p[0x1F] = p[0x1D];
    }
}
}

// TODO: clean up, give variables real names
namespace jj_24DF74 {
extern "C" int fn_27_24DF74(unsigned char* p, unsigned int a, unsigned short b) {
    if (a != 1 && a != 2)
        return 0;

    p[5] = static_cast<unsigned char>(a);
    p[4] = 1;
    *reinterpret_cast<unsigned short*>(p + 6) = b;
    return 1;
}
}

