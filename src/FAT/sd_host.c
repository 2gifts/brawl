#include <revolution/OS/OSError.h>
#include <types.h>

// HYPOTHESIS: SD card handle; the first word is the IOS file descriptor.
typedef struct ISDCard {
    s32 fd;
    u8 _04[0x20];
    s32 unk24;
} ISDCard;

void OSReport(const char* message, ...);
s32 IOS_Close(s32 fd);
s32 sduCommand(s32 fd, s32 command, s32 arg0, s32 arg1, u32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, void* arg7, void* arg8, void* arg9);

// HYPOTHESIS: sdata flag that is negative until the SD host driver is initialized.
s32 lbl_8059FE98[2] = { -1, 0 };

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_80563940[0x20] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80563960[0x60] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A1230[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1234[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1238[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A123C[0x4] __attribute__((aligned(4))) = { 0 };
void* lbl_805A1240 __attribute__((aligned(4))) = 0;
unsigned char lbl_805A1244[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1248[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A124C[0x4] __attribute__((aligned(4))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl.

void fn_803CB99C(int a, int b, int c, int d, int e, int f);

void ISD_ReadBlock(int a, int b, int c, int d) {
    fn_803CB99C(a, b, c, d, 0, 0);
}

void fn_803CBCD8(int a, int b, int c, int d, int e, int f);

void ISD_WriteBlock(int a, int b, int c, int d) {
    fn_803CBCD8(a, b, c, d, 0, 0);
}

extern void fn_803CB99C(int a, int b, int c, int d, int e, int f);

void ISD_ReadMultiBlock(int a, int b, int c, int d) {
    fn_803CB99C(a, b, c, d, 0, 0);
}

void fn_803CBCD8(int, int, int, int, int, int);

void ISD_WriteMultiBlock(int a, int b, int c, int d) {
    fn_803CBCD8(a, b, c, d, 0, 0);
}

int RFLGetWorkSize(int arg0) {
    if (arg0 != 0) {
        return 0x65F20;
    }
    return 0x4CF20;
}

void ISD_UnmountCard(ISDCard* card) {
    card->unk24 = 0;
    IOS_Close(card->fd);
    card->fd = -1;
}

s32 ISD_RegisterDeviceIntrHandler(ISDCard* card, void* handler, u32* arg) {
    s32 rc;
    u32 value;
    if (lbl_8059FE98[0] < 0) {
        rc = 0xC1000000;
        goto done;
    }
    if (handler == 0) {
        rc = 0xC1000000;
        goto done;
    }
    if (arg != 0) {
        value = *arg;
    } else {
        rc = -4;
        goto done;
    }
    rc = sduCommand(card->fd, 0x40, 0, 0, value, 0, 0, 0, 0, lbl_805A1240, handler, arg);
done:
    if (rc != 0) {
        OSReport("%s Error: %d\n", "ISD_RegisterDeviceIntrHandler", rc);
    }
    return rc;
}

s32 ISD_UnregisterDeviceIntrHandler(ISDCard* card) {
    u8 buffer[0x20] __attribute__((aligned(32)));
    s32 rc;
    if (lbl_8059FE98[0] < 0) {
        rc = 0xC1000000;
    } else {
        rc = sduCommand(card->fd, 0x41, 0, 0, 0, 0, 0, 0, 0, buffer, 0, 0);
    }
    if (rc != 0) {
        OSReport("%s Error: %d\n", "ISD_UnregisterDeviceIntrHandler", rc);
    }
    return rc;
}

// TODO: clean up, give variables real names
/* jj_803CC7A8 */
extern void OSReport(const char *, ...);
extern int fn_803CB620(int, unsigned int, int, int, int, void *, int, int, int, int *, int, int);
extern char lbl_804927E0[];

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
int fn_803CC7A8(int *arg0, int *arg1)
{
    char *base = lbl_804927E0;
    int buf[4];
    int result;

    result = 0xC1000000;
    if (arg1 != 0) {
        OSReport(base + 0x688);
        fn_803CB620(arg0[0], 0x10, 3, 1, 4, 0, 0, 0, 0, buf, 0, 0);

        result = fn_803CB620(arg0[0], 0x37, 3, 1, arg0[4], 0, 0, 0, 0, buf, 0, 0);
        if (result != 0) {
            OSReport(base + 0x6A0, result);
            return result;
        }

        result = fn_803CB620(arg0[0], 0x33, 3, 1, 0, arg1, 1, 4, 0, buf, 0, 0);
        if (result != 0) {
            OSReport(base + 0x6C0, result);
        } else {
            OSReport(base + 0x6E0, arg1[0], arg1[1]);
        }

        result = fn_803CB620(arg0[0], 0x10, 3, 1, 0x200, 0, 0, 0, 0, buf, 0, 0);
    }

    return result;
}

// TODO: clean up, give variables real names
/* jj_803CB23C */
extern void OSReport(const char *, ...);
extern int IOS_Ioctl(int, int, void *, int, void *, int);
extern char lbl_804927E0[];
#define lbl_8059FE98 (*(int*)&lbl_8059FE98)
#define lbl_805A1244 (*(int **)&lbl_805A1244)

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
int fn_803CB23C(int *arg0, int arg1, int *arg2, int arg3)
{
    char *base = lbl_804927E0;
    int result;

    if (lbl_8059FE98 < 0) {
        result = 0xC1000000;
    } else {
        lbl_805A1244[0] = arg1;
        lbl_805A1244[3] = arg3;
        if (arg2 != 0) {
            lbl_805A1244[4] = *arg2;
            result = IOS_Ioctl(*arg0, 1, lbl_805A1244, 0x18, 0, 0);
            if (result != 0) {
                OSReport(base + 0xD8);
                result = 0xC1000000;
            }
        } else {
            OSReport(base + 0xF4);
            result = 0xC1000000;
        }
    }
    if (result != 0) {
        OSReport(base + 0xAC, base + 0xC4, result);
    }
    return result;
}
#undef lbl_8059FE98
#undef lbl_805A1244

/* jj_803CB148 */
extern void OSReport(const char *, ...);
extern int IOS_Ioctl(int, int, void *, int, void *, int);
extern char lbl_804927E0[];
#define lbl_8059FE98 (*(int*)&lbl_8059FE98)
#define lbl_805A1238 (*(int **)&lbl_805A1238)
#define lbl_805A1244 (*(int **)&lbl_805A1244)

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
int fn_803CB148(int *arg0, int arg1, int *arg2, int arg3)
{
    char *base = lbl_804927E0;
    int result;

    if (lbl_8059FE98 < 0) {
        result = 0xC1000000;
    } else {
        lbl_805A1244[0] = arg1;
        lbl_805A1244[3] = arg3;
        lbl_805A1244[4] = 0;

        if (arg2 != 0) {
            result = IOS_Ioctl(*arg0, 2, lbl_805A1244, 0x18,
                                 lbl_805A1238, 4);
            if (result != 0) {
                OSReport(base + 0x74);
                result = 0xC1000000;
            } else {
                *arg2 = *lbl_805A1238;
            }
        } else {
            OSReport(base + 0x90);
            result = 0xC1000000;
        }
    }

    if (result != 0) {
        OSReport(base + 0xAC, base + 0x60, result);
    }

    return result;
}
#undef lbl_8059FE98
#undef lbl_805A1238
#undef lbl_805A1244

/* jj_803CB004 */

extern void OSReport(const char *, ...);
extern void iosFree(int, void *);
extern char lbl_804927E0[];
extern char lbl_80492820[];
#define lbl_8059FE98 (*(int*)&lbl_8059FE98)
#define lbl_805A1230 (*(int*)&lbl_805A1230)
#define lbl_805A1234 (*(int*)&lbl_805A1234)

typedef struct Req {
    void (*unk0)(int, void *);
    void *unk4;
    int *unk8;
    int unkC;
    int unk10;
    int *unk14;
    void *unk18;
} Req;

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
int fn_803CB004(int arg0, Req *arg1)
{
    switch (arg1->unkC) {
    case 4:
        if (arg0 != 0) {
            if ((u32)(arg0 + 0x3F000000) == 0x20U) {
                OSReport(lbl_804927E0, arg0);
            } else {
                OSReport(lbl_80492820, arg0);
            }
            arg1->unk8[4] = 0;
        } else {
            if (arg1->unk8 != 0) {
                arg1->unk8[4] = *arg1->unk14;
                lbl_805A1230 = 0x10000;
                arg1->unk8[9] = 2;
            }
        }
        lbl_805A1234 = 0;
        break;
    case 18:
    case 25:
    case 38:
        lbl_805A1234 = 0;
        break;
    }
    if (arg1->unk0 != 0 && (u32)arg0 != 0xC2100000U) {
        arg1->unk0(arg0, arg1->unk4);
    }
    if (arg1->unk18 != 0) {
        iosFree(lbl_8059FE98, arg1->unk18);
    }
    iosFree(lbl_8059FE98, arg1);
    return arg0;
}
#undef lbl_8059FE98
#undef lbl_805A1230
#undef lbl_805A1234

/* jj_803CC014 */

extern int IOS_Open(void *, int);
extern char lbl_804927E0[];
#define lbl_80563960 ((char*)&lbl_80563960)
#define lbl_8059FE98 (*(int*)&lbl_8059FE98)

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
int fn_803CC014(unsigned int arg0, int **arg1)
{
    char *base = lbl_804927E0;
    int value;
    int result = 0;

    if (lbl_8059FE98 < 0) {
        result = 0xC1000000;
        goto done;
    }
    if (arg0 >= 4U) {
        OSReport(base + 0x498);
        return -4;
    }
    {
        int *pointer = *arg1;
        int *nullPointer[1] = {0};
        if (pointer == nullPointer[0]) {
            OSReport(base + 0x4BC);
            return -4;
        }
    }
    value = IOS_Open(lbl_80563960 + arg0 * 32, 0);
    if (value < 0) {
        if (value == -2) {
            OSReport(base + 0x2DC, lbl_80563960 + (arg0 << 5));
        } else if (value == -6) {
            OSReport(base + 0x2F0, lbl_80563960 + (arg0 << 5));
        } else {
            OSReport(base + 0x304, lbl_80563960 + (arg0 << 5), value);
        }
    }
    (*arg1)[0] = value;
    if ((*arg1)[0] < 0) {
        result = -1;
        OSReport(base + 0x4E0);
    }
    (*arg1)[9] = 0;
    (*arg1)[6] = 0x4000;
    (*arg1)[8] = 0;
done:
    return result;
}
#undef lbl_80563960
#undef lbl_8059FE98

/* jj_803CB410 */
extern void OSReport(const char *fmt, ...);
extern int iosAlloc(int a, int b);
extern int IOS_IoctlvAsync(int a, int b, int c, int d, int *e, void *f, int *g);
extern int IOS_Ioctlv(int a, int b, int c, int d, int *e);
extern char lbl_804927E0[];
#define lbl_8059FE98 (*(int*)&lbl_8059FE98)
#define lbl_805A123C (*(int **)&lbl_805A123C)
#define lbl_805A1248 (*(int **)&lbl_805A1248)

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
int fn_803CB410(int a0, int a1, int a2, int a3, int a4, int a5, int a6, int a7,
                int flag, int *out, int a10, int a11)
{
    char *base;
    int r30;
    int r29;
    int *p;
    int *q;

    base = lbl_804927E0;
    if (flag != 0) {
        r30 = 2;
        r29 = 1;
    } else {
        r30 = 1;
        r29 = 2;
    }

    p = (int *)iosAlloc(lbl_8059FE98, 0x40);
    if (p == 0) {
        OSReport(base + 0x15C, 0x40);
        a0 = -0x16;
    } else {
        p[0] = a1;
        p[1] = a2;
        p[2] = a3;
        p[3] = a4;
        p[6] = a5;
        p[4] = a6;
        p[5] = a7;
        p[7] = 1;
        lbl_805A1248[0] = (int)p;
        lbl_805A1248[1] = 0x24;
        lbl_805A1248[2] = a5;
        lbl_805A1248[3] = a6 * a7;
        lbl_805A1248[4] = (int)lbl_805A123C;
        lbl_805A1248[5] = 0x10;

        if (a10 != 0) {
            q = (int *)iosAlloc(lbl_8059FE98, 0x1C);
            if (q == 0) {
                OSReport(base + 0x188);
                a0 = -0x16;
            } else {
                q[0] = a10;
                q[1] = a11;
                q[5] = (int)out;
                q[3] = a1;
                q[6] = (int)p;
                a0 = IOS_IoctlvAsync(a0, 7, r30, r29, lbl_805A1248, (void *)fn_803CB004, q);
                if (a0 != 0) {
                    OSReport(base + 0x19C, a0);
                }
            }
        } else {
            a0 = IOS_Ioctlv(a0, 7, r30, r29, lbl_805A1248);
            if (a0 != 0) {
                OSReport(base + 0x19C, a0);
            } else if (out != 0) {
                *out = *lbl_805A123C;
            }
        }
    }

    if ((a10 == 0 || a0 != 0) && p != 0) {
        iosFree(lbl_8059FE98, p);
    }
    return a0;
}
#undef lbl_8059FE98
#undef lbl_805A123C
#undef lbl_805A1248

