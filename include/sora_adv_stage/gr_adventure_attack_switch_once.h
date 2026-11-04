#pragma once

#include <StaticAssert.h>
#include <sora_adv_stage/gr_adventure_attack_switch.h>
#include <st/st_trigger.h>
#include <types.h>

// HYPOTHESIS: effect id at 0x18 and the trigger at 0x30 of the attack-switch record.
struct grGimmickAttackSwitchOnceData {
    u8 unk0[0x18];
    int m_effectId;
    u8 unk1C[0x14];
    stTriggerData m_trigger;
};

class grAdventureAttackSwitchOnce : public grAdventureAttackSwitch {
protected:
    // HYPOTHESIS: set when this switch takes damage; update then clears attacker info.
    u8 m_damaged;

public:
    grAdventureAttackSwitchOnce(bool* isOn, const char* taskName) : grAdventureAttackSwitch(isOn, taskName) {
        m_damaged = 0;
    }

    // HYPOTHESIS: inlined sound-slot filler; the offset is passed by value.
    void setSoundEffectSlot(u32 index, u32 soundId, Vec2f offset) {
        m_soundEffects[index].m_id = soundId;
        m_soundEffects[index].m_repeatFrame = 0;
        m_soundEffects[index].m_nodeIndex = 0;
        m_soundEffects[index].m_endFrame = 0;
        m_soundEffects[index].m_offsetPos = offset;
    }

    // HYPOTHESIS: positive-id effect path; the zero offset is copied before the id is tested again.
    void setupPositiveEffect(int effectId) {
        Vec2f made(0.0f, 0.0f);
        Vec2f offset(made);
        if (effectId > 0) {
            m_effects[0].m_id = effectId;
            m_effects[0].m_repeatFrame = 0;
            m_effects[0].m_nodeIndex = getNodeIndex(0, "effect_locator");
            m_effects[0].m_endFrame = 0;
            m_effects[0].m_offsetPos = offset;
            m_effects[0].m_scale = 1.0f;
        }
    }

    // HYPOTHESIS: default effect path; the extra inline level keeps the copied offset below its source.
    void setupDefaultEffect() {
        setEffectSlot(0, 0x1030037, Vec2f(0.0f, 0.0f));
    }

    // HYPOTHESIS: inlined effect-slot filler; the offset is passed by value.
    void setEffectSlot(u32 index, int effectId, Vec2f offset) {
        m_effects[index].m_id = effectId;
        m_effects[index].m_repeatFrame = 0;
        m_effects[index].m_nodeIndex = getNodeIndex(0, "effect_locator");
        m_effects[index].m_endFrame = 0;
        m_effects[index].m_offsetPos = offset;
        m_effects[index].m_scale = 1.0f;
    }

    virtual ~grAdventureAttackSwitchOnce();
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void setSimpleEffectVisibleProduction();
    virtual void setInitializeFlag();
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);

    static grAdventureAttackSwitchOnce* create(int mdlIndex, bool* isOn, const char* taskName);
};
static_assert(sizeof(grAdventureAttackSwitchOnce) == 0x1A8, "Class is wrong size!");
