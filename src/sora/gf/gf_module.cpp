#include <types.h>
#include <gf/gf_module.h>

bool gfModuleManager::isLoaded(const char* moduleName) {
    gfModuleInfo* info = m_moduleInfos;
    bool loaded = false;

    for (u32 i = 0; i < 16; i++, info++) {
        if (strcmp(&info->m_moduleName, moduleName) == 0) {
            loaded = (m_moduleInfos[i].m_flags >> 4) & 1;
            break;
        }
    }
    return loaded;
}

#include <gf/gf_heap_manager.h>
#include <gf/gf_module.h>

void gfModuleManager::setTempolaryLoadHeap(HeapType heapType) {
    m_loadHeapAddr = gfHeapManager::getHeap(heapType);
    m_loadHeap = heapType;
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0038[0x8] __attribute__((aligned(4))) = { 0 };
