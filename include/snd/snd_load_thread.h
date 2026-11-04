#pragma once

#include <gf/gf_thread.h>
#include <revolution/OS/OSMutex.h>
#include <snd/snd_id.h>

class sndLoadThread : public gfThread {
    OSMutex unk340;
    OSCond unk358;
    OSCond unk360;
    struct {
        u32 unk0;
        int unk4;
        SndGroupID unk8;
        int unkC;
    } unk368[149], unkCB8[1];
    u32 unkCC8;
    u32 unkCCC;
    bool unkCD0;

public:
    void run();
    void addFreeGroup(int, int);
    bool isFinish();
    bool isFinish(SndGroupID);
    bool isFinishFree();
};
