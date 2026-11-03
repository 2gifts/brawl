#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqSingleBoss : public gfSequence {
public:
    s32 unk8;
    union {
        u8 unkC[0x1C];
        struct {
            u8 unkC_pad[5];
            u8 unk11;
            u8 unk12[10];
            s32 unk1C;
            s32 unk20;
            s32 unk24;
        };
    };
    s32 unk28;

    static sqSingleBoss* create();
    sqSingleBoss(const char* name) : gfSequence(name), unk8(0), unk28(0) { }
    virtual void start();
    virtual void setNext();
    virtual void exit();
    void checkClearGet();
    void makeStageTable();
};
static_assert(sizeof(sqSingleBoss) == 0x2C, "Class is wrong size!");
