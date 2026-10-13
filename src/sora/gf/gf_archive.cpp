#include <gf/gf_archive.h>
#include <types.h>

void* gfArchive::getData(ARCNodeType nodeType, int fileIndex, u16 archiveId) {
    int nodeSize;

    return getData(0, nodeType, fileIndex, &nodeSize, archiveId);
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_8059FF98[0x8] __attribute__((aligned(4))) = { 0 };
