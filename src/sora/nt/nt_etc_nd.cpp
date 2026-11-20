#include <nt/nt_etc_nd.h>
#include <nt/nt_net_report.h>
#include <revolution/OS/OSHardware.h>
#include <types.h>

extern char lbl_80461108[0x1FC];
extern char lbl_80461304[0x48];

extern "C" int DWC_NdInitAsync(const char*, const char*);
extern "C" int DWC_NdCleanupAsync();
extern "C" int DWC_NdSetAttr(const char*, const char*, const char*);

nteND::nteND(bool arg) {
    unk340 = 0;
    unk344 = 0;
    unk348 = 0;
    unk34C = false;
    unk34D = false;
    unk34E = true;
    unk350 = 0;
    unk358 = arg;
}

nteND::~nteND() {
    unk34E = false;
    bool isTerminated;
    while (isTerminated = OSIsThreadTerminated(&m_thread) == true, !isTerminated) {
        OSResumeThread(&m_thread);
        OSSleepTicks((s64)((OS_BUS_CLOCK_SPEED / 4) / 1000) * 15);
    }
    netReport(lbl_80461108);
}

bool nteND::startupND(void* arg, const char* arg2, const char* arg3) {
    if (unk34D)
        return true;
    if (!arg)
        return false;
    if (!unk34E)
        return false;
    while (!isIdle()) {
        OSSleepTicks((s64)((OS_BUS_CLOCK_SPEED / 4) / 1000) * 15);
    }
    if (!DWC_NdInitAsync(arg2, arg3)) {
        _errorMonitor();
        return false;
    }
    unk350 = arg;
    unk340 = 1;
    OSResumeThread(&m_thread);
    return true;
}

bool nteND::finishND(void* arg) {
    if (!arg)
        return false;
    if (!unk34E)
        return false;
    while (!isIdle()) {
        OSSleepTicks((s64)((OS_BUS_CLOCK_SPEED / 4) / 1000) * 15);
    }
    if (!DWC_NdCleanupAsync()) {
        _errorMonitor();
        return false;
    }
    unk350 = arg;
    unk340 = 2;
    OSResumeThread(&m_thread);
    return true;
}

bool nteND::download(void* arg, const char* arg2, const char* arg3, const char* arg4) {
    if (!arg)
        return false;
    if (!unk34E)
        return false;
    while (!isIdle()) {
        OSSleepTicks((s64)((OS_BUS_CLOCK_SPEED / 4) / 1000) * 15);
    }
    if (!DWC_NdSetAttr(arg2, arg3, arg4)) {
        netReport(lbl_80461304);
        _errorMonitor();
        return false;
    }
    unk34C = false;
    unk350 = arg;
    unk340 = 3;
    unk344 = 0;
    unk348 = 0;
    OSResumeThread(&m_thread);
    return true;
}

// TODO: clean up, give variables real names
namespace jj_80151904 {
typedef unsigned int u32;
typedef long long s64;

extern "C" {
int fn_80355EFC();
int DWC_NdCancelAsync();
int DWC_NdGetProgress(int*, int*);
void OSSleepTicks(s64);
}

struct Context {
    unsigned char pad[0x340];
    int state;
    int value1;
    int value2;
    unsigned char flag34C;
    unsigned char pad34D;
    unsigned char flag34E;
};

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" int fn_80151904(Context* self) {
    int a;
    int b;

    do {
        switch (fn_80355EFC()) {
        case 0:
        case 4:
            return 0;
        case 1:
        case 3:
            return 1;
        case 2:
            if (self->state == 3) {
                if (self->flag34C) {
                    if (DWC_NdCancelAsync() == 0)
                        return 0;
                } else {
                    if (DWC_NdGetProgress(&a, &b)) {
                        self->value1 = a;
                        self->value2 = b;
                    }
                }
            }
            break;
        }

        OSSleepTicks(15 * (s64)((*(u32*)0x800000F8 / 4) / 1000));
    } while (self->flag34E);

    return 0;
}
}

#include <revolution/DWC/dwc_error.h>
// TODO: clean up, give variables real names
namespace jj_80151A0C {

struct Callback {
    virtual void f0(s32, s32);
};

struct Self {
    unsigned char pad_00[0x344];
    s32 a344;
    s32 a348;
    unsigned char pad_34c[8];
    Callback* callback354;
    u8 flag358;
};

extern "C" {
extern char lbl_8046134C[0x1C];
extern s32 lbl_805A03EC;
extern s32 lbl_805A03F0;
extern s32 lbl_805A03F4;
extern u8 lbl_805A03F8;
void netReport__FPCce(const char*, ...);
}

extern "C" s32 _errorMonitor__5nteNDFv(Self* self) {
    s32 errCode;
    s32 errType;
    s32 result = 0;

    if (self->flag358 != 0) {
        result = lbl_805A03EC;
        errCode = -lbl_805A03F0;
        errType = lbl_805A03F4;
        if (lbl_805A03F8 != 0) {
            lbl_805A03F8 = 0;
            self->a344 = 0;
            self->a348 = 0;
            netReport__FPCce(lbl_8046134C);
            return 1;
        }
    }
    if (result == 0)
        result = DWC_GetLastErrorEx(&errCode, &errType);
    if (result == 0)
        return 1;
    if (errCode == -0x7940) {
        self->a344 = 0;
        self->a348 = 0;
        netReport__FPCce(lbl_8046134C);
        DWC_ClearError();
        return 1;
    }
    if (self->callback354 != 0) {
        if (errCode < 0)
            errCode = -errCode;
        self->callback354->f0(errCode, errType);
    }
    if (self->flag358 == 0)
        DWC_ClearError();
    return 0;
}
}

