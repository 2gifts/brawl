#pragma once

#include <types.h>
#include <mu/mu_vc_trial.h>
#include <std/auto_ptr.h>

class MuObject;
namespace nw4r { namespace g3d { class ScnGroup; } }

class IfAdvRadarData {
public:
    IfAdvRadarData(void* arg1, void* arg2, void* arg3, void* arg4);

    void* unk0;
    void* unk4;
};

class IfAdvRadar {
public:
    IfAdvRadar(IfAdvRadarData* data, int mode);
    ~IfAdvRadar();
    static IfAdvRadar* create(IfAdvRadarData* data, int mode);
    static void destroy();
    void refresh();
    void setPlayerNum(int count);
    static int getStepType();
    void show();
    void hide();

    // HYPOTHESIS: fields follow constructor stores and player-model accesses.
    void* unk0;
    muG3dObjHolder<nw4r::g3d::ScnGroup> unk4;
    muG3dObjHolder<nw4r::g3d::G3dObj> unk8;
    std::auto_ptr<MuObject> unkC[40];
    std::auto_ptr<MuObject> unkAC[2];
    std::auto_ptr<MuObject> unkB4[40];
    std::auto_ptr<MuObject> unk154[160];
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
