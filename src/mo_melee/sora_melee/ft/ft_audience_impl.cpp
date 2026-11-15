#include <ft/ft_audience_impl.h>
#include <ft/ft_external_value_accesser.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <gr/collision/gr_collision_manager.h>
#include <snd/snd_system.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/so_external_value_accesser.h>
#include <types.h>

extern grCollisionManager* lbl_805A0360;
// HYPOTHESIS: ftExternalValueAccesser::getCallFgm (unnamed in sora_melee): the sound id the audience calls the fighter with.
extern "C" u32 fn_27_14D4E4(Fighter* fighter);

// HYPOTHESIS: three sound ids per handclap level; only the middle one is played here.
struct ftAudienceHandclapSe {
    SndID m_se0;
    SndID m_se;
    SndID m_se2;
};
static const ftAudienceHandclapSe s_handclapSe[3] = {
    { (SndID)0x1C, (SndID)0x1F, (SndID)0x19 },
    { (SndID)0x1B, (SndID)0x1E, (SndID)0x18 },
    { (SndID)0x1A, (SndID)0x1D, (SndID)0x17 },
};

static inline void stopAudienceSe(s32& handle) {
    if (handle != -1 && g_sndSystem->isPlay(handle) == true) {
        g_sndSystem->stopSE(handle, (s32)0);
    }
    handle = -1;
}

ftAudienceImpl::ftAudienceImpl() : gfTask("ftAudience", Category_None, 0, 0xd, true) {
    m_data = nullptr;
    m_soundGroup = g_sndSystem->unk6E8 = (u16)(g_sndSystem->unk6E8 + 1);
    m_isActive = false;
}

ftAudienceImpl::~ftAudienceImpl() {
    if (m_isActive == true) {
        deactivate();
    }
}

void ftAudienceImpl::activate() {
    m_data = reinterpret_cast<ftAudienceData*>(ftExternalValueAccesser::getAudienceData());
    m_48 = -1;
    m_frame = 0x7fffffff;
    m_50 = 0.0f;
    m_callEntryId = -1;
    m_tumultTimer = m_data->m_tumultInterval;
    m_callSeId = -1;
    m_callCount = m_data->m_callCountMax;
    m_fallCount = 0;
    m_cheerDefeatSe = -1;
    m_handclapSe = -1;
    m_tumultSe = -1;
    m_callSe = -1;
    m_7d = false;
    m_7e = false;
    m_isActive = true;
}

void ftAudienceImpl::deactivate() {
    g_sndSystem->stopGroup(m_soundGroup, 0);
    m_isActive = false;
}

// Plays the tumult of the level; a fighter that the audience is calling gets the call answered.
static inline void reqTumult(ftAudienceImpl* audience, int level, int entryId) {
    stopAudienceSe(audience->m_tumultSe);
    audience->m_tumultSe = g_sndSystem->playSE(s_handclapSe[level].m_se2, audience->m_soundGroup, 0, 0, -1);
    if (entryId != -1) {
        if (audience->m_callEntryId == entryId && audience->m_callCount < audience->m_data->m_callCountMax &&
            audience->m_callCount >= audience->m_data->m_callCountMin) {
            audience->m_7d = true;
        }
        g_ftManager->getOwner(entryId)->addLogInt(1, 0x3c, 0);
    }
}

// While the call runs, each finished call starts the next one, until the fighter answers or the calls run out.
void ftAudienceImpl::updateCall() {
    if (m_callCount >= m_data->m_callCountMax) {
        if (m_tumultTimer < m_data->m_tumultInterval) {
            m_tumultTimer++;
        }
    } else if (m_callSe != -1 && g_sndSystem->isPlay(m_callSe) == false) {
        m_callCount++;
        if (m_callCount < m_data->m_callCountMax) {
            if (m_7d == true) {
                m_7d = false;
                m_tumultTimer = 0;
                m_callCount = m_data->m_callCountMax;
                stopAudienceSe(m_callSe);
                if (m_7e == true) {
                    stopAudienceSe(m_cheerDefeatSe);
                    m_cheerDefeatSe = g_sndSystem->playSE((SndID)0x1A, m_soundGroup, 0, 0, -1);
                    m_7e = false;
                }
            } else {
                m_callSe = g_sndSystem->playSE((SndID)m_callSeId, m_soundGroup, 0, 0, -1);
            }
        } else {
            m_tumultTimer = 0;
            stopAudienceSe(m_callSe);
            stopAudienceSe(m_handclapSe);
            m_handclapSe = g_sndSystem->playSE((SndID)0x1D, m_soundGroup, 0, 0, -1);
        }
    }
}

// Counts the fighters below the floor; the tumult starts when enough of them have fallen.
void ftAudienceImpl::updateTumult() {
    // BYTES: declared in this order for the original registers.
    grCollisionManager* collisionManager;
    int prevFallCount = m_fallCount;
    bool isCallFighterUp = false;
    m_fallCount = 0;
    collisionManager = lbl_805A0360;
    for (int i = 0; i < g_ftManager->getEntryCount(); i++) {
        int entryId = g_ftManager->getEntryIdFromIndex(i);
        if (g_ftManager->isFighterActivate(entryId, -1) == true && g_ftManager->isFighterPositionAccessable(entryId, -1) == true) {
            Vec3f pos = soExternalValueAccesser::getPos(g_ftManager->getFighter(entryId, -1));
            if (pos.m_y < collisionManager->m_floorRange.m_down + m_data->m_tumultFallHeight) {
                m_fallCount++;
            } else if (entryId == m_callEntryId) {
                isCallFighterUp = true;
            }
        }
    }
    if (prevFallCount < m_data->m_tumultFallCount && m_fallCount >= m_data->m_tumultFallCount) {
        if (isCallFighterUp == true) {
            reqTumult(this, 2, m_callEntryId);
        } else {
            reqTumult(this, 2, -1);
        }
    }
}

void ftAudienceImpl::processFixCamera() {
    if (m_isActive == true) {
        if (m_frame < 0x7fffffff) {
            m_frame++;
        }
        updateCall();
        updateTumult();
    }
}

float ftAudienceImpl::correctCheerReaction(float reaction, float damage) {
    if (damage > m_data->m_cheerReactionMin && damage < m_data->m_cheerReactionMax) {
        return reaction * m_data->m_cheerReactionMul;
    }
    return reaction;
}

void ftAudienceImpl::reqCallFgmStopCheer() {
    if (m_callCount < m_data->m_callCountMax) {
        m_7d = true;
        m_7e = true;
    }
}

// True when the position is within the margin of either end of the floor.
bool ftAudienceImpl::checkTumultRange(float x) {
    float margin = m_data->m_tumultEdgeMargin;
    if (x < lbl_805A0360->m_floorRange.m_left + margin || x > lbl_805A0360->m_floorRange.m_right - margin) {
        return true;
    }
    return false;
}

// A damaged fighter the audience is not calling yet, and not a computer one, starts a call once the tumult has rested.
bool ftAudienceImpl::checkCall(float damage, int entryId) {
    if (g_ftManager->isFighterActivate(entryId, -1) == false || m_callEntryId == entryId ||
        (s8)g_ftManager->getFighterOperationType(entryId) == 1) {
        return false;
    }
    // BYTES: declared in this order for the original registers.
    ftAudienceData* data;
    Fighter* fighter = g_ftManager->getFighter(entryId, -1);
    data = m_data;
    if (ftExternalValueAccesser::getDamageModule(fighter)->getDamage(0) < data->m_callDamage ||
        m_tumultTimer < data->m_tumultInterval) {
        return false;
    }
    m_callSeId = fn_27_14D4E4(fighter);
    if (m_callSeId == 0xFFFFFFFF) {
        return false;
    }
    stopAudienceSe(m_callSe);
    int level;
    if (damage >= m_data->m_handclapDamage2) {
        level = 2;
    } else if (damage >= m_data->m_handclapDamage1) {
        level = 1;
    } else {
        level = -(!(damage >= m_data->m_handclapDamage0));
    }
    // MATCH-ONLY: the id is 0x1D at the top level and 0x1E otherwise; this spells out the original branch-free form.
    m_callSe = g_sndSystem->playSE((SndID)(0x1E + (~(level - 2 | 2 - level) >> 31)), m_soundGroup, 0, 0, -1);
    m_callEntryId = entryId;
    m_callCount = 0;
    return true;
}

// The handclap grows with the damage of the dead fighter; it is one level quieter unless the flag is set.
void ftAudienceImpl::reqHandclapDead(float damage, bool isLoud) {
    int level = -1;
    if (damage >= m_data->m_handclapDamage2) {
        level = 2;
    } else if (damage >= m_data->m_handclapDamage1) {
        level = 1;
    } else if (damage > 0.0f) {
        level = 0;
    }
    if (level != -1) {
        if (!isLoud && level > 0) {
            level--;
        }
        stopAudienceSe(m_handclapSe);
        m_handclapSe = g_sndSystem->playSE(s_handclapSe[level].m_se, m_soundGroup, 0, 0, -1);
    }
}

bool ftAudienceImpl::checkTumultLanding(float x, float damage, int entryId) {
    if (checkTumultRange(x) == true) {
        int level;
        if (damage >= m_data->m_handclapDamage2) {
            level = 2;
        } else if (damage >= m_data->m_handclapDamage1) {
            level = 1;
        } else {
            level = -(!(damage >= m_data->m_handclapDamage0));
        }
        if (level == -1) {
            return false;
        }
        reqTumult(this, level, entryId);
        return true;
    }
    return false;
}

bool ftAudienceImpl::checkTumultFallSpecial(float y, int entryId) {
    float down = lbl_805A0360->m_floorRange.m_down;
    if (y > down || y < down + m_data->m_fallHeightMin) {
        return false;
    }
    int level;
    if (y > down + m_data->m_fallHeight2) {
        level = 2;
    } else {
        level = y > down + m_data->m_fallHeight1;
    }
    if (level == -1) {
        return false;
    }
    reqTumult(this, level, entryId);
    return true;
}

// HYPOTHESIS: ftExternalValueAccesser::getReactionCheer (unnamed in sora_melee).
extern "C" float fn_27_14D5C0(Fighter* fighter);

static inline int getCheerLevel(const ftAudienceData* data, float reaction) {
    int level;
    if (reaction >= data->m_handclapDamage2) {
        level = 2;
    } else if (reaction >= data->m_handclapDamage1) {
        level = 1;
    } else {
        level = -(!(reaction >= data->m_handclapDamage0));
    }
    return level;
}

// A big hit is cheered: the call starts for a strong attacker, a combo of the same attacker claps louder, and
// any other hit gets the plain cheer.
void ftAudienceImpl::checkCheer(float, int attackerEntryId, int victimEntryId) {
    float reaction = fn_27_14D5C0(g_ftManager->getFighter(victimEntryId, -1));
    int level = getCheerLevel(m_data, reaction);
    if (level == -1) {
        return;
    }
    if (g_ftManager->isFighterActivate(attackerEntryId, -1) == true &&
        getCheerLevel(m_data, fn_27_14D5C0(g_ftManager->getFighter(attackerEntryId, -1))) >= 2) {
        int handclapLevel = getCheerLevel(m_data, reaction);
        if (handclapLevel >= 1 && checkCall(reaction, attackerEntryId)) {
            stopAudienceSe(m_cheerDefeatSe);
        } else {
            stopAudienceSe(m_handclapSe);
            m_handclapSe = g_sndSystem->playSE(s_handclapSe[handclapLevel].m_se, m_soundGroup, 0, 0, -1);
            if (attackerEntryId != -1 && m_callEntryId == attackerEntryId && m_callCount < m_data->m_callCountMax &&
                m_callCount >= m_data->m_callCountMin) {
                m_7d = true;
            }
        }
    } else if (m_48 == attackerEntryId && m_frame < m_data->m_comboFrame) {
        float comboReaction = m_50;
        if (reaction > comboReaction) {
            comboReaction = reaction;
        }
        int handclapLevel = getCheerLevel(m_data, comboReaction);
        if (handclapLevel >= 1 && checkCall(comboReaction, attackerEntryId)) {
            stopAudienceSe(m_cheerDefeatSe);
        } else {
            stopAudienceSe(m_handclapSe);
            m_handclapSe = g_sndSystem->playSE(s_handclapSe[handclapLevel].m_se, m_soundGroup, 0, 0, -1);
            if (attackerEntryId != -1 && m_callEntryId == attackerEntryId && m_callCount < m_data->m_callCountMax &&
                m_callCount >= m_data->m_callCountMin) {
                m_7d = true;
            }
        }
    } else {
        stopAudienceSe(m_cheerDefeatSe);
        m_cheerDefeatSe = g_sndSystem->playSE(s_handclapSe[level].m_se0, m_soundGroup, 0, 0, -1);
        if (level == 2 || (level == 1 && m_callEntryId == victimEntryId)) {
            if (m_callCount < m_data->m_callCountMax && m_callCount >= m_data->m_callCountMin) {
                m_7d = true;
            }
        }
    }
    m_48 = attackerEntryId;
    m_frame = 0;
    m_50 = reaction;
}

void ftAudienceImpl::checkCheerDefeat(int entryId) {
    if ((int)g_ftManager->getOwner(entryId)->getCheerDefeatFrame() > 0) {
        stopAudienceSe(m_cheerDefeatSe);
        m_cheerDefeatSe = g_sndSystem->playSE((SndID)0x1A, m_soundGroup, 0, 0, -1);
    }
}
