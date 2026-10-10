#include "private/iostypes.h"
#include "revolution/os.h"
#include "revolution/ipc/memory.h"

// ppc-side IOS heap allocator (shared protocol with the IOP side, but only the ppc half lives here).
// HYPOTHESIS: chunk status words are 0xBABE000x magic, mostly judging by the constants in the asm

#define CACHE_LINE_SIZE 32
#define CACHE_LINE_MASK (CACHE_LINE_SIZE - 1)

#define IOS_CHUNK_FREE          0xbabe0000
#define IOS_CHUNK_INUSE         0xbabe0001
#define IOS_CHUNK_INUSE_ALIGNED 0xbabe0002

#define IOS_HEAP_MAX 8

typedef struct IOSChunk {
    u32 status;
    u32 size;
    struct IOSChunk *prevFree;
    struct IOSChunk *nextFree;
} IOSChunk;

typedef struct IOSHeap {
    void *base;
    u32 owner; // never written on this side as far as i can tell
    u32 size;
    IOSChunk *freeList;
} IOSHeap;

static IOSHeap s_heaps[IOS_HEAP_MAX];

// merge p with the chunk right after it if they touch (gets inlined into iosFree)
static void coalesceChunk(IOSChunk *p) {
    if (p && ((u32)p->nextFree == ((u32)p + p->size + sizeof(IOSChunk)))) {
        IOSChunk *next = p->nextFree;
        p->nextFree = next->nextFree;
        if (p->nextFree) {
            p->nextFree->prevFree = p;
        }
        p->size += next->size + sizeof(IOSChunk);
    }
}

IOSHeapId iosCreateHeap(void *ptr, u32 size) {
    IOSError rv = -4;
    u32 mask;
    s32 i;
    IOSHeap *h;

    mask = OSDisableInterrupts();

    if ((u32)ptr & CACHE_LINE_MASK) {
        goto end;
    }

    for (i = 0; i < IOS_HEAP_MAX; ++i) {
        if (s_heaps[i].base == 0) {
            break;
        }
    }

    if (i == IOS_HEAP_MAX) {
        rv = -5;
        goto end;
    }

    h = s_heaps + i;
    h->base = ptr;
    h->size = size;
    h->freeList = ptr;
    h->freeList->status = IOS_CHUNK_FREE;
    h->freeList->size = size - sizeof(IOSChunk);
    h->freeList->prevFree = NULL;
    h->freeList->nextFree = NULL;
    rv = i;

end:
    OSRestoreInterrupts(mask);
    return rv;
}

// best-fit search over the free list, splits the chunk if enough is left over
void *__iosAlloc(IOSHeapId id, u32 size, u32 alignment) {
    u32 mask;
    IOSChunk *p, *best;
    IOSHeap *h;
    void *rv = NULL;

    mask = OSDisableInterrupts();

    if (!size) goto end;
    if (!alignment || (alignment & (alignment - 1))) goto end;
    if (alignment < CACHE_LINE_SIZE) alignment = CACHE_LINE_SIZE;

    size = (size + CACHE_LINE_MASK) & ~CACHE_LINE_MASK;

    if (id < 0 || id >= IOS_HEAP_MAX || !s_heaps[id].base) {
        rv = NULL;
        goto end;
    }
    h = s_heaps + id;

    p = h->freeList;
    best = 0;
    while (p) {
        u32 ptr = (u32)p + sizeof(IOSChunk);
        u32 extra = (alignment - (ptr & (alignment - 1))) & (alignment - 1);
        if ((p->size == size) && !extra) {
            best = p;
            break;
        } else if ((p->size >= (size + extra)) && (!best || (p->size < best->size))) {
            best = p;
        }
        p = p->nextFree;
    }

    p = best;
    if (p) {
        u32 ptr = (u32)p + sizeof(IOSChunk);
        u32 extra = (alignment - (ptr & (alignment - 1))) & (alignment - 1);

        if (p->size > (size + extra + sizeof(IOSChunk))) {
            IOSChunk *n = (IOSChunk *)((u8 *)p + size + extra + sizeof(IOSChunk));
            n->status = IOS_CHUNK_FREE;
            n->size = p->size - size - extra - sizeof(IOSChunk);
            n->nextFree = p->nextFree;
            if (n->nextFree) n->nextFree->prevFree = n;
            p->nextFree = n;
            p->size = size + extra;
        }

        p->status = IOS_CHUNK_INUSE;
        if (p->prevFree) {
            p->prevFree->nextFree = p->nextFree;
        } else {
            h->freeList = p->nextFree;
        }
        if (p->nextFree) {
            p->nextFree->prevFree = p->prevFree;
        }
        p->prevFree = p->nextFree = 0;
        rv = (u8 *)p + extra + sizeof(IOSChunk);
        if (extra) {
            // back-pointer header so iosFree can find the real chunk
            IOSChunk *n = (IOSChunk *)rv - 1;
            n->status = IOS_CHUNK_INUSE_ALIGNED;
            n->prevFree = p;
        }
    }

end:
    OSRestoreInterrupts(mask);
    return rv;
}

void *iosAlloc(IOSHeapId id, u32 size) {
    return __iosAlloc(id, size, CACHE_LINE_SIZE);
}

void *iosAllocAligned(IOSHeapId id, u32 size, u32 alignment) {
    return __iosAlloc(id, size, alignment);
}

IOSError iosFree(IOSHeapId id, void *ptr) {
    u32 mask;
    IOSChunk *p;
    IOSHeap *h;
    IOSError rv = -4;
    IOSChunk *prev;

    mask = OSDisableInterrupts();

    if (!ptr) {
        goto end;
    }

    if (id < 0 || id >= IOS_HEAP_MAX || !s_heaps[id].base) {
        rv = -4;
        goto end;
    }
    h = s_heaps + id;
    if (((u32)ptr) < (((u32)h->base) + sizeof(IOSChunk)) ||
        ((u32)ptr) > ((u32)h->base + h->size)) {
        goto end;
    }

    p = (IOSChunk *)ptr - 1;
    if (p->status == IOS_CHUNK_INUSE_ALIGNED) {
        p = p->prevFree;
    }
    if (p->status != IOS_CHUNK_INUSE) {
        goto end;
    }

    p->status = IOS_CHUNK_FREE;

    // free list is sorted by address, walk to the last chunk before p
    prev = h->freeList;
    while (prev) {
        if (!prev->nextFree || prev->nextFree > p) {
            break;
        }
        prev = prev->nextFree;
    }

    if (prev && p > prev) {
        p->prevFree = prev;
        p->nextFree = prev->nextFree;
        prev->nextFree = p;
        if (p->nextFree) {
            p->nextFree->prevFree = p;
        }
    } else {
        p->nextFree = h->freeList;
        h->freeList = p;
        p->prevFree = 0;
        if (p->nextFree) {
            p->nextFree->prevFree = p;
        }
    }
    coalesceChunk(p);
    coalesceChunk(p->prevFree);
    rv = 0;

end:
    OSRestoreInterrupts(mask);
    return rv;
}
