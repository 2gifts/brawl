#include <gf/gf_archive.h>
#include <gf/gf_heap_manager.h>
#include <types.h>

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
