#pragma once

#include <ai/ai_mgr.h>
#include <ai/ai_stat.h>
#include <types.h>

class aiAct;

// One attack the AI knows about (entry of the ATKD block); HYPOTHESIS: 0x18 bytes, id first.
struct tag_AI_ATK_DATA {
    u16 m_id;
    char unk2[0x16];
};

// ATKD block header followed by its entries.
struct tag_AI_ATK_DATA_HED {
    char unk0[4];
    int m_num;
    char unk8[8];
    tag_AI_ATK_DATA m_data[1];

    tag_AI_ATK_DATA* searchAtkData(int id); // HYPOTHESIS: int (the id is compared signed)
};

// Per-character AI parameter block loaded from the AIPD / ATKD data files.
// HYPOTHESIS: only the command bit masks the aiParam::chkCmd* helpers read are named.
struct aiParData {
    char unk0[0x160];
    u32 m_fallCmds;     // 0x160
    char unk164[4];
    u32 m_reviseCmds;   // 0x168
    u32 m_reflectCmds;  // 0x16C
    int m_actList[24];  // 0x170: offset of each action list (tag_AI_PAR_ACT_LIST entries ending with cmd 0) from the start of the block, 0 = none
    char unk1D0[0x10];
    int m_situationList[1]; // 0x1E0: offset of each situation entry from the start of the block; HYPOTHESIS: count unknown
};

// One entry of an action list; HYPOTHESIS: the two weights are blended by CPU level (high at rank 100, low at rank 0).
struct tag_AI_PAR_ACT_LIST {
    u8 m_cmd;        // command number + 1, 0 ends the list
    u8 m_ratioHigh;
    u8 m_ratioLow;
};

// Jump parameters of one character (HYPOTHESIS: possibly a larger view of aiReferPar).
struct aiJumpPar {
    char unk0[0x1C];
    float m_jumpHigh;    // 0x1C
    float m_jumpHighSp;  // 0x20
    float m_jumpLength;  // 0x24
};

class aiParam {
public:
    enum {
        NumActLists = 24,
    };

    aiParData* m_par;    // 0x00
    char* m_atkData;     // 0x04: ATKD block, NULL if absent
    s8 m_rank;           // 0x08: CPU level, clamped to 0..100
    u32 m_disableCmds;   // 0x0C: commands the AI may not use (bit per command)
    float m_rankRate;    // 0x10: (rank - 20) / 2 clamped to 0..30
    // 0x14-0x7F: the working parameters, each blended between the high-rank and low-rank values of m_par by the rank
    // (par_work_update). HYPOTHESIS: only the reaction delay fields are understood; their names follow damageStart.
    float m_workF14[12]; // 0x14
    u8 m_workB44[4];     // 0x44
    u8 m_reactBase;      // 0x48: frames the CPU waits before reacting to a hit
    u8 m_reactRand;      // 0x49: plus 0..m_reactRand random frames
    u8 m_reactExtra;     // 0x4A: plus this many frames,
    u8 m_reactExtraRate; // 0x4B: with this chance (percent)
    u8 m_workB4C[8];     // 0x4C
    u16 m_workH54[2];    // 0x54
    u8 m_workB58[3];     // 0x58
    // 0x5B-0x7E: how aiInput::target_check scores each candidate target. Each *Rate is the most points one criterion
    // adds, scaled by where the candidate's value falls between the matching low and high bounds.
    // HYPOTHESIS: names follow target_check's use only.
    u8 m_targetNearRate;       // 0x5B: full points at m_targetNearDist or closer, none at m_targetFarDist or further
    u8 m_targetDamageRate;     // 0x5C: a target with more damage (aiStat::m_unkC4) scores higher
    u8 m_targetLowDamageRate;  // 0x5D: a target with less damage scores higher
    u8 m_targetStockRate;      // 0x5E: stock rule: a target with more stocks (aiStat::m_unkD0) scores higher
    u8 m_targetFewStockRate;   // 0x5F: stock rule: a target with fewer stocks scores higher
    u8 m_targetAttackerRate;   // 0x60: added for a target on the team that last hit the CPU
    u8 m_targetOffstageRate;   // 0x61: added for a target with no ground below it
    u8 m_targetRandomRate;     // 0x62: the start score is random in 0..(this + 0.1)
    float m_targetFarDist;     // 0x64
    float m_targetNearDist;    // 0x68
    u16 m_targetDamageLo;      // 0x6C
    u16 m_targetDamageHi;      // 0x6E
    u16 m_targetLowDamageLo;   // 0x70
    u16 m_targetLowDamageHi;   // 0x72
    u8 m_targetStockLo;        // 0x74
    u8 m_targetStockHi;        // 0x75
    u8 m_targetFewStockLo;     // 0x76
    u8 m_targetFewStockHi;     // 0x77
    u8 m_targetHumanRate;      // 0x78: added for a target that is not a CPU (aiMgr::m_chrState 0)
    u8 m_targetTimeLeadRate;   // 0x79: time rule: a target further ahead of the last place (aiStat::m_unkD4) scores higher
    u8 m_targetCoinLeadRate;   // 0x7A: coin rule: the same
    u8 m_targetTimeLeadLo;     // 0x7B
    u8 m_targetTimeLeadHi;     // 0x7C
    u8 m_targetCoinLeadLo;     // 0x7D
    u8 m_targetCoinLeadHi;     // 0x7E

    aiParam();
    // The rank as a 0..1 ratio. HYPOTHESIS: name (aiInput::swingHit).
    float getRankRatio() const { return 0.01f * m_rank; }
    void setRank(int rank, u32 force, int update);
    void par_work_update();
    void init_par(aiParData* par, int kind, char* atkData);
    bool chkCmdRevise(int cmd);
    bool chkCmdFall(int cmd);
    bool chkCmdReflect(int cmd);
    void set_disable_cmd(int cmd);
    u16 enum_act_list_id(int list, int n); // HYPOTHESIS: the n-th enabled action of an action list (0 = none)
    int select_situation(int start, int end, aiChrIdx* chr, aiChrIdx* target); // HYPOTHESIS: parameter types from chk_situation's use

    // Picks the next action for the situation. HYPOTHESIS: parameter names and types (aiInput::md_return passes the CPU
    // type's flags m_unk116 and whether the CPU type is 0x1A).
    u16 select_action(int situation, aiChrIdx* chr, aiChrIdx* target, u16 flags, aiAct* act, int unk);

    static bool check_revision(char* data);
    static float get_down_atk_r(aiChrIdx* chr);
    static u8 get_down_atk_f(aiChrIdx* chr);
    static float get_cliff_atk_r(aiChrIdx* chr);
    static u8 get_cliff_atk_f(aiChrIdx* chr);
    static float get_width(aiChrIdx* chr);
    static float get_height(aiChrIdx* chr);
    static float get_air_high(aiChrIdx* chr);
    static float get_air_length(aiChrIdx* chr);
    static float get_jump_high(aiJumpPar* par, aiStat* stat);
    static float get_jump_high_sp(aiJumpPar* par, aiStat* stat);
    static float get_jump_length(aiJumpPar* par, aiStat* stat);
};
static_assert(sizeof(aiParam) == 0x80, "Class is wrong size!");

int get_motnum_chr(ftKind kind); // HYPOTHESIS: int return
