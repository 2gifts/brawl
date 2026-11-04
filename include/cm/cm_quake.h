// Local BrawlHeaders shadow: adds quake state and resource declarations.
#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <sr/sr_common.h>
#include <types.h>

namespace nw4r { namespace g3d { struct ResAnmChrData; } }

class cmQuake {
public:
    enum Amplitude {
        Amplitude_None = 0x0,
        Amplitude_M = 0x1,
        Amplitude_Middle = 0x1,
        Amplitude_S_Half = 0x2,
        Amplitude_Small_Hf = 0x2,
        Amplitude_S = 0x3,
        Amplitude_Small = 0x3,
        Amplitude_L = 0x4,
        Amplitude_Large = 0x4,
        Amplitude_XL = 0x5,
        Amplitude_More_Large = 0x5,
    };

    void init();
    bool loadResource(const void* resource, u32 size, Heaps::HeapType heap);

    union {
        char _[0x274];
        struct {
            float unk0;
            float unk4;
            float unk8;
            float unkC;
            union { u32 unk10; struct { u32 unk10_0 : 8; u32 unk10_8 : 24; }; };
            u32 unk14[6];
            struct { float unk0, unk4, unk8; u32 unkC; } unk2C[16];
            struct { float unk0, unk4, unk8; u32 unkC; } unk12C[16];
            void* unk22C;
            nw4r::g3d::ResAnmChrData* unk230[5];
            float unk244[5];
            float unk258;
            float unk25C;
            float unk260;
            float unk264;
            float unk268;
            float unk26C;
            u32 unk270;
        };
    };
};
static_assert(sizeof(cmQuake) == 0x274, "Class is wrong size!");

void cmReqQuake(cmQuake::Amplitude amplitude, Vec3f* offset);
void cmRemoveQuake(int index);