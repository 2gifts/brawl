#include <types.h>
#include <gf/gf_module.h>
#include <cstring>
#include <gf/gf_memory_pool.h>
#include <revolution/BASE/PPCArch.h>
#include <revolution/OS/OSError.h>

struct OSImportInfo {
    u32 id;
    u32 offset;
};

extern "C" void fn_800260B4(void* block);
extern "C" void fn_801DDC4C(gfModuleHeader* header, void* bss);

gfModule* gfModule::create(void* heap, void* buffer, size_t size) {
    u32 bufSize = static_cast<gfModuleHeader*>(buffer)->bssSize + static_cast<gfModuleHeader*>(buffer)->commandOffset + 0x40;
    gfModule* module = static_cast<gfModule*>(gfMemoryPool::alloc(heap, bufSize + 0x8C, 0x20));
    if (module == NULL) {
        fn_800260B4(heap);
        OSReport("gfModule::create Error : Can't Alloc Heap Buffer %08x %08x %08x\n", bufSize + 0x8C, bufSize, size);
        PPCHalt();
    }
    gfModuleHeader* src = static_cast<gfModuleHeader*>(buffer);
    void* bss;
    u32 fixSize = src->commandOffset;
    u32 impSize = src->impSize;
    u32 impOffset = src->impOffset;
    gfModuleHeader* header = reinterpret_cast<gfModuleHeader*>(((u32)module + sizeof(gfModule) + 0x1F) & ~0x1F);
    bss = reinterpret_cast<void*>(((u32)header + fixSize + 0x1F) & ~0x1F);
    memcpy(header, src, fixSize);
    s32 diff = (u32)src - (u32)header;
    header->relOffset = diff + src->relOffset;
    header->impOffset = diff + src->impOffset;
    OSImportInfo* imp = reinterpret_cast<OSImportInfo*>((u32)src + src->impOffset);
    for (OSImportInfo* end = reinterpret_cast<OSImportInfo*>((u32)imp + impSize); imp < end; imp++) {
        imp->offset += diff;
    }
    OSReport("\x1B[34m <<gfModule>> create Instance (adr:0x%08x  size:0x%08x text:0x%08x) \x1B[00m \n", header, bufSize + 0x8C, header->getTextSectionAddr());
    if (module != NULL) {
        module->m_flags &= ~0xC0;
        module->m_header = header;
        module->m_bssAddr = bss;
        fn_801DDC4C(header, bss);
    }
    header->impOffset = (u32)header + impOffset;
    imp = reinterpret_cast<OSImportInfo*>(header->impOffset);
    for (OSImportInfo* end = reinterpret_cast<OSImportInfo*>((u32)imp + impSize); imp < end; imp++) {
        imp->offset += (u32)header;
    }
    return module;
}

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
