#pragma once

#include <StaticAssert.h>
#include <gr/gr_gimmick.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <st/st_trigger.h>
#include <types.h>

// GELA record (BrawlCrate GELAEntry, 0x2C bytes). HYPOTHESIS: the field names come from their use in grAdventureElasticity.
struct grAdventureElasticityData {
    Vec2f m_pos;                // 0x0
    float m_rotZ;               // 0x8
    float m_bounceSpeed;        // 0xC, start speed of a bounce (negative = downwards)
    float m_gravity;            // 0x10
    float m_stopThreshold;      // 0x14
    int m_soundId;              // 0x18, played while a fighter stands on it
    u8 unk1C[4];
    int m_soundId2;             // 0x20, played while it springs back
    u8 m_isPlayerOneOnly;           // 0x24, nonzero: only player 1 triggers it
    u8 unk25[3];
    stTriggerData m_trigger;    // 0x28
};
static_assert(sizeof(grAdventureElasticityData) == 0x2C, "Class is wrong size!");

// A floor that gives way under a fighter and springs back (GELA).
class grAdventureElasticity : public grGimmick {
    u8 m_isLanding;                     // 0x14C, a fighter stands on it this frame
    u8 m_wasLanding;                    // 0x14D
    u8 m_isMoving;                      // 0x14E
    float m_offsetY;                    // 0x150
    float m_speedY;                     // 0x154
    grAdventureElasticityData* m_data;  // 0x158

public:
    grAdventureElasticity(const char* taskName) : grGimmick(taskName) {
        m_isLanding = 0;
        m_wasLanding = 0;
        m_isMoving = 0;
        m_offsetY = 0.0f;
        m_speedY = 0.0f;
        m_data = NULL;
    }
    virtual ~grAdventureElasticity();
    virtual void update(float deltaFrame);
    virtual void setVisibility(u32 unk1);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);

    static grAdventureElasticity* create(int mdlIndex, const char* taskName);

private:
    void setSoundEffect(int index, int soundId, Vec2f offset);
};
static_assert(sizeof(grAdventureElasticity) == 0x15C, "Class is wrong size!");

