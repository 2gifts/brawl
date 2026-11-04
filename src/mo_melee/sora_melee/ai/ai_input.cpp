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
    m_finalWait = 0;
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
    m_md = m_mdPrev = parent == NULL ? 1 : 0x79;
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
    m_finalWait = 0;
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

// The nearest opponent of this CPU: a fixed target (get_fix_target) wins; otherwise the nearest fighter of another team,
// preferring, for a registered main character, one that no other registered CPU is after; with jumpCheck, only fighters
// whose floor block this CPU can jump to. In Subspace Emissary mode (m_unk112) the nearest enemy is taken when no fighter
// is. Returns its own slot when there is nobody; distance (optional) gets the distance to the target.
// HYPOTHESIS: the meaning of situation 0xF (counted 1000 units further away) and of the aiStat flags is not known.
s8 aiInput::get_near_target(float* distance, int jumpCheck) {
    s8 target = get_fix_target(distance);
    if (target != m_chrIdx.m_idx) {
        return target;
    }
    aiStat* self = aiMgr::get_aiStat(&m_chrIdx);
    int i;
    int best = m_chrIdx.m_idx;
    aiMgr* mgr = g_aiMgr;
    float bestDist = 99999.9f;
    u32 bestFree = 0;
    s8 selfState;
    if (m_chrIdx.m_idx < 0 || (u32)m_chrIdx.m_idx >= aiMgr::NumChrs) {
        selfState = -1;
    } else {
        selfState = mgr->m_chrState[m_chrIdx.m_idx];
    }
    bool selfRegistered = selfState == 1;
    for (i = 0; i < aiMgr::NumPlayers; i++) {
        aiChrIdx idx;
        idx.m_idx = i;
        aiStat* other = aiMgr::get_aiStat(&idx);
        u32 free = 1;
        if (i == m_chrIdx.m_idx) {
            continue;
        }
        u32 flags = other->m_flags;
        if ((flags & 0x8000) != 0 && (flags & 0x40000000) == 0) {
            continue;
        }
        if (other->m_unkCC == self->m_unkCC) {
            continue;
        }
        if (selfRegistered) {
            aiMgr* mgr = g_aiMgr;
            s8 state;
            if (i < 0 || (u32)i >= aiMgr::NumChrs) {
                state = -1;
            } else {
                state = mgr->m_chrState[i];
            }
            if (state == 1 && mgr->getAiCpuTarget(i) == m_target.m_idx) {
                if (bestFree == 1) {
                    continue;
                }
                free = 0;
            }
        }
        if (jumpCheck != 0) {
            // HYPOTHESIS: aiParam's working area holds the rank-blended jump parameters at aiJumpPar's offsets.
            float high;
            if (m_flags & 0x20) {
                high = aiParam::get_jump_high_sp((aiJumpPar*)&m_par, self);
            } else {
                high = aiParam::get_jump_high((aiJumpPar*)&m_par, self);
            }
            aiMgr* mgr = g_aiMgr;
            if (!mgr->chk_goto_blk(self->m_floor, other->m_floor, self->m_team, self->m_pos.m_x,
                                   aiParam::get_jump_length((aiJumpPar*)&m_par, self), high)) {
                continue;
            }
        }
        aiChrIdx distIdx;
        distIdx.m_idx = i;
        float dist = self->get_distance(0.0f, &distIdx);
        if (other->m_situation == 0xF) {
            dist += 1000.0f;
        }
        if (bestDist > dist || (free != 0 && bestFree == 0)) {
            bestDist = dist;
            best = i;
            bestFree = free;
        }
    }
    if (m_unk112 != 0 && best == m_chrIdx.m_idx) {
        aiEnemy* enemy = g_aiMgr->m_enemyMgr->getEnemyNear(&self->m_pos, &bestDist);
        if (enemy != NULL) {
            best = enemy->m_index + 0xE;
        }
    }
    if (distance != NULL && best != m_chrIdx.m_idx) {
        *distance = bestDist;
    }
    return best;
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

// Ladder (situation 8). The goal height depends on the mode the CPU came from: a little higher than the action goal
// after the modes 5-7/0xB/0x16/0x7A, above or below the CPU after mode 2, the target's position otherwise. Step 1 climbs
// towards it (and gives up after a while, at the top, or on aiStat flag 0x4000000), step 2 picks a side (towards the
// target on the same floor block, else towards the goal), steps 3/4 jump off that way; after 8 frames back to mode 1
// (same floor as the target) or mode 6. HYPOTHESIS: the mode and step meanings follow the map name and the values.
int aiInput::md_ladder(Vec2f* stick, ipPadButton* button) {
    aiStat* stat = aiMgr::get_aiStat(&m_chrIdx);
    if (!aiChrIdx::IsValid(&m_target) || m_target.m_idx == m_chrIdx.m_idx) {
        target_check();
    }
    aiStat* target = aiMgr::get_aiStat(&m_target);
    switch (m_mdPrev) {
    case 5:
    case 6:
    case 7:
    case 0xB:
    case 0x16:
    case 0x7A:
        if (m_step == 0) {
            m_act->m_goalPos.m_y += 22.0f;
        }
        break;
    case 2:
        m_act->m_goalPos.m_x = stat->m_pos.m_x;
        if (stat->m_pos.m_y > target->m_pos.m_y) {
            m_act->m_goalPos.m_y = 30.0f + stat->m_pos.m_y;
        } else {
            m_act->m_goalPos.m_y = stat->m_pos.m_y - 30.0f;
        }
        break;
    default:
        m_act->m_goalPos.m_x = target->m_pos.m_x;
        m_act->m_goalPos.m_y = target->m_pos.m_y;
        break;
    }
    switch (m_step) {
    case 0:
        m_stepFrame = -8.0f * randf();
        m_step++;
    case 1:
        if (stat->m_situation != 8) {
            aiChrIdx none;
            change_md(1, &none, 0, 0);
            return 1;
        }
        if (stat->m_pos.m_y > m_act->m_goalPos.m_y) {
            stick->m_y = -1.0f;
            if (m_stepFrame > 10) {
                m_step = 2;
                m_stepFrame = 0;
            }
        } else {
            stick->m_y = 1.0f;
            if (15.0f + stat->m_pos.m_y > m_act->m_goalPos.m_y) {
                if (!aiChrIdx::GetFighterNo(&m_chrIdx)) {
                    m_step = 2;
                    m_stepFrame = 0;
                }
            } else if (stat->m_flags & 0x4000000) {
                m_step = 2;
                m_stepFrame = 0;
            } else if (m_stepFrame > 10 && (stat->m_speed.m_y < 0.001f || m_stepFrame > 180)) {
                m_step = 2;
                m_stepFrame = 0;
            }
        }
        if (m_step == 1) {
            break;
        }
    default:
        if (m_step == 2) {
            if (stat->m_floor == target->m_floor) {
                stick->m_x = stat->get_lr_tgt(&m_target);
            } else {
                stick->m_x = stat->m_pos.m_x < m_act->m_goalPos.m_x ? 1.0f : -1.0f;
            }
            if (stick->m_x > 0.0f) {
                m_step = 3;
            } else {
                m_step = 4;
            }
        } else if (m_step == 3) {
            stick->m_x = 1.0f;
        } else {
            stick->m_x = -1.0f;
        }
        button->m_mask = soController::getButtonMask(soController::Pad_Button_Jump);
        if (m_stepFrame > 8) {
            if (stat->m_floor == target->m_floor) {
                aiChrIdx none;
                change_md(1, &none, 0, 0);
            } else {
                aiChrIdx none;
                change_md(6, &none, 0, 0);
            }
            return 1;
        }
        break;
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

// Grabbing (situations 10/11). Outside a grab, a CPU above rank 50 that holds chr-stat 4 (HYPOTHESIS: an item throw) may
// switch to mode 0x65 at random, otherwise it goes back to mode 1. Chr-stat 2 for kind 0x1B: press attack (or the stick
// up in the air) every other frame after a rank-dependent wait. Chr-stat 1 for kinds 5/0x20: step 0 picks a pummel
// count (step 1: walk the held character towards the far side first, step 2: attack, step 3: down throw), then after
// a rank-dependent wait press attack or the stick down every other frame. Any other grab starts action 0x1120 on
// the nearest target. HYPOTHESIS: all meanings follow the map name and how the values are used.
int aiInput::md_catch(Vec2f* stick, ipPadButton* button) {
    aiStat* stat = aiMgr::get_aiStat(&m_chrIdx);
    s8 rank;
    if (stat->m_situation != 10 && stat->m_situation != 11) {
        if (stat->chk_chr_stat(4, 1) && !(stat->m_flags & 1)) {
            rank = m_par.m_rank;
            if (rank > 50 && randf() < 0.007f * rank) {
                aiChrIdx none;
                change_md(0x65, &none, 0x54, 0);
                return 1;
            }
        }
        aiChrIdx none;
        change_md(1, &none, 0, 0);
        return 1;
    }
    if (stat->chk_chr_stat(2, 0x1B)) {
        if (m_stepFrame >= (80 - m_par.m_rank) / 4 && !(m_stepFrame & 1)) {
            if (stat->chk_over_ground()) {
                button->m_mask |= soController::getButtonMask(soController::Pad_Button_Attack);
            } else {
                stick->m_y = 1.0f;
            }
        }
    } else if (stat->chk_chr_stat(1, 5) || stat->chk_chr_stat(1, 0x20)) {
        switch (m_step) {
        case 0:
            if (m_cpuType == 0x1B || stat->m_chrKind == 0x20 || randf() < 0.2f) {
                rank = m_par.m_rank;
                if (randf() < 0.01f * rank) {
                    m_step = 1;
                } else {
                    m_step = 2;
                    break;
                }
            } else {
                m_step = 3;
                break;
            }
        case 1:
            if (!(stat->m_flags & 1) && m_stepFrame < 8) {
                float front = stat->get_range_floor(stat->m_lr, &stat->m_pos);
                if (front > stat->get_range_floor(-stat->m_lr, &stat->m_pos)) {
                    stick->m_x = -0.5f * stat->m_lr;
                    break;
                }
                m_step++;
            } else {
                m_step++;
            }
            break;
        }
        int step = m_step;
        if (step > 1 && m_stepFrame >= (70 - m_par.m_rank) / 2 && !(m_stepFrame & 1)) {
            if (step != 3) {
                button->m_mask |= soController::getButtonMask(soController::Pad_Button_Attack);
            } else {
                stick->m_y = -1.0f;
            }
        }
    } else if (m_act->m_id != 0x1120) {
        m_target.m_idx = get_near_target(NULL, 0);
        aiChrIdx none;
        m_act->act_change(0x1120, &none, 0, 0);
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

// Smash DI (HYPOTHESIS: the map's "slide"): while the character is being launched (aiStat flag 0x400000) the stick is held
// against the attacker's direction; m_unk74 (set by damageStart) counts down faster after a single hit and ends the mode.
// Below rank 60 a random away-and-up/down direction is kept for 30 frames; from rank 60 a stronger one is picked and
// decays every frame. HYPOTHESIS: aiAct::m_unk0[0..1] holds the chosen stick between frames.
int aiInput::md_slide(Vec2f* stick, ipPadButton* button) {
    aiStat* stat = aiMgr::get_aiStat(&m_chrIdx);
    if (!(stat->m_flags & 0x400000)) {
        if (stat->m_hitCount <= 1) {
            m_unk74 -= 2;
        } else {
            m_unk74 -= 1;
        }
        if (m_unk74 <= 0) {
            m_unk74 = 0;
            aiChrIdx none;
            change_md(1, &none, 0, 0);
            return 1;
        }
    }
    float rate = m_par.getRankRatio();
    if (rate < 0.6f) {
        switch (m_step) {
        case 0:
            if (rate >= 0.4f) {
                m_act->m_unk0[0] = -0.8f * stat->get_lr_tgt(&m_target);
            } else {
                m_act->m_unk0[0] = -0.8f * ((0.2f + rate) * stat->get_lr_tgt(&m_target));
            }
            m_act->m_unk0[1] = (0.4f + rate) * (randf() < 0.3f ? 1.0f : -1.0f);
            m_stepFrame = -30;
            m_step++;
            break;
        default:
            stick->m_x = m_act->m_unk0[0];
            stick->m_y = m_act->m_unk0[1];
            if (m_stepFrame >= 0) {
                m_step = 0;
            }
            break;
        }
    } else {
        switch (m_step) {
        case 0:
            m_act->m_unk0[0] = -0.7f * stat->get_lr_tgt(&m_target);
            m_act->m_unk0[1] = -1.0f + (1.0f - rate);
            if (randf() < 0.3f) {
                m_act->m_unk0[1] *= -1.0f;
            }
            m_stepFrame = -20.0f + 10.0f * rate;
            m_step++;
        }
        stick->m_x = m_act->m_unk0[0];
        stick->m_y = m_act->m_unk0[1];
        m_act->m_unk0[0] *= 0.3f;
        if (__fabsf(m_act->m_unk0[0]) < 0.2f) {
            m_act->m_unk0[0] = 0.0f;
        }
        m_act->m_unk0[1] *= 0.3f;
        if (__fabsf(m_act->m_unk0[1]) < 0.2f) {
            m_act->m_unk0[1] = 0.0f;
        }
        if (m_stepFrame >= 0) {
            m_step = 0;
        }
    }
    return 1;
}

// Defined here, between the aiInput functions of the original unit.
aiWeaponMgr* aiMgr::getWeaponMgr() {
    return g_aiMgr->m_weaponMgr;
}

// Drops through a pass-through floor on purpose: while falling (speed y < -0.3) on a floor with flags, with no button
// held, when both ends of the floor have the same block 5 units below and that block is safe, the CPU may press the
// stick down (stick y = -1) at random (chance 0.015 per rank above 40) in the modes that go to a goal block that is
// this block, or when the target is on (or the target's floor is about level with) the block below; only when the
// predicted path over the next 2 frames crosses a floor line. HYPOTHESIS: names and the meaning of the modes and flags.
// BYTES: defined after the md_* handlers (its address is right after keepOnFloor): the unit's literal pool is still
// incomplete and its new literals placed here shift the pool offsets of the handlers written before it.
void aiInput::auto_passfall(Vec2f* stick, ipPadButton* button) {
    aiStat* stat = aiMgr::get_aiStat(&m_chrIdx);
    if (m_par.m_rank >= 45 && button->m_mask == 0 && stat->m_unkC0 == 1 && stat->m_floor->m_flags != 0 &&
        stat->m_speed.m_y < -0.3f && (stat->m_flags & 0x80000)) {
        bool danger = false;
        AiClBlock* blockL = NULL;
        AiClBlock* blockR = NULL;
        aiMgr* mgr = g_aiMgr;
        Vec3f posL(stat->m_floor->m_edgeL.m_x, stat->m_floor->m_edgeL.m_y - 5.0f, 0.0f);
        Vec3f posR(stat->m_floor->m_edgeR.m_x, stat->m_floor->m_edgeR.m_y - 5.0f, 0.0f);
        mgr->get_under_floor(&posL, &blockL, NULL, -1, -9999.9f, 0.0f, 0.0f);
        mgr->get_under_floor(&posR, &blockR, NULL, -1, -9999.9f, 0.0f, 0.0f);
        if (blockL != NULL && blockL == blockR) {
        int team = stat->m_team;
        if ((blockL->m_flags & 0x1000) || blockL->chk_fl_danger(team)) {
            danger = true;
        }
        if (!danger) {
        bool pass = false;
        float chance = 0.015f * (m_par.m_rank - 40);
        switch (m_md) {
        case 5:
        case 6:
        case 7:
        case 8:
        case 11:
        case 22:
        case 122:
            if (blockL->chk_key(&m_act->m_goalJointId)) {
                pass = true;
            }
            break;
        default: {
            aiStat* target = aiMgr::get_aiStat(&m_target);
            if (m_act->m_id == 0x10A0) {
                if (blockL == target->m_floor) {
                    pass = true;
                    break;
                }
                if (!(stat->m_floor->m_flags & 0xC0) && (blockL->m_flags & 0xC0)) {
                    pass = true;
                    break;
                }
            }
            float top = stat->m_floor->m_top;
            if (blockL->m_top > top - 26.0f) {
                if (target->m_floor == stat->m_floor) {
                    chance *= 0.5f;
                    pass = true;
                } else if ((target->m_floor == blockL || target->m_pos.m_y < top - 1.0f) && stat->m_situation != 14) {
                    pass = true;
                }
            } else if (target->m_pos.m_y < top - 1.0f && (stat->m_situation != 14 || target->m_floor != blockL)) {
                pass = true;
            }
            break;
        }
        }
        if (pass && randf() < chance) {
            Vec2f speed = stat->m_speed;
            speed *= 2.0f;
            Vec2f from;
            Vec2f to(stat->m_pos.m_x + speed.m_x, stat->m_pos.m_y + speed.m_y);
            float x = stat->m_pos.m_x;
            float y = stat->m_pos.m_y;
            from.m_x = x;
            from.m_y = y;
            if (mgr->chk_cl_cross(&from, &to, 2, NULL, NULL)) {
                stick->m_y = -1.0f;
            }
        }
        }
        }
    }
}

// Picks a character to pay attention to in the idle modes (1, 2, 6, 11, 12) while the CPU can act: the nearest other
// fighter (by centre) that is an enemy flagged 0x100, flagged 0x80, or an enemy in state 1/7; with none, under flag
// 0x200000 (or as CPU type 4, 30% of the time) the nearest target. Within 100 units (30 when another fighter of
// another m_unkCC was seen) and at random by rank it becomes the target: an enemy within 30 units in x is attacked
// (mode 0x65), a fighter in status 0xBE further than 30 units away is approached (mode 0x66), else the CPU waits
// (mode 5) and, as fighter kind 0x2E near a target flagged 0x80, may use its up special (stick up + special) when
// nothing is above it; returns 1 in that case. HYPOTHESIS: names, the meaning of the flags, modes and actions.
// BYTES: defined at the end of the file for the same literal-pool reason as auto_passfall.
int aiInput::attention_check(Vec2f* stick, ipPadButton* button) {
    aiStat* stat = aiMgr::get_aiStat(&m_chrIdx);
    if ((stat->m_flags & 0x20000) || (stat->m_flags & 0x100)) {
        return 0;
    }
    switch (m_md) {
    case 1:
    case 2:
    case 6:
    case 11:
    case 12:
        if (stat->chk_stat_gr_free() || stat->chk_stat_air_free()) {
    s8 rank;
    int best = -1;
    bool other = false;
    float bestDist = 9999980000.0f;
    int i;
    for (i = 0; i < aiMgr::NumPlayers; i++) {
        aiChrIdx idx;
        idx.m_idx = i;
        aiStat* chr = aiMgr::get_aiStat(&idx);
        if (i == m_chrIdx.m_idx) {
            continue;
        }
        if (chr->m_flags & 0x8000) {
            continue;
        }
        if ((chr->m_team != stat->m_team && (chr->m_flags & 0x100)) || (chr->m_flags & 0x80) ||
            (chr->m_team != stat->m_team && chr->chk_chr_stat(1, 7))) {
            Vec3f diff = stat->m_center - chr->m_center;
            float dist = diff.m_z * diff.m_z + (diff.m_x * diff.m_x + diff.m_y * diff.m_y);
            if (bestDist > dist) {
                bestDist = dist;
                best = i;
            }
        } else if (chr->m_unkCC != stat->m_unkCC) {
            other = true;
        }
    }
    if (best < 0 && ((stat->m_flags & 0x200000) || (m_cpuType == 4 && randf() < 0.3f))) {
        best = get_near_target(&bestDist, 0);
        bestDist *= bestDist;
        other = m_cpuType == 4;
    }
    if (best >= 0 && bestDist < (other ? 900.0f : 10000.0f)) {
        rank = m_par.m_rank;
        if (randf() <= 0.015f * rank) {
            m_target.m_idx = best;
            aiStat* target = aiMgr::get_aiStat(&m_target);
            if (target->m_team != stat->m_team && target->chk_chr_stat(1, 7)) {
                if (__fabsf(stat->m_center.m_x - target->m_center.m_x) < 30.0f) {
                    if (stat->m_flags & 1) {
                        change_md(0x65, &m_target, stat->m_unkA8 > 0 ? 0x55 : 0x3060, 0);
                    } else {
                        change_md(0x65, &m_target, randf() < 0.7f ? 0x72 : 0x21, 0);
                    }
                    m_unk111 = 0;
                }
            } else if (target->m_status == 0xBE && stat->get_range(0.0f, &m_target) > 30.0f) {
                if (m_md != 2) {
                    change_md(0x66, &m_target, 0x3010, 0);
                }
            } else {
                aiChrIdx none;
                none.m_idx = -1;
                change_md(5, &none, 0, 0);
                m_flags |= 2;
                m_unk65 = 1.0 + 1.5 * randf();
                m_unk5C = 70;
                if (stat->m_chrKind == 0x2E && bestDist < 1600.0f) {
                    aiChrIdx idx;
                    idx.m_idx = best;
                    if (aiMgr::get_aiStat(&idx)->m_flags & 0x80) {
                        rank = m_par.m_rank;
                        if (randf() <= 0.005f * rank) {
                            Vec2f from;
                            Vec2f to(stat->m_pos.m_x, 100.0f + stat->m_pos.m_y);
                            float x = stat->m_pos.m_x;
                            float y = stat->m_pos.m_y;
                            from.m_x = x;
                            from.m_y = y;
                            if (!g_aiMgr->chk_cl_cross(&from, &to, 0xFF, NULL, NULL)) {
                                stick->m_y = 1.0f;
                                button->m_mask |= soController::getButtonMask(soController::Pad_Button_Special);
                                m_unk70 = 5.0f * randf();
                                return 1;
                            }
                        }
                    }
                }
            }
        }
    }
        }
        break;
    }
    return 0;
}

// Half of a character's width plus height, the reach snake_c4_check adds to the C4's radius. HYPOTHESIS: name.
static inline float aiHalfSize(aiChrIdx* idx) {
    float height = aiParam::get_height(idx);
    float width = aiParam::get_width(idx);
    return 0.5f * (width + height);
}

// Snake's C4 (weapon kind 0x18): in modes 1, 2, 5, 6 and 11, while the CPU can act over ground and its own C4 is
// planted at least 32 units beyond its reach, it detonates the C4 (stick down + special) when an opponent (another
// m_unkCC) comes within 5 units of it, at random by rank, then waits (mode 0) and returns 1. m_unk70 is the delay
// before the next check: 30-60 frames plus up to 50 more at a low rank after a detonation, 10-40 plus the same while
// idle. HYPOTHESIS: names; chk_chr_stat(1, -1) is taken to mean "is Snake"; the meaning of m_unkCC.
// BYTES: defined at the end of the file for the same literal-pool reason as auto_passfall.
int aiInput::snake_c4_check(Vec2f* stick, ipPadButton* button) {
    Vec2f diff;
    int i;
    aiStat* stat = aiMgr::get_aiStat(&m_chrIdx);
    if (stat->chk_chr_stat(1, -1)) {
        switch (m_md) {
        case 1:
        case 2:
        case 5:
        case 6:
        case 11:
            if ((stat->chk_stat_gr_free() || stat->chk_stat_air_free()) && stat->chk_over_ground()) {
                aiWeaponMgr* weaponMgr = g_aiMgr->m_weaponMgr;
                if (weaponMgr->search1(aiChrIdx::GetPlayerNo(&m_chrIdx), 0x18) > 0) {
                    aiWeapon* c4 = weaponMgr->enum_searched();
                    float size = aiHalfSize(&m_chrIdx);
                    diff.m_x = c4->m_pos.m_x - stat->m_center.m_x;
                    diff.m_y = c4->m_pos.m_y - stat->m_center.m_y;
                    float range = 32.0f + (c4->m_radius + size);
                    if (diff.m_x * diff.m_x + diff.m_y * diff.m_y >= range * range) {
                        if (m_unk70 > 0) {
                            m_unk70--;
                            return 0;
                        }
                        for (i = 0; i < aiMgr::NumPlayers; i++) {
                            aiChrIdx idx;
                            idx.m_idx = i;
                            aiStat* chr = aiMgr::get_aiStat(&idx);
                            if (i == m_chrIdx.m_idx || (chr->m_flags & 0x8000) || chr->m_unkCC == stat->m_unkCC) {
                                continue;
                            }
                            float size = aiHalfSize(&idx);
                            diff.m_x = c4->m_pos.m_x - chr->m_center.m_x;
                            diff.m_y = c4->m_pos.m_y - chr->m_center.m_y;
                            float range = 5.0f + (c4->m_radius + size);
                            if (diff.m_x * diff.m_x + diff.m_y * diff.m_y < range * range) {
                                s8 rank = m_par.m_rank;
                                if (100.0f * randf() <= rank) {
                                    stick->m_y = -1.0f;
                                    button->m_mask |= soController::getButtonMask(soController::Pad_Button_Special);
                                    rank = m_par.m_rank;
                                    m_unk70 = 30.0f + 30.0f * randf() + 0.5f * (100 - rank);
                                    aiChrIdx none;
                                    none.m_idx = -1;
                                    change_md(0, &none, 0, 0);
                                    return 1;
                                }
                                break;
                            }
                        }
                    }
                }
            }
            break;
        }
    }
    if (m_unk70 <= 0) {
        s8 rank = m_par.m_rank;
        m_unk70 = 10.0f + 30.0f * randf() + 0.5f * (100 - rank);
    }
    return 0;
}

// Danger avoidance, in modes 1, 2, 6, 11, 12, 13 and 20: falling in the air into a danger zone, jump (or, for the
// characters 0x12, 0x2E and 0x2F, special up); while the stage has danger zones and the CPU is in one, pick the nearest
// opponent at random by rank and run away (mode 5), except on the stages 7 and 0x33 where a gimmick close by or coming
// closer makes it start mode 2 with a dodge action; otherwise, when the target (characters 6 and 0x13) is in a
// matching state in front of it at the same height, start mode 1 with action 0x80 or 0x71. Always returns 0.
// HYPOTHESIS: names; the action numbers, the stage kinds and the meaning of m_unk35A/m_unk35C (taken to count the
// danger zones), aiStat::m_unkD4 and aiMgr::m_unk374/m_unk378.
// BYTES: defined at the end of the file for the same literal-pool reason as auto_passfall.
int aiInput::danger_check(Vec2f* stick, ipPadButton* button) {
    aiStat* stat = aiMgr::get_aiStat(&m_chrIdx);
    switch (m_md) {
    case 1:
    case 2:
    case 6:
    case 11:
    case 12:
    case 13:
    case 20:
        if (stat->chk_stat_air_free() && stat->m_speed.m_y < 0.0f &&
            aiMgr::chkDangerZone((Vec2f*)&stat->m_center, stat->m_team, 0x10))
        {
            if (stat->m_unkA8 > 0) {
                button->m_mask |= soController::getButtonMask(soController::Pad_Button_Jump);
            } else {
                switch (stat->m_chrKind) {
                case 0x12:
                case 0x2E:
                case 0x2F:
                    stick->m_y = 1.0f;
                    button->m_mask |= soController::getButtonMask(soController::Pad_Button_Special);
                    break;
                }
            }
        }
        bool zones = g_aiMgr->m_unk35A != 0 || g_aiMgr->m_unk35C != 0;
        if (zones && stat->chkDangerZoneAway() && (stat->chk_stat_gr_free() || stat->chk_stat_air_free())) {
            s8 rank = m_par.m_rank;
            if (randf() <= 0.02f * rank) {
                m_target.m_idx = get_near_target(NULL, 0);
                aiStat* target = aiMgr::get_aiStat(&m_target);
                switch (g_aiMgr->m_stageKind) {
                case 7:
                case 0x33: {
                    aiWeaponMgr* weaponMgr = g_aiMgr->m_weaponMgr;
                    if (weaponMgr->search_gimmic(&stat->m_pos, 100.0f) > 0) {
                        aiWeapon* gimmick = weaponMgr->near_searched((Vec2f*)&stat->m_pos);
                        float dx = gimmick->m_pos.m_x - stat->m_pos.m_x;
                        if (__fabsf(dx) < 30.0f || gimmick->m_velocity.m_x * dx < 0.0f) {
                            switch (g_aiMgr->m_stageKind) {
                            case 7: {
                                float r = randf();
                                u16 act = stat->chk_stat_gr_free() ? 0x71 : 0x3060;
                                if (r < 0.5f) {
                                    if (dx * (target->m_pos.m_x - stat->m_pos.m_x) < 0.0f) {
                                        act = 0x52;
                                    }
                                } else if (dx * stat->m_lr > 0.0f) {
                                    act = 0x55;
                                }
                                change_md(2, &m_target, act, 0);
                                return 0;
                            }
                            }
                            u16 act;
                            if (stat->m_unkA8 <= 0) {
                                act = stat->chk_stat_gr_free() ? 0x71 : 0x3060;
                            } else {
                                float r = randf();
                                act = 0x54;
                                if (r < 0.2f) {
                                    act = 0x80;
                                } else if (r < 0.5f) {
                                    if (dx * (target->m_pos.m_x - stat->m_pos.m_x) > 0.0f) {
                                        act = 0x52;
                                    }
                                } else if (r < 0.8f) {
                                    if (dx * stat->m_lr < 0.0f) {
                                        act = 0x55;
                                    }
                                }
                            }
                            change_md(2, &m_target, act, 0);
                            return 0;
                        }
                    }
                    break;
                }
                }
                if (!(stat->m_floor->m_flags & 0x1000) && target->m_floor == stat->m_floor &&
                    (stat->m_unkD4 < g_aiMgr->m_unk374 || stat->m_unkD4 == g_aiMgr->m_unk378))
                {
                    return 0;
                }
                aiChrIdx none;
                none.m_idx = -1;
                change_md(5, &none, 0, 0);
                m_flags |= 2;
                m_unk5C = 70;
            }
        } else {
            aiStat* target = aiMgr::get_aiStat(&m_target);
            switch (target->m_chrKind) {
            case 6:
            case 0x13:
                if (target->chk_chr_stat(2, -1) && (stat->chk_stat_gr_free() || stat->m_situation == 3)) {
                    float dy = __fabsf(stat->m_center.m_y - target->m_center.m_y);
                    if (dy < 0.6f * aiParam::get_height(&m_chrIdx)) {
                        float dx = target->m_lr * (stat->m_pos.m_x - target->m_pos.m_x);
                        if (dx > 1.0f && dx < 200.0f) {
                            s8 rank = m_par.m_rank;
                            if (randf() <= 0.0015f * rank) {
                                if (randf() < 0.4f && dx > 25.0f) {
                                    change_md(1, &m_target, 0x80, 0);
                                } else {
                                    change_md(1, &m_target, 0x71, 0);
                                }
                                return 0;
                            }
                        }
                    }
                }
                break;
            }
        }
        break;
    }
    return 0;
}

// Using the Final Smash (mode 0xF) once the CPU holds it (aiStat flags 0x400/0x800): with flag 0x800 at once; else,
// in the modes 1, 2, 5, 6, 11, 12 and 0x16 when no opponent (another m_unkCC) holds flag 0x20000, or in mode 3 for the
// characters listed there, depending on the character: when over the ground, when the target is close, when the
// target (led by its speed at a higher rank) is in front within reach at about the same height, or at once. The
// characters 1, 5 and 8 first walk to the centre block of a small stage (mode 0x16). m_finalWait delays the next
// check. Always returns 0. HYPOTHESIS: names; the meaning of the character groups and of the flags.
// BYTES: defined at the end of the file for the same literal-pool reason as auto_passfall.
int aiInput::final_check(Vec2f* stick, ipPadButton* button) {
    aiStat* stat = aiMgr::get_aiStat(&m_chrIdx);
    aiStat* target = aiMgr::get_aiStat(&m_target);
    if (stat->m_flags & 0x800) {
        aiChrIdx none;
        none.m_idx = -1;
        change_md(0xF, &none, 0, 0);
        m_step = 20;
    } else if (m_finalWait > 0) {
        m_finalWait--;
    } else {
    int checkSelf = 1;
    int checkTarget = 0;
    switch (m_md) {
    case 1:
    case 2:
    case 5:
    case 6:
    case 11:
    case 12:
    case 0x16: {
        int i;
        for (i = 0; i < aiMgr::NumPlayers; i++) {
            aiChrIdx idx;
            idx.m_idx = i;
            aiStat* chr = aiMgr::get_aiStat(&idx);
            if (i == m_chrIdx.m_idx || chr->m_unkCC == stat->m_unkCC || (chr->m_flags & 0x8000)) {
                continue;
            }
            if (chr->m_flags & 0x20000) {
                break;
            }
        }
        if (i < aiMgr::NumPlayers) {
            return 0;
        }
        switch (stat->m_chrKind) {
        case 0x11:
            checkTarget = 1;
            break;
        }
        break;
    }
    case 3:
        switch (stat->m_chrKind) {
        case 0x04:
        case 0x06:
        case 0x07:
        case 0x13:
        case 0x15:
        case 0x19:
        case 0x1B:
        case 0x21:
        case 0x2C:
        case 0x2E:
        case 0x2F:
            checkSelf = 0;
            break;
        case 0x02:
        case 0x0D:
        case 0x0E:
        case 0x11:
        case 0x16:
        case 0x22:
        case 0x29:
            checkSelf = 0;
            checkTarget = 1;
            break;
        default:
            return 0;
        }
        break;
    default:
        return 0;
    }
    if (stat->m_flags & 0xC00) {
    switch (stat->m_chrKind) {
    case 0x00:
    case 0x03:
        if (stat->chk_stat_gr_free() && m_md != 6 && stat->chk_over_ground()) {
            aiChrIdx none;
            none.m_idx = -1;
            change_md(0xF, &none, 0, 0);
            if (m_md == 0xF) {
                m_step = 10;
            }
        }
        break;
    case 0x01:
    case 0x05:
    case 0x08: {
        if (m_par.m_rank > 40 && aiMgr::isSmallMap()) {
            s8 rank = m_par.m_rank;
            if (randf() < 0.009f * rank) {
                AiClBlock* floor = stat->m_floor;
                float dist = __fabsf(0.5f * (floor->m_edgeL.m_x + floor->m_edgeR.m_x) - stat->m_pos.m_x);
                dist -= aiParam::get_width(&m_chrIdx);
                if (g_aiMgr->get_center_block(stat->m_team) != stat->m_floor || dist > 30.0f) {
                    if (m_md != 0x16) {
                        aiChrIdx none;
                        none.m_idx = -1;
                        change_md(0x16, &none, 0, 0);
                    }
                    m_finalWait = 10.0f + 10.0f * randf();
                    break;
                }
            }
        }
        if ((stat->chk_stat_gr_free() || stat->chk_stat_air_free() || stat->m_situation == 3) &&
            (!checkSelf || stat->chk_over_ground()) && (!checkTarget || target->chk_over_ground()) && stat->get_distance(0.0f, &m_target) < 30.0f) {
            aiChrIdx none;
            none.m_idx = -1;
            change_md(0xF, &none, 0, 0);
        }
        break;
    }
    case 0x02:
    case 0x09:
    case 0x0D:
    case 0x0E:
    case 0x11:
    case 0x14:
    case 0x16:
    case 0x1D:
    case 0x1E:
    case 0x1F:
    case 0x22:
    case 0x29:
        if ((stat->chk_stat_gr_free() || stat->chk_stat_air_free() || stat->m_situation == 3) &&
            (!checkSelf || stat->chk_over_ground()) && (!checkTarget || target->chk_over_ground())) {
            float lead = 30.0f;
            switch (stat->m_chrKind) {
            case 0x02:
            case 0x22:
            case 0x29:
                lead = 0.0f;
                break;
            case 0x0D:
            case 0x0E:
            case 0x16:
                lead = 8.0f;
                break;
            }
            Vec2f pos;
            if (m_par.m_rank >= 20) {
                float t = lead / 80.0f * (m_par.m_rank - 20);
                float leadX = target->m_speed.m_x * t;
                float leadY = target->m_speed.m_y * t;
                pos.m_x = leadX + target->m_center.m_x;
                pos.m_y = leadY + target->m_center.m_y;
            } else {
                pos.m_x = target->m_center.m_x;
                pos.m_y = target->m_center.m_y;
            }
            float targetWidth = aiParam::get_width(&m_target);
            float width = aiParam::get_width(&m_chrIdx);
            float reach = width + targetWidth;
            float dx = stat->m_lr * (pos.m_x - stat->m_center.m_x) - reach;
            float range;
            switch (stat->m_chrKind) {
            case 0x0D:
            case 0x0E:
            case 0x11:
            case 0x14:
                range = 150.0f;
                break;
            default:
                range = 55.0f;
                break;
            }
            if (dx < range && dx > 1.0f && __fabsf(stat->m_center.m_y - pos.m_y) < 0.7f * target->m_unkA4) {
                aiChrIdx none;
                none.m_idx = -1;
                change_md(0xF, &none, 0, 0);
            }
        }
        break;
    default:
        if (m_md != 6 && (stat->chk_stat_gr_free() || stat->chk_stat_air_free() || stat->m_situation == 3) &&
            (!checkSelf || stat->chk_over_ground()) && (!checkTarget || target->chk_over_ground())) {
            aiChrIdx none;
            none.m_idx = -1;
            change_md(0xF, &none, 0, 0);
        }
        break;
    }
    }
    }
    return 0;
}
