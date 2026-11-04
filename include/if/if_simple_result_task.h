#pragma once

#include <gf/gf_task.h>
#include <mt/mt_vector.h>
#include <types.h>

class MuObject;

class ifSimpleResCoinTask : public gfTask {
public:
    // HYPOTHESIS: the object array and coin count come from ifSimpleResultTask.
    ifSimpleResCoinTask(MuObject** objects, u32 count);
    virtual void processAnim();
    virtual ~ifSimpleResCoinTask();

    u32 unk40;
    u32 unk44;
    u32 unk48;
    MuObject** unk4C;
    struct {
        u8 unk0;
        u8 unk1[0x33];
    } unk50[64];
    u32 unkD50;
    Vec3f unkD54;
};
