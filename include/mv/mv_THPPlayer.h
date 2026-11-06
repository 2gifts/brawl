// Local BrawlHeaders shadow: typed player state and function declarations
#pragma once

#include <revolution/DVD/dvd.h>
#include <types.h>

struct THPTextureSet {
    u8* ytexture;
    u8* utexture;
    u8* vtexture;
    s32 frameNumber;
};

struct THPAudioBuffer {
    s16* buffer;
    s16* curPtr;
    u32 validSample;
};

// TODO: THPPlayer? Likely has more members
struct UnkTHPClass {
    struct UnkClass {
        u32 length;
        s8 buf[0x10];
    };
    DVDFileInfo m_fInfo;
    char unk3C[4];
    u32 unk40;
    u32 unk44;
    u32 unk48;
    float unk4C;
    u32 unk50;
    u32 unk54;
    u32 unk58;
    u32 unk5C;
    s32 unk60;
    s32 unk64;
    s32 unk68;
    UnkClass unk6C;
    union {
        struct { u8 unk80[0x26]; u8 unkA6; u8 unkA7[0x1]; };
        struct {
            u32 unk80Width;
            u32 unk84;
            u32 unk88;
            u32 unk8C;
            u32 unk90;
            u32 unk94;
            u32 unk98;
            void* unk9C;
            s32 unkA0;
            u8 unkA4;
            u8 unkA5;
            u8 _A6;
            u8 unkA7Flag;
        };
    };
    s32 unkA8; // read error indicator
    union { u8 unkAC[0x8]; struct { u32 unkACword; s32 unkB0; }; };
    u32* unkB4;
    u32 unkB8; // initial read offset
    u32 unkBC; // initial read size
    union {
        struct { u32 unkC0; u8 unkC4[0x18]; u32 unkDC; };
        struct { u32 _C0[2]; s64 unkC8; s32 unkD0; s32 unkD4; u32 unkD8; u32 _DC; };
    };
    u32 unkE0;
    u32 unkE4;
    THPTextureSet* unkE8;
    u32 unkEC;
    u32 unkF0[6][3];
    THPTextureSet unk138[3];
    THPAudioBuffer unk168[6];
};
// TODO: size assertion

void PrepareReady(bool p1);

extern UnkTHPClass g_thpObject;

BOOL THPPlayerInit();
BOOL THPPlayerOpen(const char* path, s32 mode);
void InitAllMessageQueue();
BOOL ProperTimingForGettingNextFrame();
s32 THPPlayerDrawCurrentFrame(int width, int height, int x, int y, int polygonWidth, int polygonHeight);
extern "C" BOOL THPInit();
extern "C" BOOL DVDOpen(const char* path, DVDFileInfo* fileInfo);
extern "C" BOOL DVDClose(DVDFileInfo* fileInfo);
extern "C" u32 VIGetNextField();

void SetTHPPlayerWork(void* work);
class movieOpen {
public:
    void freeMovieBuffer();
};

void THPPlayerStreamUpdate();

struct _AXVPB;

BOOL THPPlayerGetVideoInfo(void* info);
BOOL THPPlayerGetAudioInfo(void* info);
u32 THPPlayerGetTotalFrame();
void THPPlayerDrawDone();
u32 GetAudioSample(s16* right, s16* left, u32 count, s32* status);
void FillStreamBuffer(s16* right, s16* left, u32 count);
void THPPlayerGetStreamAXPB(_AXVPB** right, _AXVPB** left);

u32 GetSizeTHPPlayerWork();
void FreeTHPPlayerWork();
u8 THPPlayerGetState();
BOOL THPPlayerClose();
BOOL THPPlayerPause();
BOOL THPPlayerPlay();
void StreamReInit();
