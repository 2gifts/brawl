#include <types.h>

class DWCAllocType {
public:
    static void* ntAlloc(int, u32, u32);
};

class gfMemoryPool {
public:
    static void* alloc(void* heapAddr, size_t size, int align);
};

struct OSMutex;
extern "C" void OSLockMutex(OSMutex* mutex);
extern "C" void OSUnlockMutex(OSMutex* mutex);
extern OSMutex lbl_8049EC78;

struct unk805A0434 {
    u8 unk0[0x353C];
    void* unk353C;
};
extern unk805A0434* lbl_805A0434;

void* DWCAllocType::ntAlloc(int arg0, u32 size, u32 arg2) {
    void* result;

    if (size != 0) {
        OSLockMutex(&lbl_8049EC78);
        result = gfMemoryPool::alloc(lbl_805A0434->unk353C, size, 0x20);
        OSUnlockMutex(&lbl_8049EC78);
        return result;
    }
    return nullptr;
}
