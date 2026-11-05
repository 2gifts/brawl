#pragma once

#include <StaticAssert.h>
#include <gr/gr_gimmick.h>
#include <types.h>

extern const char lbl_40_data_6380[15];

// HYPOTHESIS: The block that breaks together with its group (sora_adv_stage gr_gimmick_block_sympathetic.o); it is not a
// grGimmickBlock. Its destructor is inline: the kept copy is in gr_adventure_block_pos_sympathetic.o.
class grGimmickBlockSympathetic : public grGimmick {
protected:
    // HYPOTHESIS: startup reads a byte selector and four signed resource IDs from this record.
    struct unkData {
        u8 unk0[0x2F];
        u8 unk2F;
        int unk30;
        int unk34;
        int unk38;
        int unk3C;
    };
    // HYPOTHESIS: m_hitCircle is the pointer addHitCircle returns; it sits at 0x154, inside unk151.
    union {
        struct {
            unkData* unk14C; // 0x14C
            u8 unk150;  // 0x150
            u8 unk151[7];
        };
        struct {
            u32 _pad14C;
            u8 _pad150;
            u8 _pad151[3];
            void* m_hitCircle;
        };
    };

    friend class grAdventureBlockSympatheticManager;

public:
    grGimmickBlockSympathetic(const char* taskName) : grGimmick(taskName) {
        unk14C = 0;
        unk150 = 0;
    }
    virtual ~grGimmickBlockSympathetic();
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void invalidatedByCameraClipping();
    virtual void dbDispInvalidatedByCameraClippingSphere();

    static grGimmickBlockSympathetic* create(int mdlIndex, const char* taskName);

    void requestBreak();
    // HYPOTHESIS: inline resource accessors inferred from startup's repeated initialization.
    void setSoundEffect(int index, int soundId, Vec2f offset = Vec2f(0.0f, 0.0f)) {
        if (soundId > 0) {
            m_soundEffects[index].m_id = soundId;
            m_soundEffects[index].m_repeatFrame = 0;
            m_soundEffects[index].m_nodeIndex = 0;
            m_soundEffects[index].m_endFrame = 0;
            m_soundEffects[index].m_offsetPos = offset;
        }
    }
    // HYPOTHESIS: inline resource accessor inferred from startup's repeated initialization.
    void setEffect(int index, int effectId, Vec2f offset = Vec2f(0.0f, 0.0f)) {
        if (effectId > 0) {
            m_effects[index].m_id = effectId;
            m_effects[index].m_repeatFrame = 0;
            m_effects[index].m_nodeIndex = getNodeIndex(0, lbl_40_data_6380);
            m_effects[index].m_endFrame = 0;
            m_effects[index].m_offsetPos = offset;
            m_effects[index].m_scale = 1.0f;
        }
    }
};
static_assert(sizeof(grGimmickBlockSympathetic) == 0x158, "Class is wrong size!");
