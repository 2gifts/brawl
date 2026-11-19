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

