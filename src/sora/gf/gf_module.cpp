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

extern "C" void OSLinkFixed(gfModuleHeader* header, void* bss);

gfModule* gfModule::create(void* heap, void* buffer, size_t size) {
    u32 bufSize = static_cast<gfModuleHeader*>(buffer)->bssSize + static_cast<gfModuleHeader*>(buffer)->commandOffset + 0x40;
    gfModule* module = static_cast<gfModule*>(gfMemoryPool::alloc(heap, bufSize + 0x8C, 0x20));
    if (module == NULL) {
        reinterpret_cast<gfMemoryPool*>(heap)->dump();
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
        OSLinkFixed(header, bss);
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

// TODO: clean up, give variables real names
namespace jj_80026C10 {
extern "C" void fn_80026C10(int *p) {
    p[5] = 0;
}
}

namespace jj_80026C1C {
extern "C" {
void __dt__14gfFileIOHandleFv(void* p, int flag);
void __dl__FPv(void* p);

// TODO: clean up, give variables real names
void* fn_80026C1C(void* p, int n) {
    if (p) {
        __dt__14gfFileIOHandleFv((char*)p + 0x14, -1);
        if (n > 0)
            __dl__FPv(p);
    }
    return p;
}
}
}

// TODO: clean up, give variables real names
namespace jj_80026B14 {
struct Flags {
    unsigned char b7:1, b6:1, b5:1, b4:1, b3:1, b2:1, b1:1, b0:1;
};

struct Entry {
    int a, b, c, d, e, f;
    unsigned char x;
    Flags flags;
    unsigned char y;
    unsigned char pad[0x21];
};

struct View {
    int header[2];
    Entry entry;
};

struct Object {
    int a, b;
    Entry entries[16];
};

extern "C" {
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void fn_80026C10(void*);
void fn_80026C1C(void*);
void __construct_array(void*, void (*)(void*), void (*)(void*), unsigned long, int);
void __dt__14gfFileIOHandleFv(void*, int);
extern Object* lbl_805A0038;

// TODO: clean up, give variables real names
void fn_80026B14() {
    if (lbl_805A0038 == 0) {
        unsigned char* obj = (unsigned char*)__nw__FUlQ25Heaps8HeapType(0x3C8, 1);
        if (obj != 0) {
            __construct_array(obj + 8, fn_80026C10, fn_80026C1C, 0x3C, 16);
            *(int*)(obj + 4) = 0;
            unsigned int i = 0;
            int offset = 0;
            do {
                ((View*)(obj + offset))->entry.a = 0;
                ((View*)(obj + offset))->entry.e = 0;
                ((View*)(obj + offset))->entry.b = 0;
                int handle = 0;
                ((View*)(obj + offset))->entry.f = 0;
                __dt__14gfFileIOHandleFv(&handle, -1);
                ((View*)(obj + i * 0x3C))->entry.d = 0;
                ((View*)(obj + i * 0x3C))->entry.flags.b6 = 0;
                ((View*)(obj + i * 0x3C))->entry.flags.b7 =
                    ((View*)(obj + i * 0x3C))->entry.flags.b5 =
                    ((View*)(obj + i * 0x3C))->entry.flags.b4 =
                    ((View*)(obj + i * 0x3C))->entry.flags.b3 = 0;
                ((View*)(obj + i * 0x3C))->entry.flags.b2 = 1;
                ((View*)(obj + i * 0x3C))->entry.y = 0;
                ++i;
                offset += 0x3C;
            } while (i < 16);
            *(int*)obj = 0xFF;
        }
        lbl_805A0038 = (Object*)obj;
    }
}
}
}

namespace jj_80026CBC {
extern "C" {
int isReady__14gfFileIOHandleCFv(const void*);
int getReturnStatus__14gfFileIOHandleCFv(const void*);
void* getBuffer__14gfFileIOHandleCFv(const void*);
u32 getSize__14gfFileIOHandleCFv(const void*);
void release__14gfFileIOHandleFv(void*);
void* create__8gfModuleFPvPvUl(void*, void*, u32);
void* fn_80026828(void*, void*, u32);
void free__13gfHeapManagerFPv(void*);
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void fn_80026CBC(s32 arg0) {
    struct Loop { char* e; s32 i; } loop;
    loop.e = (char*)(arg0 + 8);
    loop.i = 0;
    do {
        u8 f = *(u8*)(loop.e + 0x19);
        if (((f >> 7) & 1) && !((f >> 4) & 1) && isReady__14gfFileIOHandleCFv(loop.e + 0x14) != 0 &&
            getReturnStatus__14gfFileIOHandleCFv(loop.e + 0x14) == 0) {
            *(u8*)(loop.e + 0x19) = *(u8*)(loop.e + 0x19) | 0x20;
            *(void**)(loop.e + 0x4) = getBuffer__14gfFileIOHandleCFv(loop.e + 0x14);
            *(u32*)(loop.e + 0x8) = getSize__14gfFileIOHandleCFv(loop.e + 0x14);
            if ((*(u8*)(loop.e + 0x19) >> 6) & 1) {
                void* b = *(void**)(loop.e + 0x4);
                if (*(s32*)(loop.e + 0xC) == 0) {
                    *(void**)loop.e = create__8gfModuleFPvPvUl(*(void**)(loop.e + 0x10), b, *(u32*)(loop.e + 0x8));
                    *(u8*)(loop.e + 0x19) = *(u8*)(loop.e + 0x19) | 0x4;
                } else {
                    *(void**)loop.e = fn_80026828(*(void**)(loop.e + 0xC), b, *(u32*)(loop.e + 0x8));
                    *(u8*)(loop.e + 0x19) = *(u8*)(loop.e + 0x19) & ~0x4;
                }
                {
                    u8* obj = *(u8**)loop.e;
                    if (!((obj[8] >> 7) & 1)) {
                        obj[8] = obj[8] | 0x80;
                        void** p3 = *(void***)obj;
                        typedef void* (*F)(void*);
                        ((F)p3[0x34 / 4])(p3);
                    }
                }
                free__13gfHeapManagerFPv(*(void**)(loop.e + 0x4));
                *(void**)(loop.e + 0x4) = 0;
            }
            release__14gfFileIOHandleFv(loop.e + 0x14);
            *(u8*)(loop.e + 0x19) = *(u8*)(loop.e + 0x19) | 0x10;
        }
        loop.i++;
        loop.e += 0x3C;
    } while (loop.i < 0x10);
}
}
}

// TODO: clean up, give variables real names
namespace jj_80026828 {

extern "C" int OSLinkFixed(int, int);

struct ModuleImport {
    u32 id;
    u32 offset;
};

extern "C" void* fn_80026828(void* output, gfModuleHeader* source) {
    if (output == 0) {
        PPCHalt();
    }
    void* bss;
    u32 importSize;
    u32 importOffset;
    gfModuleHeader* header = (gfModuleHeader*)(((u32)output + 0x2B) & ~31);
    importSize = source->impSize;
    importOffset = source->impOffset;
    bss = (void*)(((u32)header + source->commandOffset + 31) & ~31);
    memcpy(header, source, source->commandOffset);
    s32 delta = (u8*)source - (u8*)header;
    header->relOffset = delta + source->relOffset;
    header->impOffset = delta + source->impOffset;
    ModuleImport* cur = (ModuleImport*)((u8*)source + source->impOffset);
    ModuleImport* end = (ModuleImport*)((u8*)cur + importSize);
    for (; cur < end; ++cur) {
        cur->offset += delta;
    }
    header->getTextSectionAddr();
    if (output != 0) {
        *(gfModuleHeader**)output = header;
        *((s8*)output + 8) &= ~0xC0;
        *(void**)((u8*)output + 4) = bss;
        OSLinkFixed((int)header, (int)bss);
    }
    header->impOffset = (u32)header + importOffset;
    cur = (ModuleImport*)header->impOffset;
    end = (ModuleImport*)((u8*)cur + importSize);
    for (; cur < end; ++cur) {
        cur->offset += (u32)header;
    }
    return output;
}
}

// Returns the address of the first executable section (.text): the section table holds absolute addresses once the
// module is linked (it then lies above the header), offsets from the header before that.
// NONMATCHING: register allocation of the flag and the address
u32 gfModuleHeader::getTextSectionAddr() {
    bool linked = false;
    u32 offset = sectionInfoOffset;
    u32 addr = 0;
    u32 num = numSections;
    u32* section;
    if (reinterpret_cast<u32>(this) < offset) {
        linked = true;
        section = reinterpret_cast<u32*>(offset);
    } else {
        section = reinterpret_cast<u32*>(reinterpret_cast<u8*>(this) + offset);
    }
    for (u32 i = 0; i < num; i++) {
        u32 secOffset = section[0];
        if (secOffset & 1) {
            addr = secOffset & ~1;
            if (!linked) {
                addr += reinterpret_cast<u32>(this);
            }
            break;
        }
        section += 2;
    }
    return addr;
}
