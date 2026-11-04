#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqSingleAllstar : public gfSequence {
public:
    // HYPOTHESIS: sequence state and All-Star progress fields
    s32 unk8;
    s32 unkC;
    u8 unk10;
    u8 unk11[0x3];
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    u8 unk28;
    u8 unk29[0x3];
    s32 unk2C;

    static sqSingleAllstar* create();
    sqSingleAllstar(const char* name) : gfSequence(name), unk8(0) { }
    virtual void start();
    virtual void setNext();
    virtual void exit();

    void setupAllstarSetting();
    void setupAllstarStage();
    void setupRestingPlace();
    void setupContinue();
    void updateHiScore(bool);
    void setupPosition();
};
static_assert(sizeof(sqSingleAllstar) == 0x30, "Class is wrong size!");
