#include <revolution/OS/OSInterrupt.h>
#include <types.h>

// TODO: clean up, give variables real names
/* jj_803CFF50 */

typedef struct {
    char pad[0x1b40];
    int state;
    int unused;
    int error;
} State;

typedef struct {
    int a, b;
    char pad[0x18c - 8];
    int kind;
    int index;
} Entry;

extern int RFLAvailable(void);
extern Entry *RFLiGetAccInfo(int);
extern void RFLiSetWorking(int);
extern State *RFLiGetManager(void);
extern int RFLGetAsyncStatus(void);
extern void NANDSetUserData(void *, void *);
extern int NANDGetLengthAsync(void *, int, void (*)(void), void *);
extern void fn_803CFDB8(void);

// TODO: clean up, give variables real names
void fn_803CFF50(int a, int b, int c)
{
    int result;
    BOOL enabled;
    int *info;
    void *buf;
    char *ptr;
    Entry *entry;

    entry = RFLiGetAccInfo(a);
    enabled = OSDisableInterrupts();
    RFLiSetWorking(1);
    RFLiGetManager()->state = 6;
    OSRestoreInterrupts(enabled);

    entry->a = c;
    entry->b = b;

    buf = (char *)RFLiGetAccInfo(a) + 0xd4;
    entry = RFLiGetAccInfo(a);
    info = &entry->kind;
    info[0] = 16;
    info[1] = a;
    NANDSetUserData(buf, info);

    if (RFLAvailable())
        ptr = (char *)RFLiGetAccInfo(a) + 0x48;
    else
        ptr = 0;

    result = NANDGetLengthAsync(ptr, b, fn_803CFDB8, buf);
    switch (result) {
    case 0:
        break;
    case -1:
    case -3:
        switch (RFLiGetManager()->state) {
        case 0:
        case 6: {
            BOOL enabled = OSDisableInterrupts();
            RFLiSetWorking(0);
            RFLiGetManager()->state = 2;
            RFLiGetManager()->error = result;
            OSRestoreInterrupts(enabled);
            break;
        }
        }
        break;
    case -8:
    case -64:
    case -128:
    default:
        switch (RFLiGetManager()->state) {
        case 0:
        case 6: {
            BOOL enabled = OSDisableInterrupts();
            RFLiSetWorking(0);
            RFLiGetManager()->state = 5;
            RFLiGetManager()->error = result;
            OSRestoreInterrupts(enabled);
            break;
        }
        }
        break;
    }
    RFLGetAsyncStatus();
}

