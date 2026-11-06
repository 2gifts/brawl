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
