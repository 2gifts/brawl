#pragma once
// Local BrawlHeaders shadow: adds the inline constructor that IfSnapSaveTask::initProc expands (worker thread at
// priority 0x11 with a 0x2000-byte stack on the thread heap).

#include <StaticAssert.h>
#include <gf/gf_thread.h>
#include <sr/sr_common.h>
#include <types.h>

class gfJpegEncodeWork : public gfThread {
    s32 unk340_1 : 8;
    Heaps::HeapType unk340_2 : 8;
    u16 unk342;
    u16 unk344;
    s32 unk348;
    u32 unk34C;
    s32 unk350;
    u32 unk354;
    u32 unk358;
public:
    gfJpegEncodeWork() : gfThread(this, 0x11, 0x2000, Heaps::Thread) {
        unk340_1 = 0;
    }
    virtual void run();
};
static_assert(sizeof(gfJpegEncodeWork) == 0x360, "Class is the wrong size!");
