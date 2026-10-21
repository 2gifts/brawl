#pragma once
// Local BrawlHeaders shadow: verified request, channel and archive layouts used
// by the rumble pool, script interpreter and controller motor calls.
#include <types.h>
#include <StaticAssert.h>
#include <stddef.h>

class gfPadSystem;

// HYPOTHESIS: local type/field names for layouts recovered from all rumble users.
// Brawl adds a cancellation mask before the pause byte to the HSD request layout.
struct gfRumbleRequest {
    gfRumbleRequest* next;
    u32 id;
    u8 mask;
    u8 paused;
    u8 priority;
    u8 status;
    u16 loopCount;
    u16 wait;
    s32 frames;
    const u16* loopStart;
    const u16* cursor;
    const u16* pattern;
};
struct gfRumbleChannel {
    u8 lastStatus;
    u8 status;
    u8 directStatus;
    u16 count;
    gfRumbleRequest* requests;
};
struct gfRumblePattern {
    const u16* commands;
    u8 priority;
};
struct gfRumblePublic {
    u32 dataOffset;
    u32 nameOffset;
};
struct gfRumbleArchive {
    u32 fileSize;
    u32 dataSize;
    u32 relocationCount;
    u32 publicCount;
    u32 externalCount;
    u32 unk14[3];
    u8* data;
    u32* relocations;
    gfRumblePublic* publicSymbols;
    gfRumblePublic* externalSymbols;
    char* strings;
    u32 unk34[3];
};

struct gfRumble {
    gfRumbleArchive m_gcArchive;
    gfRumbleArchive m_wiiArchive;
    gfPadSystem* m_padSystem;
    gfRumbleChannel m_channels[8];
    u16 m_maxRequests;
    u8 m_disabled;
    gfRumbleRequest* m_freeRequests;
    gfRumbleRequest* m_requestPool;
    gfRumblePattern* m_gcPatterns;
    gfRumblePattern* m_wiiPatterns;
    u16 m_wiiFrameBase;
    float m_wiiFrameScale;

    gfRumble();
    void Init(gfPadSystem*, int requestsPerPad);
    void _lbInitRumble(const char* symbol);
    void _HSD_ArchiveSetup(gfRumbleArchive*, const void* image, int imageSize);
    void lbSetRumble(s32 pad, s32 id, s32 pattern, s32 frames, s32 mask);
    void lbStopRumble(s32 pad);
    void lbStopRumbleAll();
    void HSD_PadRumbleRemove(s32 pad);
    void HSD_PadRumbleRemoveAll();
    void HSD_PadRumbleRemoveId(s32 pad, s32 id, s32 pattern);
    void HSD_PadRumbleRemoveMask(s32 pad, u32 mask);
    void HSD_PadRumblePauseAll();
    void HSD_PadRumbleActiveAll();
    int _HSD_PadRumbleAdd(s32 pad, s32 id, s32 frames, u8 priority, const u16* pattern, s32 mask);
    int _HSD_PadRumbleInterpret1(gfRumbleRequest*, u8* status);
    void HSD_PadRumbleInterpret();
    void _HSD_PadRumbleInit(int count, gfRumbleRequest* pool);

    inline void freeRequest(gfRumbleChannel&, gfRumbleRequest*);
};
static_assert(sizeof(gfRumbleRequest) == 0x20, "Rumble request stride");
static_assert(offsetof(gfRumbleRequest, cursor) == 0x18, "Rumble script cursor");
static_assert(sizeof(gfRumbleChannel) == 0xC, "Rumble channel stride");
static_assert(sizeof(gfRumblePattern) == 8, "Rumble pattern descriptor");
static_assert(sizeof(gfRumbleArchive) == 0x40, "HSD archive view");
static_assert(offsetof(gfRumble, m_channels) == 0x84, "Rumble channels");
static_assert(offsetof(gfRumble, m_freeRequests) == 0xE8, "Rumble free list");
static_assert(offsetof(gfRumble, m_gcPatterns) == 0xF0, "Rumble pattern families");
static_assert(sizeof(gfRumble) == 0x100, "Rumble allocation size");
