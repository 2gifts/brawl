#pragma once

#include <gf/gf_scene.h>
#include <types.h>

class scSelStage : public gfScene {
public:
    u8 unk8[0x3A8];
    u64 unk3B0;
    u8 unk3B8[0x80];
    u64 unk438;
    u8 unk440;
};
