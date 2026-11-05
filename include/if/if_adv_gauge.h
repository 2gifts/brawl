#pragma once

#include <types.h>

class MuObject;
namespace nw4r { namespace g3d { class ResFile; class ScnMdlExpand; } }

class IfAdvGauge {
public:
    // HYPOTHESIS: array extents follow indexed stores and the following member offset.
    MuObject* unk0[0x55];
    nw4r::g3d::ScnMdlExpand* unk154[3];

    // HYPOTHESIS: table parameter types follow the equivalent adventure HUD methods.
    void createObjResFile(const void* nodeInfo, s32 nodeCount, nw4r::g3d::ResFile* resFile);
    void createExpMdl(const void* nodeInfo, s32 nodeCount);
    void addPressStart(nw4r::g3d::ScnMdlExpand* model);
    void removePressStart(nw4r::g3d::ScnMdlExpand* model);
    void drawStockSlotAnime(int count);
};
