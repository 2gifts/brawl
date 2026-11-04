#include <ai/ai_input.h>
#include <ai/ai_act.h>
#include <ai/ai_enemy.h>
#include <ai/ai_stat.h>
#include <ai/ai_weapon.h>
#include <ft/fighter.h>
#include <so/controller/so_controller_impl.h>
#include <gf/gf_archive.h>
#include <gf/gf_heap_manager.h>
#include <math.h>
#include <mt/mt_prng.h>
#include <nw4r/math/math_arithmetic.h>

aiInput::aiInput(u32 contNo, int unk2C, int player, int ftKind, s8 state, aiInput* parent, Heaps::HeapType heap)
    : IpHuman(contNo) {
    m_unk2C = unk2C;
    m_ftKind = ftKind;
    m_chrIdx.m_idx = player + (parent != NULL) * aiMgr::NumPlayers;
    m_unk43.m_idx = 0;
    m_unk112 = g_aiMgr->m_adventureMd;
    m_cpuType = 0;
    m_unk114 = 1;
    m_unk116 = 0x2FF;
    m_flags = 0x8000;
    m_target.m_idx = m_chrIdx.m_idx;
    m_unk58 = 0;
    m_reactWait = 0;
    m_unk6A = 0;
    m_unk6C = 1;
    m_unk74 = 0;
    m_unk60 = 0;
    m_unk61 = 0;
    m_unk62 = 0;
    m_unk63 = 0;
    m_unk64 = 0;
    m_unk65 = 0;
    m_unk66 = 0;
    m_unk68 = 0;
    m_unk67 = -1;
    m_unk75 = 0;
    m_child = NULL;
    m_unk84 = 0;
    m_unk88 = 0;
    if (parent == NULL) {
        aiMgr* mgr = g_aiMgr;
        s8 current;
        if (player < 0 || (u32)player >= 14) {
            current = -1;
        } else {
            current = mgr->m_chrState[player];
        }
        if (current == -1) {
            current = state;
        }
        mgr->set_ai_player(player, current, this, ftKind);
    } else {
        parent->m_child = this;
        aiSystemPar* systemPar = (aiSystemPar*)aiMgr::getAiSystemPar();
        m_par.setRank((s8)systemPar->m_childRank, 0, 1);
        aiMgr* mgr = g_aiMgr;
        mgr->regist_ai_child(aiChrIdx::GetPlayerNo(&parent->m_chrIdx), this, ftKind);
    }
    m_act = new (heap) aiAct(this, aiMgr::get_aiStat(&m_chrIdx));
    s8 target[16];
    target[0] = -1;
    m_act->act_change(0, target, 0, 0);
    m_md = m_mdNext = parent == NULL ? 1 : 0x79;
    m_step = 0;
    m_stepFrame = 0;
    m_unk5C = 0;
    m_unk5E = 0;
    m_unk8A = 0;
    m_unk111 = 0;
    m_pacData = NULL;
    m_unk70 = 0;
    m_unk3E = 0;
    m_unk3F = 0;
    m_stickBiasX = 0.0f;
    m_unk118 = 0.0f;
    m_swingChr.m_idx = -1;
    m_swingState = 0;
    m_unk78 = 0.0f;
    m_unk7C = 0.0f;
    m_unk80 = 0.0f;
    m_unk77 = 0;
    m_unk76 = 0;
}

void aiInput::reset() {
    s8 target[16];
    target[0] = -1;
    change_md(0, target, 0, 0);
    m_md = 0;
    m_target.m_idx = m_chrIdx.m_idx;
    m_unk58 = 0;
    m_reactWait = 0;
    m_unk6A = 0;
    m_unk74 = 0;
    m_unk60 = 0;
    m_unk61 = 0;
    m_unk62 = 0;
    m_unk63 = 0;
    m_unk64 = 0;
    m_unk65 = 0;
    m_unk66 = 0;
    m_unk68 = 0;
    m_unk67 = 0xFF;
    m_unk88 = 0;
    m_swingChr.m_idx = -1;
    m_swingState = 0;
    m_stickBiasX = 0.0f;
    m_unk78 = 0.0f;
    m_unk7C = 0.0f;
    m_unk80 = 0.0f;
    m_unk77 = 0;
    m_unk76 = 0;
}

void aiInput::swingChkReset(int force) {
    if (force == 0 && m_swingState >= 2) {
        return;
    }
    m_swingChr.m_idx = -1;
    m_swingState = 0;
}

void aiInput::swingChkSet(int hit) {
    if (aiChrIdx::GetFighterNo(&m_chrIdx) <= 0) {
        m_swingChr = m_target;
        if (hit != 0 && m_swingState == 0) {
            m_swingState = 1;
        }
    }
}

void aiInput::setEnable(u32 enable) {
    if (enable == 1 && (m_flags & 0x8000) == 0) {
        reset();
    }
    if (enable != 0) {
        m_flags |= 0x8000;
    } else {
        m_flags &= 0x7FFF;
    }
}

void aiInput::changeFtKind(int ftKind) {
    m_ftKind = ftKind;
    m_unk118 = 0.0f;
    reset();
    m_par.init_par(NULL, -1, NULL);
    if ((m_flags & 0x40) != 0) {
        if (m_pacData != NULL) {
            gfHeapManager::free(m_pacData);
            m_pacData = NULL;
        }
        m_flags &= ~0x40;
    }
}

void aiInput::pacRegist(void* data, int size, int owned, int check) {
    if (check != 0) {
        gfArchive archive;
        archive.setFileImage(data, size, Heaps::Tmp);
        aiParam::check_revision(static_cast<char*>(archive.getData(1, Data_Type_Misc, 0, 0xFFFE)));
    }
    m_par.init_par(NULL, -1, NULL);
    if (m_pacData != NULL && (m_flags & 0x40) != 0) {
        if (m_pacData != NULL) {
            gfHeapManager::free(m_pacData);
            m_pacData = NULL;
        }
        m_flags &= ~0x40;
    }
    if (m_ftKind <= -1 || (u32)m_ftKind >= 0x36) {
        return;
    }
    m_pacData = data;
    if (owned != 0) {
        m_flags |= 0x40;
    } else {
        m_flags &= ~0x40;
    }
    if (m_pacData != NULL) {
        gfArchive archive;
        archive.setFileImage(data, size, Heaps::Tmp);
        aiParData* par = static_cast<aiParData*>(archive.getData(1, Data_Type_Misc, 0, 0xFFFE));
        if (par != NULL) {
            char* atkData = static_cast<char*>(archive.getData(3, Data_Type_Misc, 0, 0xFFFE));
            m_par.init_par(par, m_ftKind, atkData);
            m_act->script_reload(archive.getData(2, Data_Type_Misc, 0, 0xFFFE));
        }
    }
}

void aiInput::setCpuType(int cpuType) {
    if (cpuType >= 0 && cpuType < 0x1D) {
        if (m_cpuType != cpuType && m_par.m_par != NULL) {
            reset();
        }
        m_cpuType = cpuType;
    }
    aiSystemPar* systemPar = (aiSystemPar*)aiMgr::getAiSystemPar();
    if (systemPar != NULL) {
        aiCpuTypePar* row;
        if (m_cpuType < 0x19) {
            row = &systemPar->m_cpuType[m_cpuType];
        } else {
            row = &systemPar->m_cpuType[0];
        }
        m_unk116 = row->m_unk0;
        m_unk114 = row->m_unk2;
    }
}

aiInput::~aiInput() {
    g_aiMgr->removePlayer(&m_chrIdx);
    if ((m_flags & 0x40) != 0) {
        if (m_pacData != NULL) {
            gfHeapManager::free(m_pacData);
            m_pacData = NULL;
        }
        m_flags &= ~0x40;
    }
    if (m_act != NULL) {
        delete m_act;
        m_act = NULL;
    }
}

// The CPU only fills in the input of its own character: the controller getters return the members of IpHuman that the
// AI wrote this frame.
Vec2f aiInput::getStickMain() const {
    return m_stickMain;
}

Vec2f aiInput::getStickSub() const {
    return m_stickSub;
}

ipPadButton aiInput::getButton() const {
    return m_button;
}

ipPadTrigger aiInput::getTrigger() const {
    return m_trigger;
}

s32 aiInput::getContNo() const {
    return m_contNo;
}

// Turns a stick value in pad units into the -1..1 range of IpHuman: scaled by 1 / g_ipStickRange, clamped, and
// snapped to 0 inside the dead zone.
void aiNormalizeStick(Vec2f* stick) {
    stick->m_x *= 1.0f / g_ipStickRange;
    stick->m_y *= 1.0f / g_ipStickRange;
    if (stick->m_x > 1.0f) {
        stick->m_x = 1.0f;
    } else if (stick->m_x < -1.0f) {
        stick->m_x = -1.0f;
    }
    if (stick->m_y > 1.0f) {
        stick->m_y = 1.0f;
    } else if (stick->m_y < -1.0f) {
        stick->m_y = -1.0f;
    }
    if (float(__fabs(stick->m_x)) <= g_ipStickDeadZone) {
        stick->m_x = 0.0f;
    }
    if (float(__fabs(stick->m_y)) <= g_ipStickDeadZone) {
        stick->m_y = 0.0f;
    }
}

// Whether there is a floor below the point (pos.m_x, y). HYPOTHESIS: name.
static inline u32 aiHasFloorUnder(Vec3f* pos, float y) {
    pos->m_y = y;
    pos->m_z = 0.0f;
    return g_aiMgr->get_under_floor(pos, NULL, NULL, -1, -9999.9f, 0.0f, 0.0f);
}

// Keeps the CPU on the floor it stands on: when the x position predicted `frames` ahead leaves the floor (shrunk by
// `margin`, and kept 30 units inside the stage area) and there is no floor under that point, the stick bias
// m_stickBiasX grows by 0.5 per call towards the floor up to `maxBias`; otherwise it decays to 0 by 1/12 per call.
// The bias is then added to the stick.
// HYPOTHESIS: names; `unk` is passed by the callers but not used.
void aiInput::keepOnFloor(Vec2f* stick, int* unk, float frames, float margin, float maxBias) {
    aiStat* stat = aiMgr::get_aiStat(&m_chrIdx);
    Vec3f pos;
    float areaLeft = 30.0f + g_aiMgr->m_areaLeft;
    float areaRight = g_aiMgr->m_areaRight - 30.0f;
    pos.m_x = stat->m_pos.m_x + stat->m_speed.m_x * frames;
    AiClBlock* floor = stat->m_floor;
    float left = margin + nw4r::math::FSelect(floor->m_edgeL.m_x - areaLeft, floor->m_edgeL.m_x, areaLeft);
    float right = nw4r::math::FSelect(floor->m_edgeR.m_x - areaRight, areaRight, floor->m_edgeR.m_x) - margin;
    if ((pos.m_x < left || pos.m_x > right) && !aiHasFloorUnder(&pos, stat->m_pos.m_y)) {
            if (pos.m_x < left) {
                m_stickBiasX += 0.5f;
                if (m_stickBiasX > maxBias) {
                    m_stickBiasX = maxBias;
                }
            } else {
                m_stickBiasX -= 0.5f;
                if (m_stickBiasX < -maxBias) {
                    m_stickBiasX = -maxBias;
                }
            }
    } else if (m_stickBiasX > 0.0f) {
        m_stickBiasX -= 1.0f / 12.0f;
        if (m_stickBiasX < 0.0f) {
            m_stickBiasX = 0.0f;
        }
    } else if (m_stickBiasX < 0.0f) {
        m_stickBiasX += 1.0f / 12.0f;
        if (m_stickBiasX > 0.0f) {
            m_stickBiasX = 0.0f;
        }
    }
    stick->m_x += m_stickBiasX;
}

// The CPU was hit: starts (or shortens) the reaction delay m_reactWait, picked from the rank's reaction parameters
// when frames <= 0. With a damage record it also counts the hit on the character's aiStat and, when the attacker is
// the CPU's target, raises m_unk118 (by 2..3 after its own swing hit something, else by 1..2 while below 10).
// HYPOTHESIS: the meaning of m_unk118, m_unk74/77/78/7C/80 and of the aiStat fields.
void aiInput::damageStart(soDamage* damage, s16 frames, int unk) {
    if (frames <= 0) {
        int extra = randi(100) < m_par.m_reactExtraRate ? m_par.m_reactExtra : 0;
        frames = m_par.m_reactBase + (extra + randi(m_par.m_reactRand + 1));
    }
    if (m_reactWait == 0 || m_reactWait > frames) {
        m_reactWait = frames;
    }
    if (unk) {
        m_flags |= 1;
    }
    if (damage != NULL) {
        aiStat* stat = aiMgr::get_aiStat(&m_chrIdx);
        if (m_swingChr.m_idx >= 0) {
            aiStat* target = aiMgr::get_aiStat(&m_target);
            if (!(stat->m_flags & 1) && stat->m_hitCount == 0 && damage->m_collisionLog.m_taskId == target->m_taskId) {
                m_unk118 += 2.0f + randf();
                if (m_unk118 > 20.0f) {
                    m_unk118 = 20.0f;
                }
            }
            m_swingChr.m_idx = -1;
            m_swingState = 0;
        } else {
            aiStat* target = aiMgr::get_aiStat(&m_target);
            if (!(stat->m_flags & 1) && stat->m_hitCount == 0 && damage->m_collisionLog.m_taskId == target->m_taskId &&
                m_unk118 < 10.0f)
            {
                m_unk118 += 1.0f + randf();
            }
        }
        stat->m_hitCount++;
        if (m_unk74 > 0) {
            m_unk74 = 13;
        }
        m_act->addDefendAirRate(0.3f);
        m_unk78 = 1.2f;
        if (m_unk77 != 0) {
            m_unk80 -= 0.1f;
            if (m_unk80 < 0.0f) {
                m_unk80 = 0.0f;
            }
            m_unk7C += 0.6f;
            if (m_unk7C > 1.5f) {
                m_unk7C = 1.5f;
            }
        }
    }
}

// This CPU's attack hit `target`. When it is the character the swing check was armed for, m_unk118 either decays
// towards 0 by 0.7 (more likely at a higher rank) or moves by a random amount (larger at a lower rank) in a random
// direction; the swing check is then reset. Otherwise m_unk118 decays towards 0 by a random 0..0.3.
// HYPOTHESIS: the meaning of m_unk118 and m_unk62.
void aiInput::swingHit(aiChrIdx* target, soDamage* damage) {
    m_unk62 = 50;
    if (m_swingChr.m_idx >= 0) {
        if (target->m_idx == m_swingChr.m_idx) {
            float chance = randf();
            float rank = m_par.getRankRatio();
            if (chance < 0.1f + 0.8f * rank) {
                if (m_unk118 > 0.0f) {
                    m_unk118 -= 0.7f;
                    if (m_unk118 < 0.0f) {
                        m_unk118 = 0.0f;
                    }
                } else {
                    m_unk118 += 0.7f;
                    if (m_unk118 > 0.0f) {
                        m_unk118 = 0.0f;
                    }
                }
            } else {
                float amount = (0.2f + 0.8f * (1.0f - rank)) * (5.0f * (0.5f + 0.5f * randf()));
                if (randf() < 0.5f) {
                    m_unk118 += amount;
                } else {
                    m_unk118 -= amount;
                }
            }
        }
        m_swingChr.m_idx = -1;
        m_swingState = 0;
    } else if (m_unk118 > 0.0f) {
        m_unk118 -= 0.3f * randf();
        if (m_unk118 < 0.0f) {
            m_unk118 = 0.0f;
        }
    } else {
        m_unk118 += 0.3f * randf();
        if (m_unk118 > 0.0f) {
            m_unk118 = 0.0f;
        }
    }
}

// The character slot that this CPU has to attack no matter what (the four CPU types 0xE..0x11 of the Subspace Emissary
// "follow player N" scripts), or, when the stage is 0x1C, the slot of the enemy nearest to it. Falls back to its own slot.
// HYPOTHESIS: the meaning of the aiStat flags 0x8000 / 0x40000000 and of m_unkCC (probably the team) is not known.
s8 aiInput::get_fix_target(float* distance) {
    u32 cpuType = m_cpuType;
    if (cpuType >= 0xE && cpuType <= 0x11) {
        s8 target = cpuType - 0xE;
        aiStat* self = aiMgr::get_aiStat(&m_chrIdx);
        aiChrIdx targetIdx;
        targetIdx.m_idx = target;
        aiStat* other = aiMgr::get_aiStat(&targetIdx);
        if (target != m_chrIdx.m_idx) {
            u32 flags = other->m_flags;
            if ((flags & 0x8000) == 0 || (flags & 0x40000000) != 0) {
                if (other->m_unkCC != self->m_unkCC) {
                    if (distance != NULL) {
                        aiChrIdx distIdx;
                        distIdx.m_idx = target;
                        *distance = self->get_distance(0.0f, &distIdx);
                    }
                    return target;
                }
            }
        }
    }
    switch (g_aiMgr->m_stageKind) {
    case 0x1C: {
        aiStat* self = aiMgr::get_aiStat(&m_chrIdx);
        aiEnemy* enemy = g_aiMgr->m_enemyMgr->getEnemyNear(&self->m_center, distance);
        if (enemy != NULL) {
            return enemy->m_index + 0xE;
        }
        break;
    }
    }
    return m_chrIdx.m_idx;
}

int aiInput::md_return(Vec2f* stick, ipPadButton* button) {
    aiStat* stat = aiMgr::get_aiStat(&m_chrIdx);
    if (stat->m_situation == 5 && stat->m_status != Fighter::Status::Damage_Air && m_act->m_id != 0x2050 && (m_unk116 & 2)) {
        if (++m_unk75 > 4) {
            Vec2f edge = stat->getNearFloorEdge();
            if (stat->chk_over_ground() || stat->m_pos.m_y + 10.0f * stat->m_speed.m_y > edge.m_y - 5.0f) {
                aiChrIdx none;
                m_act->act_change(0x2050, &none, 0, 0);
            }
            m_unk75 = 0;
        }
    } else {
        m_unk75 = 0;
    }
    if (m_act->m_id == 0) {
        aiChrIdx none;
        m_act->act_change(m_par.select_action(0, &m_chrIdx, &m_target, m_unk116, m_act, m_cpuType == 0x1A), &none, 0, 0);
        // A new 0x2040 action far from the floor and above the bottom of the area becomes 0x2010.
        if (m_act->m_id == 0x2040 && stat->get_range_floor(stat->m_lr, &stat->m_pos) < -80.0f &&
            stat->m_pos.m_y > 30.0f + g_aiMgr->m_areaBottom) {
            aiChrIdx none;
            m_act->act_change(0x2010, &none, 0, 0);
        }
    }
    return 1;
}

// Dizzy (situation 9): spin the stick; when the dizzy status 0xEC happens over no ground, steer back towards the stage.
// Any other situation goes back to mode 1. HYPOTHESIS: the meaning of situation 9 and status 0xEC follows the map name.
int aiInput::md_piyo(Vec2f* stick, ipPadButton* button) {
    aiStat* stat = aiMgr::get_aiStat(&m_chrIdx);
    if (stat->m_situation != 9) {
        aiChrIdx none;
        change_md(1, &none, 0, 0);
        return 1;
    }
    if (m_par.m_rank >= 20) {
        float angle = 0.03125f * (3.1415927f * m_stepFrame) * (1.0f + 0.03f * m_par.m_rank);
        stick->m_x = sin(angle);
        stick->m_y = cos(angle);
    }
    if (stat->m_status == Fighter::Status::Yoshi_Egg && !stat->chk_over_ground()) {
        Vec2f goal;
        m_act->get_return_goal(&goal.m_x, &goal.m_y, 0);
        stick->m_x = stat->m_pos.m_x > goal.m_x ? -1.0f : 1.0f;
    }
    return 1;
}

// Swimming (situation 12). Step 0 picks the way out: from the deep water or a dive/rise/drown status head for the return
// goal (step 1), otherwise swim up where the CPU is (step 2). Step 1 steers to the goal and moves on to step 2 once nothing
// is above the head; each step gives up after a while. Leaving the water goes to mode 7 over ground, 3 otherwise.
// HYPOTHESIS: the meaning of aiStat::m_unkC0 == 2 and of the modes 3 and 7.
int aiInput::md_swim(Vec2f* stick, ipPadButton* button) {
    aiStat* stat = aiMgr::get_aiStat(&m_chrIdx);
    if (stat->m_situation != 12) {
        aiChrIdx none;
        aiChrIdx* target = &none;
        change_md(stat->chk_over_ground() ? 7 : 3, target, 0, 0);
        return 1;
    }
    if (m_step == 0) {
        if (stat->m_unkC0 != 2 && stat->m_status != Fighter::Status::Swim_Dive &&
            stat->m_status != Fighter::Status::Swim_Rise && stat->m_status != Fighter::Status::Swim_Up &&
            stat->m_status != Fighter::Status::Swim_Drown_Out) {
            m_act->m_goalPos.m_x = stat->m_pos.m_x;
            m_stepFrame = 0;
            m_step = 2;
        } else {
            Vec2f goal;
            m_act->get_return_goal(&goal.m_x, &goal.m_y, 0);
            m_act->set_goal_inf(stat->m_floor, NULL, goal.m_x, goal.m_y);
            m_step = 1;
        }
    }
    switch (m_step) {
    case 1:
        stick->m_x = stat->m_pos.m_x > m_act->m_goalPos.m_x ? -1.0f : 1.0f;
        if (stat->m_status != Fighter::Status::Swim_Dive && stat->m_status != Fighter::Status::Swim_Rise &&
            stat->m_status != Fighter::Status::Swim_Up && stat->m_status != Fighter::Status::Swim_Drown_Out) {
            Vec2f pos;
            Vec2f up(stat->m_pos.m_x, 10.0f + stat->m_pos.m_y);
            float x = stat->m_pos.m_x;
            float y = stat->m_pos.m_y;
            pos.m_x = x;
            pos.m_y = y;
            if (!g_aiMgr->chk_cl_cross(&pos, &up, 0x1C, NULL, NULL)) {
                m_stepFrame = 0;
                m_step++;
                break;
            }
        }
        if (m_stepFrame >= 50) {
            m_step = 0;
        }
        break;
    case 2: {
        float dx = m_act->m_goalPos.m_x - stat->m_pos.m_x;
        if (dx > 10.0f) {
            stick->m_x = 1.0f;
        } else if (dx < -10.0f) {
            stick->m_x = -1.0f;
        }
        stick->m_y = 1.0f;
        if (m_stepFrame >= 16) {
            m_step = 0;
        }
        break;
    }
    }
    return 1;
}

// Gliding (situation 16). Step 0 starts a random delay (the frame counter goes negative) before the stick may point down.
// Step 1 presses attack at a near enemy in front; over ground the CPU dives down and, near the floor, lands with an
// attack (at random, more often at a higher rank) or gives up gliding; over no ground it steers its height back towards
// the middle of the floor block. HYPOTHESIS: the meaning of the steps follows the map name and how the values are used.
int aiInput::md_glide(Vec2f* stick, ipPadButton* button) {
    aiStat* stat = aiMgr::get_aiStat(&m_chrIdx);
    if (stat->m_situation != 16) {
        aiChrIdx none;
        change_md(1, &none, 0, 0);
        return 1;
    }
    if (m_step == 0) {
        m_stepFrame = -60.0f * randf();
        m_step++;
    }
    if (m_step == 1) {
        float dist;
        s8 target = get_near_target(&dist, 0);
        if (target != m_chrIdx.m_idx && dist < 20.0f) {
            aiChrIdx targetIdx;
            targetIdx.m_idx = target;
            if (stat->m_lr * stat->get_lr_tgt(&targetIdx) > 0.0f && !stat->m_ipBuf.getButton(0)) {
                button->m_mask |= soController::getButtonMask(soController::Pad_Button_Attack);
            }
        }
    }
    if (stat->chk_over_ground()) {
        if (m_stepFrame >= 0) {
            stick->m_y = -1.0f;
        }
        if (m_step == 1 && stat->m_pos.m_y + 10.0f * stat->m_speed.m_y < 5.0f + stat->m_floor->m_bottom &&
            !stat->m_ipBuf.getButton(0)) {
            s8 rank = m_par.m_rank;
            if (70.0f * randf() < rank) {
                button->m_mask |= soController::getButtonMask(soController::Pad_Button_Attack);
            } else {
                m_step++;
            }
        }
    } else {
        m_stepFrame--;
        AiClBlock* floor = stat->m_floor;
        if (stat->m_lr * (0.5f * (floor->m_edgeL.m_x + floor->m_edgeR.m_x) - stat->m_pos.m_x) > 0.0f) {
            if (stat->m_pos.m_y > 10.0f + floor->m_bottom) {
                if (stat->m_speed.m_y > -0.15f) {
                    stick->m_y = -0.4f;
                }
            } else if (stat->m_speed.m_y < 0.1f) {
                stick->m_y = 0.4f;
            }
        } else {
            stick->m_y = 1.0f;
        }
    }
    return 1;
}

// Idle while no opponent is a threat. Any active opponent (not flagged 0x8000, and not in situation 5 unless the CPU's rank is
// 60+ and the opponent's flag 1 is clear), or, above rank 45, a near enemy or an item to pick up, ends the mode; so do the
// Made-in-Wario AI data (mode 0x66) and a dangerous floor (mode 5). Step 0 waits for a free ground state (the Pokemon
// kinds 0x1D-0x1F may press special with the stick down), step 1 taunts at random unless an opponent is on the respawn
// platform, step 2 waits and goes back to mode 1.
// HYPOTHESIS: the meaning of m_flags 4, of aiStat flags 0x8000/1, situation 5 and of the modes 1, 5 and 0x66.
int aiInput::md_relax(Vec2f* stick, ipPadButton* button) {
    aiStat* stat = aiMgr::get_aiStat(&m_chrIdx);
    bool rebirth = false;
    if (!(m_flags & 4)) {
        int i;
        int rank = m_par.m_rank;
        for (i = 0; i < 7; i++) {
            aiChrIdx idx;
            idx.m_idx = i;
            aiStat* other = aiMgr::get_aiStat(&idx);
            if (i == m_chrIdx.m_idx || other->m_unkCC == stat->m_unkCC) {
                continue;
            }
            if (other->m_status == Fighter::Status::Rebirth) {
                rebirth = true;
            }
            if (other->m_flags & 0x8000) {
                continue;
            }
            if (other->m_situation == 5 && (rank < 60 || (other->m_flags & 1))) {
                continue;
            }
            aiChrIdx none;
            change_md(1, &none, 0, 0);
            return 1;
        }
        if (rank > 45 && (g_aiMgr->m_enemyMgr->getEnemyNear(&stat->m_center, NULL) != NULL ||
                          aiMgr::getWeaponMgr()->m_hasPickupItem)) {
            aiChrIdx none;
            change_md(1, &none, 0, 0);
            return 1;
        }
    }
    if (aiMgr::getMadeinAiData(false)) {
        change_md(0x66, &m_target, 0x90, 0);
        return 1;
    }
    AiClBlock* floor = stat->m_floor;
    int team = stat->m_team;
    bool danger = (floor->m_flags & 0x1000) || floor->chk_fl_danger(team);
    if (danger) {
        aiChrIdx none;
        change_md(5, &none, 0, 0);
        return 1;
    }
    switch (m_step) {
    case 0:
        if (stat->chk_stat_gr_free()) {
            switch (stat->m_chrKind) {
            case 0x1D:
            case 0x1E:
            case 0x1F:
                if (stat->chk_chr_stat(1, -1) || randf() < 0.1f) {
                    stick->m_y = -1.0f;
                    button->m_mask |= soController::getButtonMask(soController::Pad_Button_Special);
                }
                break;
            }
            m_stepFrame = -20.0f * randf();
            m_step++;
        }
        break;
    case 1:
        if (m_stepFrame >= 0) {
            if (!rebirth) {
                float r = randf();
                if (r < 0.3f) {
                    button->m_mask |= soController::getButtonMask(soController::Pad_Button_Appeal_Hi);
                } else if (r < 0.6f) {
                    button->m_mask |= soController::getButtonMask(soController::Pad_Button_Appeal_Lw);
                } else if (r < 0.9f) {
                    button->m_mask |= soController::getButtonMask(soController::Pad_Button_Appeal_S);
                }
            }
            m_flags &= ~4;
            m_stepFrame = -180.0f * randf() - 120.0f;
            m_step++;
        }
        break;
    default:
        if (m_stepFrame >= 0) {
            aiChrIdx none;
            change_md(1, &none, 0, 0);
            return 1;
        }
        break;
    }
    return 1;
}

// Defined here, between the aiInput functions of the original unit.
aiWeaponMgr* aiMgr::getWeaponMgr() {
    return g_aiMgr->m_weaponMgr;
}
