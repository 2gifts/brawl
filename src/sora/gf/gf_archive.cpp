#include <gf/gf_file_io_handle.h>
#include <string.h>
#include <gf/gf_archive.h>
#include <gf/gf_heap_manager.h>
#include <revolution/BASE/PPCArch.h>
#include <types.h>
#include <ec/ec_mgr.h>

void* gfArchive::getData(ARCNodeType nodeType, int fileIndex, u16 archiveId) {
    int nodeSize;

    return getData(0, nodeType, fileIndex, &nodeSize, archiveId);
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_8059FF98[0x8] __attribute__((aligned(4))) = { 0 };

void gfArchiveEntryArray::release(u32 index) {
    if (unk8[index].unk5) {
        gfHeapManager::free(unk8[index].unk8);
        unk8[index].unk8 = 0;
        unk8[index].unk5 = 0;
    }
}

void gfArchive::setFileImageInternalSub(void* data, int nodeSize, void* heap) {
    gfArchiveFileHeader* header = (gfArchiveFileHeader*)data;
    m_fileImage = data;
    m_state = 3;
    m_fileSize = nodeSize;
    unk9 = true;
    m_returnStatus = 0;
    if (!header->isValid()) {
        PPCHalt();
    }
    m_nodeCount = header->m_nodeCount;
    if (m_nodeCount != 0) {
        if (!m_hasEntryArrays) {
            if (m_Heap) {
                heap = m_Heap;
            }
            createEntryArrayAll(header, heap);
        }
        if (m_hasEffects) {
            registEffect();
        }
    }
}

bool gfArchive::isExistEffect() {
    int index = 0;
    u32 resourceId;
    for (;;) {
        if (!enumEffectResourceId(index, &resourceId)) {
            break;
        }
        if (g_ecMgr->isExistEffectResource(resourceId)) {
            return true;
        }
        index++;
    }
    return false;
}

// HYPOTHESIS: the file IO handle is embedded at 0x6C (declared as a pointer in the header).
bool gfArchive::cancelRequest() {
    if (!unk9 && !unk9_2) {
        if (*(int*)&m_requestHandle != 0) {
            ((gfFileIOHandle*)&m_requestHandle)->cancelRequest();
            return true;
        }
    }
    return false;
}

bool gfArchive::isCanceled() {
    if (!unk9 && !unk9_2 && ((gfFileIOHandle*)&m_requestHandle)->isCanceled()) {
        return true;
    }
    return false;
}

int gfArchive::getReturnStatus() {
    return m_returnStatus;
}

// HYPOTHESIS: an effect entry points at a resource whose u16 at 4 is 0x101 and whose name starts at 0x10.
void gfArchive::registEffect() {
    gfArchiveEntry* entry;
    for (gfArchiveEntryArray* array = m_entries; array != 0; array = array->unk4) {
        entry = array->unk8;
        for (u32 i = 0; i < array->unk2; entry++, i++) {
            if (entry->m_type == 8) {
                gfArchiveFileHeader* res = (gfArchiveFileHeader*)entry->unk8;
                if (res != 0) {
                    char* name;
                    if (res->unk4 == 0x101) {
                        name = (char*)res + 0x10;
                    } else {
                        name = 0;
                    }
                    if (name != 0) {
                        g_ecMgr->registResource(name, res, entry->unkC, 0xff);
                    }
                }
            }
        }
    }
}

void gfArchive::releaseEffect() {
    gfArchiveEntry* entry;
    for (gfArchiveEntryArray* array = m_entries; array != 0; array = array->unk4) {
        entry = array->unk8;
        for (u32 i = 0; i < array->unk2; entry++, i++) {
            if (entry->m_type == 8) {
                gfArchiveFileHeader* res = (gfArchiveFileHeader*)entry->unk8;
                if (res != 0) {
                    char* name;
                    if (res->unk4 == 0x101) {
                        name = (char*)res + 0x10;
                    } else {
                        name = 0;
                    }
                    if (name != 0) {
                        g_ecMgr->releaseResource(name);
                    }
                }
            }
        }
    }
}

bool gfArchive::enumEffectResourceId(int index, u32* resourceId) {
    gfArchiveEntry* entry;
    gfArchiveFileHeader* res;
    gfArchiveEntryArray* array;
    for (array = m_entries; array != 0; array = array->unk4) {
        entry = array->unk8;
        for (u32 i = 0; i < array->unk2; entry++, i++) {
            if (entry->m_type == 8) {
                res = (gfArchiveFileHeader*)entry->unk8;
                if (res != 0) {
                    if (index == 0) {
                        char* name;
                        if (res->unk4 == 0x101) {
                            name = (char*)res + 0x10;
                        } else {
                            name = 0;
                        }
                        if (name != 0) {
                            *resourceId = g_ecMgr->searchResourceID(name);
                        } else {
                            *resourceId = 0x137;
                        }
                        return true;
                    }
                    index--;
                }
            }
        }
    }
    return false;
}

void* gfArchive::getData(int groupID, ARCNodeType nodeType, int fileIndex, u16 archiveId) {
    int nodeSize;

    return getData(groupID, nodeType, fileIndex, &nodeSize, archiveId);
}

void* gfArchive::getData(ARCNodeType nodeType, int fileIndex, int* nodeSize, u16 archiveId) {
    return getData(0, nodeType, fileIndex, nodeSize, archiveId);
}

// HYPOTHESIS: entry lookup by group, node type and file index, inlined in both search paths of getData.
static inline bool entryMatches(gfArchiveEntry* entry, u32 groupID, ARCNodeType nodeType, u16 fileIndex) {
    if (entry->unk1 == groupID && entry->m_type == nodeType && entry->unk2 == fileIndex) {
        return true;
    }
    return false;
}

static inline gfArchiveEntry* findEntry(gfArchiveEntryArray* array, u32 groupID, ARCNodeType nodeType, u16 fileIndex) {
    gfArchiveEntry* entry = array->unk8;
    gfArchiveEntry* found = 0;
    for (int i = 0; i < (int)array->unk2; i++, entry++) {
        if (entryMatches(entry, groupID, nodeType, fileIndex)) {
            found = entry;
            break;
        }
    }
    return found;
}

void* gfArchive::getData(int groupID, ARCNodeType nodeType, int fileIndex, int* nodeSize, u16 archiveId) {
    gfArchiveEntryArray* array = m_entries;
    if (archiveId == 0xFFFE) {
        for (; array != 0; array = array->unk4) {
            gfArchiveEntry* entry = findEntry(array, groupID, nodeType, fileIndex);
            if (entry != 0) {
                *nodeSize = entry->unkC;
                return entry->unk8;
            }
        }
    } else {
        for (; array != 0; array = array->unk4) {
            if (archiveId == array->unk0) {
                gfArchiveEntry* entry = findEntry(array, groupID, nodeType, fileIndex);
                if (entry != 0) {
                    *nodeSize = entry->unkC;
                    return entry->unk8;
                }
            }
        }
    }
    return 0;
}

// TODO: clean up, give variables real names
namespace jj_80014480 {
extern "C" {
int strlen(const char*);
void fn_80015F78(int*, int);
void fn_80016130(int*, int, int, char*, char*, char*);
void fn_800145A0(int, int*, int, int);
void __dl__FPv(void*);
}
static inline unsigned int findlast(int* s) {
    char* base;
    unsigned int offset = 0xFFFFFFFF;
    unsigned int n;
    if (!(((unsigned int)s[0]) >> 31)) {
        base = (char*)s + 1;
        n = *(unsigned char*)s & 0x7F;
    } else {
        base = (char*)s[2];
        n = s[1];
    }
    if (n != 0) {
        n -= 1;
        if (n < 0xFFFFFFFF)
            offset = n;
        char* p = base + offset;
        for (;;) {
            if (*p == '/')
                return p - base;
            if (p <= base)
                break;
            --p;
        }
    }
    return 0xFFFFFFFF;
}
extern "C" void fn_80014480(int arg0, char* arg1) {
    struct Locals {
        char flag;
        char pad[3];
        char type;
        char pad2[3];
        int s[3];
    } l;
    l.s[0] = 0;
    l.s[1] = 0;
    l.s[2] = 0;
    int len = strlen(arg1);
    fn_80015F78(l.s, len);
    l.flag = l.type;
    fn_80016130(l.s, 0, 0, arg1, arg1 + len, &l.flag);
    fn_800145A0(arg0, l.s, 0, findlast(l.s) + 1);
    if (((unsigned int)l.s[0]) >> 31)
        __dl__FPv((void*)l.s[2]);
}
}

// TODO: clean up, give variables real names
namespace jj_80013DEC {


struct EntryFlags {
    u8 owned : 1;
    u8 other : 7;
};

struct ArchiveFlags {
    u8 complete : 1;
    u8 padding : 2;
    u8 special : 1;
    u8 low : 4;
};

extern "C" void fn_8001429C(void*);
extern "C" void fn_80014028(void*);

#define F(p, t, o) (*(t*)((u8*)(p) + (o)))

#define CLEAR(node, cursor, flags, index)                             \
    cursor = node;                                                    \
    flags = node + 13;                                                \
    for (index = 0; index < F(node, u16, 2);                          \
         flags += 16, cursor += 16, index++) {                        \
        if ((*flags >> 7) & 1) {                                     \
            gfHeapManager::free(F(cursor, void*, 16));                \
            F(cursor, void*, 16) = 0;                                \
            ((EntryFlags*)flags)->owned = 0;                           \
        }                                                             \
    }

extern "C" void fn_80013DEC(u8* self) {
    u32 index;
    u32 index1;
    u32 index3;
    u8* flags3;
    u8* cursor3;
    u8* flags2;
    u8* cursor2;
    u32 index2;
    u8* flags1;
    u8* cursor1;
    u8* node;
    u8* flags;
    u8* cursor;
    u8* child1;
    u8* child2;
    u8* child3;

    if (F(self, void*, 0x20) != 0) {
        if ((self[9] >> 3) & 1)
            fn_8001429C(self);
        if ((self[9] >> 6) & 1)
            gfHeapManager::free(F(self, void*, 0x20));
        F(self, void*, 0x20) = 0;
    }

    if (F(self, void*, 0x1c) != 0) {
        gfHeapManager::free(F(self, void*, 0x1c));
        F(self, void*, 0x1c) = 0;
    } else {
        node = F(self, u8*, 0x68);
        if (node != 0) {
            CLEAR(node, cursor, flags, index)
            child1 = F(node, u8*, 4);
            if (child1 != 0) {
                CLEAR(child1, cursor1, flags1, index1)
                child2 = F(child1, u8*, 4);
                if (child2 != 0) {
                    CLEAR(child2, cursor2, flags2, index2)
                    child3 = F(child2, u8*, 4);
                    if (child3 != 0) {
                        CLEAR(child3, cursor3, flags3, index3)
                        if (F(child3, u8*, 4) != 0)
                            fn_80014028(F(child3, u8*, 4));
                        gfHeapManager::free(child3);
                    }
                    gfHeapManager::free(child2);
                }
                gfHeapManager::free(child1);
            }
            gfHeapManager::free(node);
        }
    }

    F(self, void*, 0x68) = 0;
    ((ArchiveFlags*)(self + 9))->special = 0;
    ((ArchiveFlags*)(self + 9))->complete = 0;
}
}

namespace jj_800134C4 {

struct EntryFlags {
    u8 owned : 1;
    u8 other : 7;
};

struct ArchiveFlags {
    u8 complete : 1;
    u8 padding : 2;
    u8 special : 1;
    u8 low : 4;
};

extern "C" void fn_80014028(void*);
extern "C" void fn_80014BF4(void*, void*, s32);

#define F(p, t, o) (*(t*)((u8*)(p) + (o)))
#define CLEAR(node, cursor, flags, index) \
    cursor = node; \
    flags = node + 13; \
    for (index = 0; index < F(node, u16, 2); \
         flags += 16, cursor += 16, index++) { \
        if ((*flags >> 7) & 1) { \
            gfHeapManager::free(F(cursor, void*, 16)); \
            F(cursor, void*, 16) = 0; \
            ((EntryFlags*)flags)->owned = 0; \
        } \
    }

extern "C" void fn_800134C4(u8* self, u8* arg1) {
    u8* archive;
    u32 index;
    u32 index1;
    u32 index3;
    u8* flags3;
    u8* cursor3;
    u8* flags2;
    u8* cursor2;
    u32 index2;
    u8* flags1;
    u8* cursor1;
    u8* node;
    u8* flags;
    u8* cursor;
    u8* child1;
    u8* child2;
    u8* data;
    u8* child3;
    data = F(arg1, u8*, 12);
    if (data != 0) {
        archive = F(self, u8*, 8);
        if (!((archive[9] >> 1) & 1) && F(archive, s32, 20) != 0 && F(data, u16, 6) != 0) {
            if (F(archive, void*, 0x1c) != 0) {
                gfHeapManager::free(F(archive, void*, 0x1c));
                F(archive, void*, 0x1c) = 0;
            } else {
                node = F(archive, u8*, 0x68);
                if (node != 0) {
                    CLEAR(node, cursor, flags, index)
                    child1 = F(node, u8*, 4);
                    if (child1 != 0) {
                        CLEAR(child1, cursor1, flags1, index1)
                        child2 = F(child1, u8*, 4);
                        if (child2 != 0) {
                            CLEAR(child2, cursor2, flags2, index2)
                            child3 = F(child2, u8*, 4);
                            if (child3 != 0) {
                                CLEAR(child3, cursor3, flags3, index3)
                                if (F(child3, u8*, 4) != 0)
                                    fn_80014028(F(child3, u8*, 4));
                                gfHeapManager::free(child3);
                            }
                            gfHeapManager::free(child2);
                        }
                        gfHeapManager::free(child1);
                    }
                    gfHeapManager::free(node);
                }
            }
            F(archive, void*, 0x68) = 0;
            ((ArchiveFlags*)(archive + 9))->special = 0;
            ((ArchiveFlags*)(archive + 9))->complete = 0;
            u8* object = F(self, u8*, 8);
            fn_80014BF4(object, data, F(object, s32, 20));
        }
    }
}
}

namespace jj_80014ED4 {

extern "C" void fn_80014028(void*);
extern "C" void fn_8001429C(void*);
extern "C" void fn_80014AF8(void*, void*, int, void*);

#define FIELD(p,t,o) (*(t*)((u8*)(p)+(o)))
struct EntryFlags { u8 owned : 1; u8 other : 7; };
struct ArchiveFlags { u8 complete : 1; u8 padding : 2; u8 special : 1; u8 low : 4; };
#define CLEAR_NODE(node) \
    u8* cursor = node; \
    u8* flags = node + 13; \
    for (u32 i = 0; i < FIELD(node,u16,2); i++, flags += 16, cursor += 16) { \
        if ((*flags >> 7) & 1) { \
            gfHeapManager::free(FIELD(cursor,void*,16)); \
            FIELD(cursor,void*,16) = NULL; \
            ((EntryFlags*)flags)->owned = 0; \
        } \
    }
static inline void clearNodes1(u8* node) {
    CLEAR_NODE(node)
    u8* next = FIELD(node,u8*,4);
    if (next != NULL) fn_80014028(next);
    gfHeapManager::free(node);
}
static inline void clearNodes2(u8* node) {
    CLEAR_NODE(node)
    u8* next = FIELD(node,u8*,4);
    if (next != NULL) clearNodes1(next);
    gfHeapManager::free(node);
}
static inline void clearNodes3(u8* node) {
    CLEAR_NODE(node)
    u8* next = FIELD(node,u8*,4);
    if (next != NULL) clearNodes2(next);
    gfHeapManager::free(node);
}
static inline void clearNodes4(u8* node) {
    CLEAR_NODE(node)
    u8* next = FIELD(node,u8*,4);
    if (next != NULL) clearNodes3(next);
    gfHeapManager::free(node);
}
extern "C" int fn_80014ED4(u8* self, const char* path, Heaps::HeapType heapType) {
    u32 lastIndex;
    int size;
    void* data;
    int result;
    u8* flag3;
    u8* cursor3;
    u32 index2;
    u8* flag2;
    u8* cursor2;
    u32 index1;
    u8* flag1;
    u8* cursor1;
    u8* node;
    u32 index;
    u8* flag0;
    u8* cursor0;
    u8* child1;
    u8* child2;
    u8* child3;
    void* heap = gfHeapManager::getHeap(heapType);
    ((ArchiveFlags*)(self + 9))->complete = 0;
    result = ((gfFileIOHandle*)(self+0x6c))->read(path, (const void*)heap, 0);
    if (result) {
        void* header = ((gfFileIOHandle*)(self+0x6c))->getBuffer();
        int valid;
        if (FIELD(header,u32,0) == 0x41524300 && (u16)(FIELD(header,u16,4) + 0xff00) <= 1U) valid = 1;
        else valid = 0;
        if (valid) {
            size = ((gfFileIOHandle*)(self+0x6c))->getSize();
            data = ((gfFileIOHandle*)(self+0x6c))->getBuffer();
            if (FIELD(self,void*,0x20) != NULL) {
                if ((self[9] >> 3) & 1) fn_8001429C(self);
                if ((self[9] >> 6) & 1) gfHeapManager::free(FIELD(self,void*,0x20));
                FIELD(self,void*,0x20) = NULL;
            }
            if (FIELD(self,void*,0x1c) != NULL) {
                gfHeapManager::free(FIELD(self,void*,0x1c));
                FIELD(self,void*,0x1c) = NULL;
            } else {
                node = FIELD(self,u8*,0x68);
                if (node != NULL) {
cursor0 = node; flag0 = node + 13;
for (index = 0; index < FIELD(node,u16,2); flag0 += 16, cursor0 += 16, index++) {
if ((*flag0 >> 7) & 1) { gfHeapManager::free(FIELD(cursor0,void*,16)); FIELD(cursor0,void*,16) = NULL; ((EntryFlags*)flag0)->owned = 0; }
}
child1 = FIELD(node,u8*,4);
if (child1 != NULL) {
cursor1 = child1; flag1 = child1 + 13;
for (index1 = 0; index1 < FIELD(child1,u16,2); flag1 += 16, cursor1 += 16, index1++) {
if ((*flag1 >> 7) & 1) { gfHeapManager::free(FIELD(cursor1,void*,16)); FIELD(cursor1,void*,16) = NULL; ((EntryFlags*)flag1)->owned = 0; }
}
child2 = FIELD(child1,u8*,4);
if (child2 != NULL) {
cursor2 = child2; flag2 = child2 + 13;
for (index2 = 0; index2 < FIELD(child2,u16,2); flag2 += 16, cursor2 += 16, index2++) {
if ((*flag2 >> 7) & 1) { gfHeapManager::free(FIELD(cursor2,void*,16)); FIELD(cursor2,void*,16) = NULL; ((EntryFlags*)flag2)->owned = 0; }
}
child3 = FIELD(child2,u8*,4);
if (child3 != NULL) {
cursor3 = child3; flag3 = child3 + 13;
for (lastIndex = 0; lastIndex < FIELD(child3,u16,2); flag3 += 16, cursor3 += 16, lastIndex++) {
if ((*flag3 >> 7) & 1) { gfHeapManager::free(FIELD(cursor3,void*,16)); FIELD(cursor3,void*,16) = NULL; ((EntryFlags*)flag3)->owned = 0; }
}
if (FIELD(child3,u8*,4) != NULL) fn_80014028(FIELD(child3,u8*,4));
gfHeapManager::free(child3);
}
gfHeapManager::free(child2);
}
gfHeapManager::free(child1);
}
gfHeapManager::free(node);
}
            }
            FIELD(self,void*,0x68) = NULL;
            ((ArchiveFlags*)(self + 9))->special = 0;
            ((ArchiveFlags*)(self + 9))->complete = 0;
            fn_80014AF8(self, data, size, heap);
            self[9] |= 0x40;
            ((gfFileIOHandle*)(self+0x6c))->release();
            strncpy((char*)self + 0x28, path, 64);
            ((ArchiveFlags*)(self + 9))->complete = result;
        } else result = 0;
    }
    return result;
}
}

