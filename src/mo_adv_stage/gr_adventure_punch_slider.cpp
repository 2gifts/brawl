#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_adventure_punch_slider.h>

grAdventurePunchSlider* grAdventurePunchSlider::create(int mdlIndex, const char* taskName) {
    grAdventurePunchSlider* ground = new (Heaps::StageInstance) grAdventurePunchSlider(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventurePunchSlider::~grAdventurePunchSlider() { }
