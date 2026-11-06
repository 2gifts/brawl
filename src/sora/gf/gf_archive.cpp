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
