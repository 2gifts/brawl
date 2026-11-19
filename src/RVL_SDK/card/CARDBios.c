#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_804ECE00[0x240] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0B98[0x8] __attribute__((aligned(4))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl.

int fn_8020A53C(int);

int fn_8020A4EC(int arg0) {
    if (arg0 == 0) {
        if (fn_8020A53C(0) == -1 || fn_8020A53C(1) == -1) {
            return 0;
        }
    }
    return 1;
}

// TODO: clean up, give variables real names
/* jj_8020A438 */
extern int OSDisableInterrupts(void);
extern void OSRestoreInterrupts(int);
typedef struct Blk {
    int unk0;
    int unk4;
    char pad[0xC8];
    int unkD0;
    char pad2[0x40];
    int unk114;
} Blk;
#define lbl_804ECE00 ((Blk*)&lbl_804ECE00)

int fn_8020A438(int arg0, Blk** arg1) {
    int r;
    int en;
    Blk* b = &lbl_804ECE00[arg0];
    if (arg0 < 0 || arg0 >= 2 || b->unk114 == 0) {
        return -0x80;
    }
    en = OSDisableInterrupts();
    if (b->unk0 == 0) {
        r = -3;
    } else if (b->unk4 == -1) {
        r = -1;
    } else {
        r = 0;
        b->unk4 = -1;
        b->unkD0 = 0;
        *arg1 = b;
    }
    OSRestoreInterrupts(en);
    return r;
}
#undef lbl_804ECE00

/* jj_8020A37C */
#define Blk Blk_jj8020A37C

typedef struct Blk {
    int x0;
    int x4;
    char pad[0x84];
    int x8c;
    char pad2[0x50];
    int xe0;
    char pad3[0x30];
    int x114;
} Blk;

#define lbl_804ECE00 ((Blk*)&lbl_804ECE00)
#define lbl_805A0B98 (*(u16*)&lbl_805A0B98)
extern const char* lbl_8059EB30;
extern int lbl_8047C080[4];

u16 OSGetFontEncode(void);
void OSInitThreadQueue(int* p);
void DSPInit(void);
void OSRegisterVersion(const char* ver);
void OSCreateAlarm(void* alarm);
void OSRegisterShutdownFunction(int* info);

void fn_8020A37C(void) {
    Blk* p;
    int i;
    if (lbl_804ECE00[0].x114 == 0 || lbl_804ECE00[1].x114 == 0) {
        lbl_805A0B98 = OSGetFontEncode();
        OSRegisterVersion(lbl_8059EB30);
        DSPInit();
        p = lbl_804ECE00;
        i = 0;
        do {
            p->x4 = -3;
            OSInitThreadQueue(&p->x8c);
            OSCreateAlarm(&p->xe0);
            i += 1;
            p += 1;
        } while (i < 2);
        lbl_804ECE00[0].x114 = 0x80000000;
        lbl_804ECE00[1].x114 = 0x80000000;
        OSRegisterShutdownFunction(lbl_8047C080);
    }
}
#undef Blk
#undef lbl_804ECE00
#undef lbl_805A0B98

