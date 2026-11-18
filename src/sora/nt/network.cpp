#include <revolution/OS/OSMutex.h>
#include <nt/nt_offline.h>
#include <mu/wifi/mu_wifi_interface.h>
#include <ip/ip_switch.h>
#include <nt/network.h>
#include <nt/nt_match.h>
#include <nt/nt_net_report.h>
#include <nt/nt_report.h>
#include <nt/nt_send.h>
#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_8049EBD0[0x10] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A03B0[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A03B4[0x1] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A03B8[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A03C0[0x8] __attribute__((aligned(4))) = { 0 };

extern const char lbl_8045F3E0[];
extern const char lbl_8045F280[];
extern const char lbl_8045F490[];
extern u8 lbl_805A0414;
extern NtMatching* lbl_805A0434;
extern u8 lbl_805A03E4;

Network::~Network() {
}

bool ntShutdown::returnStatus() {
    netReport(lbl_8045F3E0);
    return true;
}

ntResetCallback::~ntResetCallback() {
}

bool Network::retryMatching(int arg1, int arg2) {
    if (gUnk805a03d8 != 3) {
        netReport(lbl_8045F280, gUnk805a03d8);
        return false;
    }
    g_NtAIDBitmap = 0;
    lbl_805A0414 = 0;
    lbl_8059E742 = 0xFF;
    lbl_8059E745 = 0xFF;
    unk1C = arg1;
    unk20 = arg2;
    unk24 = 0;
    lbl_805A0434->retryMatching(arg1, arg2);
    gUnk805a03d8 = 4;
    return true;
}

bool ntReturnStatusCallback::returnStatus() {
    Network* network = &g_Network;
    if (network == 0) {
        return true;
    }
    if (!NtBase::isMatchStatusOnline()) {
        return true;
    }
    switch (network->unk14) {
    case 0:
        if (ipSwitch::getInstance()->m_source == 1) {
            if (--network->unk18 > 0) {
                return false;
            }
        }
        netReport(lbl_8045F490);
        network->disconnectAll(false);
        network->unk14 = 1;
        network->unk18 = 3;
    case 1:
        if (--network->unk18 > 0) {
            return false;
        }
        network->unk14 = 2;
    }
    return true;
}

void Network::alarmCallback(Network* network) {
    if (gUnk805a03d8 != 0 && lbl_805A03E4 == 1) {
        OSResumeThread(&lbl_805A0434->unkA0);
    }
}

void Network::shutdown() {
    netReport("Network::shutdown\n");
    gUnk805a03d8 = 7;
    m_matching->shutdown();
}

// TODO: clean up, give variables real names
namespace jj_801466AC {

extern char lbl_8045F204[];

extern "C" void fn_8014A060(void*);
extern "C" void fn_80143C48(void*, int);
extern "C" void fn_8013DDEC(void*);

extern "C" void fn_801466AC(void* self) {
    if (gUnk805a03d8 != 3) {
        netReport(lbl_8045F204);
        return;
    }
    g_NtSend->clearSendBuffer();
    fn_8014A060(*(void**)((char*)self + 0x30));
    fn_80143C48(*(void**)((char*)self + 0x38), 0);
    fn_8013DDEC(*(void**)((char*)self + 0x34));
}
}

namespace jj_80146DC4 {
extern "C" {
void* getInstance__8ipSwitchFv(void);
int fn_8013E380(void* p, unsigned int v);
extern int gUnk805a03d8;
extern char g_Network[];
}

extern "C" void fn_80146DC4(void) {
    int* net = (int*)g_Network;
    if (net != 0) {
        if (gUnk805a03d8 == 0) {
            net[0x14 / 4] = 2;
            return;
        }
        net[0x14 / 4] = 0;
        if (*(int*)getInstance__8ipSwitchFv() != 1) {
            net[0x18 / 4] = 1;
            return;
        }
        net[0x18 / 4] = 0x1E;
        fn_8013E380((void*)net[0x34 / 4], 0x65);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_80146718 {
extern "C" {
    void netReport__FPCce(const char*, ...);
    void clearSendBuffer__6NtSendFv(void* self);
    void fn_8014A060(void* arg0);
    void fn_80143C48(void* arg0, int arg1);
    void fn_8013DDEC(void* arg0);
    void fn_80148F30(void* arg0, int arg1, int arg2, int arg3);

    extern int gUnk805a03d8;
    extern void* g_NtSend;
    extern void* lbl_805A0434;
    extern char lbl_8045F280[];
    extern char lbl_8045F204[];

    int fn_80146718(void* arg0, int arg1, int arg2, int arg3) {
        if (gUnk805a03d8 != 3) {
            netReport__FPCce(lbl_8045F280, gUnk805a03d8);
            return 0;
        }
        *(int*)((char*)arg0 + 0x1C) = arg1;
        *(int*)((char*)arg0 + 0x20) = arg2;
        *(int*)((char*)arg0 + 0x24) = arg3;
        if (gUnk805a03d8 != 3) {
            netReport__FPCce(lbl_8045F204);
        } else {
            clearSendBuffer__6NtSendFv(g_NtSend);
            fn_8014A060(*(void**)((char*)arg0 + 0x30));
            fn_80143C48(*(void**)((char*)arg0 + 0x38), 0);
            fn_8013DDEC(*(void**)((char*)arg0 + 0x34));
        }
        fn_80148F30(lbl_805A0434, arg1, arg2, arg3);
        gUnk805a03d8 = 4;
        return 1;
    }
}
}

namespace jj_80146574 {

class Class_8042B6D0 {
public:
    virtual void fn_8004805C(int);
};

struct NetworkWork : NtBase {
    u8 pad8[0x33F4 - 8];
    u8 unk33F4;
};

struct NetworkContext {
    u8 pad0[0x30];
    NetworkWork* work30;
    NtBase* work34;
    NtBase* work38;
    NtReport* work3C;
};

extern char lbl_8045F244[];
extern char lbl_8045F260[];

extern "C" void fn_80149EF8(NetworkWork*);
extern "C" void fn_80143C44(NtBase*);
extern "C" void fn_8013EFCC(NtBase*);

extern "C" void fn_80146574(NetworkContext* self) {
    if (gUnk805a03d8 == 0) {
        netReport(lbl_8045F244);
        return;
    }
    netReport(lbl_8045F260);
    ntFriendInfo* info = ntFriendInfo::getInstance();
    (*(u8**)info)[0x577] = 2;
    self->work30->unk33F4 = 1;
    fn_80149EF8(self->work30);
    if (self->work30 != 0) {
        reinterpret_cast<Class_8042B6D0*>(self->work30)->fn_8004805C(1);
    }
    self->work30 = 0;
    fn_80143C44(self->work38);
    if (self->work38 != 0) {
        reinterpret_cast<Class_8042B6D0*>(self->work38)->fn_8004805C(1);
    }
    self->work38 = 0;
    fn_8013EFCC(self->work34);
    if (self->work34 != 0) {
        reinterpret_cast<Class_8042B6D0*>(self->work34)->fn_8004805C(1);
    }
    self->work34 = 0;
    self->work3C->destroyWork();
    if (self->work3C != 0) {
        reinterpret_cast<Class_8042B6D0*>(self->work3C)->fn_8004805C(1);
    }
    self->work3C = 0;
    gUnk805a03d8 = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_80146950 {

extern "C" {
extern OSMutex lbl_8049EC30;
extern char lbl_8045F1D8[];
extern int gUnk805a03d8;
extern int g_NtMatchingStatus;
extern int lbl_805A03E8;
extern void* lbl_805A0390;
extern void* lbl_805A0434;
extern void* g_muWifiInterfaceTask;

s32 fn_801DEDC4(OSMutex* m);
void fn_8014A828(void* param);
void fn_8014B9B4(void* p);
int fn_801400EC(void* p);
int fn_80149910(void* p);
void fn_80146574(void* p);
int fn_80146878(void* p, int v);
void fn_8004A914(void* p, int v);
void* getInstance__8ipSwitchFv(void);
void netReport__FPCce(const char*, ...);
void fn_800C8F68(const char*, ...);
void fn_800CD340(void* p);
unsigned char fn_800C851C(void);

void fn_80146950(void* arg0) {
    char* base = lbl_8045F1D8;
    if (gUnk805a03d8 != 0 && fn_801DEDC4(&lbl_8049EC30) != 0) {
        fn_8014A828(lbl_805A0434);
        if (lbl_805A03E8 != 0 && *(int*)getInstance__8ipSwitchFv() == 1) {
            fn_8004A914(getInstance__8ipSwitchFv(), 0);
        }
        switch (gUnk805a03d8) {
        case 2:
            if (g_NtMatchingStatus >= 5) gUnk805a03d8 = 3;
            break;
        case 4: {
            fn_8014B9B4(lbl_805A0434);
            fn_801400EC(lbl_805A0390);
            if (lbl_805A03E8 == 0) {
                char* addr = (char*)lbl_805A0434 + 0x34ac;
                unsigned char c = *addr;
                *addr = c & 0xFFFFFF00;
                if (c != 0) {
                    netReport__FPCce(base + 0x1a0);
                    fn_80146878(arg0, 1);
                }
            }
            break;
        }
        case 6: {
            fn_8014B9B4(lbl_805A0434);
            fn_801400EC(lbl_805A0390);
            if (lbl_805A03E8 == 0 && g_NtMatchingStatus == 5) {
                gUnk805a03d8 = 3;
                netReport__FPCce(base + 0x1b4);
                fn_800C8F68(base + 0x1c8);
                if (*(unsigned char*)((char*)arg0 + 0x28) != 0) {
                    *(unsigned char*)((char*)arg0 + 0x28) = 0;
                    netReport__FPCce(base + 0x1dc);
                    *(int*)((char*)arg0 + 0x2c) = 0x78;
                    gUnk805a03d8 = 5;
                } else {
                    char* task = (char*)g_muWifiInterfaceTask;
                    unsigned char idx = fn_800C851C();
                    if (idx == 0xFF) idx = 4;
                    char* q = task + idx * 0x218;
                    q[0xc5] = 0;
                }
            }
            break;
        }
        case 5: {
            int v = *(int*)((char*)arg0 + 0x2c) - 1;
            *(int*)((char*)arg0 + 0x2c) = v;
            if (v <= 0) {
                fn_800C8F68(base + 0x1ec);
                gUnk805a03d8 = 3;
                fn_800CD340(g_muWifiInterfaceTask);
            }
            break;
        }
        case 7:
            if (fn_80149910(lbl_805A0434) != 0) {
                fn_80146574(arg0);
                gUnk805a03d8 = 0;
            }
            break;
        }
        OSUnlockMutex(&lbl_8049EC30);
    }
}
}
}

