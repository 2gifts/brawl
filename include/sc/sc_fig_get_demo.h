#pragma once

#include <ad/ad_list.h>
#include <gf/gf_scene.h>
#include <sr/sr_common.h>
#include <types.h>

class scFigureGetDemo : public gfScene {
public:
    // HYPOTHESIS: fields accessed by setAllstarGetFigure
    s32 unk8;
    s32 unkC;
    adList<s32, Heaps::MenuInstance> unk10;
};
