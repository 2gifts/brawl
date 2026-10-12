// AXAlloc: voice allocator. free voices sit on one stack, busy ones on per priority stacks, and when a high
// priority acquire finds nothing free it steals the oldest voice of a lower priority.
#include "revolution/ax.h"
#include "revolution/os.h"

#define AX_PRIORITY_MIN 1
#define AX_PRIORITY_FREE 0
#define AX_PRIORITY_MAX 31
#define STACK_MAX (AX_PRIORITY_MAX + 1)

typedef struct AllocVPB AllocVPB;

struct AllocVPB {
    AllocVPB* next;                // at 0x0
    AllocVPB* prev;                // at 0x4
    AllocVPB* next1;               // at 0x8
    u32 priority;                  // at 0xC
    void (*callback)(AllocVPB*);   // at 0x10
    u32 userContext;               // at 0x14
    u32 index;                     // at 0x18
    u32 sync;                      // at 0x1C
    u32 depop;                     // at 0x20
    void* itdBuffer;               // at 0x24
    u8 pbHead[0x10];               // at 0x28
    u16 pbState;                   // at 0x38
};

extern void __AXSetPBDefault(AXVPB* vpb);

// declared bottom up so the small bss and sbss layout comes out in retail order
static AllocVPB* __AXCallbackStack = NULL;
static AllocVPB* __AXStackTail[STACK_MAX];
static AllocVPB* __AXStackHead[STACK_MAX];

AllocVPB* __AXGetStackHead(u32 prio);
void __AXServiceCallbackStack(void);
void __AXInitVoiceStacks(void);
void __AXAllocInit(void);
void __AXAllocQuit(void);
void __AXPushFreeStack(AllocVPB* vpb);
AllocVPB* __AXPopFreeStack(void);
void __AXPushCallbackStack(AllocVPB* vpb);
AllocVPB* __AXPopCallbackStack(void);
void __AXRemoveFromStack(AllocVPB* vpb);
void __AXPushStackHead(AllocVPB* vpb, u32 prio);
AllocVPB* __AXPopStackFromBottom(u32 prio);

AllocVPB* __AXGetStackHead(u32 prio) {
    return __AXStackHead[prio];
}

void __AXServiceCallbackStack(void) {
    AllocVPB* it = __AXPopCallbackStack();

    while (it != NULL) {
        if (it->priority > AX_PRIORITY_FREE) {
            if (it->callback != NULL) {
                it->callback(it);
            }

            __AXRemoveFromStack(it);
            __AXPushFreeStack(it);
        }

        it = __AXPopCallbackStack();
    }
}

void __AXInitVoiceStacks(void) {
    u32 prio;

    __AXCallbackStack = NULL;

    for (prio = 0; prio < STACK_MAX; prio++) {
        __AXStackHead[prio] = __AXStackTail[prio] = NULL;
    }
}

void __AXAllocInit(void) {
    __AXInitVoiceStacks();
}

void __AXAllocQuit(void) {
    __AXInitVoiceStacks();
}

void __AXPushFreeStack(AllocVPB* vpb) {
    vpb->next = __AXStackHead[AX_PRIORITY_FREE];
    __AXStackHead[AX_PRIORITY_FREE] = vpb;
    vpb->priority = AX_PRIORITY_FREE;
}

AllocVPB* __AXPopFreeStack(void) {
    AllocVPB* head = __AXStackHead[AX_PRIORITY_FREE];

    if (head != NULL) {
        __AXStackHead[AX_PRIORITY_FREE] = head->next;
    }

    return head;
}

void __AXPushCallbackStack(AllocVPB* vpb) {
    vpb->next1 = __AXCallbackStack;
    __AXCallbackStack = vpb;
}

AllocVPB* __AXPopCallbackStack(void) {
    AllocVPB* head = __AXCallbackStack;

    if (head != NULL) {
        __AXCallbackStack = head->next1;
    }

    return head;
}

void __AXRemoveFromStack(AllocVPB* vpb) {
    u32 prio = vpb->priority;
    AllocVPB* head = __AXStackHead[prio];
    AllocVPB* tail = __AXStackTail[prio];

    if (head == tail) {
        __AXStackTail[prio] = NULL;
        __AXStackHead[prio] = NULL;
    } else if (vpb == head) {
        __AXStackHead[prio] = vpb->next;
        __AXStackHead[prio]->prev = NULL;
    } else if (vpb == tail) {
        __AXStackTail[prio] = vpb->prev;
        __AXStackTail[prio]->next = NULL;
    } else {
        head = vpb->prev;
        tail = vpb->next;
        head->next = tail;
        tail->prev = head;
    }
}

void __AXPushStackHead(AllocVPB* vpb, u32 prio) {
    vpb->next = __AXStackHead[prio];
    vpb->prev = NULL;

    if (vpb->next != NULL) {
        __AXStackHead[prio]->prev = vpb;
        __AXStackHead[prio] = vpb;
    } else {
        __AXStackTail[prio] = vpb;
        __AXStackHead[prio] = vpb;
    }

    vpb->priority = prio;
}

AllocVPB* __AXPopStackFromBottom(u32 prio) {
    AllocVPB* vpb = NULL;

    if (__AXStackHead[prio] != NULL) {
        if (__AXStackHead[prio] == __AXStackTail[prio]) {
            vpb = __AXStackHead[prio];
            __AXStackTail[prio] = NULL;
            __AXStackHead[prio] = NULL;
        } else if (__AXStackTail[prio] != NULL) {
            vpb = __AXStackTail[prio];
            __AXStackTail[prio] = vpb->prev;
            __AXStackTail[prio]->next = NULL;
        }
    }

    return vpb;
}

void AXFreeVoice(AllocVPB* vpb) {
    BOOL enabled = OSDisableInterrupts();

    __AXRemoveFromStack(vpb);

    if (vpb->pbState == 1) {
        vpb->depop = TRUE;
    }

    __AXSetPBDefault((AXVPB*)vpb);
    __AXPushFreeStack(vpb);

    OSRestoreInterrupts(enabled);
}

AllocVPB* AXAcquireVoice(u32 prio, void (*callback)(AllocVPB*), u32 userContext) {
    BOOL enabled = OSDisableInterrupts();
    AllocVPB* vpb = __AXPopFreeStack();

    if (vpb == NULL) {
        u32 i;

        for (i = AX_PRIORITY_MIN; i < prio; i++) {
            vpb = __AXPopStackFromBottom(i);

            if (vpb != NULL) {
                if (vpb->pbState == 1) {
                    vpb->depop = TRUE;
                }

                if (vpb->callback != NULL) {
                    vpb->callback(vpb);
                }

                break;
            }
        }
    }

    if (vpb != NULL) {
        __AXPushStackHead(vpb, prio);
        vpb->callback = callback;
        vpb->userContext = userContext;
        __AXSetPBDefault((AXVPB*)vpb);
    }

    OSRestoreInterrupts(enabled);
    return vpb;
}

void AXSetVoicePriority(AllocVPB* vpb, u32 prio) {
    BOOL enabled = OSDisableInterrupts();

    __AXRemoveFromStack(vpb);
    __AXPushStackHead(vpb, prio);

    OSRestoreInterrupts(enabled);
}
