#define MT_VEC3F_SUB_COMPACT
#include <ai/ai_weapon.h>
#include <gf/gf_memory_util.h>
#include <math.h>
#include <mt/mt_common.h>

int aiWeapon::getWnKind() {
    if (m_type != 0) {
        return -1;
    }
    return m_kind;
}

int aiWeapon::getItKind() {
    if (m_type != 1 && m_type != 4 && m_type != 5) {
        return -1;
    }
    return m_kind;
}

aiWeaponMgr::aiWeaponMgr() {
    gfMemFill(this, 0, sizeof(aiWeaponMgr));
}

aiWeaponMgr::~aiWeaponMgr() { }

void aiWeaponMgr::reset() {
    gfMemFill(this, 0, sizeof(aiWeaponMgr));
}

// Refresh a CPU's remembered weapons by object ID, keeping each mark's delay
// across pointer refreshes. Unseen marks become available on the next refresh.
void aiWeaponMgr::mark_wpn_update(aiWeaponMark* marks, int count, u8* seen, int frames,
                                  Vec3f* pos, float radius, s8 excludedTeam) {
    for (int i = 0; i < count; i++) {
        seen[i] = 0;
    }
    if (search(pos, radius, 0, excludedTeam) > 0) {
        for (;;) {
            aiWeapon* weapon = enum_searched();
            if (weapon == NULL) {
                break;
            }
            int i;
            aiWeaponMark* mark = marks;
            for (i = 0; i < count; mark++, i++) {
                if (mark->m_weapon != NULL && weapon->m_objId == mark->m_objId) {
                    marks[i].m_weapon = weapon;
                    if (marks[i].m_frames > 0) {
                        marks[i].m_frames--;
                    }
                    seen[i] = 1;
                    break;
                }
            }
            if (i >= count) {
                mark = marks;
                for (i = 0; i < count; mark++, i++) {
                    if (mark->m_weapon == NULL) {
                        marks[i].m_objId = weapon->m_objId;
                        marks[i].m_frames = frames;
                        marks[i].m_weapon = weapon;
                        if (marks[i].m_frames > 0) {
                            marks[i].m_frames--;
                        }
                        seen[i] = 1;
                        break;
                    }
                }
            }
        }
    }
    for (int i = 0; i < count; i++) {
        if (!seen[i] && marks[i].m_weapon != NULL) {
            marks[i].m_weapon = NULL;
        }
    }
}

int aiWeaponMgr::search(Vec3f* pos, float radius, int mode, s8 excludedTeam) {
    m_enumCount = 0;
    m_enumCursor = 0;
    if (m_count <= 0) {
        m_searchedCount = 0;
        return 0;
    }
    gfMemFill(m_searchedBits, 0, sizeof(m_searchedBits));
    float radiusSq = radius * radius;
    int i;
    aiWeapon* weapon = m_weapons;
    int count = 0;
    for (i = 0; i < 64; i++, weapon++) {
        if ((weapon->m_flags & 1) == 0) {
            continue;
        }
        if (mode != 0) {
            if (weapon->m_type != 4) {
                continue;
            }
            if ((weapon->m_flags & 8) == 0) {
                continue;
            }
        } else {
            if (weapon->m_type == 4 || weapon->m_type == 5) {
                continue;
            }
            if (excludedTeam >= 0 && weapon->m_team == excludedTeam) {
                continue;
            }
        }
        Vec3f diff;
        Vec3fSub(&diff, pos, &weapon->m_pos);
        float distanceSq = diff.m_z * diff.m_z + (diff.m_x * diff.m_x + diff.m_y * diff.m_y);
        // Preserve the native comparison: unordered distances also pass here.
        if (distanceSq > radiusSq) {
            continue;
        }
        count++;
        m_searchedBits[i / 8] |= 1 << (i % 8);
    }
    m_searchedCount = count;
    return count;
}

// Used by Snake's C4 decision among other callers: a negative owner accepts
// every player, and weapon kind -1 accepts every weapon kind.
int aiWeaponMgr::search1(s8 ownerPlayer, int wnKind) {
    m_enumCount = 0;
    m_enumCursor = 0;
    if (m_count <= 0) {
        m_searchedCount = 0;
        return 0;
    }
    gfMemFill(m_searchedBits, 0, sizeof(m_searchedBits));
    int i;
    aiWeapon* weapon = m_weapons;
    int count = 0;
    for (i = 0; i < 64; i++, weapon++) {
        if ((weapon->m_flags & 1) == 0 || weapon->m_type != 0) {
            continue;
        }
        if (ownerPlayer >= 0 && ownerPlayer != weapon->m_ownerPlayer) {
            continue;
        }
        if (wnKind != -1 && wnKind != weapon->m_kind) {
            continue;
        }
        count++;
        m_searchedBits[i / 8] |= 1 << (i % 8);
    }
    m_searchedCount = count;
    return count;
}

int aiWeaponMgr::search_gimmic(Vec3f* pos, float radius) {
    m_enumCount = 0;
    m_enumCursor = 0;
    if (m_count <= 0) {
        m_searchedCount = 0;
        return 0;
    }
    gfMemFill(m_searchedBits, 0, sizeof(m_searchedBits));
    float radiusSq = radius * radius;
    int i;
    aiWeapon* weapon = m_weapons;
    int count = 0;
    for (i = 0; i < 64; i++, weapon++) {
        if ((weapon->m_flags & 1) == 0 || weapon->m_type != 2) {
            continue;
        }
        Vec3f diff;
        Vec3fSub(&diff, pos, &weapon->m_pos);
        float distanceSq = diff.m_z * diff.m_z + (diff.m_x * diff.m_x + diff.m_y * diff.m_y);
        if (distanceSq > radiusSq) {
            continue;
        }
        count++;
        m_searchedBits[i / 8] |= 1 << (i % 8);
    }
    m_searchedCount = count;
    return count;
}

// A requested kind of -1 also matches the non-item sentinel from getItKind().
int aiWeaponMgr::search_item(int itKind) {
    m_enumCount = 0;
    m_enumCursor = 0;
    if (m_count <= 0) {
        m_searchedCount = 0;
        return 0;
    }
    gfMemFill(m_searchedBits, 0, sizeof(m_searchedBits));
    int i;
    aiWeapon* weapon = m_weapons;
    int count = 0;
    for (i = 0; i < 64; i++, weapon++) {
        if ((weapon->m_flags & 1) != 0 && itKind == weapon->getItKind()) {
            count++;
            m_searchedBits[i / 8] |= 1 << (i % 8);
        }
    }
    m_searchedCount = count;
    return count;
}

aiWeapon* aiWeaponMgr::enum_searched() {
    if (m_enumCount < m_searchedCount) {
        for (int i = m_enumCursor; i < 8; i++) {
            if (m_searchedBits[i] != 0) {
                for (int bit = 0; bit < 8; bit++) {
                    if ((1 << bit & m_searchedBits[i]) != 0) {
                        m_searchedBits[i] &= ~(1 << bit);
                        m_enumCount++;
                        return &m_weapons[bit + i * 8];
                    }
                }
            }
            m_enumCursor++;
        }
    }
    return NULL;
}

aiWeapon* aiWeaponMgr::near_searched(Vec2f* pos) {
    aiWeapon* nearest = NULL;
    float nearestDist = 9999979520.0f;
    for (;;) {
        aiWeapon* weapon = enum_searched();
        if (weapon == NULL) {
            break;
        }
        Vec2f diff;
        diff.m_x = pos->m_x - weapon->m_pos.m_x;
        diff.m_y = pos->m_y - weapon->m_pos.m_y;
        float dist = diff.m_x * diff.m_x + diff.m_y * diff.m_y;
        if (nearestDist > dist) {
            nearest = weapon;
            nearestDist = dist;
        }
    }
    return nearest;
}

aiWeapon* aiWeaponMgr::getWeapon(u32 objId) {
    if (m_count > 0) {
        aiWeapon* weapon = m_weapons;
        for (int i = 0; i < 64; i++, weapon++) {
            if ((weapon->m_flags & 1) != 0 && weapon->m_objId == objId) {
                return weapon;
            }
        }
    }
    return NULL;
}

// The prediction core applies this independently to the X and Y axes, then
// intersects the candidate time intervals. Its half-frame terms and sentinels
// are part of the native discrete-motion calculation.
// HYPOTHESIS: aiweapon is a namespace rather than a class with static helpers;
// the map spelling and the absence of a receiver establish the calling ABI.
namespace aiweapon {
int calc_acc(float target, float velocity, float acceleration, float radius, float position,
             float* entry0, float* exit0, float* entry1, float* exit1) {
    float distance = target - position;
    float absAcceleration = __fabsf(acceleration);
    if (absAcceleration > 0.001f) {
        float turn = -(velocity / acceleration);
        if (acceleration < 0.0f) {
            distance *= -1.0f;
            velocity *= -1.0f;
            acceleration *= -1.0f;
        }
        float h = 0.5f + turn;
        float offset = velocity * turn + (0.5f * (h * (acceleration * h)) - 0.125f * acceleration);
        float nearValue = 0.125f * acceleration + ((distance - radius) - offset);
        float nearRoot;
        if (nearValue < 0.0f) {
            nearRoot = -999999.0f;
        } else {
            nearRoot = mtSqrtf(nearValue / (0.5f * acceleration)) - 0.5f;
        }
        float farValue = 0.125f * acceleration + ((distance + radius) - offset);
        float farRoot;
        if (farValue < 0.0f) {
            farRoot = -999999.0f;
        } else {
            farRoot = mtSqrtf(farValue / (0.5f * acceleration)) - 0.5f;
        }
        *entry0 = turn - farRoot;
        *exit0 = turn - nearRoot;
        *entry1 = turn + nearRoot;
        *exit1 = turn + farRoot;
        return 1;
    }
    float absVelocity = __fabsf(velocity);
    if (absVelocity > 0.001f) {
        if (velocity < 0.0f) {
            distance *= -1.0f;
            velocity *= -1.0f;
        }
        *entry0 = (distance - radius) / velocity;
        *exit0 = (distance + radius) / velocity;
        *entry1 = 99999.0f;
        *exit1 = 99999.0f;
        return 1;
    }
    return 0;
}
}
