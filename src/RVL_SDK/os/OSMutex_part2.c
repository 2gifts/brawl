#include "revolution/os.h"

#define DequeueItem(queue, mutex, link)         \
do {                                            \
    OSMutex* __next;                            \
    OSMutex* __prev;                            \
                                                \
    __next = (mutex)->link.next;                \
    __prev = (mutex)->link.prev;                \
                                                \
    if (__next == NULL)                         \
        (queue)->tail = __prev;                 \
    else                                        \
        __next->link.prev = __prev;             \
                                                \
    if (__prev == NULL)                         \
        (queue)->head = __next;                 \
    else                                        \
        __prev->link.next = __next;             \
} while (0)


void OSInitCond(OSCond* cond) {
    OSInitThreadQueue(&cond->queue);
}

void OSWaitCond(OSCond* cond, OSMutex* mutex) {
    BOOL enabled;
    OSThread* currentThread;
    s32 count;

    enabled = OSDisableInterrupts();
    currentThread = OSGetCurrentThread();

    if (mutex->thread == currentThread) {
        count = mutex->count;
        mutex->count = 0;
        DequeueItem(&currentThread->queueMutex, mutex, link);
        mutex->thread = NULL;
        if (currentThread->priority < currentThread->base) {
            currentThread->priority = __OSGetEffectivePriority(currentThread);
        }

        OSDisableScheduler();
        OSWakeupThread(&mutex->queue);
        OSEnableScheduler();
        OSSleepThread(&cond->queue);
        OSLockMutex(mutex);
        mutex->count = count;
    }

    OSRestoreInterrupts(enabled);
}

void OSSignalCond(OSCond* cond) {
    OSWakeupThread(&cond->queue);
}
