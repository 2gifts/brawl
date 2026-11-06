#pragma once

#include <sr/sr_common.h>
#include <types.h>

// HYPOTHESIS: the 0x20-byte header at the front of a section buffer.
struct nteSectionData {
    u64 m_wiiId;
    s64 m_created;
    u32 m_crc;
    u8 m_region;
    u8 m_dataType;
    u8 m_stateType;
    u8 unk17;
    u32 unk18;
    u32 m_dataSize;
};
static_assert(sizeof(nteSectionData) == 0x20, "Class is the wrong size!");

// HYPOTHESIS: one slot of nteFile. The first 0x20 bytes are the section header.
struct nteFileEntry {
    u8 m_header[0x1C];
    u32 m_dataSize;
    void* m_data;
};
static_assert(sizeof(nteFileEntry) == 0x24, "Class is the wrong size!");

class nteFile {
public:
    nteFileEntry m_entries[5];
    u32 m_count;
    Heaps::HeapType m_heap;

    nteFile(Heaps::HeapType heap);
    ~nteFile();
    void makeFile(u8* buffer);
    u32 getDataFileSize();
    static u32 getSections_Direct(const u8* buffer);
    static nteSectionData* getSectionData_Direct(u8* buffer, u32 index);
    bool addSection(nteSectionData* section);
};
static_assert(sizeof(nteFile) == 0xBC, "Class is the wrong size!");

class nteFileDataStage {
public:
    u32 unk0;
    Heaps::HeapType unk4;
    virtual ~nteFileDataStage();
    u8* unkC;
    u32 unk10;

    nteFileDataStage(Heaps::HeapType heap);
    void setData(void* data, u32 size);
};

class nteFileDataSetting {
public:
    u32 unk0;
    Heaps::HeapType unk4;
    virtual ~nteFileDataSetting();
    void* unkC;
};

// HYPOTHESIS: the replay data object, same layout as nteFileDataStage.
class nteFileDataReplay {
public:
    u32 unk0;
    Heaps::HeapType unk4;
    virtual ~nteFileDataReplay();
    u8* unkC;
    u32 unk10;

    nteFileDataReplay(Heaps::HeapType heap);
    void setData(void* data, u32 size);
};
