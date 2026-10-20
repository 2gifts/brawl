#include <ip/ip_padrec.h>
#include <gf/gf_application.h>
#include <gf/gf_file_io_manager.h>
#include <gf/gf_heap_manager.h>
#include <gm/gm_global.h>
#include <memory.h>

// Native signature storage; the second word is outside the compared magic.
extern u32 g_replayMagic[2];

// HYPOTHESIS: source-level name for the byte-stream halfword reader.
static inline u16 readReplayCommandValue(const u8* command);

ipKeyRecorder::ipKeyRecorder(Heaps::HeapType heap, u32 capacity) {
    m_capacity = capacity;
    m_buffer = (ipReplayHeader*)gfHeapManager::alloc(heap, capacity);
    memset(m_buffer, 0, m_capacity);
    memset(m_pads, 0, 0x34);
    m_frame = 0;
    m_chunk = 0;
    m_cursor = NULL;
    m_end = (u8*)m_buffer + m_capacity;
    memset(m_callbacks, 0, sizeof(m_callbacks));
    m_mode = Idle;
    m_pauseStatus = 0;
    m_suppressFileSync = 0;
    clear(3);
}

void ipKeyRecorder::clear(u8 value) {
    ipReplayHeader* data = m_buffer;
    memset(data, 0, sizeof(ipReplayHeader));
    data->magic = g_replayMagic[0];
    data->dataSize = 0;
    data->frames = 0;
    for (int i = 0; i < 4; ++i) data->idleFrames[i] = 0;
    m_overflow = false;
    m_hasRecording = false;
    m_mode = Idle;
    m_pauseStatus = 0;
    m_suppressFileSync = 0;
    unk49 = value;
}

gmGlobalModeMelee* ipKeyRecorder::getBaseInfo() { return &m_buffer->baseInfo; }

bool ipKeyRecorder::setBaseInfo(const gmGlobalModeMelee* info) {
    memcpy(&m_buffer->baseInfo, info, sizeof(gmGlobalModeMelee));
    // The native format clears a match option whose meaning is still unknown.
    ((u8*)&m_buffer->baseInfo)[0x58] = 0;
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        gmPlayerInitData& player = m_buffer->baseInfo.m_playersInitData[i];
        memset(player.m_name, 0, 12);
        player.m_nameIndex = 0x78;
        player._0x17 = 0x78;
    }
    return true;
}

bool ipKeyRecorder::setAdvData(const void* data) {
    ipReplayHeader* header = m_buffer;
    header->advOffset = header->stageSize + sizeof(ipReplayHeader);
    header->advSize = 0x4958;
    memcpy((u8*)m_buffer + header->advOffset, data, header->advSize);
    return true;
}

void* ipKeyRecorder::getStageData() {
    ipReplayHeader* header = m_buffer;
    if (header->stageSize == 0) return NULL;
    return (u8*)header + header->stageOffset;
}

bool ipKeyRecorder::setStageData(const void* data, u16 size) {
    ipReplayHeader* header = m_buffer;
    header->stageOffset = header->advSize + sizeof(ipReplayHeader);
    header->stageSize = size;
    memcpy((u8*)m_buffer + header->stageOffset, data, header->stageSize);
    return true;
}

void ipKeyRecorder::getBufferData(void** data, u32* size) {
    *data = m_buffer;
    ipReplayHeader* header = m_buffer;
    *size = sizeof(ipReplayHeader);
    *size += header->stageSize;
    *size += header->advSize;
    *size += header->dataSize;
}

bool ipKeyRecorder::setBufferData(const void* data, u32 size) {
    if (data == NULL || size == 0) return false;
    bool loaded = false;
    if (m_capacity >= size && ((const ipReplayHeader*)data)->magic == g_replayMagic[0]) {
        m_mode = Idle;
        memcpy(m_buffer, data, size);
        loaded = true;
    }
    return loaded;
}

void ipKeyRecorder::setPadStatus(const ipPadStatus* pads, u32 count) {
    if (count == 0) return;
    if (count > 4) return;
    memcpy(m_pads, pads, count * sizeof(ipPadStatus));
}
u16 ipKeyRecorder::getMeleeInitialProcessCount() { return m_buffer->initialProcessCount; }
void ipKeyRecorder::setMeleeInitialProcessCount(u16 count) { m_buffer->initialProcessCount = count; }
void ipKeyRecorder::startFileReadSyncMode() { g_gfFileIOManager->setSyncMode(true); }

bool ipKeyRecorder::startRec(u32 chunk) {
    ipReplayHeader* header = m_buffer;
    if (chunk == 0) m_overflow = false;
    m_hasRecording = true;
    u32 offset = sizeof(ipReplayHeader) + header->stageSize + header->advSize;
    for (u8 i = 0; i < chunk; ++i) offset += header->chunks[i].size;
    header->chunks[chunk].offset = offset;
    header->chunks[chunk].size = 0;
    header->chunks[chunk].frames = 0;
    header->chunkCount = chunk + 1;
    m_chunk = chunk;
    m_padMask = 0;
    const gmGlobalModeMelee* mode = g_GameGlobal->m_modeMelee;
    for (int i = 0; i < 4; ++i) {
        if (mode->m_playersInitData[i].m_state == 0) m_padMask |= 1u << i;
    }
    m_commandCount = 0;
    m_applicationTick = g_gfApplication->unk100;
    m_phase = 0;
    seekFrame(0, m_buffer->chunks[m_chunk]);
    m_mode = Recording;
    m_pauseStatus = 0;
    m_suppressFileSync = 0;
    return true;
}

bool ipKeyRecorder::endRec() {
    if (m_mode != Recording) return false;
    m_mode = Idle;
    return true;
}

bool ipKeyRecorder::startPlay(int chunk) {
    if (m_buffer->chunks[chunk].frames == 0) return false;
    m_chunk = chunk;
    seekFrame(0, m_buffer->chunks[chunk]);
    m_mode = Playing;
    m_pauseStatus = 0;
    m_suppressFileSync = 0;
    m_phase = 0x80;
    return true;
}

bool ipKeyRecorder::endPlay() {
    if (m_mode != Playing) return false;
    m_mode = Idle;
    g_gfFileIOManager->setSyncMode(false);
    return true;
}

bool ipKeyRecorder::addCallBack(CallBack* callback) {
    bool added = false;
    for (u32 i = 0; i < 8; ++i) {
        if (m_callbacks[i] == NULL) {
            m_callbacks[i] = callback;
            added = true;
            break;
        }
    }
    return added;
}

bool ipKeyRecorder::removeCallBack(CallBack* callback) {
    bool removed = false;
    for (u8 i = 0; i < 8; ++i) {
        if (m_callbacks[i] == callback) {
            m_callbacks[i] = NULL;
            removed = true;
            break;
        }
    }
    return removed;
}

int ipKeyRecorder::getCurrentFrameNo() {
    if (m_mode == Playing) return m_frame - 1;
    if (m_mode == Recording) return m_frame;
    return -1;
}

// Pause-only commands update recorder state without entering the packet queue.
// Ordinary commands retain order, with space for seven per packet.
bool ipKeyRecorder::setSyncCommand(u32 command, int value) {
    if (m_mode != Recording) return true;
    if (command == 3) {
        if (value == 0) m_pauseStatus = 0;
        else m_pauseStatus = 1;
        return true;
    }
    if (command == 4) {
        if (value == 0) { m_pauseStatus = 0; m_suppressFileSync = 0; }
        else { m_pauseStatus = 1; m_suppressFileSync = 1; }
        return true;
    }
    if (m_suppressFileSync == 1 && command == 0) return true;
    if (m_commandCount < 7) {
        m_commands[m_commandCount].command = command;
        m_commands[m_commandCount].value = value;
        ++m_commandCount;
        switch (command) {
        case 1: m_padMask = value; break;
        case 5:
            if (value == 0) m_pauseStatus = 0;
            else m_pauseStatus = 2;
            break;
        }
        return true;
    }
    return false;
}

bool ipKeyRecorder::record() {
    if (m_mode != Recording) return false;
    u32 flags = m_padMask | ((m_commandCount & 7) << 4);
    flags |= m_phase;
    u32 size = packetSize(flags);
    u8 i;
    u8* cursor = m_cursor;
    if (cursor + size > m_end) {
        m_overflow = true;
        endRec();
        return false;
    }
    *cursor++ = flags;
    for (i = 0; i < 4; ++i) {
        if (m_padMask & (1u << i)) { memcpy(cursor, &m_pads[i], 4); cursor += 4; }
    }
    if (m_commandCount != 0) {
        memcpy(cursor, m_commands, m_commandCount * 4);
        cursor += m_commandCount * 4;
    }
    m_cursor = cursor;
    ++m_frame;
    m_commandCount = 0;
    if (m_applicationTick != g_gfApplication->unk100) {
        m_applicationTick = g_gfApplication->unk100;
        m_phase = m_phase != 0 ? 0 : 0x80;
    }
    ipReplayHeader* header = m_buffer;
    ++header->chunks[m_chunk].frames;
    header->chunks[m_chunk].size += size;
    header->dataSize += size;
    ++header->frames;
    for (u8 i = 0; i < 4; ++i) {
        if ((m_padMask & (1u << i)) && m_pads[i].m_button == 0 &&
            m_pads[i].m_stickX == 0 && m_pads[i].m_stickY == 0) ++header->idleFrames[i];
    }
    return true;
}

bool ipKeyRecorder::play(u8** packet, int* groupFrames, int* phaseFrames) {
    if (m_mode != Playing) {
        *packet = NULL; *groupFrames = 0; *phaseFrames = 0;
        return false;
    }
    u32 flags;
    u8 padCount;
    u32 commandCount;
    u8* current = m_cursor;
    flags = *current;
    u8 phase = flags & 0x80;
    commandCount = (flags >> 4) & 7;
    padCount = countPads(flags);
    int group = 1;
    u16 value;
    u8 kind;
    u8 j;
    const u8* command = current + 1 + padCount * 4;
    for (u8 i = 0; i < commandCount; ++i) {
        value = readReplayCommandValue(command);
        kind = *command;
        for (j = 0; j < 8; ++j) {
            CallBack* callback = m_callbacks[j];
            if (callback != NULL && callback->process(kind, value)) break;
        }
        command += 4;
    }
    *packet = current;
    advancePacket();
    u16 total;
    u8* lookahead;
    u16 frame = m_frame + 1;
    m_frame = frame;
    total = m_buffer->chunks[m_chunk].frames;
    if (total <= frame) endPlay();
    else {
        lookahead = m_cursor;
        for (;;) {
            u8 flags = *lookahead;
            if (phase != (flags & 0x80)) break;
            ++frame; ++group;
            if (total <= frame) break;
            int size = packetSize(flags);
            if (lookahead + size > m_end) break;
            lookahead += size;
        }
    }
    if (m_phase != phase) { m_phase = phase; *phaseFrames = group; }
    *groupFrames = group;
    return true;
}

bool ipKeyRecorder::extractPadStatus(ipPadStatus* pads, const u8* packet) {
    if (packet == NULL) return false;
    memset(pads, 0, 16);
    if ((*packet & 0xF) == 0) return false;
    const u8* cursor = packet + 1;
    for (u8 i = 0; i < 4; ++i) {
        if (*packet & (1u << i)) { memcpy(&pads[i], cursor, 4); cursor += 4; }
    }
    return true;
}

void ipKeyRecorder::clearOnamae(ipReplayHeader* data) {
    if (data->magic == g_replayMagic[0]) {
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            memset(data->baseInfo.m_playersInitData[i].m_name, 0, 12);
            data->baseInfo.m_playersInitData[i].m_nameIndex = 0x78;
        }
    }
}

// Native header-consistency check; packet contents are not traversed here.
bool ipKeyRecorder::isSafeData(const ipReplayHeader* data, u32 size) {
    if (data->magic != g_replayMagic[0]) return false;
    if (data->chunkCount == 0 || data->chunkCount > 4) return false;
    if (data->frames == 0) return false;
    if (data->dataSize > size - sizeof(ipReplayHeader)) return false;
    if ((data->advOffset != 0 && data->advSize == 0) ||
        (data->advOffset == 0 && data->advSize != 0)) return false;
    if ((data->stageOffset != 0 && data->stageSize == 0) ||
        (data->stageOffset == 0 && data->stageSize != 0)) return false;
    return true;
}

// HYPOTHESIS: private packet/seek implementation shared by recorder operations.
inline u8 ipKeyRecorder::countPads(u8 flags) {
    u8 count = 0;
    for (u8 i = 0; i < 4; ++i) if (flags & (1u << i)) ++count;
    return count;
}
inline int ipKeyRecorder::packetSize(u8 flags) {
    int size = 1 + countPads(flags) * 4;
    size += ((flags >> 4) & 7) * 4;
    return size;
}
inline bool ipKeyRecorder::advancePacket() {
    int size = packetSize(*m_cursor);
    if (m_cursor + size > m_end) return false;
    m_cursor += size;
    return true;
}
inline void ipKeyRecorder::seekFrame(int frame, const ipReplayChunk& chunk) {
    u8* start = (u8*)m_buffer + chunk.offset;
    m_frame = 0;
    m_cursor = start;
    u16 count = chunk.frames;
    for (int i = 0; i < frame; ++i) {
        if (!advancePacket() || count <= i) break;
        ++m_frame;
    }
}

static inline u16 readReplayCommandValue(const u8* command) {
    u16 value;
    // Packets are byte streams: the command halfword can be unaligned.
    __memcpy(&value, command + 2, sizeof(value));
    return value;
}
