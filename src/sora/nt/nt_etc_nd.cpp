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
