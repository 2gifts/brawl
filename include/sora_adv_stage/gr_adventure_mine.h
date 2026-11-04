#pragma once

#include <StaticAssert.h>
#include <gr/gr_gimmick.h>
#include <sora_adv_stage/gr_gimmick_block.h>
#include <types.h>

// HYPOTHESIS: The type name is from the sora_adv_stage map (createGimmickFunc<17grGimmickMineData, ...>); its layout is not
// described yet.
struct grGimmickMineData;

class grAdventureMine : public grGimmickBlock {
    friend class grAdventureMineManager;
protected:
    u32 unk15C;        // 0x15C
    u8 m_isUpdate;     // 0x160, set again each frame by the mine manager (HYPOTHESIS)
    u32 unk164;        // 0x164
    float unk168;      // 0x168
    u8 m_isWarning;    // 0x16C

public:
    // HYPOTHESIS: constructor body, read from the copy inlined into create.
    grAdventureMine(const char* taskName) : grGimmickBlock(taskName) {
        unk15C = 0;
        m_isUpdate = false;
        unk164 = 1;
        unk168 = 0.0f;
        m_isWarning = false;
    }

    virtual void processUpdate();
    virtual ~grAdventureMine();
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);

    static grAdventureMine* create(int mdlIndex, const char* taskName);

    void callWarning();
    void callCancelWarning();
    void callBreak();

private:
    // HYPOTHESIS: inlined accessor for the sound-effect work that startup fills in.
    inline void setSoundEffect(int index, int soundId, Vec2f offset) {
        m_soundEffects[index].m_id = soundId;
        m_soundEffects[index].m_repeatFrame = 0;
        m_soundEffects[index].m_nodeIndex = 0;
        m_soundEffects[index].m_endFrame = 0;
        m_soundEffects[index].m_offsetPos = offset;
    }

    // HYPOTHESIS: inlined accessor for the effect work that startup fills in.
    inline void setEffect(int index, int effectId, const char* nodeName, Vec2f offset, float scale) {
        m_effects[index].m_id = effectId;
        m_effects[index].m_repeatFrame = 0;
        m_effects[index].m_nodeIndex = getNodeIndex(0, nodeName);
        m_effects[index].m_endFrame = 0;
        m_effects[index].m_offsetPos = offset;
        m_effects[index].m_scale = scale;
    }
};
static_assert(sizeof(grAdventureMine) == 0x170, "Class is wrong size!");
