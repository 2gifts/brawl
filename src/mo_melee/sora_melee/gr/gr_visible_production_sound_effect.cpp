#include <gr/gr_visible_production_sound_effect.h>
#include <types.h>

extern int lbl_27_data_57DB0[20];

grVisibleProductionSoundEffect* grVisibleProductionSoundEffect::create(grGimmick* ground, grGimmick::SimpleEffectData* data) {
    return new (Heaps::StageInstance) grVisibleProductionSoundEffect(ground, data);
}

void grVisibleProductionSoundEffect::initialize() { }

void grVisibleProductionSoundEffect::remove() { }

void grVisibleProductionSoundEffect::productFlowStandby() { }

void grVisibleProductionSoundEffect::productFlowStart() {
    m_frame = 0.0f;
    m_started = false;
    m_soundHandle = -1;
    setState(lbl_27_data_57DB0[0]);
}

void grVisibleProductionSoundEffect::productFlowProducing(float deltaFrame) { }

void grVisibleProductionSoundEffect::productFlowEnd() { }

void grVisibleProductionSoundEffect::setForcedEnding() { }
