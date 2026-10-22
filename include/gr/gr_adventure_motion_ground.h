#pragma once

#include <StaticAssert.h>
#include <gf/gf_callback.h>
#include <gr/gr_yakumono.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <st/st_trigger.h>
#include <types.h>

// HYPOTHESIS: one sound or effect slot of the gimmick record; the field names follow grGimmick::Effect_Info, which
// the slot is copied into (an id of 0 or less leaves the slot unused, an effect node index of 0 means "effect_locator").
struct grAdventureMotionGroundSEData {
    int m_id;             // 0x0
    u16 m_repeatFrame;    // 0x4
    u16 m_nodeIndex;      // 0x6
    u16 m_endFrame;       // 0x8
    u16 unkA;
};
static_assert(sizeof(grAdventureMotionGroundSEData) == 0xC, "Class is wrong size!");

// HYPOTHESIS: the type name is from the sora_adv_stage map (createGimmickFunc<25grGimmickMotionGroundData, ...>); the
// field names come from their use in grAdventureMotionGround: an animation speed per difficulty level,
// production flags (1: material-motion productions, 2: fade production), and the sound/effect slots of the default
// (0-2), moving (3-7) and end (8-10) phases.
struct grGimmickMotionGroundData {
    float m_motionRatios[15];                         // 0x0
    u8 unk3C[2];
    u8 m_productionFlags;                             // 0x3E
    u8 m_matAnimIndex;                                // 0x3F
    grAdventureMotionGroundSEData m_defaultSE[3];     // 0x40
    grAdventureMotionGroundSEData m_defaultEffect[3]; // 0x64
    u8 m_seProduction0[0x24];                         // 0x88
    grGimmick::SimpleEffectData m_effectProduction0[3]; // 0xAC
    u8 m_seProduction1[0x24];                         // 0xD0
    grGimmick::SimpleEffectData m_effectProduction1[3]; // 0xF4
    grAdventureMotionGroundSEData m_moveSE[5];        // 0x118
    grAdventureMotionGroundSEData m_moveEffect[5];    // 0x154
    grAdventureMotionGroundSEData m_endSE[3];         // 0x190
    grAdventureMotionGroundSEData m_endEffect[3];     // 0x1B4
    stTriggerData m_isValidTrigger;                   // 0x1D8
    stTriggerData m_motionTrigger;                    // 0x1DC
    stTriggerData m_landingTrigger;                   // 0x1E0
};
static_assert(sizeof(grGimmickMotionGroundData) == 0x1E4, "Class is wrong size!");

// HYPOTHESIS: the layout that the inline constructors of these st_trigger_actor_common.o callbacks fill (class names
// from the sora_melee map and the RTTI of their vtables).
class stActTriggerMotionBind : public stTriggerCB {
    u32 unk8;
    grGimmick* m_gimmick;  // 0xC
    u32 unk10;
    u8 unk14;
    float m_motionRatio;   // 0x18
    float unk1C;
    u32 unk20;

public:
    stActTriggerMotionBind(grGimmick* gimmick, float motionRatio) {
        unk8 = 0;
        m_gimmick = gimmick;
        unk10 = 0;
        unk14 = 1;
        m_motionRatio = motionRatio;
        unk1C = 0.0f;
    }
    virtual void userProc();
    virtual ~stActTriggerMotionBind();
};
static_assert(sizeof(stActTriggerMotionBind) == 0x24, "Class is wrong size!");

class stActTriggerMotionStop : public stTriggerCB {
    u32 unk8;
    grGimmick* m_gimmick;  // 0xC
    u32 unk10;
    u8 unk14;
    float m_motionRatio;   // 0x18
    u32 unk1C;

public:
    stActTriggerMotionStop(grGimmick* gimmick, float motionRatio) {
        unk8 = 0;
        m_gimmick = gimmick;
        unk10 = 0;
        unk14 = 1;
        m_motionRatio = motionRatio;
    }
    virtual void userProc();
    virtual ~stActTriggerMotionStop();
};
static_assert(sizeof(stActTriggerMotionStop) == 0x20, "Class is wrong size!");

class grAdventureMotionGround : public grYakumono {
protected:
    grGimmickMotionGroundData* m_data; // 0x150

public:
    grAdventureMotionGround(const char* taskName);
    virtual ~grAdventureMotionGround();
    virtual void update(float deltaFrame);
    virtual void setVisibility(u32 unk1);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void setInitializeFlag();
    virtual void changeNodeAnim(u32 chrIndex, u32 modelAnimIndex);
    virtual void dbDispInvalidatedByCameraClippingSphere();
    virtual void setMotionAttack();

    static grAdventureMotionGround* create(int mdlIndex, const char* taskName);

    void updateSEandEffect();
    void defaultSE();
    void defaultEffect();
    void startMoveSE();
    void startMoveEffect();
    void endMoveSE();
    void endMoveEffect();
};
static_assert(sizeof(grAdventureMotionGround) == 0x154, "Class is wrong size!");
