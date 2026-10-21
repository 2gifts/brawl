#include <gf/gf_rumble.h>
#include <gf/gf_pad_system.h>
#include <gf/gf_archive.h>
#include <gf/gf_heap_manager.h>
#include <revolution/OS/OSInterrupt.h>
#include <string.h>

// Archive image and length are supplied by the resource-loading code.
void* lbl_805A0058;
int lbl_805A005C;
// The native default channel is zero-initialized; reserved padding is not copied.
gfRumbleChannel lbl_80495380 __attribute__((aligned(8)));

namespace {
    enum MotorStatus { HardStop, Stop, Run };
    enum Command { End, MotorRun, MotorStop, MotorHardStop, Loop, LoopEnd };
}

gfRumble::gfRumble() {
    m_padSystem = NULL;
    m_requestPool = NULL;
    m_gcPatterns = NULL;
    m_wiiPatterns = NULL;
    m_wiiFrameBase = 30;
    m_wiiFrameScale = 1.0f;
    memset(&m_gcArchive, 0, sizeof(m_gcArchive));
    memset(&m_wiiArchive, 0, sizeof(m_wiiArchive));
}

void gfRumble::Init(gfPadSystem* padSystem, int requestsPerPad) {
    m_padSystem = padSystem;
    _lbInitRumble("lbRumbleData");
    int count = requestsPerPad * 8;
    if (m_requestPool != NULL) {
        gfHeapManager::free(m_requestPool);
        m_requestPool = NULL;
    }
    if (count != 0) m_requestPool = static_cast<gfRumbleRequest*>(gfHeapManager::alloc(Heaps::WiiPad, count * sizeof(gfRumbleRequest)));
    _HSD_PadRumbleInit(count, m_requestPool);
}

void gfRumble::_lbInitRumble(const char* symbol) {
    m_gcPatterns = NULL;
    m_wiiPatterns = NULL;
    gfArchive archive;
    archive.setFileImage(lbl_805A0058, lbl_805A005C, Heaps::WiiPad);
    int size;
    void* image = archive.getData(Data_Type_Misc, 0, &size, 0xFFFE);
    _HSD_ArchiveSetup(&m_gcArchive, image, size);
    u32 count = m_gcArchive.publicCount;
    for (u32 i = 0; i < count; ++i) {
        gfRumblePublic* symbols = m_gcArchive.publicSymbols;
        if (strcmp(m_gcArchive.strings + symbols[i].nameOffset, symbol) == 0) {
            m_gcPatterns = reinterpret_cast<gfRumblePattern*>(m_gcArchive.data + symbols[i].dataOffset);
            break;
        }
    }
    image = archive.getData(Data_Type_Misc, 1, &size, 0xFFFE);
    _HSD_ArchiveSetup(&m_wiiArchive, image, size);
    count = m_wiiArchive.publicCount;
    for (u32 i = 0; i < count; ++i) {
        gfRumblePublic* symbols = m_wiiArchive.publicSymbols;
        if (strcmp(m_wiiArchive.strings + symbols[i].nameOffset, symbol) == 0) {
            m_wiiPatterns = reinterpret_cast<gfRumblePattern*>(m_wiiArchive.data + symbols[i].dataOffset);
            break;
        }
    }
}

// Copy the HSD data, relocation and symbol sections into independently owned
// storage, then relocate the pointers embedded in the data section.
void gfRumble::_HSD_ArchiveSetup(gfRumbleArchive* archive, const void* image, int imageSize) {
    if (archive->data) gfHeapManager::free(archive->data);
    if (archive->relocations) gfHeapManager::free(archive->relocations);
    if (archive->publicSymbols) gfHeapManager::free(archive->publicSymbols);
    if (archive->externalSymbols) gfHeapManager::free(archive->externalSymbols);
    if (archive->strings) gfHeapManager::free(archive->strings);
    memset(archive, 0, sizeof(*archive));
    memcpy(archive, image, 0x20);
    const u8* bytes = static_cast<const u8*>(image);
    u32 size;
    u32 offset = archive->dataSize;
    archive->data = static_cast<u8*>(gfHeapManager::alloc(Heaps::WiiPad, offset));
    memcpy(archive->data, bytes + 0x20, offset);
    offset += 0x20;
    if (archive->relocationCount != 0) {
        size = archive->relocationCount * sizeof(u32);
        archive->relocations = static_cast<u32*>(gfHeapManager::alloc(Heaps::WiiPad, size));
        memcpy(archive->relocations, bytes + offset, size);
        offset += size;
    }
    if (archive->publicCount != 0) {
        size = archive->publicCount * sizeof(gfRumblePublic);
        archive->publicSymbols = static_cast<gfRumblePublic*>(gfHeapManager::alloc(Heaps::WiiPad, size));
        memcpy(archive->publicSymbols, bytes + offset, size);
        offset += size;
    }
    if (archive->externalCount != 0) {
        size = archive->externalCount * sizeof(gfRumblePublic);
        archive->externalSymbols = static_cast<gfRumblePublic*>(gfHeapManager::alloc(Heaps::WiiPad, size));
        memcpy(archive->externalSymbols, bytes + offset, size);
        offset += size;
    }
    size = archive->fileSize - offset;
    if (size != 0) {
        archive->strings = static_cast<char*>(gfHeapManager::alloc(Heaps::WiiPad, size));
        memcpy(archive->strings, bytes + offset, size);
    }
    for (u32 i = 0; i < archive->relocationCount; ++i) {
        u32* pointer = reinterpret_cast<u32*>(archive->data + archive->relocations[i]);
        *pointer += reinterpret_cast<u32>(archive->data);
    }
}

void gfRumble::lbSetRumble(s32 pad, s32 id, s32 pattern, s32 frames, s32 mask) {
    if (pad < 4) {
        s32 duration = frames != 0 ? frames : -2;
        _HSD_PadRumbleAdd(pad, id, duration, m_gcPatterns[pattern].priority, m_gcPatterns[pattern].commands, mask);
        return;
    }
    if (frames > 30) {
        u16 base = m_wiiFrameBase;
        frames = base + static_cast<s32>(m_wiiFrameScale * static_cast<float>(frames - base));
    }
    s32 duration = frames != 0 ? frames : -2;
    _HSD_PadRumbleAdd(pad, id, duration, m_wiiPatterns[pattern].priority, m_wiiPatterns[pattern].commands, mask);
}

inline void gfRumble::freeRequest(gfRumbleChannel& channel, gfRumbleRequest* request) {
    gfRumbleRequest** link = &channel.requests;
    while (*link != request) link = &(*link)->next;
    *link = request->next;
    --channel.count;
    request->next = m_freeRequests;
    m_freeRequests = request;
}

void gfRumble::lbStopRumble(s32 pad) {
    gfRumbleChannel& channel = m_channels[pad];
    HSD_PadRumbleRemove(pad);
    BOOL interrupts = OSDisableInterrupts();
    channel.directStatus = Stop;
    OSRestoreInterrupts(interrupts);
}
void gfRumble::lbStopRumbleAll() {
    for (int pad = 0; pad < 8; ++pad) {
        HSD_PadRumbleRemove(pad);
        BOOL interrupts = OSDisableInterrupts();
        m_channels[pad].directStatus = Stop;
        OSRestoreInterrupts(interrupts);
    }
}
void gfRumble::HSD_PadRumbleRemove(s32 pad) {
    gfRumbleChannel& channel = m_channels[pad];
    BOOL interrupts = OSDisableInterrupts();
    gfRumbleRequest* request = channel.requests;
    if (request) {
        while (request) {
            gfRumbleRequest* next = request->next;
            freeRequest(channel, request);
            request = next;
        }
    }
    OSRestoreInterrupts(interrupts);
}
void gfRumble::HSD_PadRumbleRemoveAll() {
    for (int pad = 0; pad < 8; ++pad) HSD_PadRumbleRemove(pad);
}
void gfRumble::HSD_PadRumbleRemoveId(s32 pad, s32 id, s32 pattern) {
    const u16* commands = NULL;
    gfRumbleChannel& channel = m_channels[pad];
    if (pattern != -1) {
        if (pad < 4) commands = m_gcPatterns[pattern].commands;
        else commands = m_wiiPatterns[pattern].commands;
    }
    BOOL interrupts = OSDisableInterrupts();
    gfRumbleRequest* request = channel.requests;
    while (request) {
        gfRumbleRequest* next = request->next;
        if (request->id == static_cast<u32>(id)) {
            if (commands != NULL) {
                if (request->pattern == commands) freeRequest(channel, request);
            } else freeRequest(channel, request);
        }
        request = next;
    }
    OSRestoreInterrupts(interrupts);
}
void gfRumble::HSD_PadRumbleRemoveMask(s32 pad, u32 mask) {
    gfRumbleChannel& channel = m_channels[pad];
    BOOL interrupts = OSDisableInterrupts();
    gfRumbleRequest* request = channel.requests;
    while (request) {
        gfRumbleRequest* next = request->next;
        if (request->mask & mask) freeRequest(channel, request);
        request = next;
    }
    OSRestoreInterrupts(interrupts);
}
void gfRumble::HSD_PadRumblePauseAll() {
    for (int pad = 0; pad < 8; ++pad) {
        BOOL interrupts = OSDisableInterrupts();
        gfRumbleRequest* request = m_channels[pad].requests;
        while (request) {
            gfRumbleRequest* next = request->next;
            request->paused = 1;
            request = next;
        }
        OSRestoreInterrupts(interrupts);
    }
}
void gfRumble::HSD_PadRumbleActiveAll() {
    for (int pad = 0; pad < 8; ++pad) {
        BOOL interrupts = OSDisableInterrupts();
        gfRumbleRequest* request = m_channels[pad].requests;
        while (request) {
            gfRumbleRequest* next = request->next;
            request->paused = 0;
            request = next;
        }
        OSRestoreInterrupts(interrupts);
    }
}

// Lower priority values are visited first. Equal-priority requests are appended
// after existing peers; the last unpaused script to write a status wins.
int gfRumble::_HSD_PadRumbleAdd(s32 pad, s32 id, s32 frames, u8 priority, const u16* pattern, s32 mask) {
    if (((*pattern >> 13) & 7) == End) return 0;
    int added = 0;
    gfRumbleChannel& channel = m_channels[pad];
    BOOL interrupts = OSDisableInterrupts();
    gfRumbleRequest** link;
    gfRumbleRequest* request = m_freeRequests;
    if (request && channel.count < m_maxRequests) {
        m_freeRequests = request->next;
        request->id = id;
        request->mask = mask;
        request->paused = 0;
        request->priority = priority;
        request->status = HardStop;
        request->loopCount = 0;
        request->wait = 0;
        request->frames = frames;
        request->loopStart = NULL;
        request->pattern = pattern;
        request->cursor = pattern;
        link = &channel.requests;
        while (*link && (*link)->priority <= request->priority) link = &(*link)->next;
        request->next = *link;
        added = 1;
        *link = request;
        ++channel.count;
    }
    OSRestoreInterrupts(interrupts);
    return added;
}

// Commands use three opcode bits and thirteen frame/count bits. Zero-time
// commands run in this update; a timed command consumes its first frame now.
// -2 ends after one pass, -1 repeats indefinitely, positive frames cap playback.
int gfRumble::_HSD_PadRumbleInterpret1(gfRumbleRequest* request, u8* status) {
    if (request->paused == 1) return 0;
    while (request->wait == 0) {
        const u16* cursor = request->cursor;
        switch ((*cursor >> 13) & 7) {
        case End:
            if (request->frames == -2) return 1;
            request->cursor = request->pattern;
            if (((*request->pattern >> 13) & 7) == End) return 1;
            break;
        case MotorRun:
            request->status = Run;
            request->wait = *cursor & 0x1FFF;
            request->cursor = cursor + 1;
            break;
        case MotorStop:
            request->status = Stop;
            request->wait = *cursor & 0x1FFF;
            request->cursor = cursor + 1;
            break;
        case MotorHardStop:
            request->status = HardStop;
            request->wait = *cursor & 0x1FFF;
            request->cursor = cursor + 1;
            break;
        case Loop:
            request->loopCount = *cursor & 0x1FFF;
            request->cursor = cursor + 1;
            request->loopStart = cursor + 1;
            break;
        case LoopEnd:
            --request->loopCount;
            if (request->loopCount != 0) request->cursor = request->loopStart;
            else request->cursor = cursor + 1;
            break;
        }
    }
    *status = request->status;
    --request->wait;
    if (request->frames != -1 && request->frames != -2) {
        if (--request->frames == 0) return 1;
    }
    return 0;
}

void gfRumble::HSD_PadRumbleInterpret() {
    gfRumbleChannel* channel = m_channels;
    gfRumbleRequest* request;
    gfRumbleRequest* next;
    int pad;
    for (pad = 0; pad < 8; ++pad, ++channel) {
        channel->status = HardStop;
        if (m_disabled == 0) {
            channel->status = channel->directStatus;
            request = channel->requests;
            while (request) {
                next = request->next;
                if (_HSD_PadRumbleInterpret1(request, &channel->status)) freeRequest(*channel, request);
                request = next;
            }
        }
        if (channel->status != channel->lastStatus) {
            switch (channel->status) {
            case HardStop: m_padSystem->stopMotorH(pad); break;
            case Stop: m_padSystem->stopMotor(pad); break;
            case Run: m_padSystem->startMotor(pad); break;
            }
            channel->lastStatus = channel->status;
        }
    }
}

void gfRumble::_HSD_PadRumbleInit(int count, gfRumbleRequest* pool) {
    m_disabled = 0;
    m_maxRequests = count;
    m_freeRequests = pool;
    if (count != 0) {
        for (int i = 0; i < count - 1; ++i) m_freeRequests[i].next = &m_freeRequests[i + 1];
        m_freeRequests[count - 1].next = NULL;
    }
    for (int pad = 0; pad < 8; ++pad) {
        m_channels[pad].lastStatus = lbl_80495380.lastStatus;
        m_channels[pad].status = lbl_80495380.status;
        m_channels[pad].directStatus = lbl_80495380.directStatus;
        m_channels[pad].count = lbl_80495380.count;
        m_channels[pad].requests = lbl_80495380.requests;
    }
}
