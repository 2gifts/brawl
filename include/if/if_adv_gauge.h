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
    // HYPOTHESIS: the tail of the object, from the stores of setCrowdDelAnimeNum, removeScore and removeCountTime; size from the allocation in create.
    int unk30C;
    u8 unk310[0x50];
    u8 unk360;
    u8 unk361;
    u8 unk362;
    u8 unk363;

    IfAdvGauge();
    ~IfAdvGauge();
    static IfAdvGauge* create();
    static void destroyLocator();
    void createModel(nw4r::g3d::ResFile* resFile);
    void createModel2(nw4r::g3d::ResFile* resFile);
    void createModelStRes(nw4r::g3d::ResFile* resFile, nw4r::g3d::ResFile* stResFile);
    void removeScore();
    void drawCrowd();
    void setCrowdDelAnimeNum(s32 num);
    void removeCountTime();
    void deleteStockSlotFirstIcon();

    // HYPOTHESIS: table parameter types follow the equivalent adventure HUD methods.
    void createObjResFile(const void* nodeInfo, s32 nodeCount, nw4r::g3d::ResFile* resFile);
    void createExpMdl(const void* nodeInfo, s32 nodeCount) __attribute__((never_inline));
    void addPressStart(nw4r::g3d::ScnMdlExpand* model);
    void removePressStart(nw4r::g3d::ScnMdlExpand* model);
    void drawStockSlotAnime(int count);
    void setCrowd(int crowd);
    void draw2PEnt();
    void drawStockBase(int show);
    static void createLocator(nw4r::g3d::ResFile* resFile);
    static void createLocator2(nw4r::g3d::ResFile* resFile);
};
