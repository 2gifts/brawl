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
