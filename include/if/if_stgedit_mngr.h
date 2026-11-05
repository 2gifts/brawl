#pragma once

#include <types.h>

class MuObject;
class IfStgedit;
namespace nw4r { namespace g3d { class ScnMdlExpand; class ScnGroup; } }

// HYPOTHESIS: the task names this class IfStgeditMngr; the map identifies IfAdvGauge.
class IfStgeditMngr {
public:
    union {
        u8 unk0[0x30];
        struct {
            u8 unk0_0[4];
            nw4r::g3d::ScnGroup* unk4;
            u8 unk8[8];
            IfStgedit* unk10;
            u8 unk14[0x1C];
        };
    };
    MuObject* unk30;
    MuObject* unk34[50];
    u8 unkFC[0x60];
    nw4r::g3d::ScnMdlExpand* unk15C;
    u8 unk160[0xE0];
    int unk240[50];
    int unk308;
    u8 unk30C[4];
    u8 unk310[50];

    void freeResource();
    void setVisible(bool isVisible);
    void setVisibleZoomHelp(bool isVisible);
    void destroyModel();
};
