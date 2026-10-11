// mem_allocator: thin allocator objects that forward to an exp heap or a frame heap.
#include "revolution/mem/allocator.h"
#include "revolution/mem/expHeap.h"

extern void* MEMAllocFromFrmHeapEx(MEMiHeapHead* heap, u32 size, s32 align);

static void* AllocatorAllocForExpHeap_(MEMAllocator* allocator, u32 size) {
    return MEMAllocFromExpHeapEx((MEMHeapHandle)allocator->pHeap, size, allocator->heapParam1);
}

static void AllocatorFreeForExpHeap_(MEMAllocator* allocator, void* block) {
    MEMFreeToExpHeap((MEMHeapHandle)allocator->pHeap, block);
}

static void* AllocatorAllocForFrmHeap_(MEMAllocator* allocator, u32 size) {
    return MEMAllocFromFrmHeapEx((MEMiHeapHead*)allocator->pHeap, size, allocator->heapParam1);
}

static void AllocatorFreeForFrmHeap_(MEMAllocator* allocator, void* block) {
#pragma unused(allocator)
#pragma unused(block)
}

void* MEMAllocFromAllocator(MEMAllocator* allocator, u32 size) {
    return allocator->pFunc->pfAlloc(allocator, size);
}

void MEMFreeToAllocator(MEMAllocator* allocator, void* block) {
    allocator->pFunc->pfFree(allocator, block);
}

void MEMInitAllocatorForExpHeap(MEMAllocator* allocator, MEMHeapHandle heap, int align) {
    static const MEMAllocatorFunc sAllocatorFunc = {AllocatorAllocForExpHeap_, AllocatorFreeForExpHeap_};
    allocator->pFunc = &sAllocatorFunc;
    allocator->pHeap = heap;
    allocator->heapParam1 = align;
    allocator->heapParam2 = 0;
}

void MEMInitAllocatorForFrmHeap(MEMAllocator* allocator, MEMiHeapHead* heap, int align) {
    static const MEMAllocatorFunc sAllocatorFunc = {AllocatorAllocForFrmHeap_, AllocatorFreeForFrmHeap_};
    allocator->pFunc = &sAllocatorFunc;
    allocator->pHeap = heap;
    allocator->heapParam1 = align;
    allocator->heapParam2 = 0;
}
