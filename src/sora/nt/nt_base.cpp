#include <revolution/OS/OSMutex.h>
#include <mt/mt_prng.h>
#include <nt/nt_report.h>
#include <nt/nt_match.h>
#include <nt/nt_net_report.h>
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

// HYPOTHESIS: lbl_805A0418 holds a (start index, player count) pair per station.
// TODO: clean up, give variables real names
int NtBase::setSIDPlayer(u8 station, u8 count) {
    u8* slot = &lbl_805A0418[station * 2];
    slot[1] = count;
    int sum = 0;
    for (int i = 0; i < 4; i++) {
        lbl_805A0418[i * 2] = sum;
        sum += lbl_805A0418[i * 2 + 1];
    }
    return sum;
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

void NtBase::clearAppErrorStatus() {
    if (lbl_805A03E8 != 0 && lbl_805A03E8 > 7 && !reinterpret_cast<NtMatching*>(this)->isNetShutdownErrorOccured()) {
        lbl_805A03E8 = 0;
        netReport("Clear App Error status\n");
    }
}

// TODO: clean up, give variables real names
extern "C" void fn_80147394(void) {
    lbl_805A03D0 = 1;
}

// TODO: clean up, give variables real names
extern "C" void fn_801473A0(void) {
    lbl_805A03D0 = 0;
}

// TODO: clean up, give variables real names
namespace jj_80146F34 {
class NtReport {
public:
    void report(const char*, ...);
};

extern char lbl_8045F4A8[];

// TODO: clean up, give variables real names
extern "C" int fn_80146F34(unsigned char* p) {
    unsigned char a = p[0];
    int r = (unsigned char)(a + p[1]) == 0;
    if (!r) {
        ((NtReport*)lbl_8045F4A8)->report((const char*)a);
    }
    return r;
}
}

// TODO: clean up, give variables real names
namespace jj_80146E44 {

struct NtMutexes {
    OSMutex m0;
    OSMutex m1;
    OSMutex m2;
    OSMutex m3;
};

extern NtMutexes lbl_8049EC30;
extern OSCond lbl_805A03C8;
extern signed char lbl_805A03D0;
extern s32 lbl_805A03D4;
extern unsigned char lbl_805A0418[8];
extern s32 gUnk805a03d8;
extern s32 g_NtMatchingStatus;
extern s32 lbl_805A03E0;
extern unsigned char lbl_805A03E4;
extern s32 lbl_805A03FC;
extern s32 lbl_805A0400;
extern s32 lbl_805A0404;
extern unsigned char lbl_805A0408;
extern unsigned char lbl_805A0409;
extern unsigned char lbl_805A0410[4];
extern unsigned char lbl_805A0414;
extern unsigned char lbl_805A0415;
extern unsigned char lbl_8059E742;
extern unsigned char lbl_8059E743;
extern unsigned char lbl_8059E744;
extern unsigned char lbl_8059E745;
extern s32 g_NtAIDBitmap;
extern s32 lbl_805A03E8;
extern s32 lbl_805A03EC;
extern s32 lbl_805A03F0;
extern s32 lbl_805A03F4;
extern s32 lbl_805A0428;
extern mtRand lbl_805A0420;

extern "C" {
void init__6mtRandFl(mtRand*, u32);
}

#pragma push
#pragma opt_propagation off
// TODO: clean up, give variables real names
extern "C" void fn_80146E44(void) {
    NtMutexes* base = &lbl_8049EC30;
    OSMutex NtMutexes::*first = &NtMutexes::m0;
    lbl_805A03D0 = 0;
    OSInitMutex(&base->m3);
    OSInitCond(&lbl_805A03C8);
    OSInitMutex(&(base->*first));
    OSInitMutex(&base->m1);
    lbl_805A03D4 = 0;
    gUnk805a03d8 = 0;
    g_NtMatchingStatus = 0;
    lbl_805A03E0 = 0;
    lbl_805A03E4 = 0;
    lbl_805A03FC = 0;
    lbl_805A0400 = -1;
    lbl_805A0404 = 0;
    lbl_805A0408 = 0;
    lbl_805A0409 = 0x1E;
    memset(lbl_805A0418, 0, 8);
    lbl_805A0414 = 0;
    lbl_8059E742 = 0xFF;
    lbl_8059E743 = 0xFF;
    lbl_8059E744 = 0xFF;
    lbl_8059E745 = 0xFF;
    lbl_805A0415 = 0;
    memset(lbl_805A0410, 0, 4);
    g_NtAIDBitmap = 0;
    lbl_805A03E8 = 0;
    lbl_805A03EC = 0;
    lbl_805A03F0 = 0;
    lbl_805A03F4 = 0;
    lbl_805A0428 = 0;
    init__6mtRandFl(&lbl_805A0420, OSGetTick());
}
#pragma pop
}

