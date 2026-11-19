#include <ai/ai_act.h>
#include <ai/ai_ip_buf.h>
#include <ai/ai_param.h>
#include <mt/mt_prng.h>

// aiParam: reads the per-character AI tuning (sizes, jump reach, attack flags).

static inline int aiClampRank(int rank) {
    rank = (s8)rank > 0 ? rank : 0;
    int value = 100;
    if ((s8)rank < 100) {
        value = rank;
    }
    return value;
}

static inline float aiRankToRate(u8 rank) {
    float rate = (s8)rank - 20;
    rate *= 0.5f;
    if (rate < 0.0f) {
        rate = 0.0f;
    } else if (rate > 30.0f) {
        rate = 30.0f;
    }
    return rate;
}

aiParam::aiParam() {
    m_par = NULL;
    m_atkData = NULL;
    m_rank = aiClampRank(100);
    m_rankRate = aiRankToRate(m_rank);
    m_disableCmds = 0;
}

void aiParam::setRank(int rank, u32 force, int update) {
    if ((s8)m_rank != (s8)rank || force == 1) {
        int r = (s8)rank > 0 ? rank : 0;
        m_rank = (s8)r < 100 ? r : 100;
        m_rankRate = aiRankToRate(m_rank);
        if (m_par != NULL && update != 0) {
            par_work_update();
        }
    }
}

// HYPOTHESIS: aiParam::chk_situation (condition id, own and target character), still unnamed.
extern "C" int fn_27_21E24C(aiParam*, u8, aiChrIdx*, aiChrIdx*);

int aiParam::select_situation(int start, int end, aiChrIdx* chr, aiChrIdx* target) {
    int i;
    int skip = 0;
    for (i = start; i < end; i++) {
        u8* entry = (u8*)m_par + m_par->m_situationList[i];
        if (skip > 0) {
            if (entry[2] & 8) {
                skip--;
            }
            if (skip > 0) {
                if (entry[3] == 0) {
                    skip++;
                }
                continue;
            }
        }
        int result;
        u8 flags;
        for (u32 j = 0; j < 2; j++) {
            result = fn_27_21E24C(this, entry[j], chr, target);
            if (j == 0) {
                flags = entry[2];
                if (flags & 1) {
                    result = !result;
                }
            } else {
                flags = entry[2];
                if (flags & 2) {
                    result = !result;
                }
            }
            if (result) {
                if (flags & 4) {
                    break;
                }
            } else if (!(flags & 4)) {
                break;
            }
        }
        if (entry[3] == 0) {
            if (!result) {
                skip = 1;
            }
        } else if (result) {
            break;
        }
    }
    return i;
}

// Binds the parameter block of a character's AI pac. The attack data is only taken when its block starts with the
// "ATKD" tag.
void aiParam::init_par(aiParData* par, int kind, char* atkData) {
    if (kind == -1 || par == NULL) {
        m_par = NULL;
        m_atkData = NULL;
        return;
    }
    m_par = par;
    m_atkData = NULL;
    if (atkData != NULL && atkData[0] == 'A' && atkData[1] == 'T' && atkData[2] == 'K' && atkData[3] == 'D') {
        m_atkData = atkData;
        get_motnum_chr((ftKind)kind);
    }
    par_work_update();
}

bool aiParam::check_revision(char* data) {
    if (data != NULL && data[0] == 'A' && data[1] == 'I' && data[2] == 'P' && data[3] == 'D'
        && *(u32*)(data + 4) == 12) {
        return true;
    }
    return false;
}

float aiParam::get_down_atk_r(aiChrIdx* chr) {
    if (!aiChrIdx::IsPlayer(chr)) {
        return aiMgr::getAiReferPar(0)->m_downAtkRate;
    }
    return aiMgr::getAiReferPar(aiMgr::get_aiStat(chr)->m_chrKind)->m_downAtkRate;
}

u8 aiParam::get_down_atk_f(aiChrIdx* chr) {
    if (!aiChrIdx::IsPlayer(chr)) {
        return aiMgr::getAiReferPar(0)->m_downAtkFlag;
    }
    return aiMgr::getAiReferPar(aiMgr::get_aiStat(chr)->m_chrKind)->m_downAtkFlag;
}

float aiParam::get_cliff_atk_r(aiChrIdx* chr) {
    if (!aiChrIdx::IsPlayer(chr)) {
        return aiMgr::getAiReferPar(0)->m_cliffAtkRate;
    }
    return aiMgr::getAiReferPar(aiMgr::get_aiStat(chr)->m_chrKind)->m_cliffAtkRate;
}

u8 aiParam::get_cliff_atk_f(aiChrIdx* chr) {
    if (!aiChrIdx::IsPlayer(chr)) {
        return aiMgr::getAiReferPar(0)->m_cliffAtkFlag;
    }
    return aiMgr::getAiReferPar(aiMgr::get_aiStat(chr)->m_chrKind)->m_cliffAtkFlag;
}

float aiParam::get_width(aiChrIdx* chr) {
    if (aiChrIdx::IsPlayer(chr)) {
        float scale = aiMgr::get_aiStat(chr)->m_scale.m_x;
        aiReferPar* par = aiMgr::getAiReferPar(aiMgr::get_aiStat(chr)->m_chrKind);
        return par->m_width * scale;
    }
    float scale = aiMgr::get_aiStat(chr)->m_scale.m_x;
    aiReferPar* par = aiMgr::getAiReferPar(0);
    return par->m_width * scale;
}

float aiParam::get_height(aiChrIdx* chr) {
    if (aiChrIdx::IsPlayer(chr)) {
        float scale = aiMgr::get_aiStat(chr)->m_scale.m_y;
        aiReferPar* par = aiMgr::getAiReferPar(aiMgr::get_aiStat(chr)->m_chrKind);
        return par->m_height * scale;
    }
    float scale = aiMgr::get_aiStat(chr)->m_scale.m_y;
    aiReferPar* par = aiMgr::getAiReferPar(0);
    return par->m_height * scale;
}

float aiParam::get_air_high(aiChrIdx* chr) {
    aiStat* stat = aiMgr::get_aiStat(chr);
    float high;
    if (!aiChrIdx::IsPlayer(chr)) {
        high = (stat->m_scale.m_y * 0.8f + 0.2f) * (aiMgr::getAiReferPar(0)->m_airHigh - 3.0f);
    } else {
        high = (stat->m_scale.m_y * 0.8f + 0.2f) * (aiMgr::getAiReferPar(stat->m_chrKind)->m_airHigh - 3.0f);
    }
    return high;
}

float aiParam::get_air_length(aiChrIdx* chr) {
    aiStat* stat = aiMgr::get_aiStat(chr);
    float length;
    if (!aiChrIdx::IsPlayer(chr)) {
        length = (stat->m_scale.m_x * 0.8f + 0.2f) * (aiMgr::getAiReferPar(0)->m_airLength - 3.0f);
    } else {
        length = (stat->m_scale.m_x * 0.8f + 0.2f) * (aiMgr::getAiReferPar(stat->m_chrKind)->m_airLength - 3.0f);
    }
    return length;
}

float aiParam::get_jump_high(aiJumpPar* par, aiStat* stat) {
    return par->m_jumpHigh * (stat->m_scale.m_y * 0.8f + 0.2f);
}

float aiParam::get_jump_high_sp(aiJumpPar* par, aiStat* stat) {
    return par->m_jumpHighSp * (stat->m_scale.m_y * 0.8f + 0.2f);
}

float aiParam::get_jump_length(aiJumpPar* par, aiStat* stat) {
    return par->m_jumpLength * (stat->m_scale.m_x * 0.8f + 0.2f);
}

void aiParam::set_disable_cmd(int cmd) {
    if ((u32)cmd >= 0x16) {
        return;
    }
    m_disableCmds |= 1 << cmd;
}

// HYPOTHESIS: inverse of ai_act_id2cmd, inlined by the callers (cmd 0-12 -> 0x6031.., 13-21 -> 0x6034..).
static inline int ai_cmd2act_id(int cmd) {
    if (cmd >= 0) {
        if (cmd < 13) {
            return cmd + 0x6031;
        }
        if (cmd < 0x16) {
            return cmd + 0x6034;
        }
    }
    return 0;
}

u16 aiParam::get_act_list_id(int list, u8* hold, u8* mode, u8* param, bool unk) {
    if (list >= 0 && (u32)list < NumActLists && m_par->m_actList[list] != 0) {
        tag_AI_PAR_ACT_LIST* act = (tag_AI_PAR_ACT_LIST*)((char*)m_par + m_par->m_actList[list]);
        int num = 0;
        float total = 0.0f;
        float highRate = 0.01f * m_rank;
        float lowRate = 1.0f - highRate;
        // BYTES: each loop is written out twice, with and without the m_disableCmds test (the original has both copies).
        if (m_disableCmds != 0) {
            for (; act->m_cmd != 0; act++) {
                if ((m_disableCmds & (1 << (act->m_cmd - 1))) == 0) {
                    num++;
                    total += act->m_ratioHigh * highRate + act->m_ratioLow * lowRate;
                }
            }
        } else {
            for (; act->m_cmd != 0; act++) {
                num++;
                total += act->m_ratioHigh * highRate + act->m_ratioLow * lowRate;
            }
        }
        if (num > 0) {
            total *= randf();
            act = (tag_AI_PAR_ACT_LIST*)((char*)m_par + m_par->m_actList[list]);
            if (m_disableCmds != 0) {
                for (; act[1].m_cmd != 0; act++) {
                    if ((m_disableCmds & (1 << (act->m_cmd - 1))) == 0) {
                        total -= act->m_ratioHigh * highRate + act->m_ratioLow * lowRate;
                        if (total <= 0.0f) {
                            break;
                        }
                    }
                }
            } else {
                for (; act[1].m_cmd != 0; act++) {
                    total -= act->m_ratioHigh * highRate + act->m_ratioLow * lowRate;
                    if (total <= 0.0f) {
                        break;
                    }
                }
            }
            u8 cmd = act->m_cmd - 1;
            if (cmd < 0x16) {
                if (hold != NULL && mode != NULL) {
                    calc_command_param(hold, mode, param, cmd);
                }
                m_disableCmds = 0;
                return ai_cmd2act_id(cmd);
            }
        }
    }
    m_disableCmds = 0;
    return 0;
}

u16 aiParam::enum_act_list_id(int list, int n) {
    if (list >= 0 && (u32)list < NumActLists && m_par->m_actList[list] != 0) {
        tag_AI_PAR_ACT_LIST* act = (tag_AI_PAR_ACT_LIST*)((char*)m_par + m_par->m_actList[list]);
        float highRate = 0.01f * m_rank;
        float lowRate = 1.0f - highRate;
        for (; act->m_cmd != 0; act++) {
            if ((m_disableCmds & (1 << (act->m_cmd - 1))) == 0
                && (int)(act->m_ratioHigh * highRate + act->m_ratioLow * lowRate) > 0
                && --n < 0) {
                break;
            }
        }
        if (n < 0) {
            u8 cmd = act->m_cmd - 1;
            if (cmd < 0x16) {
                return ai_cmd2act_id(cmd);
            }
        }
    }
    return 0;
}

bool aiParam::chkCmdRevise(int cmd) {
    return m_par->m_reviseCmds & (1 << cmd);
}

bool aiParam::chkCmdFall(int cmd) {
    return m_par->m_fallCmds & (1 << cmd);
}

bool aiParam::chkCmdReflect(int cmd) {
    return m_par->m_reflectCmds & (1 << cmd);
}

void aiParam::calc_command_param(u8* hold, u8* mode, u8* param, int cmd) {
    u32 bit = 1 << cmd;
    if ((m_par->m_holdModeCmds & bit) || (m_par->m_modeCmds & bit)) {
        *mode = 3;
    } else {
        *mode = 0;
    }
    if ((m_par->m_randHoldCmds & bit) || (m_par->m_holdModeCmds & bit)) {
        switch (cmd) {
        case 4:
        case 5:
        case 6:
            if (randf() < 0.9f) {
                *hold = 10.0f * randf();
            } else {
                *hold = 10.0f + 50.0f * randf();
            }
            break;
        default:
            if (randf() < 0.4f) {
                *hold = 20.0f * randf();
            } else {
                *hold = 15.0f + 45.0f * randf();
            }
            break;
        }
        if ((m_par->m_randHoldCmds & bit) && randf() < 0.8f) {
            *mode = 0;
        }
    } else if (m_par->m_modeCmds & bit) {
        *hold = *mode + 7;
    } else {
        *hold = 0;
    }
    if (param != NULL) {
        if (cmd >= 0 && cmd < 22) {
            *param = m_par->m_cmdParam[cmd];
        } else {
            *param = 0;
        }
    }
}

u8 tag_AI_PAR_ACT::getRatio(s8 rank, float adjust) {
    int ratio;
    if (rank >= m_rankHigh) {
        ratio = m_ratioHigh;
    } else if (rank <= m_rankLow) {
        ratio = m_ratioLow;
    } else {
        float t = (float)(rank - m_rankLow) / (float)(m_rankHigh - m_rankLow);
        float rest = 1.0f - t;
        ratio = t * m_ratioHigh + rest * m_ratioLow;
    }
    if (adjust > 0.0001f) {
        if (ratio < 100) {
            ratio += (int)((100 - ratio) * adjust);
        } else {
            ratio += (int)((255 - ratio) * adjust);
        }
    } else if (adjust < -0.0001f) {
        ratio += (int)(ratio * adjust);
    }
    if (ratio < 0) {
        ratio = 0;
    } else if (ratio > 255) {
        ratio = 255;
    }
    return ratio;
}

int ai_act_id2cmd(aiAct::actId id) {
    if (id > 0x6030) {
        if (id < 0x6040) {
            return id - 0x6031;
        }
        int cmd = id - 0x6034;
        if (cmd < 0x16) {
            return cmd;
        }
    }
    return 0;
}

tag_AI_ATK_DATA* tag_AI_ATK_DATA_HED::searchAtkData(int id) {
    for (u32 i = 0; i < m_num; i++) {
        if (id == m_data[i].m_id) {
            return &m_data[i];
        }
    }
    return NULL;
}

int get_motnum_chr(ftKind kind) {
    switch (kind) {
    case Fighter_Mario:
        return 0x1DE;
    case Fighter_DonkeyKong:
        return 0x204;
    case Fighter_Link:
        return 0x1E4;
    case Fighter_Samus:
        return 0x1EC;
    case Fighter_Yoshi:
        return 0x1E8;
    case Fighter_Kirby:
        return 0x31D;
    case Fighter_Fox:
        return 0x1F2;
    case Fighter_Pikachu:
        return 0x1E6;
    case Fighter_Luigi:
        return 0x1E2;
    case Fighter_CaptainFalcon:
        return 0x1EB;
    case Fighter_Ness:
        return 0x1EA;
    case Fighter_Bowser:
        return 0x1E3;
    case Fighter_Peach:
        return 0x1E1;
    case Fighter_Zelda:
        return 0x1E0;
    case Fighter_Sheik:
        return 0x1E6;
    case Fighter_Popo:
    case Fighter_Nana:
        return 0x1F2;
    case Fighter_Marth:
        return 0x1F5;
    case Fighter_MrGameAndWatch:
        return 0x202;
    case Fighter_Falco:
        return 0x1EC;
    case Fighter_Ganondorf:
        return 0x1E8;
    case Fighter_Wario:
        return 0x213;
    case Fighter_MetaKnight:
        return 0x1EC;
    case Fighter_Pit:
        return 0x1F4;
    case Fighter_ZeroSuitSamus:
        return 0x1EB;
    case Fighter_Olimar:
        return 0x1E0;
    case Fighter_Lucas:
        return 0x1EB;
    case Fighter_DiddyKong:
        return 0x1FB;
    case Fighter_Charizard:
        return 0x1DD;
    case Fighter_Squirtle:
        return 0x1E4;
    case Fighter_Ivysaur:
        return 0x1DD;
    case Fighter_KingDedede:
        return 0x217;
    case Fighter_Lucario:
        return 0x1EB;
    case Fighter_Ike:
        return 0x1F4;
    case Fighter_ROB:
        return 0x1E7;
    case Fighter_Jigglypuff:
        return 0x1F5;
    case Fighter_ToonLink:
        return 0x1E4;
    case Fighter_Wolf:
        return 0x1EE;
    case Fighter_Snake:
        return 0x1FE;
    case Fighter_Sonic:
        return 0x1F1;
    case Fighter_GigaBowser:
        return 0x1E4;
    case Fighter_WarioMan:
        return 0x213;
    case Fighter_Alloy_Red:
        return 0x1CF;
    case Fighter_Alloy_Blue:
        return 0x1CF;
    case Fighter_Alloy_Yellow:
        return 0x1CF;
    case Fighter_Alloy_Green:
        return 0x1CF;
    default:
        return -1;
    }
}

// aiIpBuf: a 16-frame history of the stick and buttons a CPU character pressed.

aiIpBuf::aiIpBuf() {
    for (int i = 0; i < 16; i++) {
        m_stick[i] = Vec2f(0.0f, 0.0f);
        m_button[i] = 0;
    }
    m_index = 0;
}

void aiIpBuf::update(Vec2f* stick, u32* button) {
    m_index++;
    if (m_index >= 16) {
        m_index = 0;
    }
    m_stick[m_index] = *stick;
    m_button[m_index] = *button;
}

void aiIpBuf::getData(int framesBack, Vec2f* stick, u32* button) {
    if (framesBack < 0) {
        framesBack = 0;
    } else if (framesBack >= 16) {
        framesBack = 15;
    }
    framesBack = m_index - framesBack;
    if (framesBack < 0) {
        framesBack += 16;
    }
    if (stick != NULL) {
        *stick = m_stick[framesBack];
    }
    if (button != NULL) {
        *button = m_button[framesBack];
    }
}

Vec2f aiIpBuf::getStick(int framesBack) {
    Vec2f stick;
    getData(framesBack, &stick, NULL);
    return stick;
}

u32 aiIpBuf::getButton(int framesBack) {
    u32 button;
    getData(framesBack, NULL, &button);
    return button;
}

#include <ai/ai_mgr.h>
#include <nw4r/ut/ut_algorithm.h>
// TODO: clean up, give variables real names
namespace jj_21E24C {

struct Environment {
    float f00, f04, f08, f0C;
    char pad10[8];
    float f18, f1C, f20, f24, f28;
};

struct FighterState {
    char pad00[0x50];
    unsigned int flags;
    char pad54[8];
    Vec3f position;
    float f68, f6C;
    char pad70[0x10];
    float f80, f84;
    char pad88[0x18];
    float fA0;
    char padA4[4];
    short sA8;
    char padAA[2];
    int action;
    char padB0[0xC];
    Environment* environment;
    char padC0[0xFA];
    unsigned char status;
};

struct Parameters {
    char pad00[0x14];
    float f14, f18;
    char pad1C[4];
    float f20, f24;
};

extern "C" {
extern const float lbl_27_rodata_3290[];
FighterState* get_aiStat__5aiMgrFP8aiChrIdx(s8*);
int chk_over_ground__6aiStatFv(FighterState*);
int chk_over_ground1__6aiStatFP8aiChrIdx(FighterState*, s8*);
int chk_chr_stat__6aiStatFUli(FighterState*, unsigned int, int);
float get_range__6aiStatFfP8aiChrIdx(FighterState*, s8*, float);
float get_distance__6aiStatFfP8aiChrIdx(FighterState*, s8*, float);
float get_range_floor__6aiStatFfP5Vec3f(FighterState*, Vec3f*, float);
int chk_goto_blk__5aiMgrFP9AiClBlockP9AiClBlockifff(aiMgr*, Environment*, Environment*, int, float, float, float);
int chkChildOptionMd__5aiMgrFi(aiMgr*, int);
int fn_27_21E24C(Parameters*, unsigned int, s8*, s8*);
}

extern "C" int fn_27_21E24C(
    Parameters* p, unsigned int kind, s8* selfId, s8* targetId) {
    const float* c = lbl_27_rodata_3290;
    FighterState* self = get_aiStat__5aiMgrFP8aiChrIdx(selfId);
    int result = 0;
    FighterState* target = get_aiStat__5aiMgrFP8aiChrIdx(targetId);
    Vec3f position = target->position;
    float x = get_range__6aiStatFfP8aiChrIdx(self, targetId, c[1]);
    float y = get_distance__6aiStatFfP8aiChrIdx(self, targetId, c[1]);

    switch (kind) {
    case 0:
        result = 1;
        break;

    case 1:
        if (chk_over_ground__6aiStatFv(target) == 0 && target->status != 8) {
            Environment* e = self->environment;
            if (!(target->position.m_x > c[9] + e->f18) ||
                !(target->position.m_x < e->f20 - c[9]) ||
                !(target->position.m_y < e->f28 - c[9]))
                result = 1;
        }
        break;

    case 2: {
        float dy = position.m_y - self->position.m_y;
        if (dy > c[7]) {
            if (dy > c[2] && dy < c[17])
                result = 1;
            else if (dy > c[18] && (target->flags & 1) && target->status == 5)
                result = 1;
            else if (target->environment != self->environment) {
                if (!(target->flags & 1))
                    result = 1;
                else if (*(int*)((char*)g_aiMgr + 0x370) == 5 &&
                         position.m_y > c[10])
                    result = 1;
            }
        }
        break;
    }

    case 3:
        if (!(self->flags & 1) &&
            target->environment != self->environment &&
            position.m_y - self->position.m_y < c[19] &&
            chk_over_ground__6aiStatFv(target) != 0 &&
            (*(int*)((char*)g_aiMgr + 0x370) != 5 ||
             !(self->position.m_y > c[10]))) {
            if (position.m_y - self->position.m_y < c[20])
                result = 1;
            else if (!(target->flags & 1)) {
                float height = p->f20 * (c[16] + c[21] * self->f6C);
                float width = p->f24 * (c[16] + c[21] * self->f68);
                if (chk_goto_blk__5aiMgrFP9AiClBlockP9AiClBlockifff(
                        g_aiMgr, self->environment, target->environment, -1,
                        self->position.m_x, width, height))
                    result = 1;
            }
        }
        break;

    case 4:
        if (x >= p->f14)
            result = 1;
        break;

    case 5:
        if (x < p->f14 && x >= p->f18)
            result = 1;
        break;

    case 6:
        if (x < p->f18 && x >= c[1])
            result = 1;
        break;

    case 19: {
        float range = p->f18;
        if (x < c[0] * range && x >= c[1])
            result = 1;
        break;
    }

    case 20: {
        float range = p->f18;
        if (nw4r::ut::Abs(x) < c[0] * range)
            result = 1;
        break;
    }

    case 7:
        if (x < c[1])
            result = 1;
        break;

    case 8:
        if (y >= p->f14)
            result = 1;
        break;

    case 23: {
        float range = p->f18;
        if (y < c[0] * range)
            result = 1;
        break;
    }

    case 9:
        if (self->environment == target->environment)
            result = 1;
        break;

    case 10:
        if (target->status == 4 || target->status == 6)
            result = 1;
        break;

    case 26:
        if (target->status == 5)
            result = 1;
        break;

    case 27:
        if (target->status == 14)
            result = 1;
        break;

    case 11:
        if (self->sA8 > 0 && self->status != 14)
            result = 1;
        break;

    case 12:
        if (self->action == 0 &&
            chk_chr_stat__6aiStatFUli(self, 1, -1) == 0 &&
            chk_over_ground__6aiStatFv(self) == 0 &&
            self->status != 14)
            result = 1;
        break;

    case 13:
        if (self->flags & 4)
            result = 1;
        break;

    case 14:
        switch (self->action) {
        case 29:
        case 30:
        case 31:
            if (chk_chr_stat__6aiStatFUli(self, 1, -1))
                result = 1;
            break;
        }
        break;

    case 15:
        if (chk_chr_stat__6aiStatFUli(self, 1, 27) == 0)
            result = 1;
        break;

    case 16:
        switch (self->action) {
        case 22:
            if (chk_chr_stat__6aiStatFUli(self, 1, -1) == 0)
                result = 1;
            break;
        case 23:
            if (chk_chr_stat__6aiStatFUli(self, 1, -1) == 0)
                result = 1;
            break;
        case 29:
            if (chk_chr_stat__6aiStatFUli(self, 2, -1) == 0)
                result = 1;
            break;
        }
        break;

    case 17:
        if ((self->flags & 1) && self->f84 < c[1])
            result = 1;
        break;

    case 18:
        if (self->flags & 1)
            result = 1;
        break;

    case 21:
        if (chk_over_ground1__6aiStatFP8aiChrIdx(self, selfId) == 0) {
            float value = get_range_floor__6aiStatFfP5Vec3f(self, &self->position, self->fA0);
            if (value > c[22] && value < c[23]) {
                Environment* e = self->environment;
                if (self->position.m_y > e->f0C - c[9] &&
                    ((self->position.m_x < e->f00 && self->f80 > c[24]) ||
                     (self->position.m_x > e->f08 && self->f80 < c[0])))
                    result = 1;
            }
        }
        break;

    case 22:
        if (chk_chr_stat__6aiStatFUli(self, 1, 12))
            result = 1;
        break;

    case 24:
        if (fn_27_21E24C(p, 18, selfId, targetId) == 0 &&
            fn_27_21E24C(p, 1, selfId, targetId) == 0 &&
            fn_27_21E24C(p, 4, selfId, targetId) == 0 &&
            fn_27_21E24C(p, 2, selfId, targetId) != 0 &&
            fn_27_21E24C(p, 9, selfId, targetId) != 0)
            result = 1;
        break;

    case 25:
        if (chk_chr_stat__6aiStatFUli(self, 8, 46))
            result = 1;
        break;

    case 28:
        if (chkChildOptionMd__5aiMgrFi(g_aiMgr, *selfId))
            result = 1;
        break;

    case 29:
        if (chk_chr_stat__6aiStatFUli(self, 4, 27) == 0)
            result = 1;
        break;
    }

    return result;
}
}

// TODO: clean up, give variables real names
namespace jj_21E9BC {

typedef struct FState {
    u8 pad0[0x50];
    u32 unk50;
    u8 pad54[0x8];
    f32 unk5C;
    f32 unk60;
    u8 pad64[0x44];
    s16 unkA8;
    u8 padAA[0x2];
    s32 unkAC;
    u8 padB0[0x4];
    s32 unkB4;
    u8 padB8[0x4];
    void* unkBC;
    u8 padC0[0x1C];
    f32 unkDC;
    u8 padE0[0xDA];
    u8 unk1BA;
} FState;

extern "C" {
FState* get_aiStat__5aiMgrFP8aiChrIdx(s8* fighterId);
s32 IsPlayer__8aiChrIdxFPC8aiChrIdx(s8* fighterId);
s32 chk_over_ground__6aiStatFv(FState* st);
s32 chk_chr_stat__6aiStatFUli(FState* st, u32 a, s32 b);
f32 get_range__6aiStatFfP8aiChrIdx(FState* st, s8* id, f32 f);
f32 get_lr_tgt__6aiStatFP8aiChrIdx(FState* st, s8* id);
f32 get_range_floor__6aiStatFfP5Vec3f(FState* st, void* vec, f32 f);
u16 enum_act_list_id__7aiParamFii(void* a, s32 b, s32 c);
}

extern "C" const float lbl_27_rodata_3290[];

extern "C" s32 fn_27_21E9BC(FState* p, void* arg1, s8* arg2, s32 script, u32 flags, void* arg5) {
    const f32* c;
    FState* s;
    f32 f31;
    f32 t;
    f32* bc;
    u32 f50;
    u8 b8;

    c = lbl_27_rodata_3290;
    s = get_aiStat__5aiMgrFP8aiChrIdx(arg2);

    if (!(flags & 1)) {
        switch (script & 0x7FF0) {
        case 0x1090:
            return 1;
        }
    }
    if (flags & 2) goto L_EA9C;
    if ((u32)script == 0x603B) {
        if (p->unkAC == 0x1D || p->unkAC == 0x1E || p->unkAC == 0x1F) goto L_EAE8;
    }
    if (((u32)script & 0xF000) == 0x1000 || ((u32)script & 0xF000) == 0x6000) return 1;
    switch (script & 0x7FF0) {
    case 0x2020:
    case 0x2030:
    case 0x3030:
        return 1;
    }
    goto L_EAE8;

L_EA9C:
    if (flags & 4) goto L_EAC8;
    switch (script & 0x7FF0) {
    case 0x1070:
    case 0x1080:
        return 1;
    }
L_EAC8:
    if (flags & 0x10) goto L_EAE8;
    switch (script & 0x7FF0) {
    case 0x1050:
        return 1;
    }

L_EAE8:
    if (flags & 8) goto L_EB08;
    switch (script & 0x7FF0) {
    case 0x1070:
        return 1;
    }

L_EB08:
    if (p->unkA8 > 0) goto L_EB44;
    switch (script & 0x7FF0) {
    case 0x50:
    case 0x80:
    case 0x1070:
        return 1;
    }

L_EB44:
    if (flags & 0x20) {
        if (!(p->unkDC < c[25])) {
            b8 = s->unk1BA;
            if (b8 != 9 && (u8)(b8 + 0xFC) > 1) goto L_EB90;
        }
    }
    switch (script & 0x7FF0) {
    case 0x3020:
        return 1;
    }

L_EB90:
    if (flags & 0x200) goto L_EBB0;
    switch (script & 0x7FF0) {
    case 0x70:
        return 1;
    }

L_EBB0:
    f50 = p->unk50;
    if ((f50 & 0x80) || (f50 & 0x100)) {
        switch (script) {
        case 0x21:
        case 0x31:
        case 0x55:
        case 0x1040:
        case 0x1050:
        case 0x10D0:
        case 0x3030:
            return 1;
        }
    }

    if (f50 & 1) {
        switch (script & 0x7FF0) {
        case 0x20:
        case 0x30:
        case 0x40:
        case 0x70:
        case 0x10A0:
        case 0x3010:
        case 0x3020:
        case 0x3030:
        case 0x3040:
        case 0x3050:
            return 1;
        }
    }

L_ECAC:
    if (chk_over_ground__6aiStatFv(p) == 0) {
        switch (script) {
        case 0x10A0:
            return 1;
        case 0x1140:
            if (p->unkA8 < 1) return 1;
            break;
        case 0x2020:
            bc = (f32*)p->unkBC;
            if (p->unk5C < bc[0]) {
                if (p->unk60 < c[9] + bc[1]) return 1;
            } else {
                if (p->unk60 < c[9] + bc[3]) return 1;
            }
            break;
        }
    }

    if (chk_over_ground__6aiStatFv(s) == 0) {
        switch (script) {
        case 0x80:
            return 1;
        }
    }
    if (s->unkB4 == 0xBE) {
        if (((u32)script & 0xF000) == 0x1000) return 1;
        switch (script & 0x7FF0) {
        case 0x50:
            return 1;
        }
    }
    if (chk_chr_stat__6aiStatFUli(p, 2, 0x12) != 0) {
        switch (script) {
        case 0x1140:
        case 0x10A0:
            return 1;
        }
    }
    if (IsPlayer__8aiChrIdxFPC8aiChrIdx(arg2) == 0) {
        switch (script) {
        case 0x3080:
        case 0x10F0:
        case 0x1060:
        case 0x30B0:
        case 0x3090:
            return 1;
        }
        if (*(s32*)((u8*)g_aiMgr + 0x370) == 0x1C) {
            switch (script) {
            case 0x3020:
            case 0x1110:
            case 0x10A0:
            case 0x71:
            case 0x72:
            case 0x3010:
            case 0x1160:
            case 0x3050:
            case 0x3040:
            case 0x3030:
            case 0x30A0:
            case 0x3060:
                return 1;
            }
        }
    }

    switch (script) {
    case 0x21:
    case 0x31:
        goto L_EF80;
    case 0x22:
    case 0x32:
        goto L_EF34;
    case 0x10F0:
        goto L_EFB0;
    case 0x3030:
        goto L_F010;
    case 0x3060:
        goto L_F024;
    default:
        goto L_F038;
    }

L_EF34:
    f31 = *(f32*)((u8*)arg1 + 0x18);
    t = get_range__6aiStatFfP8aiChrIdx(p, arg2, c[1]);
    if (t > f31) goto L_F038;
    f31 = *(f32*)((u8*)arg1 + 0x18);
    t = get_range_floor__6aiStatFfP5Vec3f(p, (u8*)p + 0x5C, -get_lr_tgt__6aiStatFP8aiChrIdx(p, arg2));
    if (t < f31) return 1;
    goto L_F038;

L_EF80:
    f31 = *(f32*)((u8*)arg1 + 0x14);
    t = get_range_floor__6aiStatFfP5Vec3f(p, (u8*)p + 0x5C, -get_lr_tgt__6aiStatFP8aiChrIdx(p, arg2));
    if (t < f31) return 1;
    goto L_F038;

L_EFB0:
    if (!(s->unk50 & 0x20000)) {
        if (s->unk1BA != 6 || enum_act_list_id__7aiParamFii(arg1, 9, 0) == 0) return 1;
    }
    if (arg5 == 0) goto L_F038;
    if ((u32)script == *(u16*)((u8*)arg5 + 0x78) || (u32)script == *(u16*)((u8*)arg5 + 0x7C)) return 1;
    goto L_F038;

L_F010:
    if (p->unk50 & 4) return 1;
    goto L_F038;

L_F024:
    if (p->unkB4 == 0x49) return 1;

L_F038:
    return 0;
}
}

