#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <types.h>

class aiInput;
class aiStat;
class aiWeapon;
struct ipPadButton;
struct AiClBlock;

// One command of an AI script: a type byte and the size of the command in bytes (the next command follows it).
// HYPOTHESIS: layout from tag_AI_SCRIPT_CMD::if_skip only; types 6/7 open a conditional block, 8 is its else, 9 its end.
struct tag_AI_SCRIPT_CMD {
    u8 m_type;   // 0x0
    u8 unk1;
    u16 m_size;  // 0x2

    tag_AI_SCRIPT_CMD* if_skip();
};

// One AI script: an id, then its commands.
struct tag_AI_SCRIPT {
    u32 m_id;    // 0x0
};

// A pack of AI scripts (a fighter's own pack or the common one): the scripts are found through offsets from the pack.
// HYPOTHESIS: layout from tag_AI_SCRIPT_PACK::get_script only.
struct tag_AI_SCRIPT_PACK {
    u32 unk0;
    u32 m_numScripts;  // 0x4
    char unk8[8];
    u32 m_offsets[1];  // 0x10: m_numScripts offsets of the scripts from the start of the pack

    // HYPOTHESIS: looks in this pack first, then in common; an id with none of the bits 0xFFFF800F set (or one that
    // is not found) is looked up again by its 0x7FF0 bits.
    tag_AI_SCRIPT* get_script(tag_AI_SCRIPT_PACK* common, u32 id);
};

// The CPU's action selector: picks an action id from the loaded AI script and tracks per-action cooldowns.
// HYPOTHESIS: layout recovered from the small accessors only; size from aiInput's allocation.
class aiAct {
public:
    // HYPOTHESIS: action ids; 0x6031.. and 0x6034.. are the command ranges ai_act_id2cmd maps.
    enum actId {};

    float m_unk0[24];      // 0x00
    void* m_script;        // 0x60: loaded AI script data
    void* m_unk64;         // 0x64
    void* m_unk68;         // 0x68
    void* m_unk6C;         // 0x6C
    aiInput* m_input;      // 0x70
    aiStat* m_stat;        // 0x74: the aiStat of the controlled character
    u16 m_id;              // 0x78: action id being run
    u16 m_unk7A;
    u16 m_prevId;          // 0x7C: previous non-zero action id
    u16 m_nextId;          // 0x7E: HYPOTHESIS: the action to run after this one (the act_* handlers store it before ending)
    u8 m_unk80;
    u8 m_unk81;
    u16 m_reflectUse;      // 0x82: chkUseReflector's result: 1 = decided this time, 0x20/0x40 = HYPOTHESIS: the two ways to use it
    float m_unk84;
    float m_unk88;
    float m_unk8C;
    u8 m_unk90;
    u8 m_unk91;
    u8 m_unk92;
    u8 m_unk93;
    int m_unk94;           // 0x94: HYPOTHESIS: a frame count (act_r_dfjump sets 40)
    float m_unk98;         // 0x98: -1 when reset
    u16 m_reflectDone;     // 0x9C: frames the reflect-done penalty lasts
    float m_reflectRate;   // 0xA0
    float m_defendAirRate; // 0xA4
    int m_step;            // 0xA8: step within the running action (the act_* handlers)
    int m_stepFrame;       // 0xAC: HYPOTHESIS: frames in the current step (act_r_dfjump waits for more than 4)
    Vec3f m_unkB0;
    Vec3f m_unkBC;
    Vec3f m_unkC8;
    u16 m_unkD4[4];
    float m_lr;            // 0xDC: 1 when reset; HYPOTHESIS: the way (+1 / -1) to walk to the goal (aiInput::md_hammer)
    // The goal that set_goal_inf records: a position and the floor blocks of the goal and of the way there.
    Vec2f m_goalPos;       // 0xE0
    Vec2f m_goalEdgeL;     // 0xE8: the goal block's walkable ends
    Vec2f m_goalEdgeR;     // 0xF0
    u32 m_goalJointId;     // 0xF8: the goal block's joint (tag_AiClJointList::m_id), 0 = static floor or none
    u32 m_goalUnk48;       // 0xFC: the goal block's AiClBlock::m_unk48
    u32 m_viaJointId;      // 0x100: the same for the block on the way
    u32 m_viaUnk48;        // 0x104
    int m_unk108;
    int m_unk10C;
    float m_unk110;

    aiAct(aiInput* input, aiStat* stat);
    ~aiAct();
    void act_change(int id, void* target, int unk1, int unk2);
    void script_reload(void* script);
    void set_id(u16 id);
    // HYPOTHESIS: the id of an action of the requested kind (the act_* handlers pass a kind and three zeros); the
    // parameter types are guesses from the register use.
    u16 get_c_req_id(int kind, int unk1, int unk2, int unk3);
    // HYPOTHESIS: the parameter order; the blocks come in r4/r5 and the position in f1/f2.
    void set_goal_inf(AiClBlock* goal, AiClBlock* via, float x, float y);
    void setReflectDone();
    void addDefendAirRate(float amount);
    // The point to head back to the stage from (x, y). HYPOTHESIS: the flag's meaning (act passes id == 0x27).
    void get_return_goal(float* x, float* y, int unk);
    // A point to drop to: past the end of the hazardous floor the CPU stands on, or a random point on a safe floor
    // block (on the side away from where the target is heading).
    void get_safe_fall(float* x, float* y);
    // Decides once (until m_reflectUse is cleared) whether to use a reflector against weapon, by character kind and a
    // rank-scaled chance; returns the 0x20/0x40 bits. HYPOTHESIS: the meaning of the bits and of the kinds tested.
    u32 chkUseReflector(aiWeapon* weapon);
    // HYPOTHESIS: the act_* handlers take the stick and buttons to write, like aiInput's md_* handlers.
    // The fall-through jump down (the name from the map): drop through the floor, then steer to the get_safe_fall point.
    void act_r_dfjump(Vec2f* stick, ipPadButton* button);
    // After a meteor smash (the name from the map): ends action 0x10A0 once the target is in situation 4 or 6;
    // otherwise, when another opponent is within 30 of the CPU, switches aiInput to mode 1 on that one.
    void act_a_meteo_aft(Vec2f* stick, ipPadButton* button);
    // A jump after a shot (the name from the map): from rank 55, for an airborne CPU that has fired (aiInput
    // m_unk116 bits 1-2), presses special towards the nearest floor edge. HYPOTHESIS: the meaning of the tests.
    void act_r_jump_aft_shot(Vec2f* stick, ipPadButton* button);
    // Breaking a wall (the name from the map): face the target, or away from it when a wall blocks that side, walk
    // into the wall, then attack and pick the next action. HYPOTHESIS: the meaning of the tests.
    void act_a_brk_wall(Vec2f* stick, ipPadButton* button);
    // Sheik's forward throw on the ground (the name from the map): holds special while grounded (ranks below 50 may
    // give up), then either wiggles the stick at and around the target or circles it (step 10). HYPOTHESIS: the
    // meaning of the steps.
    void act_c_g_blow_f_sheik(Vec2f* stick, ipPadButton* button);
    // Strolling (the name from the map): walk towards a random point of the floor (m_unk0[0], a fraction of its
    // width) for a while (m_unk0[1] frames, rank-scaled), slower in some WarioWare microgames; ends early when an
    // opponent comes near. On Corneria it may instead jump off to the left (step 10). HYPOTHESIS: the meaning of the
    // tests and of m_unk0[0]/[1].
    void act_m_stroll(Vec2f* stick, ipPadButton* button);
};
static_assert(sizeof(aiAct) == 0x114, "Class is wrong size!");

int ai_act_id2cmd(aiAct::actId id);
