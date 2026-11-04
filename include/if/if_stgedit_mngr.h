#pragma once

#include <types.h>

class MuObject;
namespace nw4r { namespace g3d { class ScnMdlExpand; } }

// HYPOTHESIS: the task names this class IfStgeditMngr; the map identifies IfAdvGauge.
class IfStgeditMngr {
public:
    u8 unk0[0x30];
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
};
