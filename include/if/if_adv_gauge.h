#pragma once

#include <types.h>

class MuObject;
namespace nw4r { namespace g3d { class ResFile; class ScnMdlExpand; } }

class IfAdvGauge {
public:
    // HYPOTHESIS: array extents follow indexed stores and the following member offset.
    MuObject* unk0[0x55];
    nw4r::g3d::ScnMdlExpand* unk154[7];
    // HYPOTHESIS: layout between the known fields is not yet recovered.
    u8 unk170[0x238 - 0x170];
    nw4r::g3d::ResFile* unk238;
    int unk23C;
    u8 unk240[0x308 - 0x240];
    int unk308;

    // HYPOTHESIS: table parameter types follow the equivalent adventure HUD methods.
    void createObjResFile(const void* nodeInfo, s32 nodeCount, nw4r::g3d::ResFile* resFile);
    void createExpMdl(const void* nodeInfo, s32 nodeCount);
    void addPressStart(nw4r::g3d::ScnMdlExpand* model);
    void removePressStart(nw4r::g3d::ScnMdlExpand* model);
    void drawStockSlotAnime(int count);
    void setCrowd(int crowd);
    void draw2PEnt();
    void drawStockBase(int show);
    static void createLocator(nw4r::g3d::ResFile* resFile);
    static void createLocator2(nw4r::g3d::ResFile* resFile);
};
