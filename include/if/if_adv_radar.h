#pragma once

#include <types.h>

class MuObject;
namespace nw4r { namespace g3d { class ScnGroup; } }

class IfAdvRadar {
public:
    void refresh();
    void setPlayerNum(int count);
    static int getStepType();
    void show();
    void hide();

    // HYPOTHESIS: fields follow constructor stores and player-model accesses.
    void* unk0;
    nw4r::g3d::ScnGroup* unk4;
    MuObject* unk8;
    MuObject* unkC[40];
    MuObject* unkAC[2];
    MuObject* unkB4[40];
    MuObject* unk154[160];
    u32 unk3D4;
    void* unk3D8;
    u32 unk3DC;
    u32 unk3E0;
    u32 unk3E4;
    u32 unk3E8;
    u32 unk3EC;
    u32 unk3F0;
    u32 unk3F4;
    u32 unk3F8;
    u32 unk3FC;
    float unk400;
    float unk404;
    u8 unk408_7 : 1;
    u8 unk408_6 : 1;
    u8 unk408_0 : 6;
};
