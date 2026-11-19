#include <gf/gf_heap_manager.h>
#include <gf/gf_memory_pool.h>
#include <revolution/BASE/PPCArch.h>
#include <revolution/OS/OSArena.h>
#include <revolution/OS/OSError.h>
#include <sr/sr_common.h>
#include <types.h>

extern const char lbl_804224D8[];

// HYPOTHESIS: layout of the heap tables that gfApplication and gfBootTask pass in, ended by a NULL name.
struct gfHeapCreateInfo {
    const char* m_name;
    int m_heapId;
    int m_memArena;
    int m_heapSize;
};

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0000[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0008[0x8] __attribute__((aligned(4))) = { 0 };

void gfHeapManager::init() {
    for (int i = 0; i < 0x47; i++) {
        HeapInfo* info = &g_HeapInfos[i];
        info->m_name = NULL;
        info->m_memoryPool = NULL;
        info->m_size = 0;
    }
    // HYPOTHESIS: the first .sbss byte is an initialized flag.
    *reinterpret_cast<bool*>(lbl_805A0000) = true;
}

MEMAllocator* gfHeapManager::getMEMAllocator(Heaps::HeapType heapType) {
    gfMemoryPool* pool = g_HeapInfos[heapType].m_memoryPool;
    MEMAllocator* allocator = NULL;
    if (pool != NULL) {
        allocator = &pool->m_allocator;
    }
    return allocator;
}

void gfHeapManager::registHeap(int heapId, gfMemoryPool* pool) {
    const char* name = pool->m_name;
    u32 size = static_cast<u8*>(pool->m_end) - static_cast<u8*>(pool->m_start);
    u32 arena = 0;
    if ((reinterpret_cast<u32>(pool) >> 28) == 9) {
        arena = 1;
    }
    g_HeapInfos[heapId].m_memoryPool = pool;
    g_HeapInfos[heapId].m_name = name;
    g_HeapInfos[heapId].m_size = size;
    g_HeapInfos[heapId].m_arena = arena;
}

void gfHeapManager::removeHeap(int heapId) {
    g_HeapInfos[heapId].m_name = NULL;
    g_HeapInfos[heapId].m_memoryPool = NULL;
    g_HeapInfos[heapId].m_size = 0;
}

extern "C" void fn_800244C4(const gfHeapCreateInfo* infos) {
    for (;;) {
        if (infos->m_name == NULL) {
            break;
        }
        if (g_HeapInfos[infos->m_heapId].m_size == 0) {
            gfHeapManager::createHeap(infos->m_heapId, infos->m_name, infos->m_memArena, infos->m_heapSize);
        }
        infos++;
    }
    OSGetMEM1ArenaLo();
    OSGetMEM1ArenaHi();
    OSGetMEM2ArenaLo();
    OSGetMEM2ArenaHi();
}

extern "C" void fn_80024790(const gfHeapCreateInfo* infos) {
    const gfHeapCreateInfo* info = infos;
    for (;;) {
        if (info->m_name == NULL) {
            break;
        }
        info++;
    }
    do {
        int heapId = info[-1].m_heapId;
        info--;
        if (g_HeapInfos[heapId].m_size != 0) {
            gfHeapManager::destroyHeap(info->m_heapId);
        }
    } while (info != infos);
    OSGetMEM1ArenaLo();
    OSGetMEM1ArenaHi();
    OSGetMEM2ArenaLo();
    OSGetMEM2ArenaHi();
}

void gfHeapManager::destroyHeap(int heapId) {
    HeapInfo* info = &g_HeapInfos[heapId];
    gfMemoryPool* pool = info->m_memoryPool;
    if (pool == NULL) {
        OSReport("gfHeapManager>> Heap(%d) allready destroyed!!\n", heapId);
        return;
    }
    if (pool->m_allocCount != 0) {
        OSReport("gfHeapManager>> Heap(%-16s) has allocated block (%d)!!\n", pool->m_name, pool->m_allocCount);
        gfMemoryPool* dumped = g_HeapInfos[heapId].m_memoryPool;
        if (dumped != NULL) {
            OSReport("----------------------------------------\n");
            OSReport("-HeapName: %s\n", g_HeapInfos[heapId].m_name);
            OSReport("----------------------------------------\n");
            dumped->dump();
        }
        PPCHalt();
    }
    if (((reinterpret_cast<u32>(pool) >> 24) & 0xF0) == 0x80) {
        if (pool->m_end != OSGetMEM1ArenaLo()) {
            OSReport("end address error mem1 %08x %08x\n", pool->m_end, OSGetMEM1ArenaLo());
            PPCHalt();
        }
        if (pool->m_end == OSGetMEM1ArenaLo()) {
            OSSetMEM1ArenaLo(pool->m_start);
        }
    } else {
        if (pool->m_end != OSGetMEM2ArenaLo()) {
            OSReport("end address error mem2 %08x %08x\n", pool->m_end, OSGetMEM2ArenaLo());
            PPCHalt();
        }
        if (pool->m_end == OSGetMEM2ArenaLo()) {
            OSSetMEM2ArenaLo(pool->m_start);
        }
    }
    if (srSystemGetDebugLevel() < 4) {
        // HYPOTHESIS: the size conversion of a compiled-out report.
        float size = static_cast<s32>(static_cast<u8*>(pool->m_end) - static_cast<u8*>(pool->m_start));
    }
    g_HeapInfos[heapId].m_name = NULL;
    info->m_memoryPool = NULL;
    g_HeapInfos[heapId].m_size = 0;
}

void* gfHeapManager::getHeap(Heaps::HeapType heapType) {
    return g_HeapInfos[heapType].m_memoryPool;
}

void* gfHeapManager::alloc(Heaps::HeapType heapType, size_t size) {
    return gfMemoryPool::alloc(g_HeapInfos[heapType].m_memoryPool, size, 0x20);
}

void* gfHeapManager::alloc(Heaps::HeapType heapType, size_t size, s32 align) {
    return gfMemoryPool::alloc(g_HeapInfos[heapType].m_memoryPool, size, align);
}

void* gfHeapManager::calloc(Heaps::HeapType heapType, size_t size) {
    return gfMemoryPool::calloc(g_HeapInfos[heapType].m_memoryPool, size, 0x20);
}

int gfHeapManager::getMaxFreeSize(Heaps::HeapType heapType) {
    return g_HeapInfos[heapType].m_memoryPool->getMaxFreeBlockSize();
}


void gfHeapManager::free(void* ptr) {
    ::free(ptr);
}

void gfHeapManager::dumpAll() {
    const char* strs = lbl_804224D8; // MATCH-ONLY: keeps the string table base in a saved register
    HeapInfo* info;
    gfMemoryPool* pool;
    s32 total = 0;
    s32 i;

    OSReport(strs + 0x1EC);
    OSReport(strs + 0x228);
    OSReport(strs + 0x1EC);

    info = g_HeapInfos;
    for (i = 0; i < 0x47; i++) {
        pool = info->m_memoryPool;
        if (pool != NULL) {
            pool->dump();
            total += static_cast<u8*>(pool->m_end) - static_cast<u8*>(pool->m_start);
        }
        info++;
    }

    OSReport(strs + 0x268, total);
    OSReport(strs + 0x1EC);
}

// TODO: clean up, give variables real names
namespace jj_800242DC {

extern "C" {
extern u8 lbl_805A0000;

// TODO: clean up, give variables real names
void fn_800242DC(void) {
    int i;
    for (i = 0; i < 0x40; i += 16) {
        HeapInfo* p = &g_HeapInfos[i];
        p[0].m_name = 0;
        p[0].m_memoryPool = 0;
        p[0].m_size = 0;
        p[1].m_name = 0;
        p[1].m_memoryPool = 0;
        p[1].m_size = 0;
        p[2].m_name = 0;
        p[2].m_memoryPool = 0;
        p[2].m_size = 0;
        p[3].m_name = 0;
        p[3].m_memoryPool = 0;
        p[3].m_size = 0;
        p[4].m_name = 0;
        p[4].m_memoryPool = 0;
        p[4].m_size = 0;
        p[5].m_name = 0;
        p[5].m_memoryPool = 0;
        p[5].m_size = 0;
        p[6].m_name = 0;
        p[6].m_memoryPool = 0;
        p[6].m_size = 0;
        p[7].m_name = 0;
        p[7].m_memoryPool = 0;
        p[7].m_size = 0;
        p[8].m_name = 0;
        p[8].m_memoryPool = 0;
        p[8].m_size = 0;
        p[9].m_name = 0;
        p[9].m_memoryPool = 0;
        p[9].m_size = 0;
        p[10].m_name = 0;
        p[10].m_memoryPool = 0;
        p[10].m_size = 0;
        p[11].m_name = 0;
        p[11].m_memoryPool = 0;
        p[11].m_size = 0;
        p[12].m_name = 0;
        p[12].m_memoryPool = 0;
        p[12].m_size = 0;
        p[13].m_name = 0;
        p[13].m_memoryPool = 0;
        p[13].m_size = 0;
        p[14].m_name = 0;
        p[14].m_memoryPool = 0;
        p[14].m_size = 0;
        p[15].m_name = 0;
        p[15].m_memoryPool = 0;
        p[15].m_size = 0;
    }
    HeapInfo* q = &g_HeapInfos[i];
    q[0].m_name = 0;
    q[0].m_memoryPool = 0;
    q[0].m_size = 0;
    q[1].m_name = 0;
    q[1].m_memoryPool = 0;
    q[1].m_size = 0;
    q[2].m_name = 0;
    q[2].m_memoryPool = 0;
    q[2].m_size = 0;
    q[3].m_name = 0;
    q[3].m_memoryPool = 0;
    q[3].m_size = 0;
    q[4].m_name = 0;
    q[4].m_memoryPool = 0;
    q[4].m_size = 0;
    q[5].m_name = 0;
    q[5].m_memoryPool = 0;
    q[5].m_size = 0;
    q[6].m_name = 0;
    q[6].m_memoryPool = 0;
    q[6].m_size = 0;
    lbl_805A0000 = 1;
}
}
}

