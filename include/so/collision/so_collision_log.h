#pragma once

// SHADOW of BrawlHeaders so/collision/so_collision_log.h (-I include comes first): the task category is a plain byte,
// not BrawlHeaders' enum bit-field (which loads with lwz/extract and builds logs through a word on the stack).

#include <StaticAssert.h>
#include <types.h>
#include <gf/gf_task.h>
#include <mt/mt_vector.h>

class soCollisionLog {
public:
    char _spacer[12];
    int m_taskId;
    Vec3f m_pos;
    short m_life;
    short m_30;
    s8 m_teamNo;
    // HYPOTHESIS: what the collision touched: 0 a hurtbox, 1 a target with its own reaction frame, 2 a shield, 3 a
    // reflector (every reader compares it unsigned).
    u8 m_kind;
    u8 m_taskCategory; // a gfTask::Category kept in a byte (every reader loads it with lbz)
    u8 _35; // HYPOTHESIS: nonzero for a valid collision part (ftUtil::isCounterShield)
    u8 m_collsionIndex;
    u8 m_damageIndex;
    char _38;
    bool m_isAbsolute;
    char _40;
    bool _41; // HYPOTHESIS: a flag (soSoundModuleImpl tests it against true)
    char _42;
    char _43[1];

}; // size 44
static_assert(sizeof(soCollisionLog) == 44, "Class is wrong size!"); // size 44
