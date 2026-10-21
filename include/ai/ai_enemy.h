#pragma once

#include <mt/mt_vector.h>
#include <types.h>

// A stable CPU target slot, used for Subspace enemies and WarioWare targets.
// Layout verified through the manager's allocation/retirement and position updates.
class aiEnemy {
public:
    enum Flags {
        Active = 1,
        GroundTouch = 2,
        Retired = 4,
    };

    u32 m_objId;      // 0x00: enemy create ID, or WarioWare target index + 1
    u8 m_index;       // 0x04: slot number in aiEnemyMgr
    u8 unk5;
    u16 m_flags;      // 0x06: retired slots keep Active until the next manager update
    int m_status;     // 0x08: status kind
    Vec3f m_pos;      // 0x0C
    Vec2f m_move;     // 0x18: position change since the last update
    float m_lr;       // 0x20

    aiEnemy() {
        m_flags = 0;
    }
    void init(u32 objId) {
        m_objId = objId;
        m_flags = Active;
        m_status = -1;
        m_pos = Vec3f(0.0f, 0.0f, 0.0f);
        m_move = Vec2f(0.0f, 0.0f);
        m_lr = 0.0f;
    }
    // Stage targets have positions but no Enemy object or status process.
    void update(Vec3f* pos) {
        m_flags = Active;
        m_status = -1;
        Vec2f move;
        move.m_x = m_pos.m_x;
        move.m_y = m_pos.m_y;
        m_pos = *pos;
        move.m_x = m_pos.m_x - move.m_x;
        move.m_y = m_pos.m_y - move.m_y;
        m_move = move;
        m_lr = 1.0f;
    }
    // MATCH-ONLY: the manager retains both native out-of-line refresh calls.
    void update() __attribute__((never_inline));
};
static_assert(sizeof(aiEnemy) == 0x24, "Class is wrong size!");

class aiEnemyMgr {
public:
    aiEnemy m_enemies[32];  // 0x000
    int m_count;            // 0x480: number of active enemies

    aiEnemyMgr();
    ~aiEnemyMgr();
    void reset();
    void update();
    aiEnemy* getEnemy(int index);
    aiEnemy* getEnemyNear(Vec3f* pos, float* distanceSq);
};
static_assert(sizeof(aiEnemyMgr) == 0x484, "Class is wrong size!");
