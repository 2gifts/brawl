#include <mt/mt_prng.h>
#include <nt/nt_report.h>
#include <revolution/OS/OSTime.h>
#include <string.h>
#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
OSMutex lbl_8049EC60;

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
OSCond lbl_805A03C8;
u8 lbl_805A03D0 __attribute__((aligned(4)));
s32 lbl_805A03D4;
s32 gUnk805a03d8;
s32 g_NtMatchingStatus;
s32 lbl_805A03E0;
u8 lbl_805A03E4 __attribute__((aligned(4)));
s32 lbl_805A03E8;
s32 lbl_805A03EC;
s32 lbl_805A03F0;
s32 lbl_805A03F4;
unsigned char lbl_805A03F8[0x1] __attribute__((aligned(4))) = { 0 };
s32 lbl_805A03FC;
s32 lbl_805A0400;
s32 lbl_805A0404;
u8 lbl_805A0408 __attribute__((aligned(4)));
u8 lbl_805A0409;
unsigned char lbl_805A040A[0x2] __attribute__((aligned(2))) = { 0 };
u32 g_NtAIDBitmap;
unsigned char lbl_805A0410[0x1] __attribute__((aligned(4)));
u8 lbl_805A0414 __attribute__((aligned(4)));
u8 lbl_805A0415;
unsigned char lbl_805A0418[0x8] __attribute__((aligned(4)));
mtRand lbl_805A0420(0);
s32 lbl_805A0428[2];

const char* NtBase::getErrorStr(s32 error) {
    const char* msgs[] = {
        "None",
        "SOInit failed",
        "Can't get DHCP",
        "DWC login err",
        "DWC matching err",
        "DWC match cancel",
        "DWC shutdown err",
        "DWC error",
        "Login timeout",
        "ASync error",
        "Sync error",
        "Timeout error",
        "No parent",
        "Node error",
        "Open tray",
    };
    if (error < 0 || error >= 15) {
        return "Unknown NetError";
    }
    return msgs[error];
}

void NtBase::initStaticWork() {
    OSInitMutex(&lbl_8049EC60);
    OSInitCond(&lbl_805A03C8);
    lbl_805A03D0 = 0;
    lbl_805A0420.init(OSGetTick());
}

void NtBase::clearErrorStatus() {
    lbl_805A03E8 = 0;
    lbl_805A03EC = 0;
    lbl_805A03F0 = 0;
    lbl_805A03F4 = 0;
}
