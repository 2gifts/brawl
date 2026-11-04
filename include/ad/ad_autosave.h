// Local BrawlHeaders shadow: adds typed autosave state and recorder fields.
#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <types.h>

class gfSysRecorder2;

class adAutoSave : public gfTask {
    union {
        u8 unk0[0x10];
        struct {
            u32 unk40;
            u32 unk44;
            bool unk48;
            u8 unk49[3];
            gfSysRecorder2* unk4C;
        };
    };
public:
    virtual void processDefault();
    virtual void init();
    virtual ~adAutoSave();

    static adAutoSave* create(bool p1);
    static bool isAlive();
};
static_assert(sizeof(adAutoSave) == 0x50, "Class is the wrong size!");
