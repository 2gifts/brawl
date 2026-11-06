#include <types.h>

void DWC_Printf(u32 level, const char* format, ...);

// HYPOTHESIS: layout of the match control block, only the fields these functions touch.
typedef struct DWCMatchControl {
    u8 _00[0xD];
    u8 unkD;
    u8 _0E;
    u8 unkF;
    void* qr2Obj;
    u8 _14;
    u8 unk15;
    u8 unk16;
    u8 _17;
    u8 unk18;
    u8 _19[0x15];
    u16 unk2E;
    u32 unk30;
    u8 _34[0x188];
    u8 unk1BC;
    u8 _1BD[0x12B];
    u8 aidList[1];
} DWCMatchControl;

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_805338B0[0x738] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80533FE8[0x20] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0F90[0x4] __attribute__((aligned(4))) = { 0 };
int lbl_805A0F94 __attribute__((aligned(4))) = 0;
unsigned char lbl_805A0F98[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0F9C[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0FA0[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0FA4[0x4] __attribute__((aligned(4))) = { 0 };
DWCMatchControl* lbl_805A0FA8[2] __attribute__((aligned(4))) = { 0 };

int DWCi_QR2CountCallback(void) {
    return 0;
}

int DWCi_SendServerPolling(void) {
    return 1;
}

BOOL DWCi_IsShutdownMatch(void) {
    return lbl_805A0FA8[0] == 0;
}

void DWCi_NNProgressCallback(int state) {
    DWC_Printf(0x40, "NN, Got state update: %d\n", state);
}

int DWCi_GetAllAIDList(u8** aidList) {
    if (lbl_805A0FA8[0] == 0) {
        return 0;
    }
    *aidList = lbl_805A0FA8[0]->aidList;
    return lbl_805A0FA8[0]->unkD + 1;
}

void DWCi_HandleQR2Error(int error);

void DWCi_QR2AddErrorCallback(int error, const char* errorMessage) {
    DWC_Printf(2, "QR2 Failed query addition to master server %d\n", error);
    DWC_Printf(2, "%s\n", errorMessage);
    DWCi_HandleQR2Error(error);
}

void DWCi_QR2PublicAddrCallback(u32 ip, u16 port) {
    DWC_Printf(0x40, "Got my query IP %d.%d.%d.%d & port %d.\n", ip >> 24, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF, port);
    lbl_805A0FA8[0]->unk30 = ip;
    lbl_805A0FA8[0]->unk2E = port;
}
