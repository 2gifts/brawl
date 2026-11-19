#include <if/if_mngr.h>
#include <st/operator/st_operator_rule.h>
#include <st/st_event_rule.h>
#include <string.h>
#include <types.h>
#include <ft/ft_manager.h>

struct stOperatorRuleCharacterKindEntry {
    u8 m_gmKind;
    u8 m_muKind;
    u8 unk2;
    u8 m_figureId;
};

extern "C" {
// HYPOTHESIS: per-fighter-kind table (sora_melee .data 0x4EDEC), 0x33 entries.
extern stOperatorRuleCharacterKindEntry lbl_27_data_4EDEC[];
// HYPOTHESIS: the reset value of the replay-save state (sora_melee .data 0x4EDD8).
extern u32 lbl_27_data_4EDD8;
}

stOperatorRule::~stOperatorRule() { }

void stOperatorRule::processBegin() { }

bool stOperatorRule::isEnd() {
    return false;
}

bool stOperatorRule::isSuddenDeath() {
    return false;
}

void stOperatorRule::calcScore() { }

void stOperatorRule::notifyEventKnockout(u32) { }

bool stOperatorRule::notifyEventDead(int entryId, u32 playerIndex, int) {
    return true;
}

void stOperatorRule::notifyEventSetupCorps() { }

void stOperatorRule::nortifyNetworkOtherNodeExit() { }

u32 stOperatorRule::exchangeGmCharacterKindToMuCharKind(gmCharacterKind kind) {
    bool valid = false;
    if (kind >= 0 && kind < 0x33) {
        valid = true;
    }
    if (valid) {
        return lbl_27_data_4EDEC[kind].m_muKind;
    }
    return 0x23;
}

u32 stOperatorRule::exchangeGmCharacterKind2FigureId(gmCharacterKind kind) {
    bool valid = false;
    if (kind >= 0 && kind < 0x33) {
        valid = true;
    }
    if (valid) {
        return lbl_27_data_4EDEC[kind].m_figureId;
    }
    return 0;
}

bool stOperatorRule::canSaveReplay() {
    return !unk75;
}

void stOperatorRule::startSaveReplay() {
    unkB4 = lbl_27_data_4EDD8;
    unkB8 = 0;
    unkBC = false;
}

void stOperatorRule::notifyEventBeat(u32 playerIndex1, u32 playerIndex2) {
    if (m_condition->m_meleeInitData.m_0x3_2 && g_IfMngr) {
        if (m_condition->m_meleeInitData.m_isTeams == true &&
            (u8)m_condition->m_playersInitData[playerIndex1].m_teamNo ==
                (u8)m_condition->m_playersInitData[playerIndex2].m_teamNo) {
            g_IfMngr->m_ifPlayers[m_condition->m_playersInitData[playerIndex1].m_playerId]->setScoreVary(-1);
            g_IfMngr->m_ifPlayers[m_condition->m_playersInitData[playerIndex2].m_playerId]->setScoreVary(-1);
            return;
        } else {
            g_IfMngr->m_ifPlayers[m_condition->m_playersInitData[playerIndex1].m_playerId]->setScoreVary(1);
        }
        g_IfMngr->m_ifPlayers[m_condition->m_playersInitData[playerIndex2].m_playerId]->setScoreVary(-1);
    }
}

void stOperatorRule::startOperator() {
    if (unk54) {
        unk54->notifyEventStartGame();
    }
}

void stOperatorRule::notifyEventStartStage() {
    unk78 = 0;
    memset(unk7C, 0, sizeof(unk7C));
    memset(unk8C, 0, sizeof(unk8C));
    checkDispCorps();
    if (unk54) {
        unk54->notifyEventStartStage();
    }
}

void stOperatorRule::notifyEventYoshiEgg() {
    if (unk54) {
        unk54->notifyEventYoshiEgg();
    }
}

void stOperatorRule::notifyEventPikminBloom() {
    if (unk54) {
        unk54->notifyEventPikminBloom();
    }
}

void stOperatorRule::updateRealtimeHandi(gmResultInfo* result, float deltaFrame) {
    if (m_condition->m_meleeInitData.m_0x5_5) {
        if (0.0f < deltaFrame) {
            unk78 = 0;
            int highest;
            int lowest;
            int count = 0;
            for (int player = 0; player < 4; player++) {
                if (g_ftManager->isValidEntryId(g_ftManager->getEntryId(player))) {
                    gmPlayerResultInfo* playerResult = &result->m_playersResultInfo[m_condition->m_playersInitData[player].m_playerNo];
                    int score;
                    if (m_condition->m_meleeInitData.m_isStamina) {
                        score = 0;
                    } else if (m_condition->m_meleeInitData.m_gameRule == 2) {
                        score = playerResult->m_coins;
                    } else if (m_condition->m_meleeInitData.m_gameRule == 1) {
                        score = playerResult->m_startStockCount;
                    } else {
                        score = playerResult->m_rankCount;
                    }
                    if (!unk78) {
                        highest = score;
                        lowest = score;
                    } else {
                        highest = highest > score ? highest : score;
                        lowest = lowest < score ? lowest : score;
                    }
                    unk7C[player] = score;
                    unk78 |= 1 << player;
                    count++;
                }
            }
            int difference = highest - lowest;
            float scale = 0.5f * difference;
            float strength;
            if (m_condition->m_meleeInitData.m_gameRule == 0) {
                strength = difference < 5 ? 0.5f : 1.0f;
            } else if (m_condition->m_meleeInitData.m_gameRule == 1) {
                strength = difference < 2 ? 0.5f : 1.0f;
            } else {
                strength = 1.0f;
            }
            strength *= count / 4.0f;
            float step = 0.04f * deltaFrame / 60.0f;
            for (int player = 0; player < 4; player++) {
                if (unk78 & (1 << player)) {
                    float target = scale == 0.0f ? 0.0f : strength * ((unk7C[player] - lowest) / scale - 1.0f);
                    unk9CValues[player] = target;
                    float current = unk8C[player];
                    if (current != target) {
                        if (current < target) {
                            current += step;
                            if (current > target) current = target;
                        } else {
                            current -= step;
                            if (current < target) current = target;
                        }
                        unk8C[player] = current;
                    }
                }
            }
        }
        for (int player = 0; player < 4; player++) {
            if (unk78 & (1 << player)) {
                int entryId = g_ftManager->getEntryId(player);
                if (g_ftManager->isValidEntryId(entryId)) {
                    g_ftManager->getOwner(entryId)->setAutoHandi(unk8C[player]);
                }
            }
        }
    }
}

#include <ip/ip_switch.h>
// TODO: clean up, give variables real names
namespace jj_247D98 {
extern "C" {
extern const char lbl_27_data_4EDDC[];
extern const char lbl_27_data_4EF40[];
extern void* lbl_27_data_4EDD8;
extern void* g_sndSystem;

void __ct__10stOperatorFPCc(void* self, const char* name);

void* __ct__14stOperatorRuleFv(void* self) {
    char* p = (char*)self;
    char* snd;
    int cnt;

    __ct__10stOperatorFPCc(self, lbl_27_data_4EDDC);
    *(const void**)(p + 0x3C) = lbl_27_data_4EF40;
    *(int*)(p + 0x48) = 0;
    *(int*)(p + 0x4C) = 0;
    *(int*)(p + 0x50) = 0;
    *(int*)(p + 0x54) = 0;
    *(unsigned char*)(p + 0x74) = 0;
    *(unsigned char*)(p + 0x75) = 0;
    *(unsigned char*)(p + 0x76) = 0;
    snd = (char*)g_sndSystem;
    cnt = (unsigned short)(*(int*)(snd + 0x6E8) + 1);
    *(int*)(snd + 0x6E8) = cnt;
    *(int*)(p + 0xAC) = cnt;
    *(unsigned int*)(p + 0xB0) = 0xFF439EB2;
    *(void**)(p + 0xB4) = lbl_27_data_4EDD8;
    *(int*)(p + 0xB8) = 0;
    *(unsigned char*)(p + 0xBD) = 0;
    *(unsigned char*)(p + 0xBE) = 0;
    *(int*)(p + 0x58) = 0;
    *(int*)(p + 0x5C) = 0;
    *(int*)(p + 0x60) = 0;
    *(int*)(p + 0x64) = 0;
    *(int*)(p + 0x68) = 0;
    *(int*)(p + 0x6C) = 0;
    *(int*)(p + 0x70) = 0;
    return self;
}
}
}

// TODO: clean up, give variables real names
namespace jj_248478 {
struct Entry {
    u8 id;
    u8 value;
    s8 active;
    u8 pad[0x11];
};

struct GameData {
    u8 pad0[4];
    u8 mode;
    u8 pad5[0x17];
    u8 start;
    u8 count;
    u8 pad1e[6];
    Entry entries[1];
};

struct Global {
    u8 pad[0x3c];
    GameData* data;
};

struct State {
    u8 pad[0x14];
    u8 mode;
};

struct Object {
    u8 pad[0x44];
    State* state;
    u8 pad48[0x68];
    int count;
};

struct Result {
    int id;
    u8 value;
    u8 pad[3];
};

extern "C" Global* g_GameGlobal;
extern "C" void* g_IfMngr;
extern "C" const u8 lbl_27_data_4EDEC[];
extern "C" void dispCorps__6IfMngrFPCvi(void*, Result*, int);

inline u8 convert(int id) {
    bool valid = false;
    if (id >= 0 && id < 0x33) {
        valid = true;
    }
    if (valid) {
        return lbl_27_data_4EDEC[id * 4 + 2];
    }
    return 0x3d;
}

extern "C" void checkDispCorps__14stOperatorRuleFv(Object* self) {
    Result results[36];

    if (self->state->mode == 4) {
        GameData* data = g_GameGlobal->data;
        if (data->mode == 1) {
            Result* dst = results;
            int start = data->start;
            int end = start + data->count;
            int count = 0;
            Entry* entry = data->entries + start;

            for (int i = start; i < end; ++i, ++entry) {
                if (entry->active != 0) {
                    dst->id = convert(entry->id);
                    ++count;
                    dst->value = entry->value;
                    ++dst;
                }
            }

            if (self->count != count) {
                self->count = count;
                if (g_IfMngr) {
                    dispCorps__6IfMngrFPCvi(g_IfMngr, results, count);
                }
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_2485A8 {

extern "C" {
void setPause__13gfApplicationFii(void*, unsigned int, unsigned int);
void fn_8010FE58(void*, void*);
void stopController__20stOperatorControllerFv(void*);
void lbStopRumbleAll__8gfRumbleFv(void*);
void HSD_PadRumblePauseAll__8gfRumbleFv(void*);
void stopMotorAllForce__11gfPadSystemFv(void*);
void* allStopSE__9sndSystemFib(void*, int, int);
int isEndMatchingEnd__19muWifiInterfaceTaskFv(int);
int fn_800D1BC8(int);
void fn_800D1ADC(int, int);
void saveReplay__6IfMngrFv(void*);
void* isSaveReplayFinished__6IfMngrFPi(void*, void*);
void setVisibleReplaySave__8IfCenterFbibb(void*, int, int, int, int);
int fn_8010FF20(int);
int fn_8010FFBC(int);
void* fn_80110058(int);

void playSE__9sndSystemF5SndIDiiii(void*, int, int, int, int, int);
void OSReport(const char*, ...);
extern void* g_IfMngr;
extern void* g_gfApplication;
extern void* g_gfPadSystem;
extern void* g_sndSystem;
extern int g_muWifiInterfaceTask;
extern unsigned char lbl_27_bss_59D8;
extern char lbl_27_data_4EDD8[];
}

inline char* getInstance__8ipSwitchFv(char*) { return (char*)ipSwitch::getInstance(); }

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
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void slot21();
    virtual void slot22();
    virtual void slot23();
    virtual void slot24();
    virtual void slot25();
    virtual void slot26();
    virtual void slot27(int);
};

extern "C" unsigned char fn_27_2485A8(char* p) {
    char* d = lbl_27_data_4EDD8;
    void* r3 = p;
    void* t;
    void* q;
    void* ifm;
    int r29, r28, r27;
    int r27b, r28b, r29b;
    int sp8;
    int state;

    if (((unsigned int)*(int*)((char*)g_gfApplication + 0xEC) >> 27) & 0x10) {
        return 0;
    }
    state = *(int*)(p + 0xB8);
    switch (state) {
    case 0x0:
        *(int*)(p + 0xB4) = *(int*)(d + 0xE0);
        *(int*)(p + 0xB8) = 0x274;
    case 0x274:
        setPause__13gfApplicationFii(g_gfApplication, 1, 1);
        fn_8010FE58(p + 0xC0, p + 0xC4);
        t = *(void**)(p + 0x4C);
        if (t != 0) {
            ((Object_vptr_3c*)t)->slot27(1);
        }
        lbl_27_bss_59D8 = 0;
        t = *(void**)(p + 0x50);
        if (t != 0) {
            stopController__20stOperatorControllerFv(t);
        }
        *(int*)(p + 0xB4) = *(int*)(d + 0xE4);
        *(int*)(p + 0xB8) = 0x28C;
        break;
    case 0x28C:
        lbStopRumbleAll__8gfRumbleFv(*(void**)((char*)g_gfPadSystem + 0xB68));
        HSD_PadRumblePauseAll__8gfRumbleFv(*(void**)((char*)g_gfPadSystem + 0xB68));
        stopMotorAllForce__11gfPadSystemFv(g_gfPadSystem);
        r3 = allStopSE__9sndSystemFib(g_sndSystem, 0, 0);
        *(int*)(p + 0xC8) = 0x258;
    case 0x2C3:
    block_25:
        r29 = 0;
        r28 = 0;
        r27 = 0;
        r3 = getInstance__8ipSwitchFv((char*)r3);
        if (r3 != 0) {
            r3 = getInstance__8ipSwitchFv((char*)r3);
            if (*(void**)((char*)r3 + 0x18) != 0) {
                r27 = 1;
            }
        }
        if (r27 != 0) {
            r3 = *(void**)((char*)getInstance__8ipSwitchFv((char*)r3) + 0x18);
            if (*(unsigned char*)((char*)r3 + 0x3B) == 0) {
                r28 = 1;
            }
        }
        if (r28 != 0) {
            q = *(void**)((char*)getInstance__8ipSwitchFv((char*)r3) + 0x18);
            q = *(void**)((char*)q + 0x24);
            if (*(unsigned short*)((char*)q + 0x6) <= 0x2B98) {
                r29 = 1;
            }
        }
        ifm = g_IfMngr;
        setVisibleReplaySave__8IfCenterFbibb(*(void**)((char*)ifm + 0x44), 1, *(int*)(p + 0xC0), r29, *(unsigned char*)((char*)ifm + 0xA8));
        q = fn_80110058(*(int*)(p + 0xC4));
        if (q != 0) {
            r27b = 0;
            r28b = 0;
            r29b = 0;
            r3 = getInstance__8ipSwitchFv((char*)q);
            if (r3 != 0) {
                r3 = getInstance__8ipSwitchFv((char*)r3);
                if (*(void**)((char*)r3 + 0x18) != 0) {
                    r29b = 1;
                }
            }
            if (r29b != 0) {
                r3 = *(void**)((char*)getInstance__8ipSwitchFv((char*)r3) + 0x18);
                if (*(unsigned char*)((char*)r3 + 0x3B) == 0) {
                    r28b = 1;
                }
            }
            if (r28b != 0) {
                q = *(void**)((char*)getInstance__8ipSwitchFv((char*)r3) + 0x18);
                q = *(void**)((char*)q + 0x24);
                if (*(unsigned short*)((char*)q + 0x6) <= 0x2B98) {
                    r27b = 1;
                }
            }
            if (r27b != 0) {
                playSE__9sndSystemF5SndIDiiii(g_sndSystem, 2, 0x10000, 0, 0, -1);
                setVisibleReplaySave__8IfCenterFbibb(*(void**)((char*)g_IfMngr + 0x44), 0, 0, 1, *(unsigned char*)((char*)g_IfMngr + 0xA8));
                goto block_60;
            } else {
                playSE__9sndSystemF5SndIDiiii(g_sndSystem, 3, 0x10000, 0, 0, -1);
                goto block_47;
            }
        } else {
block_47:
            if (fn_8010FFBC(*(int*)(p + 0xC4)) != 0) {
                playSE__9sndSystemF5SndIDiiii(g_sndSystem, 1, 0x10000, 0, 0, -1);
                setVisibleReplaySave__8IfCenterFbibb(*(void**)((char*)g_IfMngr + 0x44), 0, 0, 1, *(unsigned char*)((char*)g_IfMngr + 0xA8));
                goto block_70;
            }
            if (g_muWifiInterfaceTask == 0 && fn_8010FF20(*(int*)(p + 0xC4)) != 0) {
                playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0x13, 0x10000, 0, 0, -1);
                setVisibleReplaySave__8IfCenterFbibb(*(void**)((char*)g_IfMngr + 0x44), 0, 0, 1, *(unsigned char*)((char*)g_IfMngr + 0xA8));
                *(unsigned char*)(p + 0xBD) = 1;
                goto block_70;
            }
            if (g_muWifiInterfaceTask != 0 && *(unsigned char*)(p + 0xBE) == 0) {
                if (*(int*)(p + 0xC8) <= 0x12C && (*(int*)(p + 0xC8) % 60) == 0) {
                    playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0xA, -1, 0, 0, -1);
                }
                if (--*(int*)(p + 0xC8) <= 0) {
                    playSE__9sndSystemF5SndIDiiii(g_sndSystem, 1, 0x10000, 0, 0, -1);
                    setVisibleReplaySave__8IfCenterFbibb(*(void**)((char*)g_IfMngr + 0x44), 0, 0, 1, *(unsigned char*)((char*)g_IfMngr + 0xA8));
                    goto block_70;
                }
                goto block_59;
            } else {
block_59:
                *(int*)(p + 0xB4) = *(int*)(d + 0xE8);
                *(int*)(p + 0xB8) = 0x2C3;
            }
        }
        break;
block_60:
        saveReplay__6IfMngrFv(g_IfMngr);
block_61:
        *(int*)(p + 0xB4) = *(int*)(d + 0xEC);
        *(int*)(p + 0xB8) = 0x2CC;
        break;
    case 0x2CC:
        r3 = isSaveReplayFinished__6IfMngrFPi(g_IfMngr, &sp8);
        if (r3 == 0) {
            if (g_muWifiInterfaceTask != 0 && *(unsigned char*)(p + 0xBE) == 0) {
                if (*(int*)(p + 0xC8) <= 0x12C && (*(int*)(p + 0xC8) % 60) == 0) {
                    playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0xA, -1, 0, 0, -1);
                }
                if (--*(int*)(p + 0xC8) <= 0) {
                    fn_800D1ADC(g_muWifiInterfaceTask, 0);
                    *(unsigned char*)(p + 0xBE) = 1;
                }
            }
            goto block_61;
        }
        goto block_25;
block_70:
        *(int*)(p + 0xC8) = 2;
        goto st2e5;
block_wait:
        *(int*)(p + 0xC8) -= 1;
        *(int*)(p + 0xB4) = *(int*)(d + 0xF0);
        *(int*)(p + 0xB8) = 0x2E5;
        break;
    case 0x2E5:
st2e5:
        if (*(int*)(p + 0xC8) > 0) goto block_wait;
        if (g_muWifiInterfaceTask == 0) goto block_87;
        if (*(unsigned char*)(p + 0xBE) != 0) goto block_81;
        fn_800D1ADC(g_muWifiInterfaceTask, 1);
        *(int*)(p + 0xC8) = 0x258;
block_76:
        *(int*)(p + 0xB4) = *(int*)(d + 0xF4);
        *(int*)(p + 0xB8) = 0x2F3;
        break;
    case 0x2F3:
        r3 = (void*)p;
        {
            int tmp = *(int*)((char*)r3 + 0xC8);
            *(int*)((char*)r3 + 0xC8) = tmp - 1;
            if (fn_800D1BC8(g_muWifiInterfaceTask) != 0 || *(int*)(p + 0xC8) <= 0) {
                if (fn_800D1BC8(g_muWifiInterfaceTask) == 0) {
                    OSReport(d + 0x100);
                    fn_800D1ADC(g_muWifiInterfaceTask, 0);
                    *(unsigned char*)(p + 0xBE) = 1;
                    goto block_81;
                }
                goto block_87;
            }
            goto block_76;
        }
block_81:
        *(int*)(p + 0xC8) = 0x258;
block_82:
        *(int*)(p + 0xB4) = *(int*)(d + 0xF8);
        *(int*)(p + 0xB8) = 0x302;
        break;
    case 0x302:
        r3 = (void*)p;
        {
            int tmp2 = *(int*)((char*)r3 + 0xC8);
            *(int*)((char*)r3 + 0xC8) = tmp2 - 1;
            if (isEndMatchingEnd__19muWifiInterfaceTaskFv(g_muWifiInterfaceTask) != 0 || *(int*)(p + 0xC8) <= 0) {
                if (isEndMatchingEnd__19muWifiInterfaceTaskFv(g_muWifiInterfaceTask) == 0) {
                    OSReport(d + 0x134);
                }
block_87:
                *(unsigned char*)(p + 0xBC) = 1;
                *(int*)(p + 0xB4) = *(int*)(d + 0xFC);
                *(int*)(p + 0xB8) = 0x310;
                break;
            }
            goto block_82;
        }
        break;
    }
    return *(unsigned char*)(p + 0xBC);
}
}

// TODO: clean up, give variables real names
namespace jj_248C60 {
extern "C" void fn_27_248C60(void) {}
}

