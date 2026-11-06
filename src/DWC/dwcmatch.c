#include <types.h>

void* memset(void*, int, u32);
void DWC_Printf(u32 level, const char* format, ...);

// HYPOTHESIS: layout of the match control block, only the fields these functions touch.
typedef struct DWCMatchControl {
    u8 _00[0xD];
    u8 unkD;
    u8 unkE;
    u8 unkF;
    void* qr2Obj;
    u8 _14;
    u8 unk15;
    u8 unk16;
    u8 _17;
    u8 unk18;
    u8 _19;
    u8 unk1A;
    u8 _1B;
    u32 unk1C;
    u8 _20[8];
    int unk28;
    u8 _2C[2];
    u16 unk2E;
    u32 unk30;
    u32 unk34;
    u8 _38[0xC0];
    void* unkF8;
    u8 _FC[0x94];
    u8 unk190;
    u8 _191[3];
    int unk194;
    int unk198;
    int unk19C;
    u8 _1A0[0x14];
    int unk1B4;
    u8 _1B8[4];
    u8 unk1BC;
    u8 unk1BD;
    u8 _1BE[0x62];
    void* unk220;
    u8 _224[0xC4];
    u8 aidList[1];
    u8 _2E9[0x1F];
    u32 unk308;
    u8 _30C[0xDC];
    u8 unk3E8;
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

int DWCi_IsError(void);
void* ServerBrowserGetServer(void* serverBrowser, int index);
int SBServerGetIntValueA(void* server, const char* key, int defaultValue);
char* strchr(const char* string, int character);
void DWCi_Np_CPUCopyFast(void* destination, const void* source, u32 size);
void DWCi_ProcessRecvMatchCommand(u8 command, u32 profileId, int arg1, int arg2, u8* data, int count);

int DWC_GetMatchIntValue(int index, const char* key, int defaultValue) {
    void* server;
    if (lbl_805A0FA8[0] == 0 || DWCi_IsError()) {
        return defaultValue;
    }
    server = ServerBrowserGetServer(lbl_805A0FA8[0]->unkF8, index);
    if (server != 0) {
        return SBServerGetIntValueA(server, key, defaultValue);
    }
    return defaultValue;
}

u8 DWCi_HexAsciiToU8(char high, char low) {
    u8 highNibble = 0;
    u8 lowNibble = 0;
    if (high >= '0' && high <= '9') {
        highNibble = high - '0';
    } else if (high >= 'a' && high <= 'f') {
        highNibble = high - 'a' + 10;
    } else if (high >= 'A' && high <= 'F') {
        highNibble = high - 'A' + 10;
    }
    if (low >= '0' && low <= '9') {
        lowNibble = low - '0';
    } else if (low >= 'a' && low <= 'f') {
        lowNibble = low - 'a' + 10;
    } else if (low >= 'A' && low <= 'F') {
        lowNibble = low - 'A' + 10;
    }
    return (highNibble << 4) | lowNibble;
}

int DWCi_GetGPBuddyAdditionalMsg(char* out, char* message, int index) {
    char* end;
    char* separator;
    int length;
    int i;
    end = strchr(message, 0);
    for (i = 0; i < index; i++) {
        separator = strchr(message, '/');
        if (separator == 0) {
            return -1;
        }
        message = separator + 1;
    }
    separator = strchr(message, '/');
    if (separator == 0) {
        separator = end;
    }
    if (message == separator) {
        return -1;
    }
    length = separator - message;
    DWCi_Np_CPUCopyFast(out, message, length);
    out[length] = 0;
    return length;
}

void DWCi_SetNumValidConnection(void) {
    u32 bitmap;
    DWCMatchControl* control;
    int count = -1;
    int i;
    control = lbl_805A0FA8[0];
    bitmap = control->unk308;
    for (i = 0; i < 32; i++) {
        if ((1 << i) & bitmap) {
            count++;
        }
    }
    if (count == -1) {
        control->unkE = 0;
    } else {
        control->unkE = count;
    }
}

u32 DWC_GetAIDBitmap(void);
u8 DWC_GetMyAID(void);
int DWC_GetState(void);
BOOL DWC_IsServerMyself(void);
void DWCi_DoCancelMatching(void);
int DWCi_SBUpdateAsync(int arg);
int DWCi_HandleSBError(int result);
int DWCi_HandleGPError(int result);
int DWCi_SendResvCommand(void* reservation, int arg);
void DWCi_RestartFromCancel(int arg);
void DWCi_NNStartupAsync(int arg1, int cookie, int arg3);
int DWCi_HandleNNError(void);

BOOL DWC_CancelMatching(void) {
    BOOL canCancel;
    if (DWCi_IsError()) {
        canCancel = 0;
    } else if (lbl_805A0FA8[0] == 0 || DWC_GetState() != 5 || lbl_805A0FA8[0]->unk1BD != 0) {
        canCancel = 0;
    } else {
        canCancel = 1;
    }
    if (canCancel) {
        DWCi_DoCancelMatching();
        return 1;
    }
    DWC_Printf(4, "Now unable to cancel.\n");
    return 0;
}

int DWC_GetSuspendMatching(void) {
    int result = -1;
    u32 bitmap;
    int isOther;
    if (lbl_805A0FA8[0] == 0) {
        return 0;
    }
    bitmap = lbl_805A0FA8[0]->unk1C;
    bitmap &= DWC_GetAIDBitmap();
    if (DWC_IsServerMyself()) {
        if (lbl_805A0FA8[0]->unk1B4 == 0xF) {
            isOther = 1;
        } else {
            isOther = 0;
        }
    } else {
        isOther = 1;
    }
    if (!isOther) {
        bitmap &= ~(1 << DWC_GetMyAID());
    }
    if (!(bitmap ^ DWC_GetAIDBitmap())) {
        result = 1;
    } else if ((bitmap & DWC_GetAIDBitmap()) == 0) {
        result = 0;
    }
    if (result != -1) {
        lbl_805A0FA8[0]->unk28 = (result == 1);
    }
    return lbl_805A0FA8[0]->unk28;
}

void DWCi_MatchGPRecvBuddyMsgCallback(int connection, u32 profileId, u8* message) {
    u8 data[128][4];
    char text[16];
    int count;
    int j;
    DWC_Printf(0x40, "<GP> RECV-0x%02x <- [--------:-----] [pid=%u]\n", message[0], profileId);
    for (count = 0; count < 128; count++) {
        if (DWCi_GetGPBuddyAdditionalMsg(text, (char*)(message + 1), count) == -1) {
            break;
        }
        for (j = 0; j < 4; j++) {
            data[count][j] = DWCi_HexAsciiToU8(text[j * 2], text[j * 2 + 1]);
        }
    }
    DWCi_ProcessRecvMatchCommand(message[0], profileId, 0, 0, &data[0][0], count);
}
