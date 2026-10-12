#include <gf/gf_memory_pool.h>
#include <types.h>

struct gfMemoryPoolBlock {
    u8 unk0[0x4];
    s32 unk4;
    gfMemoryPoolBlock* unk8;
};

struct gfMemoryPoolList {
    u8 unk0[0x8];
    gfMemoryPoolBlock* unk8;
};

struct gfMemoryPoolState {
    u8 unk0[0x20];
    gfMemoryPoolList* unk20;
};

u32 gfMemoryPool::getMaxFreeBlockSize()
{
    gfMemoryPoolState* self = reinterpret_cast<gfMemoryPoolState*>(this);
    u32 maxSize = 0;
    gfMemoryPoolBlock* block = self->unk20->unk8;
    for (;;) {
        s32 size = block->unk4 - 0x20;
        size = size > 0 ? size : 0;
        if (maxSize < size) {
            maxSize = size;
        }
        gfMemoryPoolBlock* next = block->unk8;
        if (block >= next) {
            return maxSize;
        }
        block = next;
    }
}
