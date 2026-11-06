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
