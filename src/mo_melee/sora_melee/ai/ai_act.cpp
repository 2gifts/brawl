#include <ai/ai_act.h>
#include <ai/ai_cl_block.h>
#include <ai/ai_input.h>
#include <ai/ai_mgr.h>
#include <ai/ai_stat.h>
#include <mt/mt_prng.h>
#include <math.h>
#include <so/controller/so_controller_impl.h>

extern "C" {
// The runtime's null pointer-to-member constant (ptmf.c). HYPOTHESIS: m_unkB0/BC/C8 are member-function
// pointers reset to null; they are still typed as Vec3f here.
extern const Vec3f __ptmf_null;
}

aiAct::aiAct(aiInput* input, aiStat* stat) {
    for (int i = 0; i < 24; i++) {
        m_unk0[i] = 0.0f;
    }
    for (int i = 0; i < 4; i++) {
        m_unkD4[i] = 0;
    }
    m_input = input;
    m_stat = stat;
    m_id = m_prevId = m_unk7A = m_nextId = 0;
    m_unk80 = 0;
    m_script = m_unk64 = NULL;
    m_unk68 = m_unk6C = NULL;
    m_unk81 = 0;
    m_unk82 = 0;
    m_unk84 = 0.0f;
    m_unk88 = 0.0f;
    m_unk8C = 0.0f;
    m_unk90 = 0;
    m_unk91 = 0;
    m_unk92 = 0;
    m_unk93 = 0;
    m_unk94 = 0;
    m_unk98 = -1.0f;
    m_reflectDone = 0;
    m_reflectRate = 1.0f;
    m_defendAirRate = 0.0f;
    m_step = 0;
    m_stepFrame = 0;
    m_unkB0 = m_unkBC = m_unkC8 = __ptmf_null;
    m_lr = 1.0f;
    m_goalPos.m_x = m_goalPos.m_y = m_goalEdgeL.m_x = m_goalEdgeL.m_y = m_goalEdgeR.m_x = m_goalEdgeR.m_y = 0.0f;
    m_goalUnk48 = m_viaUnk48 = m_unk10C = 0;
    m_goalJointId = m_viaJointId = m_unk108 = 0;
    m_unk110 = 0.0f;
}

void aiAct::script_reload(void* script) {
    s8 target[16];
    target[0] = -1;
    act_change(0, target, 0, 0);
    m_script = script;
}

aiAct::~aiAct() { }

void aiAct::set_id(u16 id) {
    if (m_id != 0) {
        m_prevId = m_id;
    }
    m_id = id;
    m_input->swingChkReset(0);
}

void aiAct::set_goal_inf(AiClBlock* goal, AiClBlock* via, float x, float y) {
    m_goalPos.m_x = x;
    m_goalPos.m_y = y;
    if (goal != NULL) {
        m_goalEdgeL = goal->m_edgeL;
        m_goalEdgeR = goal->m_edgeR;
        if (goal->m_joint == NULL) {
            m_goalJointId = 0;
            m_goalUnk48 = 0;
        } else {
            m_goalUnk48 = goal->m_unk48;
            m_goalJointId = goal->m_joint->m_id;
        }
    } else {
        m_goalJointId = 0;
    }
    if (via != NULL) {
        if (via->m_joint == NULL) {
            m_viaJointId = 0;
            m_viaUnk48 = 0;
        } else {
            m_viaUnk48 = via->m_unk48;
            m_viaJointId = via->m_joint->m_id;
        }
    } else {
        m_viaJointId = 0;
    }
}

void aiAct::get_safe_fall(float* x, float* y) {
    if (m_stat->m_unkC0 == 1 && (m_stat->m_floor->m_flags & 0x10000)) {
        if (m_stat->m_speed.m_x > 0.0f) {
            *x = m_stat->m_floor->m_edgeR.m_x + 30.0f;
            *y = m_stat->m_floor->m_edgeR.m_y + 15.0f;
        } else {
            *x = m_stat->m_floor->m_edgeL.m_x - 30.0f;
            *y = m_stat->m_floor->m_edgeL.m_y + 15.0f;
        }
        return;
    }
    AiClBlock* floor = g_aiMgr->get_safe_floor(m_input, m_stat);
    aiStat* target = aiMgr::get_aiStat(&m_input->m_target);
    float rate = 0.2f + 0.15f * randf();
    if (floor == NULL) {
        floor = m_stat->m_floor;
    }
    float center = 0.5f * (floor->m_edgeL.m_x + floor->m_edgeR.m_x);
    float targetX = target->m_pos.m_x + 15.0f * target->m_speed.m_x;
    if (targetX > 30.0f + center) {
        *x = -1.0f;
    } else if (targetX < center - 30.0f) {
        *x = 1.0f;
    } else {
        *x = randf() - 0.5f;
    }
    if (*x < 0.0f) {
        *x = floor->m_edgeR.m_x - floor->m_edgeL.m_x;
        *y = floor->m_edgeR.m_y - floor->m_edgeL.m_y;
        *x *= rate;
        *y *= rate;
        *x += floor->m_edgeL.m_x;
        *y += floor->m_edgeL.m_y;
    } else {
        *x = floor->m_edgeL.m_x - floor->m_edgeR.m_x;
        *y = floor->m_edgeL.m_y - floor->m_edgeR.m_y;
        *x *= rate;
        *y *= rate;
        *x += floor->m_edgeR.m_x;
        *y += floor->m_edgeR.m_y;
    }
}

void aiAct::setReflectDone() {
    m_reflectDone = 180;
    m_reflectRate *= 0.85f;
}

void aiAct::addDefendAirRate(float amount) {
    m_defendAirRate = m_defendAirRate + amount;
}

tag_AI_SCRIPT* tag_AI_SCRIPT_PACK::get_script(tag_AI_SCRIPT_PACK* common, u32 id) {
    int i;
    tag_AI_SCRIPT* script;
    if ((id & 0xFFFF800F) != 0) {
        for (i = 0; i < 2; i++) {
            tag_AI_SCRIPT_PACK* pack = i == 0 ? this : common;
            if (pack != NULL) {
                for (u32 j = 0; j < pack->m_numScripts; j++) {
                    script = (tag_AI_SCRIPT*)((u8*)pack + pack->m_offsets[j]);
                    if (script != NULL && script->m_id == id) {
                        return script;
                    }
                }
            }
        }
    }
    u32 key = id & 0x7FF0;
    for (i = 0; i < 2; i++) {
        tag_AI_SCRIPT_PACK* pack = i == 0 ? this : common;
        if (pack != NULL) {
            for (u32 j = 0; j < pack->m_numScripts; j++) {
                script = (tag_AI_SCRIPT*)((u8*)pack + pack->m_offsets[j]);
                if (script != NULL && (script->m_id & ~0x8000) == key) {
                    return script;
                }
            }
        }
    }
    return NULL;
}

// Skips to the end of the conditional block that starts at this command: to its else (when this command is not an
// else itself) or to the end of the block, counting the nested blocks on the way.
tag_AI_SCRIPT_CMD* tag_AI_SCRIPT_CMD::if_skip() {
    tag_AI_SCRIPT_CMD* cmd = this;
    u8 first = cmd->m_type;
    u8 type;
    int depth = first == 8;
    u8 prev = 0;
    while (true) {
        type = cmd->m_type;
        if ((type == 6 || type == 7) && prev != 8) {
            depth++;
        }
        if (first != 8 && type == 8 && depth == 1) {
            return cmd;
        }
        if (type == 9 && --depth <= 0) {
            return cmd;
        }
        prev = type;
        cmd = (tag_AI_SCRIPT_CMD*)((u8*)cmd + cmd->m_size);
    }
}

void aiAct::act_r_dfjump(Vec2f* stick, ipPadButton* button) {
    if (!(m_stat->m_flags & 1)) {
        set_id(0);
        return;
    }
    if (m_step == 0) {
        if (m_stat->m_unkA8 == 0) {
            m_nextId = 0x2050;
            set_id(0);
            return;
        }
        m_input->m_unk6C = 2;
        m_unk94 = 40;
        m_step++;
    }
    switch (m_step) {
        case 1:
            get_safe_fall(&m_goalPos.m_x, &m_goalPos.m_y);
            m_step++;
            break;
    }
    stick->m_x = 0.0625f * (m_goalPos.m_x - m_stat->m_pos.m_x);
    switch (m_step) {
        case 2:
            if (m_stat->chk_transition(0x20000)) {
                stick->m_y = 1.0f;
                m_stepFrame = 0;
                m_step++;
            }
            break;
        case 3:
            stick->m_y = 1.0f;
            if (m_stepFrame > 4) {
                m_step++;
            }
            break;
        default:
            if (m_stat->m_speed.m_y < 0.0f) {
                m_nextId = 0x2050;
                set_id(0);
            }
            break;
    }
}

// The length of a vector from its squared length (0 for a denormal). HYPOTHESIS: the same inline as in ai_input.cpp.
static inline float aiLengthOf(float lengthSq) {
    float length;
    if ((float)fabs(lengthSq) <= 1.17549435e-38f) {
        length = 0.0f;
    } else {
        length = lengthSq * rsqrtf(lengthSq);
    }
    return length;
}

void aiAct::act_a_meteo_aft(Vec2f* stick, ipPadButton* button) {
    if (m_id == 0x10A0 && (aiMgr::get_aiStat(&m_input->m_target)->m_situation == 4 ||
                           aiMgr::get_aiStat(&m_input->m_target)->m_situation == 6)) {
        set_id(0);
        return;
    }
    int i;
    float nearestDist = 9999.9f;
    int nearest;
    for (i = 0; i < 7; i++) {
        aiChrIdx idx;
        idx.m_idx = i;
        aiStat* stat = aiMgr::get_aiStat(&idx);
        u32 flags = stat->m_flags;
        if ((flags & 0x8000) || stat->m_situation == 5 || stat->m_situation == 4 || (flags & 0x40000000) ||
            stat->m_situation == 9 || (flags & 0x400000) || stat->m_situation == 14) {
            continue;
        }
        if (stat->m_unkCC == m_stat->m_unkCC) {
            continue;
        }
        Vec3f diff = stat->m_pos - m_stat->m_pos;
        float lengthSq = diff.m_z * diff.m_z + (diff.m_x * diff.m_x + diff.m_y * diff.m_y);
        float dist = aiLengthOf(lengthSq);
        if (nearestDist > dist) {
            nearestDist = dist;
            nearest = i;
        }
    }
    if (nearestDist < 30.0f && nearest != m_input->m_target.m_idx) {
        aiChrIdx target;
        target.m_idx = nearest;
        m_input->change_md(1, &target, 0, 1);
        m_input->m_unk111 = 1;
        m_nextId = 1;
    }
}

void aiAct::act_r_jump_aft_shot(Vec2f* stick, ipPadButton* button) {
    if (button->m_mask != 0 || m_input->m_par.m_rank < 55 || !(m_input->m_unk116 & 6) ||
        !m_stat->chk_stat_air_free()) {
        return;
    }
    Vec2f edge = m_stat->getNearFloorEdge();
    switch (m_stat->m_chrKind) {
        case 0:
            if (m_stat->m_pos.m_y > edge.m_y - 10.0f && randf() < 0.015f) {
                stick->m_x = edge.m_x > m_stat->m_pos.m_x ? 1.0f : -1.0f;
                button->m_mask |= soController::getButtonMask(soController::Pad_Button_Special);
                return;
            }
            break;
    }
    if ((aiMgr::get_aiStat(&m_input->m_target)->m_flags & 0x800000) ||
        m_stat->m_lr * (edge.m_x - m_stat->m_pos.m_x) < 0.0f || m_stat->get_distance(0.0f, &m_input->m_target) < 60.0f) {
        return;
    }
    float height = 60.0f;
    switch (m_stat->m_chrKind) {
        case 0:
            height = 35.0f;
            break;
    }
    if (m_stat->m_pos.m_y < edge.m_y + height ||
        __fabsf(m_stat->m_pos.m_y - edge.m_y) < 0.9f * __fabsf(m_stat->m_pos.m_x - edge.m_x)) {
        return;
    }
    s8 rank = m_input->m_par.m_rank;
    if (randf() < 0.1f * (0.01f * rank)) {
        switch (m_stat->m_chrKind) {
            case 2:
            case 0x29:
                if (m_stat->chk_chr_stat(1, -1)) {
                    break;
                }
                stick->m_x = m_stat->m_lr;
                stick->m_y = 0.0f;
                button->m_mask |= soController::getButtonMask(soController::Pad_Button_Special);
                break;
            default:
                stick->m_x = stick->m_y = 0.0f;
                button->m_mask |= soController::getButtonMask(soController::Pad_Button_Special);
                break;
        }
    }
}
