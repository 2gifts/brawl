#pragma once

#include <gf/gf_task.h>
#include <gm/gm_global.h>
#include <mt/mt_vector.h>
#include <types.h>

class MuObject;
namespace nw4r {
namespace g3d {
class ScnMdlExpand;
}
}

// HYPOTHESIS: one record of the muClearGetDataList table (0x28 bytes, a negative first word ends it).
struct muClearGetData {
    int unk0;
    union {
        u8 unk4[0x24];
        int unk4Words[9];
    };
};

class muClearGetDataManager {
public:
    muClearGetData* unk0;
    u32 unk4;
    int unk8[128];

    ~muClearGetDataManager();
    void setData(u8* buffer, u32 size);
    void setOpenState(u32 index);
    // HYPOTHESIS: inlined accessor that copies the open states into the save record.
    void saveFlags() {
        u8* flags = g_GameGlobal->m_record->unk1528;
        for (int i = 0; i < 128; i++) {
            flags[i] = unk8[i];
        }
    }
};

class muClearGetTask : public gfTask {
public:
    gfTask* unk40;
    int unk44;
    int unk48;
    u8 unk4C[0x100 - 0x4C];
    MuObject* unk100[0x299];
    nw4r::g3d::ScnMdlExpand* unkB64[0x84];
    u8 unkD74[0xE84 - 0xD74];
    muClearGetDataManager unkE84;
    u32 unk108C;
    u32 unk1090;
    u32 unk1094;
    union {
        struct {
            bool unk1098;
            u8 unk1099[0x10A8 - 0x1099];
        };
        struct {
            u8 _1098[8];
            u32 unk10A0;
            bool unk10A4;
        };
    };
    u32 unk10A8[128];
    u32 unk12A8;
    float unk12AC;
    u32 unk12B0;
    u32 unk12B4;
    union {
        u8 unk12B8[0x12CC - 0x12B8];
        u32 unk12B8Words[5];
    };
    Vec3f unk12CC;
    bool unk12D8;
    u8 unk12D9[7];
    int unk12E0;
    u8 unk12E4;
    bool unk12E5;
    u8 unk12E6[2];
    u32 unk12E8;

    bool isEnd();
    void setCursor();
    void releaseHammer(int);
    u32 setBreakBoxId(u32);
    void setPosRot(u32);
    void setPrizeModel(u32, int);
    bool calcHammerClose();
};
