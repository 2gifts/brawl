#pragma once

#include <StaticAssert.h>
#include <gr/gr_gimmick.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <st/st_trigger.h>
#include <types.h>

// GTIK record. HYPOTHESIS: the field names come from their use in grAdventureTikuwa (a crumbling "chikuwa" floor).
struct grAdventureTikuwaData {
    Vec2f m_pos;                // 0x0
    float m_rotZ;               // 0x8
    float m_fallSpeed;          // 0xC
    float m_fallAccel;          // 0x10
    u16 m_waitFrames;           // 0x14, from the difficulty table
    u16 m_shakeFrames;          // 0x16
    float m_shakeRange;         // 0x18
    u16 m_fallFrames;           // 0x1C
    u16 m_respawnFrames;        // 0x1E, 0: never comes back
    u8 unk20[0xC];
    stTriggerData m_trigger;    // 0x2C
};
static_assert(sizeof(grAdventureTikuwaData) == 0x30, "Class is wrong size!");

class grAdventureTikuwa : public grGimmick {
    u8 m_state;                     // 0x14C: 0 idle, 1 waiting, 2 shaking, 3 falling, 4 gone, 5 reappearing
    u8 m_isLanding;                 // 0x14D
    float m_timer;                  // 0x150
    float m_speedY;                 // 0x154
    grAdventureTikuwaData* m_data;  // 0x158
    float m_blinkTimer;             // 0x15C
    u32 m_blinkFrames;              // 0x160

public:
    grAdventureTikuwa(const char* taskName) : grGimmick(taskName) {
        m_state = 0;
        m_timer = 0.0f;
        m_speedY = 0.0f;
        m_data = NULL;
    }
    virtual ~grAdventureTikuwa();
    virtual void update(float deltaFrame);
    virtual void setVisibility(u32 unk1);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);

    static grAdventureTikuwa* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grAdventureTikuwa) == 0x164, "Class is wrong size!");

