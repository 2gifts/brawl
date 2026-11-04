#include <memory.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_adventure_shutter.h>

grAdventureShutter* grAdventureShutter::create(int mdlIndex, const char* taskName) {
    grAdventureShutter* ground = new (Heaps::StageInstance) grAdventureShutter(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureShutter::~grAdventureShutter() { }
