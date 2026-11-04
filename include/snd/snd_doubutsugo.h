#pragma once

#include <types.h>

// HYPOTHESIS: the object at 0x8 is an interface with a virtual slot 6 taking one int.
class DoubutsugoSource {
public:
    virtual void vf00();
    virtual void vf04();
    virtual void vf08();
    virtual void vf0C();
    virtual void vf18(int arg);
};

// HYPOTHESIS: the Animalese voice player of Doubutsugo.o (map); layout from Init only.
class Doubutsugo {
public:
    char _00[0x8];
    DoubutsugoSource* m_source; // 0x08
    char _0C[0x8];
    f32 unk14;               // 0x14
    char _18[0x10];
    f32 unk28;               // 0x28
    char _2C[0xC];
    u8 unk38;                // 0x38
    u8 unk39;                // 0x39
    char _3A[2];
    int unk3C;              // 0x3C
    int unk40;              // 0x40
    int unk44;              // 0x44
    int unk48;              // 0x48
    int unk4C;              // 0x4C
    int unk50;              // 0x50
    int unk54;              // 0x54
    int unk58;              // 0x58
    int unk5C;              // 0x5C
    int unk60;              // 0x60
    int unk64;              // 0x64
    int unk68;              // 0x68
    int unk6C;              // 0x6C
    int unk70;              // 0x70
    int unk74;              // 0x74
    int unk78;              // 0x78
    int unk7C;              // 0x7C
    char _80[0x44];
    f32 unkC4;             // 0xC4

    void Init();
};
