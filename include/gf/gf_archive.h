// Local BrawlHeaders shadow: adds gfArchiveEntryArray declarations.
#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <memory.h>
#include <types.h>

enum ARCNodeType {
    Data_Type_None = 0x0,
    Data_Type_Misc = 0x1,
    Data_Type_Model = 0x2,
    Data_Type_Tex = 0x3,
    Data_Type_Anim = 0x4,
    Data_Type_Scene = 0x5,
    Data_Type_6 = 0x6,
    Data_Type_Group = 0x7,
    Data_Type_Effect = 0x8
};

class gfArchiveEntryArray;
class gfArchiveFileHeader {
public:
    // TODO
    union {
        u8 unk0[0x4];
        u32 m_magic;
    };
    u16 unk4;
    // HYPOTHESIS: number of archive nodes.
    u16 m_nodeCount;
public:
    void* getDataChunk();
    // HYPOTHESIS: checks the "ARC" magic and the format version.
    bool isValid() {
        if (m_magic == 0x41524300 && (u16)(unk4 + 0xFF00) <= 1) {
            return true;
        }
        return false;
    }
};
// TODO size assertion

class gfArchive {
public:
    // 0
    gfArchive* m_prevArchive;
    // 4
    gfArchive* m_nextArchive;
    // 8
    // HYPOTHESIS: archive state byte.
    s32 m_state : 8;
    bool unk9 : 1;
    bool unk9_1 : 1;
    bool unk9_2 : 1;
    // HYPOTHESIS: bit 0x10 of byte 9, set once the entry arrays exist.
    bool m_hasEntryArrays : 1;
    // HYPOTHESIS: bit 0x08 of byte 9, the archive holds effect resources.
    bool m_hasEffects : 1;
    s16 m_refCount; // 0xA
    union {
        u8 unkC[0x8];
        struct {
            u8 _padC[0x2];
            // HYPOTHESIS: copy of the file header node count.
            u16 m_nodeCount;
        };
    };
    // 20
    void* m_Heap;
    // 24
    union {
        char _spacer1[0x10];
        struct {
            u8 _pad18[0x4];
            // HYPOTHESIS: first entry array allocated for the archive.
            gfArchiveEntryArray* m_entryArray;
            // HYPOTHESIS: file image passed to setFileImageInternalSub.
            void* m_fileImage;
            // HYPOTHESIS: file image size.
            int m_fileSize;
        };
    };
    // 40
    char m_archiveFilePath[64];
    // 104
    int* m_entries;
    // 108
    gfFileIOHandle* m_requestHandle;
    // 112
    int m_returnStatus;
    // 116
    char _spacer2[4];

    // 120
    gfArchive();
    gfArchive(HeapType heap, int* unk1);
    virtual void notify(int unk1, gfFileIORequest* unk2);
    void* getData(ARCNodeType nodeType, int fileIndex, u16 archiveId);
    void* getData(int groupID, ARCNodeType nodeType, int fileIndex, u16 archiveId);
    void* getData(ARCNodeType nodeType, int fileIndex, int* nodeSize, u16 archiveId);
    void* getData(int groupID, ARCNodeType nodeType, int fileIndex, int* nodeSize, u16 archiveId);
    void setFileImage(void* data, int nodeSize, HeapType heapType);
    void update();
    void setFileImageInternalSub(void* data, int nodeSize, void* heap);
    void createEntryArrayAll(gfArchiveFileHeader* header, void* heap);
    void registEffect();
    void releaseEffect();
    bool enumEffectResourceId(int unk1, u32* unk2);
    void setupEntry(gfArchiveEntryArray* array, gfArchiveFileHeader* header);
    ~gfArchive();

    // 124
    char _spacer3[4];
};
static_assert(sizeof(gfArchive) == 0x80, "Class is wrong size!");

// HYPOTHESIS: one node of the archive data chunk, 0x20 byte header followed by the data.
struct gfArchiveNode {
    u16 unk0;
    u16 unk2;
    u32 m_size;
    u8 unk8;
    u8 unk9;
    u16 m_refIndex;
    u8 _padC[0x14];
};

// HYPOTHESIS: entry-array fields follow the release routines' loads and stride.
struct gfArchiveEntry {
    union {
        u32 unk0;
        struct {
            s32 m_type : 8;
            u8 unk1;
            u16 unk2;
        };
    };
    u8 unk4;
    u8 unk5 : 1;
    u8 unk5_1 : 1;
    u8 unk5_2 : 6;
    u16 unk6;
    void* unk8;
    u32 unkC;
};

// HYPOTHESIS: working copy of a node used while building an entry.
struct gfArchiveNodeInfo {
    u32 unk0;
    u16 unk4;
    u8 unk6;
    u8 unk7;
    u32 m_size;
    void* m_data;
};

class gfArchiveEntryArray {
public:
    u16 unk0;
    u16 unk2;
    gfArchiveEntryArray* unk4;
    gfArchiveEntry unk8[1];

    void release(u32 index);
    void release();
};
