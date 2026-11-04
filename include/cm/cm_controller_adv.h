#pragma once

#include <gf/gf_camera_controller.h>
#include <mt/mt_vector.h>

class cmAdventureController : public gfCameraController {
public:
    // HYPOTHESIS: signed mode bytes packed in the word following the base.
    s32 unk8 : 8;
    s32 unk9 : 8;
    u32 unkA : 16;
    u8 unkC[0xD0];
    float unkDC;
    u8 unkE0[8];
    Vec2f unkE8;
};
