#pragma once

#include <gr/gr_yakumono.h>

class grAdventureMine;

struct grGimmickMineData {
    u8 unk0[0x10];
    float unk10;
    u8 unk14[0x60];
    float unk74;
    u8 unk78[0x10];
    float unk88;
};

class grAdventureMineManager : public grYakumono {
public:
    u8 unk150[0x20];
    u8 unk170;
    grAdventureMine* unk174[13];
    u8 unk1A8;
    grGimmickMineData* unk1AC;

    virtual void update(float deltaFrame);
    virtual void onInflict(soCollisionLog* collisionLog, u32 unk2, float power);
    void chSympatheticDetonation();
    void chWarning();
    void frowExplosion(const int& index, float deltaFrame);
    void frowExploding(const int& index, float deltaFrame);
    void frowStandby(const int& index, float deltaFrame);
    void frowEnding(const int& index, float deltaFrame);
};

static_assert(sizeof(grAdventureMineManager) == 0x1B0, "Class is wrong size!");
