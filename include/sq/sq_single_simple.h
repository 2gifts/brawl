#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqSingleSimple : public gfSequence {
public:
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    u8 unk2C;
    u8 unk2D;
    u8 unk2E;
    u8 unk2F;
    u8 unk30;
    u8 unk31;
    u8 unk32;
    u8 unk33;
    u8 unk34[0x4];

    static sqSingleSimple* create();
    sqSingleSimple(const char* name) : gfSequence(name), unk8(0), unk14(0), unk18(0) { }
    virtual void start();
    virtual void exit();
    void setSimpleSetting();
    void setStageContinueSetting();
    void updateHiScore(int charKind, int flag, u32 score);
};
static_assert(sizeof(sqSingleSimple) == 0x38, "Class is wrong size!");
