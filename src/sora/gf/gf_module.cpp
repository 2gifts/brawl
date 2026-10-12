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
