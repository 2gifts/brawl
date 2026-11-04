#pragma once

#include <gf/gf_file_io_handle.h>
#include <types.h>

class Vec2f;

class tySealListDataManager {
public:
    // HYPOTHESIS: anonymous record layout from the 0x64-byte stride and member loads.
    struct {
        u32 unk0;
        u8 unk4[0x18];
        u32 unk1C;
        u32 unk20;
        u8 unk24[0x40];
    }* unk0;
    u8 unk4[8];
    u32 unkC;
    u32 unk10;
    u16 unk14[1000];
    u32 unk7E4[1000];
    u32 unk1784[1000];
    u8 unk2724[1000];
    u8 unk2B0C[1000];
    u32 unk2EF4;
    u8 unk2EF8[0xC];
    u32 unk2F04;
    u32 unk2F08[1000];
    bool unk3EA8;
    gfFileIOHandle unk3EAC;
    bool unk3EB0;

    bool isLoadFinish();
    void setData(u8* data, u32 size);
    void sortNew();
    void clearNarrow();
    u32 getSealList(u32 count, u32 start, void** output);
    // HYPOTHESIS: the record's original type name is unknown.
    void* getSealDataId(u32 id);
    bool isPutChara(int chara, u32 id);
    u32 getPutCharaNum(u32 id);
    u32 getPutCharaNo(u32 id, u32 index);
};

// HYPOTHESIS: polygon arguments are arrays of two-float vertices and unsigned counts.
bool isPut(Vec2f* first, u32 firstCount, Vec2f* second, u32 secondCount);
bool isCross(Vec2f* first, u32 firstCount, Vec2f* second, u32 secondCount);
bool isPuttingOn(Vec2f* first, u32 firstCount, Vec2f* second, u32 secondCount, bool both);
