#include "revolution.h"

#include <revolution/OS/OSInterrupt.h>
// TODO: clean up, give variables real names
/* jj_8020221C */

typedef struct {
    u8 pad[0x1c];
    u32 flags;
    u8 pad2[0x10];
    u16 a;
    u16 b;
} A;

void fn_8020221C(A *arg0, int arg1) {
    BOOL status = OSDisableInterrupts();
    switch (arg1) {
    case 0: arg0->a = 2; break;
    case 1: arg0->a = 1; break;
    case 2: arg0->a = 0; arg0->b = 0; break;
    case 3: arg0->a = 0; arg0->b = 1; break;
    case 4: arg0->a = 0; arg0->b = 2; break;
    }
    arg0->flags |= 1;
    OSRestoreInterrupts(status);
}

// TODO: clean up, give variables real names
/* jj_802022DC */

typedef struct {
    char pad0[0x1c];
    unsigned int flags;
    unsigned int unk20;
    char pad1[0x14];
    unsigned short unk38;
} Obj;

void AXSetVoiceState(Obj* arg0, unsigned short arg1) {
    BOOL status = OSDisableInterrupts();
    if (arg0->unk38 == arg1) {
        OSRestoreInterrupts(status);
        return;
    }
    arg0->unk38 = arg1;
    arg0->flags |= 4;
    if (arg1 == 0) {
        arg0->unk20 = 1;
    }
    OSRestoreInterrupts(status);
}

// TODO: clean up, give variables real names
/* jj_8020234C */
typedef unsigned short u16;

extern int OSDisableInterrupts(void);
extern int OSRestoreInterrupts(int);

void AXSetVoiceAddr(void *arg0, void *arg1)
{
    int enabled = OSDisableInterrupts();

    *(u32 *)((char *)arg0 + 0x96) = *(u32 *)((char *)arg1 + 0);
    *(u32 *)((char *)arg0 + 0x9A) = *(u32 *)((char *)arg1 + 4);
    *(u32 *)((char *)arg0 + 0x9E) = *(u32 *)((char *)arg1 + 8);
    *(u32 *)((char *)arg0 + 0xA2) = *(u32 *)((char *)arg1 + 12);

    switch ((int)*(u16 *)((char *)arg1 + 2)) {
    case 0:
        break;
    case 10:
        *(u32 *)((char *)arg0 + 0xA6) = 0;
        *(u32 *)((char *)arg0 + 0xAA) = 0;
        *(u32 *)((char *)arg0 + 0xAE) = 0;
        *(u32 *)((char *)arg0 + 0xB2) = 0;
        *(u32 *)((char *)arg0 + 0xB6) = 0;
        *(u32 *)((char *)arg0 + 0xBA) = 0;
        *(u32 *)((char *)arg0 + 0xBE) = 0;
        *(u32 *)((char *)arg0 + 0xC2) = 0;
        *(u32 *)((char *)arg0 + 0xC6) = 0x08000000;
        *(u32 *)((char *)arg0 + 0xCA) = 0;
        break;
    case 25:
        *(u32 *)((char *)arg0 + 0xA6) = 0;
        *(u32 *)((char *)arg0 + 0xAA) = 0;
        *(u32 *)((char *)arg0 + 0xAE) = 0;
        *(u32 *)((char *)arg0 + 0xB2) = 0;
        *(u32 *)((char *)arg0 + 0xB6) = 0;
        *(u32 *)((char *)arg0 + 0xBA) = 0;
        *(u32 *)((char *)arg0 + 0xBE) = 0;
        *(u32 *)((char *)arg0 + 0xC2) = 0;
        *(u32 *)((char *)arg0 + 0xC6) = 0x01000000;
        *(u32 *)((char *)arg0 + 0xCA) = 0;
        break;
    }

    *(u32 *)((char *)arg0 + 0x1C) =
        (*(u32 *)((char *)arg0 + 0x1C) & 0xFFFF87FF) | 0x8400;

    OSRestoreInterrupts(enabled);
}

