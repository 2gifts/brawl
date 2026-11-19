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

extern int fn_803CD270(void);
extern Entry *fn_803CD3B4(int);
extern void fn_803CD318(int);
extern State *fn_803CD324(void);
extern int fn_803CD32C(void);
extern void fn_8020B898(void *, void *);
extern int fn_8020B268(void *, int, void (*)(void), void *);
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

    entry = fn_803CD3B4(a);
    enabled = OSDisableInterrupts();
    fn_803CD318(1);
    fn_803CD324()->state = 6;
    OSRestoreInterrupts(enabled);

    entry->a = c;
    entry->b = b;

    buf = (char *)fn_803CD3B4(a) + 0xd4;
    entry = fn_803CD3B4(a);
    info = &entry->kind;
    info[0] = 16;
    info[1] = a;
    fn_8020B898(buf, info);

    if (fn_803CD270())
        ptr = (char *)fn_803CD3B4(a) + 0x48;
    else
        ptr = 0;

    result = fn_8020B268(ptr, b, fn_803CFDB8, buf);
    switch (result) {
    case 0:
        break;
    case -1:
    case -3:
        switch (fn_803CD324()->state) {
        case 0:
        case 6: {
            BOOL enabled = OSDisableInterrupts();
            fn_803CD318(0);
            fn_803CD324()->state = 2;
            fn_803CD324()->error = result;
            OSRestoreInterrupts(enabled);
            break;
        }
        }
        break;
    case -8:
    case -64:
    case -128:
    default:
        switch (fn_803CD324()->state) {
        case 0:
        case 6: {
            BOOL enabled = OSDisableInterrupts();
            fn_803CD318(0);
            fn_803CD324()->state = 5;
            fn_803CD324()->error = result;
            OSRestoreInterrupts(enabled);
            break;
        }
        }
        break;
    }
    fn_803CD32C();
}

