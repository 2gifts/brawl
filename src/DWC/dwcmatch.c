#include <types.h>

void* memset(void*, int, u32);
void DWC_Printf(u32 level, const char* format, ...);

// HYPOTHESIS: layout of the match control block, only the fields these functions touch.
typedef struct DWCMatchControl {
    void* gpConnection;
    void** gt2Holder; // HYPOTHESIS: points at a struct whose first word is the GT2 socket
    u32 unk8; // HYPOTHESIS: a word argument stored at init
    u8 _0C;
    u8 unkD;
    u8 unkE;
    u8 unkF;
    void* qr2Obj;
    // HYPOTHESIS: reloaded on each compare, so these state/count bytes are volatile.
    volatile u8 _14;
    volatile u8 unk15;
    volatile u8 unk16;
    volatile u8 unk17;
    u8 unk18;
    volatile u8 unk19;
    u8 unk1A;
    u8 _1B;
    u32 unk1C;
    s64 unk20; // HYPOTHESIS: an OSTime. The following word is its high half.
    int unk28;
    u8 unk2C;
    u8 _2D;
    u16 unk2E;
    u32 unk30;
    u32 unk34;
    u32 unk38[0x20];
    u16 unkB8[0x20];
    void* unkF8;
    u32 unkFC;
    s64 unk100;
    int profiles[0x20]; // HYPOTHESIS: index 0 is this player. Same storage as the old word at 0x108 plus the following list.
    s64 unk188; // HYPOTHESIS: cleared at init as a pair of words
    u8 unk190;
    u8 unk191;
    u16 unk192;
    int unk194;
    int unk198;
    int unk19C;
    int unk1A0;
    int unk1A4;
    u8 unk1A8;
    u8 unk1A9;
    u16 unk1AA;
    u32 unk1AC;
    int unk1B0;
    int unk1B4;
    u8 unk1B8;
    u8 unk1B9;
    u8 unk1BA;
    u8 _1BB;
    u8 unk1BC;
    u8 unk1BD;
    u8 unk1BE;
    u8 unk1BF;
    u8 unk1C0;
    u8 unk1C1;
    u16 unk1C2;
    u16 unk1C4;
    u16 unk1C6;
    u32 unk1C8;
    u32 unk1CC;
    s64 unk1D0;
    u32 unk1D8;
    u32 _1DC;
    s64 unk1E0;
    u32 unk1E8;
    u32 unk1EC;
    u32 unk1F0; // HYPOTHESIS: cleared at init
    u8 _1F4[4];
    s64 unk1F8;
    s64 unk200;
    s64 unk208;
    s64 unk210;
    u32 unk218;
    int unk21C;
    void* unk220;
    u32 unk224;
    u32 unk228[0x20];
    u16 unk2A8[0x20];
    u8 aidList[1];
    u8 _2E9[0x1F];
    u32 unk308;
    u32 unk30C;
    u32 unk310;
    u32 unk314; // HYPOTHESIS: init argument
    u32 unk318; // HYPOTHESIS: init argument
    u8 unk31C[0x40];
    u32 unk35C; // HYPOTHESIS: cleared at init
    u32 unk360;
    u32 unk364;
    u32 unk368[0x20];
    u8 unk3E8;
    u8 unk3E9;
    u16 unk3EA;
    u32 unk3EC;
    u32 unk3F0[0x20];
    u32 unk470;
    int unk474;
    s64 unk478;
    int unk480;
    int unk484;
    void* unk488;
    void* unk48C;
    u32 unk490; // HYPOTHESIS: cleared at init
    u32 unk494;
    u32 unk498;
    u32 unk49C;
    u32 unk4A0;
    u8 unk4A4;
    u8 unk4A5;
    u8 unk4A6;
    u8 unk4A7;
    s64 unk4A8;
    u8 unk4B0;
    u8 _4B1[7];
    s64 unk4B8; // HYPOTHESIS: cleared at init
    s64 unk4C0;
    void* unk4C8;
    u8 unk4CC[4];
    u8 unk4D0[4];
    u32 unk4D4[0x20]; // HYPOTHESIS: 4-byte reservation copy followed by a 0x80-byte block.
    void* unk554;
    u32 unk558;
    u32 unk55C;
} DWCMatchControl;

// HYPOTHESIS: names and meanings of the .sdata words are guesses from use.
int lbl_8059F0B0 = -1;
int lbl_8059F0B4 = -1;
int lbl_8059F0B8 = 86000000;
int lbl_8059F0BC = 86000000;
int lbl_8059F0C0 = 500;

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
typedef struct DWCMatchEntry {
    u8 unk0;
    u8 unk1; // HYPOTHESIS: 0 when the value is an int, 1 when it is a string
    u16 unk2;
    void* unk4;
    void* unk8; // HYPOTHESIS: points at the caller's value
} DWCMatchEntry;
// HYPOTHESIS: 0x9A entries of 0xC bytes.
DWCMatchEntry lbl_805338B0[0x9A] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80533FE8[0x20] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
void* lbl_805A0F90 __attribute__((aligned(4))) = 0;
int lbl_805A0F94 __attribute__((aligned(4))) = 0;
int lbl_805A0F98 __attribute__((aligned(4))) = 0;
int lbl_805A0F9C __attribute__((aligned(4))) = 0;
int lbl_805A0FA0 __attribute__((aligned(4))) = 0;
int lbl_805A0FA4 __attribute__((aligned(4))) = 0;
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

int fn_80342270(u8** aidList) {
    u8* cursor;
    u8* out;
    u8 aid;
    DWCMatchControl* control;
    int i;
    if (lbl_805A0FA8[0] == 0) {
        return 0;
    }
    memset(lbl_80533FE8, 0, 0x20);
    control = lbl_805A0FA8[0];
    cursor = (u8*)control;
    out = lbl_80533FE8;
    for (i = 0; i <= control->unkE; i++) {
        aid = ((DWCMatchControl*)cursor)->aidList[0];
        if ((control->unk308 & (1 << aid)) == 0) {
            break;
        }
        *out = aid;
        cursor++;
        out++;
    }
    *aidList = lbl_80533FE8;
    return lbl_805A0FA8[0]->unkE + 1;
}

int DWCi_HandleQR2Error(int error);

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
int DWCi_ProcessRecvMatchCommand(u8 command, u32 profileId, int arg1, int arg2, u8* data, int count);

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
int fn_80349C88(int result);
int fn_80349C88(int result);
void DWCi_ResetMatchParam(int arg);
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

void ServerBrowserFree(void* serverBrowser);
void NNFreeNegotiateList(void);
void DWC_Free(int kind, void* allocation, int size);

void DWCi_CloseMatching(void) {
    int i;
    DWCMatchEntry* entry;
    DWC_Printf(0x40, " Close Matching....\n");
    if (lbl_805A0FA8[0] != 0) {
        if (lbl_805A0FA8[0]->unkF8 != 0) {
            if (lbl_805A0F98 == 0) {
                ServerBrowserFree(lbl_805A0FA8[0]->unkF8);
                lbl_805A0FA8[0]->unkF8 = 0;
            } else {
                lbl_805A0F9C = 1;
            }
        }
        if (lbl_805A0FA0 == 0) {
            NNFreeNegotiateList();
        } else {
            lbl_805A0FA4 = 1;
        }
        lbl_805A0FA8[0]->unk1B4 = 0;
        if (lbl_805A0F90 != 0) {
            DWC_Free(4, lbl_805A0F90, 0);
            lbl_805A0F90 = 0;
        }
        entry = lbl_805338B0;
        for (i = 0; i < 0x9A; i++, entry++) {
            if (entry->unk4 != 0) {
                DWC_Free(4, entry->unk4, 0);
            }
        }
        memset(lbl_805338B0, 0, 0x738);
        lbl_805A0FA8[0]->unk2C = 1;
    }
}

void qr2_keybuffer_add(void* keyBuffer, int key);

void DWCi_QR2KeyListCallback(int keyType, void* keyBuffer) {
    DWCMatchEntry* entry;
    int i;
    switch (keyType) {
    case 0:
        qr2_keybuffer_add(keyBuffer, 8);
        qr2_keybuffer_add(keyBuffer, 0xA);
        qr2_keybuffer_add(keyBuffer, 0x32);
        qr2_keybuffer_add(keyBuffer, 0x33);
        qr2_keybuffer_add(keyBuffer, 0x34);
        qr2_keybuffer_add(keyBuffer, 0x35);
        qr2_keybuffer_add(keyBuffer, 0x36);
        qr2_keybuffer_add(keyBuffer, 0x37);
        qr2_keybuffer_add(keyBuffer, 0x38);
        qr2_keybuffer_add(keyBuffer, 0x39);
        qr2_keybuffer_add(keyBuffer, 0x3A);
        entry = lbl_805338B0;
        for (i = 0; i < 0x9A; i++, entry++) {
            if (entry->unk0 != 0) {
                qr2_keybuffer_add(keyBuffer, entry->unk0);
            }
        }
        break;
    case 1:
    case 2:
        break;
    }
    DWC_Printf(0x200, "QR2, Received KeyListReq : keytype %d\n", keyType);
}


char* strcpy(char* destination, const char* source);
char* gt2AddressToString(u32 ip, u16 port, char* buffer);
int ServerBrowserSendMessageToServerA(void* serverBrowser, const char* ip, u16 port, char* message, int length);

// HYPOTHESIS: wire layout of a server browser command message.
typedef struct DWCSBCommandMessage {
    char magic[4];
    u32 version;
    u8 command;
    u8 size;
    u16 port;
    u32 ip;
    u32 profileId;
    u32 data[33];
} DWCSBCommandMessage;

int DWCi_SendSBMsgCommand(u8 command, u32 ip, u16 port, u32* data, int count) {
    DWCSBCommandMessage message;
    int result;
    u32 swapped;
    DWCMatchControl* control;
    if (data != 0 && count != 0) {
        DWCi_Np_CPUCopyFast(message.data, data, count * 4);
    } else {
        count = 0;
    }
    strcpy(message.magic, "SBCM");
    message.version = 0x0B000000;
    message.command = command;
    message.size = count * 4;
    message.port = (lbl_805A0FA8[0]->unk2E >> 8) | (lbl_805A0FA8[0]->unk2E << 8);
    message.ip = lbl_805A0FA8[0]->unk30;
    swapped = ((lbl_805A0FA8[0]->unk218 >> 8) & 0x00FF00FF) | ((lbl_805A0FA8[0]->unk218 << 8) & 0xFF00FF00);
    message.profileId = (swapped << 16) | (swapped >> 16);
    DWC_Printf(0x40, "<SB> SEND-0x%02x -> [%08x:%d] [pid=--------]\n", command, ip, port);
    control = lbl_805A0FA8[0];
    result = ServerBrowserSendMessageToServerA(control->unkF8, gt2AddressToString(ip, 0, 0), port, (char*)&message, message.size + 0x14);
    if (result != 0) {
        if (result == 2) {
            return result;
        }
    }
    return result;
}

s64 OSGetTime(void);
int DWCi_SendReliable(int type, u8 aid, u8* data, int size);

void DWCi_SendMatchSynPacket(u8 aid, int type) {
    u8 data[4];
    u8 last;
    DWCMatchControl* control;
    int i;
    DWC_Printf(0x80, "Sent SYN %d packet to aid %d.\n", type - 2, aid);
    switch (type) {
    case 2:
        control = lbl_805A0FA8[0];
        data[0] = (control->aidList[control->unkD] == aid);
        last = control->unkD;
        for (i = 1; (u8)i <= last; i++) {
            if (aid == control->aidList[(u8)i]) {
                data[1] = i;
                data[2] = aid;
                break;
            }
        }
        break;
    case 3:
        data[0] = lbl_805A0FA8[0]->unk1C2;
        data[1] = (u32)lbl_805A0FA8[0]->unk1C2 >> 8;
        break;
    case 4:
        data[0] = lbl_805A0FA8[0]->unk558;
        data[1] = lbl_805A0FA8[0]->unk558 >> 8;
        data[2] = lbl_805A0FA8[0]->unk558 >> 16;
        data[3] = lbl_805A0FA8[0]->unk558 >> 24;
        break;
    }
    DWCi_SendReliable(type, aid, data, 4);
    lbl_805A0FA8[0]->unk1F8 = OSGetTime();
}

void ServerBrowserClear(void* serverBrowser);
int gpIsBuddy(void* connection, int profile);
int DWCi_SendResvCommandToFriend(int arg1, int arg2, int arg3);

int DWCi_StartMatching(void) {
    int result;
    if (lbl_805A0FA8[0]->unkF8 != 0) {
        ServerBrowserClear(lbl_805A0FA8[0]->unkF8);
    }
    if (lbl_805A0FA8[0]->unk4A5 != 0) {
        lbl_805A0FA8[0]->unk1B4 = 0xC;
    } else if (lbl_805A0FA8[0]->unk16 == 0) {
        lbl_805A0FA8[0]->unk1B4 = 2;
    } else if (lbl_805A0FA8[0]->unk16 == 1) {
        lbl_805A0FA8[0]->unk1B4 = 3;
        result = DWCi_SendResvCommandToFriend(0, 1, 0);
        if (lbl_805A0FA8[0]->unk16 == 0) {
            result = DWCi_HandleSBError(result);
        } else {
            result = fn_80349C88(result);
        }
        if (result != 0) {
            return 0;
        }
        return 1;
    } else {
        if (gpIsBuddy(lbl_805A0FA8[0]->gpConnection, lbl_805A0FA8[0]->profiles[0])) {
            lbl_805A0FA8[0]->unk1B4 = 3;
            result = DWCi_SendResvCommand((void*)lbl_805A0FA8[0]->profiles[0], 0);
            if (lbl_805A0FA8[0]->unk16 == 0) {
                result = DWCi_HandleSBError(result);
            } else {
                result = fn_80349C88(result);
            }
            if (result != 0) {
                return 0;
            }
        } else {
            lbl_805A0FA8[0]->unk1B4 = 0xD;
            DWCi_SBUpdateAsync(lbl_805A0FA8[0]->profiles[0]);
        }
        return 1;
    }
    lbl_805A0FA8[0]->unk21C = 0;
    result = DWCi_SBUpdateAsync(lbl_805A0FA8[0]->unk21C);
    if (DWCi_HandleSBError(result) != 0) {
        return 0;
    }
    return 1;
}

void DWCi_QR2PlayerKeyCallback(void) {
}

DWCMatchControl* DWCi_GetLoginTimeView(void) {
    return lbl_805A0FA8[0];
}

void DWCi_NNFreeNegotiateList(void) {
    if (lbl_805A0FA0 == 0) {
        NNFreeNegotiateList();
        return;
    }
    lbl_805A0FA4 = 1;
}

void DWCi_QR2TeamKeyCallback(void) {
}

void qr2_send_statechanged(void* qr2Obj);

void fn_80341454(void) {
    if (lbl_805A0FA8[0]->unk15 != 2) {
        lbl_805A0FA8[0]->_14 = 0;
        lbl_805A0FA8[0]->unk18 = 0;
        qr2_send_statechanged(lbl_805A0FA8[0]->qr2Obj);
    }
}

void DWCi_ShutdownMatch(void) {
    int i;
    DWCMatchEntry* entry;
    lbl_805A0FA8[0] = 0;
    if (lbl_805A0F90 != 0) {
        DWC_Free(4, lbl_805A0F90, 0);
        lbl_805A0F90 = 0;
    }
    entry = lbl_805338B0;
    for (i = 0; i < 0x9A; i++, entry++) {
        if (entry->unk4 != 0) {
            DWC_Free(4, entry->unk4, 0);
        }
    }
    memset(lbl_805338B0, 0, 0x738);
}

int DWC_SetGT2ConnectionLatencyLimit(int a, int b) {
    if (b < a) {
        return 0;
    }
    lbl_8059F0B8 = a;
    lbl_8059F0BC = b;
    return 1;
}

void DWC_GetGT2ConnectionLatencyLimit(int* a, int* b) {
    if (a != 0) {
        *a = lbl_8059F0B8;
    }
    if (b != 0) {
        *b = lbl_8059F0BC;
    }
}

int DWCi_GetLatencyLevel(void) {
    s64 ms = lbl_805A0FA8[0]->unk4C0 / (s64)(*(u32*)0x800000F8 / 4 / 1000);
    int level;
    if (lbl_8059F0B0 != -1) {
        return lbl_8059F0B0;
    }
    level = ((int)ms + 4999) / 5000;
    if (level > 10) {
        level = 10;
    }
    return level;
}

int snprintf(char* buffer, u32 size, const char* format, ...);
void DWC_SetCommonKeyValueString(const char* key, const char* value, char* buffer, int size);
void DWC_AddCommonKeyValueString(const char* key, const char* value, char* buffer, int size);
int DWCi_SetGPStatus(int status, const char* statusString, const char* locationString);
void gt2CloseAllConnectionsHard(void* socket);
void DWCi_SetError(int error, int errorCode);
int DWCi_GetFriendListIndex(u32 pid);

int DWCi_GPSetServerStatus(int status) {
    char value[12];
    char buffer[39];
    const char* modifier;
    const char* key;
    snprintf(value, 12, "%u", lbl_805A0FA8[0]->unk19 + 1);
    DWC_SetCommonKeyValueString("SCM", value, buffer, 0x2F);
    snprintf(value, 12, "%u", lbl_805A0FA8[0]->unkD + 1);
    DWC_AddCommonKeyValueString("SCN", value, buffer, 0x2F);
    key = "MDF";
    modifier = "N";
    if (lbl_805A0FA8[0]->unk1BA != 0) {
        modifier = "Y";
    }
    DWC_AddCommonKeyValueString(key, modifier, buffer, 0x2F);
    snprintf(value, 12, "%u", 11);
    DWC_AddCommonKeyValueString("VER", value, buffer, 0x2F);
    snprintf(value, 12, "%u", DWCi_GetLatencyLevel());
    DWC_AddCommonKeyValueString("LLV", value, buffer, 0x2F);
    return DWCi_SetGPStatus(status, buffer, 0);
}


int DWCi_RequestSuspendMatching(int suspend);

int DWC_RequestSuspendMatching(int suspend) {
    return DWCi_RequestSuspendMatching(suspend);
}

void DWCi_QR2NatnegCallback(int cookie) {
    DWCMatchControl* control;
    DWC_Printf(0x40, "Got NN request, cookie = %x.\n", cookie);
    if (lbl_805A0FA8[0]->unk1B4 == 1) {
        lbl_805A0FA8[0]->unk1B4 = 5;
    } else if (lbl_805A0FA8[0]->unk1B4 != 5 && lbl_805A0FA8[0]->unk1B4 != 0x10) {
        DWC_Printf(0x40, "But already canceled reservation.\n");
        return;
    }
    if (cookie == lbl_805A0FA8[0]->unk194) {
        lbl_805A0FA8[0]->unk190++;
    } else {
        lbl_805A0FA8[0]->unk190 = 0;
        lbl_805A0FA8[0]->unk194 = cookie;
    }
    control = lbl_805A0FA8[0];
    control->unk19C = 0;
    control->unk198 = 0;
    DWCi_NNStartupAsync(1, cookie, 0);
    if (DWCi_HandleNNError() == 0) {
        lbl_805A0FA8[0]->unk3E8 = 0xFF;
    }
}

void DWCi_CancelPreConnectedServerProcessWithResetLevel(int clientPid, int level);

void fn_80341DDC(int clientPid) {
    DWCMatchControl* control;
    int level;
    control = lbl_805A0FA8[0];
    if (control->unk1BC == 2) {
        return;
    }
    level = 2;
    if (control->unk16 == 0 || control->unk16 == 1) {
        if (control->unkF == 0) {
            level = 1;
        }
    }
    DWCi_CancelPreConnectedServerProcessWithResetLevel(clientPid, level);
}

int DWCi_ResumeMatching(void) {
    DWCMatchControl* control;
    int result;
    int handled;
    lbl_805A0FA8[0]->unk1A = 0;
    lbl_805A0FA8[0]->unk34 = 0;
    lbl_805A0FA8[0]->unk1BC = 0;
    control = lbl_805A0FA8[0];
    if (control->unk220 != 0) {
        if (control->unk16 == 0) {
            control->unk1B4 = 2;
            result = DWCi_SBUpdateAsync(0);
            if (DWCi_HandleSBError(result) != 0) {
                return result;
            }
        } else if (control->unk16 == 1) {
            control->unk1B4 = 3;
            result = DWCi_SendResvCommand(lbl_805A0FA8[0]->unk220, 0);
            if (lbl_805A0FA8[0]->unk16 == 0) {
                handled = DWCi_HandleSBError(result);
            } else {
                handled = fn_80349C88(result);
            }
            if (handled != 0) {
                return result;
            }
        }
    } else {
        DWCi_RestartFromCancel(1);
    }
    return 0;
}

void fn_8033CBEC(DWCMatchControl* control, void* conn, void** gt2, u32 word8, u32 name, u32 secret, u32 opt1, u32 opt2) {
    int i;
    DWCMatchEntry* entry;
    lbl_805A0FA8[0] = control;
    control->gpConnection = conn;
    lbl_805A0FA8[0]->gt2Holder = gt2;
    lbl_805A0FA8[0]->unk8 = word8;
    lbl_805A0FA8[0]->qr2Obj = 0;
    lbl_805A0FA8[0]->unk30 = 0;
    lbl_805A0FA8[0]->unk2E = 0;
    lbl_805A0FA8[0]->unkF8 = 0;
    lbl_805A0FA8[0]->unk1B4 = 0;
    lbl_805A0FA8[0]->_1B = 0;
    lbl_805A0FA8[0]->unk1F0 = 0;
    lbl_805A0FA8[0]->unk218 = 0;
    lbl_805A0FA8[0]->unk30C = name;
    lbl_805A0FA8[0]->unk310 = secret;
    lbl_805A0FA8[0]->unk314 = opt1;
    lbl_805A0FA8[0]->unk318 = opt2;
    memset((u8*)lbl_805A0FA8[0] + 0x31C, 0, 0x40);
    lbl_805A0FA8[0]->unk35C = 0;
    lbl_805A0FA8[0]->unk480 = 0;
    lbl_805A0FA8[0]->unk484 = 0;
    lbl_805A0FA8[0]->unk490 = 0;
    lbl_805A0FA8[0]->unk494 = 0;
    control = lbl_805A0FA8[0];
    control->unk4B8 = 0;
    control->unk4C0 = 0;
    control->unk498 = 0;
    lbl_805A0FA8[0]->unk49C = 0;
    lbl_805A0FA8[0]->unk4A0 = 0;
    lbl_805A0FA8[0]->unk4A4 = 0;
    lbl_805A0FA8[0]->unk4A5 = 0;
    lbl_805A0FA8[0]->unk16 = 0;
    lbl_805A0FA8[0]->unk17 = 0;
    lbl_805A0FA8[0]->unk188 = 0;
    DWC_Printf(4, "ServerBrowserLimitUpdate timeout reset.(%s)\n", "DWCi_MatchInit");
    entry = lbl_805338B0;
    for (i = 0; i < 0x9A; i++, entry++) {
        if (entry->unk4 != 0) {
            DWC_Free(4, entry->unk4, 0);
        }
    }
    memset(lbl_805338B0, 0, 0x738);
    DWCi_ResetMatchParam(0);
}

int fn_8033FE10(int a, int b) {
    if (lbl_805A0F94 == 0) {
        return 1;
    }
    if (a == 0 || b == 0) {
        return 0;
    }
    return __abs(a - b) <= 1;
}

void fn_80341220(int error, int errorCode) {
    int isServer;
    int isSelf;
    DWCMatchControl* control;
    if (lbl_805A0FA8[0] == 0 || error == 0) {
        return;
    }
    lbl_805A0FA8[0]->unk1BC = 2;
    gt2CloseAllConnectionsHard(*lbl_805A0FA8[0]->gt2Holder);
    lbl_805A0FA8[0]->unk1BC = 0;
    DWCi_SetError(error, errorCode);
    DWCi_GPSetServerStatus(1);
    control = lbl_805A0FA8[0];
    isServer = (control->unk15 == 2);
    isSelf = (control->unk224 == 0);
    ((void (*)(int, int, int, int, int, int))control->unk480)(
        error, 0, isSelf, isServer,
        DWCi_GetFriendListIndex(control->unk224), control->unk484);
    DWCi_CloseMatching();
}

void* ServerBrowserNewA(u32 a, u32 b, u32 c, int d, int e, int f, int g, void* cb, void* userdata);
void fn_8034AA48(void* sb, int reason, void* server, void* instance);
int fn_80349F28(int err);
void* memcpy(void* dst, const void* src, u32 size);
void fn_8034296C(u8 a, u8 b, int c, int d);
int DWCi_QR2Startup(u32 profileID);

int fn_8033DB6C(u32 a0, int a1, int a2, void* a3, void* a4, u8 a5, void* a6, const void* a7, void* a8) {
    u8 buf[4];
    DWCMatchControl* control = lbl_805A0FA8[0];
    if (control->unkF8 == 0) {
        u32 gamename = control->unk30C;
        control->unkF8 = ServerBrowserNewA(gamename, gamename, control->unk310, 0, 0x14, 1, 0, (void*)fn_8034AA48, 0);
    }
    if (lbl_805A0FA8[0]->unkF8 == 0) {
        if (fn_80349F28(5) != 0) {
            return -1;
        }
    }
    memcpy(buf, a7, 4);
    fn_8034296C(3, 0, a1, a2);
    lbl_805A0FA8[0]->unk4A0 = a0;
    lbl_805A0FA8[0]->unk4A4 = 0;
    lbl_805A0FA8[0]->unk4A5 = a5;
    lbl_805A0FA8[0]->unk4C8 = a6;
    memcpy(lbl_805A0FA8[0]->unk4CC, buf, 4);
    lbl_805A0FA8[0]->unk554 = a8;
    if (lbl_805A0FA8[0]->qr2Obj == 0) {
        if (DWCi_QR2Startup(lbl_805A0FA8[0]->unk218) != 0) {
            return 0;
        }
    }
    {
        DWCMatchControl* c = lbl_805A0FA8[0];
        if (c->unk30 != 0 && c->unk2E != 0) {
            if (!DWCi_StartMatching()) {
                return 0;
            }
        } else {
            c->unk1B4 = 0xE;
            lbl_805A0FA8[0]->unk210 = OSGetTime();
        }
    }
    lbl_805A0FA8[0]->unk480 = a1;
    lbl_805A0FA8[0]->unk484 = a2;
    lbl_805A0FA8[0]->unk488 = a3;
    lbl_805A0FA8[0]->unk48C = a4;
    lbl_805A0FA8[0]->unk15 = 3;
    lbl_805A0FA8[0]->unk17 = 1;
    qr2_send_statechanged(lbl_805A0FA8[0]->qr2Obj);
    lbl_805A0FA8[0]->unk1C1 = 0;
    lbl_805A0FA8[0]->unk210 = OSGetTime();
    return 1;
}

int ServerBrowserCount(void* serverBrowser);
void SBServerAddIntKeyValue(void* server, const char* key, int value);
void ServerBrowserSortA(void* serverBrowser, int ascending, const char* key, int compareMode);
int DWCi_GetMathRand32(int max);

void fn_8034B67C(void) {
    static const int weights[6] = { 3, 3, 2, 2, 1, 1 };
    int probs[6];
    int* pProb;
    const int* pWeight;
    int i;
    int j;
    u32 randVal;
    int maxEval = 0;
    int totalWeight = 0;
    if (ServerBrowserCount(lbl_805A0FA8[0]->unkF8) <= 1) {
        return;
    }
    pWeight = weights;
    for (i = 0; i < ServerBrowserCount(lbl_805A0FA8[0]->unkF8); i++) {
        int eval = SBServerGetIntValueA(ServerBrowserGetServer(lbl_805A0FA8[0]->unkF8, i), "dwc_eval", -1);
        if (eval > maxEval) {
            maxEval = eval;
        }
        totalWeight += *pWeight;
        pWeight++;
    }
    randVal = DWCi_GetMathRand32(100);
    pProb = probs;
    pWeight = weights;
    for (j = 0; j < ServerBrowserCount(lbl_805A0FA8[0]->unkF8); j++) {
        if (j == ServerBrowserCount(lbl_805A0FA8[0]->unkF8) - 1) {
            probs[j] = 100;
            break;
        }
        {
            int cur = (*pWeight * 100) / totalWeight;
            int prev = (j > 0) ? pProb[-1] : 0;
            *pProb = cur + prev;
        }
        if (randVal < (u32)*pProb) {
            break;
        }
        pProb++;
        pWeight++;
    }
    DWC_Printf(0x40, "Server[%d] is selected (%d/100: rand %d)\n", j, probs[j], randVal);
    if (maxEval < 0x7FFFFFFF) {
        maxEval++;
    }
    SBServerAddIntKeyValue(ServerBrowserGetServer(lbl_805A0FA8[0]->unkF8, j), "dwc_eval", maxEval);
    ServerBrowserSortA(lbl_805A0FA8[0]->unkF8, 0, "dwc_eval", 0);
}

void DWCi_StopLogin(int error, int errorCode);
void DWCi_StopFriendProcess(int error, int errorCode);

// HYPOTHESIS: same body as fn_80341220. Declared in the order that matches
// once this copy is inlined into the error handlers; the out-of-line
// fn_80341220 keeps the declaration order that matches on its own.
static void DWCi_StopMatchingInlined(int error, int errorCode) {
    if (lbl_805A0FA8[0] == 0 || error == 0) {
        return;
    }
    lbl_805A0FA8[0]->unk1BC = 2;
    gt2CloseAllConnectionsHard(*lbl_805A0FA8[0]->gt2Holder);
    lbl_805A0FA8[0]->unk1BC = 0;
    DWCi_SetError(error, errorCode);
    DWCi_GPSetServerStatus(1);
    {
        DWCMatchControl* control = lbl_805A0FA8[0];
        int isSelf;
        int isServer;
        isServer = (control->unk15 == 2);
        isSelf = (control->unk224 == 0);
        ((void (*)(int, int, int, int, int, int))control->unk480)(
            error, 0, isSelf, isServer,
            DWCi_GetFriendListIndex(control->unk224), control->unk484);
    }
    DWCi_CloseMatching();
}

void fn_8033D4C0(u8 a0, int a1, int a2, void* a3, void* a4, void* a5, const void* a6, void* a7) {
    DWCMatchControl* control;
    u32 groupId;
    fn_8034296C(2, a0, a1, a2);
    lbl_805A0FA8[0]->unk488 = a3;
    lbl_805A0FA8[0]->unk48C = a4;
    lbl_805A0FA8[0]->unk4C8 = a5;
    if (a6 != 0) {
        memcpy(lbl_805A0FA8[0]->unk4CC, a6, 4);
    } else {
        memset(lbl_805A0FA8[0]->unk4CC, 0, 4);
    }
    lbl_805A0FA8[0]->unk554 = a7;
    memcpy(lbl_805A0FA8[0]->unk4D4, lbl_805A0FA8[0]->unk4CC, 4);
    lbl_805A0FA8[0]->profiles[0] = lbl_805A0FA8[0]->unk218;
    lbl_805A0FA8[0]->unk308 = 1;
    lbl_805A0FA8[0]->unkE = 0;
    lbl_805A0FA8[0]->unk1B4 = 0xF;
    if (fn_80349C88(DWCi_GPSetServerStatus(6)) != 0) {
        return;
    }
    lbl_805A0FA8[0]->unk17 = 2;
    if (lbl_805A0FA8[0]->qr2Obj == 0) {
        DWCi_QR2Startup(lbl_805A0FA8[0]->unk218);
    }
    control = lbl_805A0FA8[0];
    groupId = (DWCi_GetMathRand32(10000000) + control->unk218) + 100000000;
    DWC_Printf(0x200001, "Create New Group ID:%u\n", groupId);
    lbl_805A0FA8[0]->unk4A0 = groupId;
    control = lbl_805A0FA8[0];
    if (control->unkF8 == 0) {
        u32 gamename = control->unk30C;
        control->unkF8 = ServerBrowserNewA(gamename, gamename, control->unk310, 0, 0x14, 1, 0, (void*)fn_8034AA48, 0);
    }
    qr2_send_statechanged(lbl_805A0FA8[0]->qr2Obj);
}

int fn_80349C88(int result) {
    int code;
    int err;
    if (result == 0) {
        return 0;
    }
    DWC_Printf(2, "Match, GP error %d\n", result);
    switch (result) {
    case 1:
        err = 9;
        code = -1;
        break;
    case 2:
        err = 9;
        code = -2;
        break;
    case 3:
        err = 6;
        code = -10;
        break;
    case 4:
        err = 6;
        code = -20;
        break;
    }
    DWCi_StopMatchingInlined(err, -81000 + code);
    return result;
}

int fn_80349F28(int result) {
    int code;
    int err;
    if (result == 0) {
        return 0;
    }
    DWC_Printf(2, "Match, SB error %d\n", result);
    switch (result) {
    case 1:
        err = 6;
        code = -50;
        break;
    case 2:
        err = 6;
        code = -30;
        break;
    case 3:
        err = 6;
        code = -20;
        break;
    case 4:
        err = 6;
        code = -40;
        break;
    case 5:
        err = 9;
        code = -1;
        break;
    case 6:
        err = 9;
        code = -2;
        break;
    }
    DWCi_StopMatchingInlined(err, -85000 + code);
    return result;
}

int DWCi_HandleQR2Error(int result) {
    int code;
    int err;
    if (result == 0) {
        return 0;
    }
    DWC_Printf(2, "Match, QR2 error %d\n", result);
    switch (result) {
    case 1:
        err = 6;
        code = -50;
        break;
    case 2:
        err = 6;
        code = -60;
        break;
    case 3:
        err = 6;
        code = -30;
        break;
    case 4:
        err = 6;
        code = -80;
        break;
    case 5:
        err = 6;
        code = -20;
        break;
    }
    switch (DWC_GetState()) {
    case 2:
        DWCi_StopLogin(err, -64000 + code);
        break;
    case 4:
        DWCi_StopFriendProcess(err, -74000 + code);
        break;
    case 5:
        DWCi_StopMatchingInlined(err, -84000 + code);
        break;
    default:
        DWCi_SetError(err, -94000 + code);
        break;
    }
    return result;
}

int fn_8034A500(int result) {
    int code;
    int err;
    if (result == 0) {
        return 0;
    }
    DWC_Printf(2, "Match, NN error %d\n", result);
    switch (result) {
    case 1:
        err = 9;
        code = -1;
        break;
    case 2:
        err = 6;
        code = -50;
        break;
    case 3:
        err = 6;
        code = -30;
        break;
    }
    DWCi_StopMatchingInlined(err, -86000 + code);
    return result;
}

int fn_8034A790(int result) {
    int code;
    int err;
    if (result == 0) {
        return 0;
    }
    DWC_Printf(2, "Match, GT2 error %d\n", result);
    switch (result) {
    case 1:
        err = 9;
        code = -1;
        break;
    case 2:
    case 5:
        err = 0;
        code = 0;
        result = 0;
        break;
    case 3:
        err = 6;
        code = -10;
        break;
    case 4:
        err = 6;
        code = -30;
        break;
    case 6:
        err = 6;
        code = -70;
        break;
    case 7:
        err = 6;
        code = -80;
        break;
    }
    if (err != 0) {
        DWCi_StopMatchingInlined(err, -87000 + code);
    }
    return result;
}

int fn_80342BA8(int arg);
int fn_80346D80(int a0, int a1, int a2);

void fn_80348ACC(int level) {
    int isServer;
    int isSelf;
    DWCMatchControl* control;
    if (level == 0) {
        if (fn_80349C88(DWCi_GPSetServerStatus(1)) != 0) {
            return;
        }
        DWCi_CloseMatching();
        control = lbl_805A0FA8[0];
        {
            u32 pid = control->unk224;
            isServer = (pid != 0) ? 1 : (control->unk15 == 2);
            isSelf = (pid == 0);
            ((void (*)(int, int, int, int, int, int))control->unk480)(
                0, 1, isSelf, isServer,
                DWCi_GetFriendListIndex(control->unk224), control->unk484);
        }
        lbl_805A0FA8[0]->unk1BD = 0;
    } else {
        DWCi_ResetMatchParam(level);
        control = lbl_805A0FA8[0];
        if (control->unk16 == 2 || control->unk16 == 3) {
            int isSelf;
            DWCMatchControl* c = lbl_805A0FA8[0];
            isSelf = (c->unk224 == 0);
            ((void (*)(int, int, int, int, int, int))control->unk480)(
                0, 1, isSelf, 0,
                DWCi_GetFriendListIndex(c->unk224), c->unk484);
        } else if (control->unk16 == 0) {
            int isSelf = (control->unk224 == 0);
            ((void (*)(int, int, int, int, int, int))control->unk480)(
                0, 1, isSelf, 0,
                DWCi_GetFriendListIndex(control->unk224), control->unk484);
            if (level == 1) {
                if (fn_80349F28(fn_80342BA8(0)) == 0) {
                    return;
                }
            }
        } else if (control->unk16 == 1) {
            int isSelf = (control->unk224 == 0);
            ((void (*)(int, int, int, int, int, int))control->unk480)(
                0, 1, isSelf, 0,
                DWCi_GetFriendListIndex(control->unk224), control->unk484);
            if (level == 1) {
                fn_80346D80(0, 0, 0);
            }
        } else {
            DWC_Printf(2, "ERROR - DWCi_RestartFromCancel : matchType %d, level %d\n", control->unk15, level);
        }
    }
}

// BYTES: these keys share the original unit-wide string table.
extern const char lbl_8048AA60[];

void fn_8037B354(int key, const char* name);

void fn_8034296C(u8 type, u8 count, int callback, int parameter) {
    const char* keys = lbl_8048AA60;
    DWCi_ResetMatchParam(0);
    lbl_805A0FA8[0]->unk15 = type;
    lbl_805A0FA8[0]->unk18 = count;
    lbl_805A0FA8[0]->unk480 = callback;
    lbl_805A0FA8[0]->unk484 = parameter;
    lbl_805A0FA8[0]->unk191 = 0;
    lbl_805A0FA8[0]->aidList[0] = 0;
    fn_8037B354(0x32, "dwc_pid");
    fn_8037B354(0x33, keys + 0x16c);
    fn_8037B354(0x34, keys + 0x178);
    fn_8037B354(0x35, keys + 0x160);
    fn_8037B354(0x36, keys + 0xd00);
    fn_8037B354(0x37, keys + 0xd0c);
    fn_8037B354(0x38, keys + 0xd18);
    lbl_805A0FA8[0]->unk17 = 0;
    fn_8037B354(0x39, keys + 0xd24);
    lbl_805A0FA8[0]->unk4B0 = 1;
    lbl_805A0FA8[0]->unk19 = count;
    fn_8037B354(0x3A, keys + 0xd34);
    lbl_805A0FA8[0]->unk1C = 0;
    lbl_805A0FA8[0]->unk28 = 0;
    lbl_805A0FA8[0]->unk20 = 0;
}

// HYPOTHESIS: socket address layout used by the Wii QR2 and NAT callbacks.
typedef struct DWCMatchAddress {
    u8 length;
    u8 family;
    u16 port;
    u32 ip;
} DWCMatchAddress;
u16 SOHtoNs(u16 value);
int memcmp(const void* left, const void* right, u32 size);
extern const u8 lbl_8059FB68[];
void qr2_parse_queryA(void* qr2, u8* data, int size, DWCMatchAddress* from);
void NNProcessData(u8* data, int size, DWCMatchAddress* from);

int fn_8033FC9C(void* socket, u32 ip, u16 port, u8* data, int size) {
    DWCMatchAddress from;
    const char* messages = lbl_8048AA60;
    u8 first;
    if (size == 0 || data == 0) {
        return 0;
    }
    memset(&from, 0, sizeof(from));
    from.family = 2;
    from.ip = ip;
    from.port = SOHtoNs(port);
    first = data[0];
    if ((first == 0xFE && data[1] == 0xFD) || first == '\\') {
        DWC_Printf(0x40, messages + 0x520);
        if (lbl_805A0FA8[0]->qr2Obj != 0) {
            qr2_parse_queryA(lbl_805A0FA8[0]->qr2Obj, data, size, &from);
        } else {
            DWC_Printf(0x40, messages + 0x548);
        }
    } else if (memcmp(data, lbl_8059FB68, 6) == 0) {
        DWC_Printf(0x40, messages + 0x560);
        NNProcessData(data, size, &from);
    } else if (first == 0xFE && data[1] == 0xFE) {
        DWC_Printf(0x80, messages + 0x588);
        return 0;
    } else {
        DWC_Printf(8, messages + 0x5B8);
        return 0;
    }
    return 1;
}

u16 gt2GetLocalPort(void* socket);
int gt2GetSocketSOCKET(void* socket);
int qr2_init_socketA(void** qr2, int socket, int port, u32 gamename, u32 secret, int ispublic, int natneg, void* serverKeyCb, void* playerKeyCb, void* teamKeyCb, void* keyListCb, void* countCb, void* addErrorCb, void* userdata);
void qr2_shutdown(void* qr2);
void qr2_register_publicaddress_callback(void* qr2, void* cb);
void qr2_register_natneg_callback(void* qr2, void* cb);
void qr2_register_clientmessage_callback(void* qr2, void* cb);
void fn_8034B834(void);
void fn_8034C7CC(u8* data, int len);

static inline u32 DWCi_Swap32(u32 x) {
    u32 swapped = ((x >> 8) & 0x00FF00FF) | ((x << 8) & 0xFF00FF00);
    return (swapped >> 16) | (swapped << 16);
}

int DWCi_QR2Startup(u32 profileID) {
    int result;
    DWCMatchControl* control;
    int retry;
    if (lbl_805A0FA8[0]->qr2Obj != 0) {
        DWC_Printf(0x40, "QR2 is already set up.\n");
        return 0;
    }
    lbl_805A0FA8[0]->unk218 = profileID;
    for (retry = 0; retry < 1; retry++) {
        control = lbl_805A0FA8[0];
        result = qr2_init_socketA(
            &control->qr2Obj,
            gt2GetSocketSOCKET(*control->gt2Holder),
            gt2GetLocalPort(*control->gt2Holder),
            control->unk30C,
            control->unk310,
            1,
            1,
            (void*)fn_8034B834,
            (void*)DWCi_QR2PlayerKeyCallback,
            (void*)DWCi_QR2TeamKeyCallback,
            (void*)DWCi_QR2KeyListCallback,
            (void*)DWCi_QR2CountCallback,
            (void*)DWCi_QR2AddErrorCallback,
            0);
        if (result == 0) {
            break;
        }
        qr2_shutdown(lbl_805A0FA8[0]->qr2Obj);
        lbl_805A0FA8[0]->qr2Obj = 0;
        if (result != 3 || retry == 0) {
            DWCi_HandleQR2Error(result);
            return result;
        }
    }
    lbl_805A0FA8[0]->unk30 = 0;
    lbl_805A0FA8[0]->unk2E = 0;
    qr2_register_publicaddress_callback(lbl_805A0FA8[0]->qr2Obj, (void*)DWCi_QR2PublicAddrCallback);
    qr2_register_natneg_callback(lbl_805A0FA8[0]->qr2Obj, (void*)DWCi_QR2NatnegCallback);
    qr2_register_clientmessage_callback(lbl_805A0FA8[0]->qr2Obj, (void*)fn_8034C7CC);
    qr2_send_statechanged(lbl_805A0FA8[0]->qr2Obj);
    return result;
}

int fn_80343500(u8 cmd, u32 pid, u32 ip, u16 port, u32* data, int count);
void fn_8033A650(u8 aid);

static inline u8 DWCi_GetAIDFromProfileID1(int pid) {
    DWCMatchControl* c = lbl_805A0FA8[0];
    int count = c->unkD;
    u8* pidCursor = (u8*)c + 4;
    int j;
    for (j = 1; j <= count; pidCursor += 4, j++) {
        if (pid == ((DWCMatchControl*)pidCursor)->profiles[0]) {
            return c->aidList[j];
        }
    }
    return 0xFF;
}

int fn_80348EFC(u32 aidBitmap) {
    u32 closedPids[32];
    u32 connectedPids[32];
    int closedCount = 0;
    int connectedCount = 0;
    int result;
    {
        int i;
        DWCMatchControl* control = lbl_805A0FA8[0];
        u8* aidCursor = (u8*)control + 1;
        u8* pidCursor = (u8*)control + 4;
        u32* pConn = connectedPids;
        u32* pClosed = closedPids;
        for (i = 1; i <= control->unkD; i++) {
            if (aidBitmap & (1 << ((DWCMatchControl*)aidCursor)->aidList[0])) {
                connectedCount++;
                *pConn++ = ((DWCMatchControl*)pidCursor)->profiles[0];
            } else {
                u32 pid = ((DWCMatchControl*)pidCursor)->profiles[0];
                closedCount++;
                *pClosed++ = DWCi_Swap32(pid);
            }
            aidCursor++;
            pidCursor += 4;
        }
    }
    {
        u32* pConn = connectedPids;
        int i;
        for (i = 0; i < connectedCount; pConn++, i++) {
            result = fn_80343500(0x10, *pConn, 0, 0, closedPids, closedCount);
            if (lbl_805A0FA8[0]->unk16 == 0) {
                result = fn_80349F28(result);
            } else {
                result = fn_80349C88(result);
            }
            if (result != 0) {
                return 0;
            }
        }
    }
    lbl_805A0FA8[0]->unk1BC = 2;
    {
        u32* pClosed = closedPids;
        int i;
        for (i = 0; i < closedCount; pClosed++, i++) {
            u8 aid = DWCi_GetAIDFromProfileID1(*pClosed);
            if (aid != 0xFF) {
                fn_8033A650(aid);
            }
        }
    }
    lbl_805A0FA8[0]->unk1BC = 0;
    return 1;
}

int strncmp(const char* left, const char* right, u32 size);

void fn_8034C7CC(u8* data, int len) {
    DWCSBCommandMessage msg;
    const char* msgs = lbl_8048AA60;
    int offset = 0;
    if (DWC_GetState() != 5 &&
        !(DWC_GetState() == 6 && (lbl_805A0FA8[0]->unk15 == 2 || lbl_805A0FA8[0]->unk15 == 3))) {
        DWC_Printf(4, msgs + 0x21F8);
        return;
    }
    while (offset + 0x14 <= len) {
        u32 ver;
        u32 pid;
        DWCi_Np_CPUCopyFast(&msg, data, 0x14);
        msg.version = DWCi_Swap32(msg.version);
        msg.profileId = DWCi_Swap32(msg.profileId);
        msg.port = (msg.port >> 8) | (msg.port << 8);
        if (strncmp(msg.magic, "SBCM", 4) != 0) {
            DWC_Printf(8, msgs + 0x2220);
            return;
        }
        if (msg.version != 0xB) {
            DWC_Printf(8, msgs + 0x223C);
            return;
        }
        DWCi_Np_CPUCopyFast(msg.data, data + 0x14, msg.size);
        DWC_Printf(0x40, msgs + 0x2260, msg.command, msg.ip, msg.port, msg.profileId);
        if (!DWCi_ProcessRecvMatchCommand(msg.command, msg.profileId, msg.ip, msg.port, (u8*)msg.data, (int)msg.size >> 2)) {
            return;
        }
        offset += msg.size + 0x14;
    }
}

int fn_8033FE64(int latency, int level, int arg2, int arg3) {
    const char* msgs = lbl_8048AA60;
    int ret = 0;
    DWC_Printf(0x200000, msgs + 0x5E8);
    if (!fn_8033FE10(DWCi_GetLatencyLevel(), level)) {
        DWC_Printf(0x200000, msgs + 0x5FC);
    } else if (latency <= lbl_8059F0B8) {
        DWC_Printf(0x200000, msgs + 0x610);
        ret = 1;
    } else if (lbl_8059F0BC <= latency) {
        DWC_Printf(0x200000, msgs + 0x624);
    } else if (arg2 == 1) {
        DWC_Printf(0x200000, msgs + 0x638);
        ret = 1;
    } else if (arg2 > 1 && latency <= arg3 + lbl_8059F0C0) {
        DWC_Printf(0x200000, msgs + 0x64C);
        ret = 1;
    } else {
        DWC_Printf(0x200000, msgs + 0x660);
    }
    return ret;
}

void ServerBrowserRemoveServer(void* sb, void* server);

int fn_8034B494(int doSort) {
    int i;
    int deleted = 0;
    for (i = 0; i < ServerBrowserCount(lbl_805A0FA8[0]->unkF8); i++) {
        void* server = ServerBrowserGetServer(lbl_805A0FA8[0]->unkF8, i);
        if (lbl_805A0FA8[0]->unk16 == 0) {
            int pid = SBServerGetIntValueA(server, "dwc_pid", 0);
            DWCMatchControl* control = lbl_805A0FA8[0];
            int removed = 0;
            int count = control->unkD;
            u8* cursor = (u8*)control + 4;
            int j;
            for (j = 1; j <= count; j++, cursor += 4) {
                if (pid == ((DWCMatchControl*)cursor)->profiles[0]) {
                    ServerBrowserRemoveServer(control->unkF8, server);
                    removed = 1;
                    i--;
                    break;
                }
            }
            if (removed) {
                continue;
            }
        }
        if (lbl_805A0FA8[0]->unk490 != 0) {
            int eval = ((int (*)(int, int))lbl_805A0FA8[0]->unk490)(i, lbl_805A0FA8[0]->unk494);
            if (eval > 0) {
                if (eval > 0x7FFFFF) {
                    eval = 0x7FFFFF;
                }
                SBServerAddIntKeyValue(server, "dwc_eval", (eval << 8) | DWCi_GetMathRand32(0x100));
            } else {
                ServerBrowserRemoveServer(lbl_805A0FA8[0]->unkF8, server);
                DWC_Printf(0x400, "Deleted server [%d] (eval point is %d).\n", i, eval);
                deleted = 1;
                i--;
            }
        } else {
            SBServerAddIntKeyValue(server, "dwc_eval", DWCi_GetMathRand32(0x80));
        }
    }
    if (doSort != 0 && ServerBrowserCount(lbl_805A0FA8[0]->unkF8) != 0) {
        ServerBrowserSortA(lbl_805A0FA8[0]->unkF8, 0, "dwc_eval", 0);
    }
    if (deleted != 0 && ServerBrowserCount(lbl_805A0FA8[0]->unkF8) == 0) {
        return 0;
    }
    return 1;
}

int fn_8034D940(u8 aid, int suspend, int arg2);

static inline u8 DWCi_GetAIDFromProfileID0(DWCMatchControl* control, int pid) {
    DWCMatchControl* c = lbl_805A0FA8[0];
    int count = c->unkD;
    u8* cursor = (u8*)c;
    int i;
    for (i = 0; i <= count; cursor += 4, i++) {
        if (pid == ((DWCMatchControl*)cursor)->profiles[0]) {
            return control->aidList[i];
        }
    }
    return 0xFF;
}

int DWCi_RequestSuspendMatching(int suspend) {
    DWCMatchControl* control;
    if (DWCi_IsError() || lbl_805A0FA8[0] == 0 || lbl_805A0FA8[0]->unk17 == 0 ||
        (DWC_GetState() != 5 && DWC_GetState() != 6) ||
        ((control = lbl_805A0FA8[0])->unk17 == 0 || (control->unk1B4 != 0xF && control->unk1B4 != 1))) {
        return 0;
    }
    if ((int)control->unk17 == 2) {
        return fn_8034D940(DWCi_GetAIDFromProfileID0(control, lbl_805A0FA8[0]->unk218), suspend, 0);
    } else {
        s64 elapsed = (OSGetTime() - control->unk20) / (s64)(*(u32*)0x800000F8 / 4 / 1000);
        if (elapsed > 3000) {
            u32 data[4];
            u32* pData;
            u32 pid;
            u32 susp;
            int result;
            pid = lbl_805A0FA8[0]->unk218;
            susp = DWCi_Swap32(suspend);
            pData = data;
            data[1] = 0;
            data[2] = susp;
            data[0] = DWCi_Swap32(pid);
            data[3] = DWCi_Swap32(DWC_GetMyAID());
            result = DWCi_SendSBMsgCommand(0x82, lbl_805A0FA8[0]->unk38[0], lbl_805A0FA8[0]->unkB8[0], pData, 4);
            if (lbl_805A0FA8[0]->unk16 == 0) {
                result = fn_80349F28(result);
            } else {
                result = fn_80349C88(result);
            }
            if (result != 0) {
                return 0;
            }
            lbl_805A0FA8[0]->unk20 = OSGetTime();
        }
    }
    return 1;
}

void fn_8034D6F0(int createNew) {
    int status;
    if (createNew != 0) {
        DWCMatchControl* control;
        u32 groupId;
        lbl_805A0FA8[0]->profiles[0] = lbl_805A0FA8[0]->unk218;
        lbl_805A0FA8[0]->unk308 = 1;
        lbl_805A0FA8[0]->unkE = 0;
        control = lbl_805A0FA8[0];
        groupId = (DWCi_GetMathRand32(10000000) + control->unk218) + 100000000;
        DWC_Printf(0x200001, "Create New Group ID:%u\n", groupId);
        lbl_805A0FA8[0]->unk4A0 = groupId;
    }
    lbl_805A0FA8[0]->unk15 = 2;
    lbl_805A0FA8[0]->unk1B4 = 0xF;
    lbl_805A0FA8[0]->unk17 = 2;
    lbl_805A0FA8[0]->unk34 = 0;
    lbl_805A0FA8[0]->unk1A = 0;
    status = 6;
    {
        DWCMatchControl* c = lbl_805A0FA8[0];
        char value[12];
        char buffer[0x2C];
        const char* modifier;
        const char* key;
        if (c->unk16 == 1) {
            status = 4;
        }
        snprintf(value, 12, "%u", c->unk19 + 1);
        DWC_SetCommonKeyValueString("SCM", value, buffer, 0x2F);
        snprintf(value, 12, "%u", lbl_805A0FA8[0]->unkD + 1);
        DWC_AddCommonKeyValueString("SCN", value, buffer, 0x2F);
        key = "MDF";
        modifier = "N";
        if (lbl_805A0FA8[0]->unk1BA != 0) {
            modifier = "Y";
        }
        DWC_AddCommonKeyValueString(key, modifier, buffer, 0x2F);
        snprintf(value, 12, "%u", 11);
        DWC_AddCommonKeyValueString("VER", value, buffer, 0x2F);
        snprintf(value, 12, "%u", DWCi_GetLatencyLevel());
        DWC_AddCommonKeyValueString("LLV", value, buffer, 0x2F);
        fn_80349C88(DWCi_SetGPStatus(status, buffer, 0));
    }
    lbl_805A0FA8[0]->unk21C = 0;
}

u32 SBServerGetPublicInetAddress(void* server);
u16 SBServerGetPublicQueryPort(void* server);

int fn_80346B10(u32 pid, int force) {
    u32 data[16];
    u16 port = 0;
    u32 ip = 0;
    u8 cmd;
    DWCMatchControl* c0;
    if (force != 0 || ((c0 = lbl_805A0FA8[0])->unk30 == 0 && c0->unk2E == 0)) {
        s64 t;
        DWCMatchControl* c;
        lbl_805A0FA8[0]->unk1CC = 1;
        t = OSGetTime();
        c = lbl_805A0FA8[0];
        c->unk1D0 = t;
        c->profiles[0] = pid;
        if (force == 0) {
            DWC_Printf(4, "Delay ResvCommand - qr2IP & qr2Port = 0.\n");
        }
        return 0;
    } else if (c0->unk16 == 0) {
        void* server = ServerBrowserGetServer(c0->unkF8, 0);
        DWCMatchControl* c;
        c = lbl_805A0FA8[0];
        c->profiles[0] = SBServerGetIntValueA(server, "dwc_pid", 0);
        c = lbl_805A0FA8[0];
        c->unk38[0] = SBServerGetPublicInetAddress(server);
        c = lbl_805A0FA8[0];
        c->unkB8[0] = SBServerGetPublicQueryPort(server);
        ip = lbl_805A0FA8[0]->unk38[0];
        port = lbl_805A0FA8[0]->unkB8[0];
        lbl_805A0FA8[0]->unk21C = lbl_805A0FA8[0]->profiles[0];
    } else {
        if (c0->unk16 == 1) {
            c0->profiles[0] = pid;
        }
        lbl_805A0FA8[0]->unk21C = pid;
        if (!gpIsBuddy(lbl_805A0FA8[0]->gpConnection, pid)) {
            void* server = ServerBrowserGetServer(lbl_805A0FA8[0]->unkF8, 0);
            ip = SBServerGetPublicInetAddress(server);
            port = SBServerGetPublicQueryPort(server);
        }
    }
    lbl_805A0FA8[0]->unk1D8 = 6000;
    {
        s64 t = OSGetTime();
        DWCMatchControl* c = lbl_805A0FA8[0];
        c->unk1E0 = t;
        c->unk1CC = 0;
    }
    cmd = 1;
    if (lbl_805A0FA8[0]->unk220 != 0) {
        cmd = 0xB;
    }
    {
        DWCMatchControl* c = lbl_805A0FA8[0];
        data[0] = DWCi_Swap32(c->unk16);
        data[1] = c->unk30;
        data[2] = DWCi_Swap32(c->unk2E);
        data[3] = DWCi_Swap32(c->unk4A5);
        data[4] = *(u32*)c->unk4CC;
    }
    return fn_80343500(cmd, pid, ip, port, data, 5);
}

void NNCancel(int cookie);
void** DWCi_GetGT2ConnectionByProfileID(u32 pid, int count);
void gt2CloseConnectionHard(void* conn);
int fn_80341E20(int index, int count);

static inline int DWCi_SendCancelMatchSynCommand(const char* msgs, int type, u32 targetPid) {
    u32 data;
    int result;
    DWC_Printf(0x80, msgs + 0x1AC8, type, targetPid);
    if (type == 0) {
        data = DWCi_Swap32(lbl_805A0FA8[0]->unk224);
        result = fn_80343500(0xD, targetPid, 0, 0, &data, 1);
    } else if (type == 1) {
        result = fn_80343500(0xE, targetPid, 0, 0, &data, 0);
    } else {
        result = fn_80343500(0xF, targetPid, 0, 0, &data, 0);
    }
    if (lbl_805A0FA8[0]->unk16 == 0) {
        result = fn_80349F28(result);
    } else {
        result = fn_80349C88(result);
    }
    if (result != 0) {
        return 0;
    }
    lbl_805A0FA8[0]->unk200 = OSGetTime();
    return 1;
}

int fn_80347390(u32 pid, int level) {
    const char* msgs = lbl_8048AA60;
    DWCMatchControl* control = lbl_805A0FA8[0];
    if (control->unkD < 0x1F) {
        control->profiles[control->unkD + 1] = 0;
    }
    lbl_805A0FA8[0]->unk3E8 = 0xFF;
    if (lbl_805A0FA8[0]->unk1B0 != 0) {
        NNCancel(lbl_805A0FA8[0]->unk1B0);
        lbl_805A0FA8[0]->unk1B0 = 0;
    }
    lbl_805A0FA8[0]->_14 = lbl_805A0FA8[0]->unkD;
    lbl_805A0FA8[0]->unk21C = 0;
    if ((int)lbl_805A0FA8[0]->unk17 == 2) {
        void** conn;
        int i;
        DWC_Printf(4, msgs + 0x1A98);
        lbl_805A0FA8[0]->unk1A = 0;
        lbl_805A0FA8[0]->unk34 = 0;
        lbl_805A0FA8[0]->unk1B4 = 0x13;
        lbl_805A0FA8[0]->unk1EC = 0;
        lbl_805A0FA8[0]->unk1C4 = 0;
        lbl_805A0FA8[0]->unk224 = pid;
        conn = DWCi_GetGT2ConnectionByProfileID(pid, lbl_805A0FA8[0]->unkD + 1);
        if (conn != 0) {
            lbl_805A0FA8[0]->unk1BC = 2;
            gt2CloseConnectionHard(*conn);
            lbl_805A0FA8[0]->unk1BC = 0;
        } else {
            DWCMatchControl* c = lbl_805A0FA8[0];
            int count = c->unkD + 1;
            if (c != 0) {
                u8* cursor = (u8*)c;
                int j;
                for (j = 0; j < count; cursor += 4, j++) {
                    if ((int)pid == ((DWCMatchControl*)cursor)->profiles[0]) {
                        fn_80341E20(j, count);
                        break;
                    }
                }
            }
        }
        {
            DWCMatchControl* cLoop;
            for (i = 1; i <= (cLoop = lbl_805A0FA8[0])->unkD; i++) {
                u32 targetPid = cLoop->profiles[i];
                if (!DWCi_SendCancelMatchSynCommand(msgs, 0, targetPid)) {
                    return 0;
                }
            }
        }
        if (lbl_805A0FA8[0]->unkD == 0) {
            fn_80348ACC(level);
        }
    } else {
        DWC_Printf(4, msgs + 0x1AEC);
    }
    return 1;
}

static inline int DWCi_GetSuspendMatchingInlined(DWCMatchControl** pCtrl) {
    DWCMatchControl* c = *pCtrl;
    int result = -1;
    u32 bitmap;
    int isOther;
    if (c == 0) {
        return 0;
    }
    bitmap = c->unk1C;
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
    *pCtrl = lbl_805A0FA8[0];
    return (*pCtrl)->unk28;
}

u8 fn_8034649C(int pid, u32 ip, u16 port, u32 arg3, int arg4, int arg5);

u8 fn_80346220(u32 pid, u32 ip, u16 port, u32 arg3, int arg4, int arg5) {
    const char* msgs = lbl_8048AA60;
    DWCMatchControl* control = lbl_805A0FA8[0];
    u8 ret;
    switch (control->unk17) {
    case 1:
        if (control->unk4A0 != 0) {
            DWCMatchControl* c;
            DWC_Printf(0x200040, msgs + 0x18C8);
            c = lbl_805A0FA8[0];
            if (c->unk19 != 0 && c->_14 == c->unk19) {
                DWC_Printf(0x200040, msgs + 0x18E8, c->unk18, c->_14);
                ret = 3;
            } else if (arg3 == 1 && c->unk16 != 1) {
                DWC_Printf(0x200040, msgs + 0x1910);
                ret = 3;
            } else if (DWCi_GetSuspendMatchingInlined(&c)) {
                DWC_Printf(0x200040, msgs + 0x1934);
                ret = 3;
            } else if (arg3 == 1 && !gpIsBuddy(c->gpConnection, pid)) {
                DWC_Printf(0x200040, msgs + 0x1960);
                ret = 0xFF;
            } else if (lbl_805A0FA8[0]->unk4C8 != 0 && !((int (*)(void*, void*))lbl_805A0FA8[0]->unk4C8)(lbl_805A0FA8[0]->unk4D0, lbl_805A0FA8[0]->unk554)) {
                ret = 3;
            } else {
                ret = 0x80;
                DWC_Printf(0x200040, msgs + 0x19A0, lbl_805A0FA8[0]->unk18, lbl_805A0FA8[0]->_14);
            }
        } else {
            ret = 3;
        }
        break;
    case 0:
    case 2:
        ret = fn_8034649C(pid, ip, port, arg3, arg4, arg5);
        break;
    }
    return ret;
}

u32 strlen(const char* str);
int gpSendBuddyMessageA(void* conn, u32 pid, const char* msg);

int fn_80343500(u8 cmd, u32 pid, u32 ip, u16 port, u32* data, int count) {
    const char* msgs = lbl_8048AA60;
    char addBuf[512];
    char msgBuf[512];
    char hexBuf[16];
    int len = 0;
    int ret;
    if (gpIsBuddy(lbl_805A0FA8[0]->gpConnection, pid)) {
        int hdrLen;
        int addLen;
        void* conn;
        char* body;
        DWC_Printf(0x200004, msgs + 0xF3C, pid);
        if (data != 0 && count != 0) {
            u8* p = (u8*)data;
            int i;
            len = snprintf(addBuf, 0x200, msgs + 0xF58, p[0], p[1], p[2], p[3]);
            p += 4;
            for (i = 1; i < count; i++) {
                int n = snprintf(hexBuf, 0x10, msgs + 0xF6C, p[0], p[1], p[2], p[3]);
                DWCi_Np_CPUCopyFast(addBuf + len, hexBuf, n);
                len += n;
                p += 4;
            }
        }
        addBuf[len] = 0;
        conn = lbl_805A0FA8[0]->gpConnection;
        hdrLen = snprintf(msgBuf, 0x200, "%s%dv%s", "GPCM", 11, "MAT");
        msgBuf[hdrLen] = cmd;
        body = &msgBuf[1] + hdrLen;
        *body = 0;
        addLen = strlen(addBuf);
        DWCi_Np_CPUCopyFast(body, addBuf, addLen);
        body[addLen] = 0;
        ret = gpSendBuddyMessageA(conn, pid, msgBuf);
        DWC_Printf(0x40, msgs + 0xF80, cmd, pid);
    } else {
        if (ip == 0 && port == 0) {
            u8* cursor;
            int i;
            DWC_Printf(0x200004, msgs + 0xFB0, cmd, pid);
            cursor = (u8*)lbl_805A0FA8[0];
            for (i = 0; i < 32; cursor += 4, i++) {
                if ((int)pid == ((DWCMatchControl*)cursor)->profiles[0]) {
                    ip = lbl_805A0FA8[0]->unk38[i];
                    port = lbl_805A0FA8[0]->unkB8[i];
                    DWC_Printf(0x200004, msgs + 0xFE0);
                    break;
                }
            }
        }
        DWC_Printf(0x200004, msgs + 0x100C, ip, port);
        ret = DWCi_SendSBMsgCommand(cmd, ip, port, data, count);
    }
    if (cmd == 2 || cmd == 6 || (cmd >= 8 && cmd <= 9)) {
        DWCMatchControl* c;
        lbl_805A0FA8[0]->unk3E8 = cmd;
        lbl_805A0FA8[0]->unk3EA = port;
        lbl_805A0FA8[0]->unk3EC = ip;
        lbl_805A0FA8[0]->unk470 = pid;
        lbl_805A0FA8[0]->unk474 = count;
        {
            s64 t = OSGetTime();
            c = lbl_805A0FA8[0];
            c->unk478 = t;
        }
        if (data != 0 && count != 0) {
            DWCi_Np_CPUCopyFast(c->unk3F0, data, count * 4);
        }
    }
    return ret;
}

int fn_80347604(u32 pid, int error, int errorCode);
void fn_80347990(int arg);

int fn_803491F8(void) {
    const char* msgs = lbl_8048AA60;
    DWCMatchControl* control = lbl_805A0FA8[0];
    u64 elapsed;
    int state = control->unk1B4;
    if (state == 8 || (u32)(state - 0x15) <= 1) {
        elapsed = (OSGetTime() - control->unk1F8) / (s64)(*(u32*)0x800000F8 / 4 / 1000);
    } else {
        return 1;
    }
    switch (lbl_805A0FA8[0]->unk1B4) {
    case 8:
        if (elapsed > 6000) {
            DWC_Printf(0x80, msgs + 0x1CB0, lbl_805A0FA8[0]->unk1C1, 5);
            if (DWC_GetState() == 5 && lbl_805A0FA8[0]->unk1C1 >= 5) {
                DWC_Printf(0x80, msgs + 0x1CD4);
                if (!fn_80347604(lbl_805A0FA8[0]->profiles[0], 6, -80430)) {
                    return 0;
                }
            } else {
                lbl_805A0FA8[0]->unk1C1++;
                DWCi_SendMatchSynPacket(lbl_805A0FA8[0]->aidList[0], 3);
            }
        }
        break;
    case 0x15:
        if (elapsed > 6000) {
            lbl_805A0FA8[0]->unk1BF++;
            if (lbl_805A0FA8[0]->unk1BF > 5) {
                DWCMatchControl* c = lbl_805A0FA8[0];
                DWC_Printf(0x40, msgs + 0x1D08, c->unk1E8);
                c = lbl_805A0FA8[0];
                if (c->unk16 == 0 || c->unk16 == 1) {
                    c->unk1BC = 2;
                    gt2CloseAllConnectionsHard(*lbl_805A0FA8[0]->gt2Holder);
                    lbl_805A0FA8[0]->unk1BC = 0;
                    fn_80348ACC(1);
                } else {
                    if (!fn_80348EFC(c->unk1E8)) {
                        return 0;
                    }
                    if (lbl_805A0FA8[0]->unkD != 0) {
                        lbl_805A0FA8[0]->unk1BF = 0;
                        lbl_805A0FA8[0]->unk1F8 = OSGetTime();
                    } else {
                        DWCMatchControl* c2 = lbl_805A0FA8[0];
                        int level = 2;
                        u32 clientPid = c2->unk224;
                        if ((c2->unk16 == 0 || c2->unk16 == 1) && c2->unkF == 0) {
                            level = 1;
                        }
                        if (!fn_80347390(clientPid, level)) {
                            return 0;
                        }
                    }
                }
            } else {
                int i;
                for (i = 1; i <= lbl_805A0FA8[0]->unkD; i++) {
                    u8 aid = lbl_805A0FA8[0]->aidList[i];
                    if (!(lbl_805A0FA8[0]->unk1E8 & (1 << aid))) {
                        DWCi_SendMatchSynPacket(aid, 2);
                    }
                }
            }
        }
        break;
    case 0x16:
        if (elapsed > lbl_805A0FA8[0]->unk1C2) {
            fn_80347990(4);
        }
        break;
    }
    return 1;
}

int fn_8034708C(u32 pid) {
    DWCMatchControl* c;
    s64 t;
    lbl_805A0FA8[0]->unk220 = 0;
    lbl_805A0FA8[0]->unk21C = 0;
    lbl_805A0FA8[0]->_1BB = 0;
    t = OSGetTime();
    c = lbl_805A0FA8[0];
    c->unk1E0 = t;
    if (c->unk4A5 == 1) {
        c->unk1B4 = 0xC;
        if (fn_80349F28(fn_80342BA8(0)) != 0) {
            return 0;
        }
    } else if (c->unk16 == 0) {
        c->unk1B4 = 2;
        if (fn_80349F28(fn_80342BA8(0)) != 0) {
            return 0;
        }
    } else if (c->unk16 == 1) {
        int result = fn_80346D80(1, 0, pid);
        if (lbl_805A0FA8[0]->unk16 == 0) {
            result = fn_80349F28(result);
        } else {
            result = fn_80349C88(result);
        }
        if (result != 0) {
            return 0;
        }
    } else if (c->unk16 == 3) {
        if (c != 0) {
            c->unk1BC = 2;
            gt2CloseAllConnectionsHard(*lbl_805A0FA8[0]->gt2Holder);
            lbl_805A0FA8[0]->unk1BC = 0;
            DWCi_SetError(6, -80410);
            DWCi_GPSetServerStatus(1);
            {
                int isServer;
                int isSelf;
                DWCMatchControl* control = lbl_805A0FA8[0];
                isServer = (control->unk15 == 2);
                isSelf = (control->unk224 == 0);
                ((void (*)(int, int, int, int, int, int))control->unk480)(
                    6, 0, isSelf, isServer,
                    DWCi_GetFriendListIndex(control->unk224), control->unk484);
            }
            DWCi_CloseMatching();
        }
        return 0;
    }
    return 1;
}

const char* SBServerGetPrivateAddress(void* server);
u32 SBServerGetPrivateInetAddress(void* server);
u16 SBServerGetPrivateQueryPort(void* server);
const char* SBServerGetPublicAddress(void* server);
int SBServerHasPrivateAddress(void* server);
const char* SBServerGetStringValueA(void* server, const char* key, const char* defaultValue);

void fn_8034B17C(void* server) {
    const char* msgs = lbl_8048AA60;
    DWCMatchEntry* entry;
    int i;
    DWC_Printf(0x400, msgs + 0x1F54, SBServerGetPrivateAddress(server));
    DWC_Printf(0x400, msgs + 0x1F78, SBServerGetPrivateInetAddress(server));
    DWC_Printf(0x400, msgs + 0x1F9C, SBServerGetPrivateQueryPort(server));
    DWC_Printf(0x400, msgs + 0x1FC0, SBServerGetPublicAddress(server));
    DWC_Printf(0x400, msgs + 0x1FE4, SBServerGetPublicInetAddress(server));
    DWC_Printf(0x400, msgs + 0x2008, SBServerGetPublicQueryPort(server));
    DWC_Printf(0x400, msgs + 0x202C, SBServerHasPrivateAddress(server));
    DWC_Printf(0x400, msgs + 0x2050, SBServerGetIntValueA(server, msgs + 0x1EB8, -1));
    DWC_Printf(0x400, msgs + 0x2064, SBServerGetIntValueA(server, msgs + 0x1EC4, -1));
    DWC_Printf(0x400, msgs + 0x2078, "dwc_pid", SBServerGetIntValueA(server, "dwc_pid", 0));
    DWC_Printf(0x400, msgs + 0x2088, msgs + 0x178, SBServerGetIntValueA(server, msgs + 0x178, -1));
    DWC_Printf(0x400, msgs + 0x2094, msgs + 0x16C, SBServerGetIntValueA(server, msgs + 0x16C, -1));
    DWC_Printf(0x400, msgs + 0x2094, msgs + 0x160, SBServerGetIntValueA(server, msgs + 0x160, -1));
    DWC_Printf(0x400, msgs + 0x2094, msgs + 0xD0C, SBServerGetIntValueA(server, msgs + 0xD0C, -1));
    DWC_Printf(0x400, msgs + 0x2094, msgs + 0xD18, SBServerGetIntValueA(server, msgs + 0xD18, -1));
    DWC_Printf(0x400, msgs + 0x2094, msgs + 0xD24, SBServerGetIntValueA(server, msgs + 0xD24, -1));
    DWC_Printf(0x400, msgs + 0x2094, msgs + 0xD34, SBServerGetIntValueA(server, msgs + 0xD34, -1));
    entry = lbl_805338B0;
    for (i = 0; i < 0x9A; i++, entry++) {
        if (entry->unk0 != 0) {
            if (entry->unk1 != 0) {
                const char* val = SBServerGetStringValueA(server, (const char*)entry->unk4, "NONE");
                DWC_Printf(0x400, msgs + 0x20A0, entry->unk4, val);
            } else {
                int val = SBServerGetIntValueA(server, (const char*)entry->unk4, -1);
                DWC_Printf(0x400, msgs + 0x20AC, entry->unk4, val);
            }
        }
    }
}

int fn_80341E20(int index, int count) {
    int pid;
    if (lbl_805A0FA8[0] == 0) {
        return 0;
    }
    pid = lbl_805A0FA8[0]->profiles[index];
    lbl_805A0FA8[0]->unk308 &= ~(1 << lbl_805A0FA8[0]->aidList[index]);
    DWCi_SetNumValidConnection();
    if (index < count - 1) {
        int i;
        for (i = index; i < count - 1; i++) {
            lbl_805A0FA8[0]->unk38[i] = lbl_805A0FA8[0]->unk38[i + 1];
            lbl_805A0FA8[0]->unkB8[i] = lbl_805A0FA8[0]->unkB8[i + 1];
            lbl_805A0FA8[0]->profiles[i] = lbl_805A0FA8[0]->profiles[i + 1];
            lbl_805A0FA8[0]->unk228[i] = lbl_805A0FA8[0]->unk228[i + 1];
            lbl_805A0FA8[0]->unk2A8[i] = lbl_805A0FA8[0]->unk2A8[i + 1];
            lbl_805A0FA8[0]->aidList[i] = lbl_805A0FA8[0]->aidList[i + 1];
        }
    }
    if (count > 0) {
        lbl_805A0FA8[0]->unk38[count - 1] = 0;
        lbl_805A0FA8[0]->unkB8[count - 1] = 0;
        lbl_805A0FA8[0]->profiles[count - 1] = 0;
        lbl_805A0FA8[0]->unk228[count - 1] = 0;
        lbl_805A0FA8[0]->unk2A8[count - 1] = 0;
        lbl_805A0FA8[0]->aidList[count - 1] = 0;
    }
    return pid;
}

int fn_803467EC(u32 pid, u32 ip, u16 port) {
    u32 data[16];
    u8 aid;
    int i;
    int res;
    DWCMatchControl* c = lbl_805A0FA8[0];
    if (c->unk1A != 0 && (int)pid == (int)c->unk34) {
        return 0;
    }
    c->unk1A = 1;
    lbl_805A0FA8[0]->unk34 = pid;
    lbl_805A0FA8[0]->unk1CC = 0;
    lbl_805A0FA8[0]->unk1D8 = 0;
    qr2_send_statechanged(lbl_805A0FA8[0]->qr2Obj);
    lbl_805A0FA8[0]->unk21C = 0;
    lbl_805A0FA8[0]->profiles[lbl_805A0FA8[0]->_14 + 1] = pid;
    lbl_805A0FA8[0]->unk38[lbl_805A0FA8[0]->_14 + 1] = ip;
    lbl_805A0FA8[0]->unkB8[lbl_805A0FA8[0]->_14 + 1] = port;
    lbl_805A0FA8[0]->unk1C8 = ip;
    lbl_805A0FA8[0]->unk1C6 = port;
    for (aid = 0; aid < 32; aid++) {
        int j;
        for (j = 0; j <= lbl_805A0FA8[0]->_14; j++) {
            if (aid == lbl_805A0FA8[0]->aidList[j]) {
                break;
            }
        }
        if (j > lbl_805A0FA8[0]->_14) {
            break;
        }
    }
    lbl_805A0FA8[0]->aidList[lbl_805A0FA8[0]->_14 + 1] = aid;
    memcpy(&lbl_805A0FA8[0]->unk4D4[aid], lbl_805A0FA8[0]->unk4D0, 4);
    data[0] = DWCi_Swap32(pid);
    data[3] = DWCi_Swap32(port);
    data[2] = ip;
    data[1] = DWCi_Swap32(lbl_805A0FA8[0]->aidList[lbl_805A0FA8[0]->_14 + 1]);
    data[4] = *(u32*)lbl_805A0FA8[0]->unk4D0;
    for (i = 1; i <= lbl_805A0FA8[0]->_14; i++) {
        res = fn_80343500(7, lbl_805A0FA8[0]->profiles[i], lbl_805A0FA8[0]->unk38[i], lbl_805A0FA8[0]->unkB8[i], data, 5);
        if (res != 0) {
            return res;
        }
    }
    return 0;
}

static inline int DWCi_SendCancelMatchSynCommandLit(int type, u32 targetPid) {
    u32 data;
    int result;
    DWC_Printf(0x80, "Sent CANCEL SYN %d command to %u.\n", type, targetPid);
    if (type == 0) {
        data = DWCi_Swap32(lbl_805A0FA8[0]->unk224);
        result = fn_80343500(0xD, targetPid, 0, 0, &data, 1);
    } else {
        result = fn_80343500(0xE, targetPid, 0, 0, &data, 0);
    }
    if (lbl_805A0FA8[0]->unk16 == 0) {
        result = fn_80349F28(result);
    } else {
        result = fn_80349C88(result);
    }
    if (result != 0) {
        return 0;
    }
    lbl_805A0FA8[0]->unk200 = OSGetTime();
    return 1;
}

int fn_80349964(void) {
    DWCMatchControl* control = lbl_805A0FA8[0];
    u64 elapsed;
    int state = control->unk1B4;
    if (state == 7 || (u32)(state - 0x13) <= 1) {
        elapsed = (OSGetTime() - control->unk200) / (s64)(*(u32*)0x800000F8 / 4 / 1000);
    } else {
        return 1;
    }
    switch (lbl_805A0FA8[0]->unk1B4) {
    case 7:
        if (elapsed > 6000) {
            if (!DWCi_SendCancelMatchSynCommandLit(1, lbl_805A0FA8[0]->profiles[0])) {
                return 0;
            }
        }
        break;
    case 0x13:
        if (elapsed > 6000) {
            lbl_805A0FA8[0]->unk1C0++;
            if (lbl_805A0FA8[0]->unk1C0 > 5) {
                DWC_Printf(0x40, "Timeout: wait cancel SYN-ACK (aidbitmap 0x%x).\n", lbl_805A0FA8[0]->unk1EC);
                if (!fn_80348EFC(lbl_805A0FA8[0]->unk1EC)) {
                    return 0;
                }
                if (lbl_805A0FA8[0]->unkD != 0) {
                    lbl_805A0FA8[0]->unk1C0 = 0;
                    lbl_805A0FA8[0]->unk200 = OSGetTime();
                } else {
                    fn_80348ACC(2);
                }
            } else {
                DWCMatchControl* cLoop;
                int i;
                for (i = 1; i <= (cLoop = lbl_805A0FA8[0])->unkD; i++) {
                    u8 aid = lbl_805A0FA8[0]->aidList[i];
                    if (!(lbl_805A0FA8[0]->unk1EC & (1 << aid))) {
                        if (!DWCi_SendCancelMatchSynCommandLit(0, cLoop->profiles[i])) {
                            return 0;
                        }
                    }
                }
            }
        }
        break;
    case 0x14:
        if (elapsed > lbl_805A0FA8[0]->unk1C4) {
            fn_80348ACC(2);
        }
        break;
    }
    return 1;
}

// HYPOTHESIS: GPBuddyStatus layout for gpGetBuddyStatus.
typedef struct DWCiGPBuddyStatus {
    int profile;
    int status;
    char statusString[256];
    char locationString[256];
    u32 ip;
    u16 port;
} DWCiGPBuddyStatus;

void* DWCi_GetUserData(void);
int DWC_GetGsProfileId(void* userData, void* friendData);
int DWCi_Acc_IsValidFriendData(void* friendData);
int gpGetBuddyIndex(void* conn, int pid, int* index);
int gpGetBuddyStatus(void* conn, int index, DWCiGPBuddyStatus* status);
int DWC_GetCommonValueString(const char* key, char* value, const char* src, char sep);
u32 strtoul(const char* str, char** endptr, int base);

int fn_80346D80(int arg0, int arg1, int arg2) {
    char valBuf[4];
    int buddyIdx;
    DWCiGPBuddyStatus status;
    int startIdx;
    int started;
    int advance;
    int pid;
    advance = (arg1 == 0);
    if (arg1 != 0) {
        startIdx = lbl_805A0FA8[0]->unk1B9;
    } else {
        DWCMatchControl* c = lbl_805A0FA8[0];
        int cur = c->unk1B9;
        startIdx = (cur < (int)c->unk35C - 1) ? (cur + 1) : 0;
    }
    started = 0;
    while (1) {
        if (advance || started) {
            lbl_805A0FA8[0]->unk1B9++;
            if ((int)lbl_805A0FA8[0]->unk1B9 >= (int)lbl_805A0FA8[0]->unk35C) {
                lbl_805A0FA8[0]->unk1B9 = 0;
            }
        }
        if (started && startIdx == (int)lbl_805A0FA8[0]->unk1B9) {
            s64 t;
            DWCMatchControl* c;
            lbl_805A0FA8[0]->unk1D8 = 3000;
            t = OSGetTime();
            c = lbl_805A0FA8[0];
            c->unk1E0 = t;
            c->unk1CC = 0;
            return 0;
        }
        started = 1;
        {
            DWCMatchControl* c = lbl_805A0FA8[0];
            void* userData = DWCi_GetUserData();
            u8 friendIdx = c->unk31C[c->unk1B9];
            pid = DWC_GetGsProfileId(userData, (u8*)c->unk314 + friendIdx * 0xC);
        }
        if (pid == 0 || pid == -1) {
            continue;
        }
        {
            DWCMatchControl* c = lbl_805A0FA8[0];
            u8 friendIdx = c->unk31C[c->unk1B9];
            if (!DWCi_Acc_IsValidFriendData((u8*)c->unk314 + friendIdx * 0xC)) {
                continue;
            }
        }
        {
            DWCMatchControl* c = lbl_805A0FA8[0];
            int count = c->unkD;
            u8* cursor = (u8*)c + 4;
            int j;
            for (j = 1; j <= count; cursor += 4, j++) {
                if (pid == ((DWCMatchControl*)cursor)->profiles[0]) {
                    break;
                }
            }
            if (j <= c->unkD) {
                continue;
            }
        }
        valBuf[0] = '0';
        {
            int r1 = gpGetBuddyIndex(lbl_805A0FA8[0]->gpConnection, pid, &buddyIdx);
            int r2 = gpGetBuddyStatus(lbl_805A0FA8[0]->gpConnection, buddyIdx, &status);
            if ((r1 | r2) != 0) {
                continue;
            }
        }
        if (status.status != 4 && status.status != 2) {
            continue;
        }
        if (DWC_GetCommonValueString("LLV", valBuf, status.statusString, '/') == -1) {
            continue;
        }
        {
            int llv = strtoul(valBuf, 0, 10);
            if (!fn_8033FE10(DWCi_GetLatencyLevel(), llv)) {
                DWC_Printf(2, "pid(%d) and me is not match login latency LV.\n", pid);
                continue;
            }
        }
        break;
    }
    if ((u32)pid == arg2) {
        arg0 = 1;
    }
    return fn_80346B10(pid, arg0);
}

void DWCi_ResetMatchParam(int level) {
    lbl_805A0FA8[0]->_0C = 0;
    lbl_805A0FA8[0]->unk190 = 0;
    {
        DWCMatchControl* c = lbl_805A0FA8[0];
        c->unk192 = DWCi_GetMathRand32(0x10000);
    }
    lbl_805A0FA8[0]->unk194 = 0;
    {
        DWCMatchControl* c = lbl_805A0FA8[0];
        c->unk19C = 0;
        c->unk198 = 0;
        c->unk1A4 = 0;
        c->unk1A0 = 0;
        c->unk1B8 = 0;
    }
    lbl_805A0FA8[0]->unk1BD = 0;
    lbl_805A0FA8[0]->unk1BE = 0;
    lbl_805A0FA8[0]->unk1BF = 0;
    lbl_805A0FA8[0]->unk1C0 = 0;
    lbl_805A0FA8[0]->_1BB = 0;
    lbl_805A0FA8[0]->unk1BC = 0;
    lbl_805A0FA8[0]->unk1C4 = 0;
    lbl_805A0FA8[0]->unk1C6 = 0;
    lbl_805A0FA8[0]->unk1C8 = 0;
    {
        DWCMatchControl* c = lbl_805A0FA8[0];
        c->unk1F8 = 0;
        c->unk208 = 0;
        memset(&c->unk3E8, 0, 0x98);
    }
    if (level == 2) {
        lbl_805A0FA8[0]->_14 = lbl_805A0FA8[0]->unkD;
        if ((int)lbl_805A0FA8[0]->unk17 == 2) {
            lbl_805A0FA8[0]->unk1B4 = 0xF;
        } else if ((int)lbl_805A0FA8[0]->unk17 == 1) {
            lbl_805A0FA8[0]->unk1B4 = 1;
        } else {
            DWC_Printf(0x200008, "DWCi_ResetMatchParam: Host is Free!\n");
        }
        return;
    }
    lbl_805A0FA8[0]->unkD = 0;
    lbl_805A0FA8[0]->unkE = 0;
    lbl_805A0FA8[0]->_14 = 0;
    lbl_805A0FA8[0]->unk1A = 0;
    lbl_805A0FA8[0]->unk34 = 0;
    lbl_805A0FA8[0]->unkFC = 0;
    lbl_805A0FA8[0]->unk1B9 = 0;
    lbl_805A0FA8[0]->unk1C2 = 0;
    lbl_805A0FA8[0]->unk1CC = 0;
    {
        DWCMatchControl* c = lbl_805A0FA8[0];
        c->unk1D0 = 0;
        c->unk1D8 = 0;
    }
    {
        DWCMatchControl* c = lbl_805A0FA8[0];
        c->unk1E0 = 0;
        c->unk1E8 = 0;
    }
    lbl_805A0FA8[0]->unk21C = 0;
    lbl_805A0FA8[0]->unk220 = 0;
    lbl_805A0FA8[0]->unk308 = 0;
    memset(lbl_805A0FA8[0]->unk38, 0, 0x80);
    memset(lbl_805A0FA8[0]->unkB8, 0, 0x40);
    memset(lbl_805A0FA8[0]->profiles, 0, 0x80);
    memset(&lbl_805A0FA8[0]->unk1A8, 0, 0xC);
    memset(lbl_805A0FA8[0]->unk228, 0, 0x80);
    memset(lbl_805A0FA8[0]->unk2A8, 0, 0x40);
    memset(lbl_805A0FA8[0]->aidList, 0, 0x20);
    memset(&lbl_805A0FA8[0]->unk360, 0, 0x84);
    lbl_805A0FA8[0]->unk498 = 0;
    lbl_805A0FA8[0]->unk49C = 0;
    lbl_805A0FA8[0]->unk4A4 = 0;
    lbl_805A0FA8[0]->unk4B0 = 1;
    lbl_805A0FA8[0]->unk17 = 0;
    lbl_805A0FA8[0]->unkF = 0;
    lbl_805A0FA8[0]->unk4C8 = 0;
    lbl_805A0FA8[0]->unk554 = 0;
    memset(lbl_805A0FA8[0]->aidList, 0, 0x20);
    if (level == 1) {
        if (lbl_805A0FA8[0]->unk16 == 0) {
            lbl_805A0FA8[0]->unk1B4 = 2;
        } else if (lbl_805A0FA8[0]->unk16 == 1) {
            lbl_805A0FA8[0]->unk1B4 = 3;
        }
        return;
    }
    lbl_805A0FA8[0]->unk15 = 0;
    lbl_805A0FA8[0]->unk18 = 0;
    lbl_805A0FA8[0]->unk2C = 0;
    lbl_805A0FA8[0]->unk224 = 0;
    lbl_805A0FA8[0]->unk1BA = 0;
    lbl_805A0FA8[0]->unk1C1 = 0;
    {
        DWCMatchControl* c = lbl_805A0FA8[0];
        c->unk210 = 0;
        c->unk488 = 0;
    }
    lbl_805A0FA8[0]->unk48C = 0;
    memset(lbl_805A0FA8[0]->unk4D4, 0, 0x80);
    memset(lbl_805A0FA8[0]->unk4D0, 0, 4);
    memset(lbl_805A0FA8[0]->unk4CC, 0, 4);
    lbl_805A0FA8[0]->unk558 = 0;
    lbl_805A0FA8[0]->unk55C = 0;
}

void fn_8033D7D4(u32 pid, int a1, int a2, void* a3, void* a4, void* a5, const void* a6, void* a7) {
    u8 buf[4];
    memcpy(buf, a6, 4);
    fn_8034296C(3, 0, a1, a2);
    lbl_805A0FA8[0]->unk488 = a3;
    lbl_805A0FA8[0]->unk48C = a4;
    lbl_805A0FA8[0]->unk4C8 = a5;
    memcpy(lbl_805A0FA8[0]->unk4CC, buf, 4);
    lbl_805A0FA8[0]->unk554 = a7;
    lbl_805A0FA8[0]->unk1A = 1;
    {
        DWCMatchControl* c = lbl_805A0FA8[0];
        c->unk34 = c->unk218;
    }
    lbl_805A0FA8[0]->unk17 = 1;
    lbl_805A0FA8[0]->profiles[0] = pid;
    lbl_805A0FA8[0]->unk1B4 = 3;
    {
        DWCMatchControl* control = lbl_805A0FA8[0];
        if (control->unkF8 == 0) {
            u32 gamename = control->unk30C;
            control->unkF8 = ServerBrowserNewA(gamename, gamename, control->unk310, 0, 0x14, 1, 0, (void*)fn_8034AA48, 0);
        }
    }
    if (lbl_805A0FA8[0]->unkF8 == 0) {
        if (fn_80349F28(5) != 0) {
            return;
        }
    }
    if (fn_80349C88(DWCi_GPSetServerStatus(5)) != 0) {
        return;
    }
    if (lbl_805A0FA8[0]->qr2Obj == 0) {
        if (DWCi_QR2Startup(lbl_805A0FA8[0]->unk218) != 0) {
            return;
        }
    }
    if (lbl_805A0FA8[0]->unk4A5 == 0) {
        DWCMatchControl* c = lbl_805A0FA8[0];
        if (c->unk30 != 0 && c->unk2E != 0) {
            if (!DWCi_StartMatching()) {
                return;
            }
        } else {
            c->unk1B4 = 0xE;
            lbl_805A0FA8[0]->unk210 = OSGetTime();
        }
    } else {
        qr2_send_statechanged(lbl_805A0FA8[0]->qr2Obj);
        if (gpIsBuddy(lbl_805A0FA8[0]->gpConnection, lbl_805A0FA8[0]->profiles[0])) {
            int result;
            lbl_805A0FA8[0]->unk1B4 = 3;
            result = fn_80346B10(lbl_805A0FA8[0]->profiles[0], 0);
            if (lbl_805A0FA8[0]->unk16 == 0) {
                result = fn_80349F28(result);
            } else {
                result = fn_80349C88(result);
            }
            if (result == 0) {
                return;
            }
            return;
        } else {
            lbl_805A0FA8[0]->unk1B4 = 0xD;
            if (fn_80349F28(fn_80342BA8(lbl_805A0FA8[0]->profiles[0])) != 0) {
                return;
            }
        }
    }
}

int fn_8034D940(u8 aid, int suspend, int sendToClients) {
    DWCMatchControl* control = lbl_805A0FA8[0];
    u8 myAid = DWCi_GetAIDFromProfileID0(control, control->unk218);
    u32 oldBitmap = control->unk1C;
    u32 newBitmap;
    if (suspend != 0) {
        control->unk1C = oldBitmap | (1 << aid);
    } else {
        control->unk1C = oldBitmap & ~(1 << aid);
    }
    newBitmap = lbl_805A0FA8[0]->unk1C;
    if (oldBitmap != newBitmap) {
        if (aid == myAid) {
            DWC_Printf(0x210000, "**Server** AID(%u) suspend(%u) bitmap(%u->%u).\n", aid, suspend, oldBitmap, newBitmap);
        }
        DWCi_GPSetServerStatus(-1);
    }
    if (sendToClients == 0) {
        return 1;
    }
    {
        u32 data[4];
        u32 bitmap = lbl_805A0FA8[0]->unk1C;
        int isOther;
        int i;
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
        {
            u32 pid = lbl_805A0FA8[0]->unk218;
            u32 s1 = DWCi_Swap32(suspend);
            u32 s2 = DWCi_Swap32(bitmap);
            u32 s0 = DWCi_Swap32(pid);
            data[1] = 0x01000000;
            data[0] = s0;
            data[2] = s1;
            data[3] = s2;
        }
        {
            DWCMatchControl* cLoop;
            for (i = 1; i < (cLoop = lbl_805A0FA8[0])->unkD + 1; i++) {
                if (aid == cLoop->aidList[i]) {
                    int result = DWCi_SendSBMsgCommand(0x82, lbl_805A0FA8[0]->unk38[i], lbl_805A0FA8[0]->unkB8[i], data, 4);
                    if (lbl_805A0FA8[0]->unk16 == 0) {
                        result = fn_80349F28(result);
                    } else {
                        result = fn_80349C88(result);
                    }
                    if (result != 0) {
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

int fn_80347604(u32 pid, int error, int errorCode) {
    const char* msgs = lbl_8048AA60;
    int ret = 1;
    DWCMatchControl* c = lbl_805A0FA8[0];
    if (c->unk4A5 != 0 || c->unk16 == 3) {
        if (c->unkD != 0) {
            c->unk1BC = 2;
            gt2CloseAllConnectionsHard(*lbl_805A0FA8[0]->gt2Holder);
            lbl_805A0FA8[0]->unk1BC = 0;
        }
        DWCi_StopMatchingInlined(error, errorCode);
        return 0;
    } else {
        c->_14 = c->unkD;
        lbl_805A0FA8[0]->unk220 = 0;
        if (lbl_805A0FA8[0]->unk1B0 != 0) {
            NNCancel(lbl_805A0FA8[0]->unk1B0);
            lbl_805A0FA8[0]->unk1B0 = 0;
        }
        if (lbl_805A0FA8[0]->unkD != 0) {
            DWC_Printf(0x40, msgs + 0x1B1C);
            if (lbl_805A0FA8[0]->unk16 == 2 || lbl_805A0FA8[0]->unk16 == 3) {
                DWC_Printf(8, msgs + 0x2E0);
            } else {
                lbl_805A0FA8[0]->unk1BC = 2;
                gt2CloseAllConnectionsHard(*lbl_805A0FA8[0]->gt2Holder);
                lbl_805A0FA8[0]->unk1BC = 0;
                DWC_Printf(0x40, msgs + 0x310);
                fn_80348ACC(1);
            }
        } else {
            DWC_Printf(0x40, msgs + 0x1B48);
            lbl_805A0FA8[0]->unk1B4 = 3;
            DWC_Printf(0x40, msgs + 0x1B6C);
            ret = fn_8034708C(0);
        }
    }
    return ret;
}

int DWCi_GetProfileIDFromList(u8 idx);

static inline int DWCi_IsInFriendIdxList(int pid) {
    DWCMatchControl* c = lbl_805A0FA8[0];
    int i;
    if (c->unk314 == 0) {
        return 0;
    }
    for (i = 0; i < (int)(c = lbl_805A0FA8[0])->unk35C; i++) {
        int fpid = DWCi_GetProfileIDFromList(c->unk31C[i]);
        if (fpid > 0 && fpid == pid) {
            return 1;
        }
    }
    return 0;
}

u8 fn_8034649C(int pid, u32 ip, u16 port, u32 arg3, int arg4, int arg5) {
    u8 ret;
    DWCMatchControl* c;
    if (arg5 == 0) {
        if (lbl_805A0FA8[0]->unk16 == 1) {
            if (!gpIsBuddy(lbl_805A0FA8[0]->gpConnection, pid)) {
                return 0xFF;
            }
            if (!DWCi_IsInFriendIdxList(pid)) {
                DWC_Printf(0x40, "This friend doesn't exist in friendIdxList.\n");
                return 3;
            }
        } else if (lbl_805A0FA8[0]->unk16 == 2) {
            if (!gpIsBuddy(lbl_805A0FA8[0]->gpConnection, pid)) {
                return 0xFF;
            }
        }
    }
    c = lbl_805A0FA8[0];
    if (c->unk16 == 0 || c->unk16 == 1) {
    }
    {
        u8 maxNum = c->unk19;
        if ((int)c->_14 == (int)maxNum) {
            ret = 3;
        } else if (DWCi_GetSuspendMatchingInlined(&c)) {
            ret = 3;
        } else {
            int state = c->unk1B4;
            if (!((state == 0xF || state == 2 || state == 3) && (c->unk30 != 0 || c->unk2E != 0) && (ip != 0 || port != 0))) {
                if (state == 0x10 && pid == (int)c->unk34) {
                    lbl_805A0FA8[0]->unk478 = OSGetTime();
                    return 2;
                }
                DWC_Printf(0x200040, "please wait: qr2IP:%u qr2Port:%u qr2IPM:%u qr2PortM: %u\n", ip, port, lbl_805A0FA8[0]->unk30, lbl_805A0FA8[0]->unk2E);
                ret = 4;
            } else {
                if (c->unk21C != 0) {
                    if (pid == c->unk21C) {
                        if (arg4 != 0 || ((int)c->unk218 < pid && pid != (int)c->unk220)) {
                            ret = 2;
                        } else {
                            ret = 0xFF;
                        }
                    } else {
                        ret = 3;
                    }
                } else {
                    ret = 2;
                }
            }
        }
    }
    if (ret == 2) {
        if (lbl_805A0FA8[0]->unk4C8 != 0 && !((int (*)(void*, void*))lbl_805A0FA8[0]->unk4C8)(lbl_805A0FA8[0]->unk4D0, lbl_805A0FA8[0]->unk554)) {
            return 3;
        }
        if (lbl_805A0FA8[0]->unk17 == 0) {
            fn_8034D6F0(1);
        }
    }
    return ret;
}

void fn_80341484(u8 aid, int type, u8* data) {
    const char* msgs = lbl_8048AA60;
    DWC_Printf(0x80, msgs + 0xBD0, type - 2, aid);
    switch (type) {
    case 2:
        if (lbl_805A0FA8[0]->unk1B4 == 1) {
            u8 idx;
            if (data[0] == 1) {
                lbl_805A0FA8[0]->unk224 = 0;
            }
            idx = data[1];
            lbl_805A0FA8[0]->aidList[idx] = data[2];
            lbl_805A0FA8[0]->profiles[idx] = lbl_805A0FA8[0]->unk218;
            if (lbl_805A0FA8[0]->unk16 == 0 || lbl_805A0FA8[0]->unk16 == 1) {
                lbl_805A0FA8[0]->unk18 = lbl_805A0FA8[0]->unkD;
            }
            lbl_805A0FA8[0]->unk1C1 = 0;
            lbl_805A0FA8[0]->unk1B4 = 8;
            DWCi_SendMatchSynPacket(aid, 3);
        }
        break;
    case 3:
        if (lbl_805A0FA8[0]->unk1B4 == 0x15) {
            u16 val;
            u32 bitmap;
            int i;
            lbl_805A0FA8[0]->unk1E8 |= (1 << aid);
            val = data[0] | (data[1] << 8);
            if (val > lbl_805A0FA8[0]->unk1C2) {
                lbl_805A0FA8[0]->unk1C2 = val;
            }
            if (lbl_805A0FA8[0]->unk558 == 0) {
                DWC_Printf(0x200000, msgs + 0xBF8);
                lbl_805A0FA8[0]->unk558 = (u32)val << 1;
            }
            bitmap = 0;
            for (i = 1; i <= lbl_805A0FA8[0]->unkD; i++) {
                bitmap |= (1 << lbl_805A0FA8[0]->aidList[i]);
            }
            if (bitmap == lbl_805A0FA8[0]->unk1E8) {
                DWCMatchControl* cLoop;
                for (i = 1; i <= (cLoop = lbl_805A0FA8[0])->unkD; i++) {
                    DWCi_SendMatchSynPacket(cLoop->aidList[i], 4);
                }
                lbl_805A0FA8[0]->unk1B4 = 0x16;
                DWC_Printf(0x80, msgs + 0xC20, lbl_805A0FA8[0]->unk1C2);
            }
        } else {
            DWCi_SendMatchSynPacket(aid, 4);
        }
        break;
    case 4:
        if (lbl_805A0FA8[0]->unk1B4 == 8) {
            lbl_805A0FA8[0]->unk558 = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
            DWC_Printf(0x200000, msgs + 0xC3C, lbl_805A0FA8[0]->unk558);
            fn_80347990(4);
        }
        break;
    }
}

int strcmp(const char* a, const char* b);
void* DWC_Alloc(int kind, u32 size);

static inline u8 DWCi_GetNewKeyId(void) {
    u8 i;
    for (i = 0; i < 0x9A; i++) {
        if (lbl_805338B0[i].unk0 == 0) {
            return i + 0x64;
        }
    }
    return 0;
}

u8 fn_8033C180(u8 keyId, const char* keyString, const int* valueSrc) {
    int idx;
    if (keyString == 0 || valueSrc == 0) {
        return 0;
    }
    if (keyId >= 0x64 && lbl_805338B0[keyId - 0x64].unk0 != 0) {
        if (lbl_805338B0[keyId - 0x64].unk4 != 0 && strcmp((const char*)lbl_805338B0[keyId - 0x64].unk4, keyString) != 0) {
            return 0;
        }
    } else {
        keyId = DWCi_GetNewKeyId();
        if (keyId == 0) {
            return 0;
        }
    }
    idx = keyId - 0x64;
    lbl_805338B0[idx].unk0 = keyId;
    lbl_805338B0[idx].unk1 = 0;
    lbl_805338B0[idx].unk2 = 0;
    if (lbl_805338B0[idx].unk4 != 0) {
        DWC_Free(4, lbl_805338B0[idx].unk4, 0);
    }
    lbl_805338B0[idx].unk4 = DWC_Alloc(4, strlen(keyString) + 1);
    if (lbl_805338B0[idx].unk4 == 0) {
        DWCi_StopMatchingInlined(9, -80001);
        return 0;
    }
    strcpy((char*)lbl_805338B0[idx].unk4, keyString);
    DWC_Printf(4, "DWC_AddMatchKeyInt: key='%s', value=%d\n", keyString, *valueSrc);
    lbl_805338B0[idx].unk8 = (void*)valueSrc;
    fn_8037B354(keyId, (const char*)lbl_805338B0[idx].unk4);
    return keyId;
}

u8 fn_8033C5D8(u8 keyId, const char* keyString, const char* valueSrc) {
    int idx;
    if (keyString == 0 || valueSrc == 0) {
        return 0;
    }
    if (keyId >= 0x64 && lbl_805338B0[keyId - 0x64].unk0 != 0) {
        if (lbl_805338B0[keyId - 0x64].unk4 != 0 && strcmp((const char*)lbl_805338B0[keyId - 0x64].unk4, keyString) != 0) {
            return 0;
        }
    } else {
        keyId = DWCi_GetNewKeyId();
        if (keyId == 0) {
            return 0;
        }
    }
    idx = keyId - 0x64;
    lbl_805338B0[idx].unk0 = keyId;
    lbl_805338B0[idx].unk1 = 1;
    lbl_805338B0[idx].unk2 = 0;
    if (lbl_805338B0[idx].unk4 != 0) {
        DWC_Free(4, lbl_805338B0[idx].unk4, 0);
    }
    lbl_805338B0[idx].unk4 = DWC_Alloc(4, strlen(keyString) + 1);
    if (lbl_805338B0[idx].unk4 == 0) {
        DWCi_StopMatchingInlined(9, -80001);
        return 0;
    }
    strcpy((char*)lbl_805338B0[idx].unk4, keyString);
    DWC_Printf(4, "DWC_AddMatchKeyString: key='%s' value='%s'\n", keyString, valueSrc);
    lbl_805338B0[idx].unk8 = (void*)valueSrc;
    fn_8037B354(keyId, (const char*)lbl_805338B0[idx].unk4);
    return keyId;
}

u32 ServerBrowserGetMyPublicIPAddr(void* sb);
u32 SOGetHostID(void);
u32 SONtoHl(u32 v);
int ServerBrowserSendNatNegotiateCookieToServerA(void* sb, const char* ip, u16 port, int cookie);
int NNBeginNegotiationWithSocket(int sock, int cookie, int clientIndex, void* progressCb, void* completedCb, void* userdata);
void fn_8034C99C(int result, int sock, void* addr, void* userdata);

static inline int DWCi_IsPrivateIP(u32 ip) {
    u32 b0 = ip >> 24;
    u32 b1 = (ip >> 16) & 0xFF;
    if (b0 == 10) {
        return 1;
    }
    if (b0 == 172 && b1 >= 16 && b1 <= 31) {
        return 1;
    }
    if (b0 == 192 && b1 == 168) {
        return 1;
    }
    return 0;
}

int fn_80343118(int isServer, int cookie, void* server) {
    const char* msgs = lbl_8048AA60;
    DWCMatchControl* c = lbl_805A0FA8[0];
    int idx = c->_14;
    int ret = 0;
    int needNN;
    u32 newCookie;
    if (isServer == 0) {
        newCookie = (c->unk192 << 16) | (c->unk218 & 0xFFFF);
        if (SBServerHasPrivateAddress(server)) {
            if (SBServerGetPublicInetAddress(server) == ServerBrowserGetMyPublicIPAddr(lbl_805A0FA8[0]->unkF8)) {
                DWC_Printf(0x40, msgs + 0xE48, idx);
                lbl_805A0FA8[0]->unk228[idx] = SBServerGetPrivateInetAddress(server);
                lbl_805A0FA8[0]->unk2A8[idx] = SBServerGetPrivateQueryPort(server);
                needNN = 0;
            } else {
                DWC_Printf(0x40, msgs + 0xE70, idx);
                needNN = 1;
            }
        } else {
            if (DWCi_IsPrivateIP(SONtoHl(SOGetHostID()))) {
                DWC_Printf(0x40, msgs + 0xE8C, idx);
                needNN = 1;
            } else {
                DWC_Printf(0x40, msgs + 0xEC0, idx);
                lbl_805A0FA8[0]->unk228[idx] = SBServerGetPublicInetAddress(server);
                lbl_805A0FA8[0]->unk2A8[idx] = SBServerGetPublicQueryPort(server);
                needNN = 0;
            }
        }
        if (needNN != 0) {
            lbl_805A0FA8[0]->unk192 = DWCi_GetMathRand32(0x10000);
            lbl_805A0FA8[0]->unk1B0 = newCookie;
        } else {
            u32 data[2];
            u16 port;
            u32 ip;
            int res;
            data[0] = SOGetHostID();
            data[1] = DWCi_Swap32(gt2GetLocalPort(*lbl_805A0FA8[0]->gt2Holder));
            port = SBServerGetPublicQueryPort(server);
            ip = SBServerGetPublicInetAddress(server);
            res = fn_80343500(6, lbl_805A0FA8[0]->profiles[idx], ip, port, data, 2);
            lbl_805A0FA8[0]->unk3E9 = 0;
            if (res != 0) {
                return 2;
            }
            lbl_805A0FA8[0]->unk1B0 = 0;
        }
        lbl_805A0FA8[0]->unk1A8 = 0;
        lbl_805A0FA8[0]->unk1A9 = 0;
        lbl_805A0FA8[0]->unk1AA = SBServerGetPublicQueryPort(server);
        lbl_805A0FA8[0]->unk1AC = SBServerGetPublicInetAddress(server);
    } else {
        c->unk1A8 = 1;
        needNN = 1;
        lbl_805A0FA8[0]->unk1A9 = 0;
        lbl_805A0FA8[0]->unk1AA = 0;
        lbl_805A0FA8[0]->unk1AC = 0;
        lbl_805A0FA8[0]->unk1B0 = cookie;
    }
    if (needNN != 0) {
        DWCMatchControl* ctrl = lbl_805A0FA8[0];
        u8* nnInfo = &ctrl->unk1A8;
        if (ctrl->unk1A8 == 0) {
            if (fn_80349F28(ServerBrowserSendNatNegotiateCookieToServerA(ctrl->unkF8, gt2AddressToString(*(u32*)(nnInfo + 4), 0, 0), *(u16*)(nnInfo + 2), *(int*)(nnInfo + 8))) != 0) {
                return 2;
            }
            DWC_Printf(0x40, msgs + 0xEEC, *(int*)(nnInfo + 8));
        }
        ret = NNBeginNegotiationWithSocket(gt2GetSocketSOCKET(*lbl_805A0FA8[0]->gt2Holder), *(int*)(nnInfo + 8), nnInfo[0], (void*)DWCi_NNProgressCallback, (void*)fn_8034C99C, nnInfo);
        if (ret != 0) {
            if (ret == 3) {
                DWC_Printf(4, msgs + 0xF04);
            }
        }
    } else {
        DWCMatchControl* ctrl = lbl_805A0FA8[0];
        fn_8034C99C(0, gt2GetSocketSOCKET(*ctrl->gt2Holder), 0, &ctrl->unk1A8);
        ctrl = lbl_805A0FA8[0];
        ctrl->unk1A4 = 0;
        ctrl->unk1A0 = 0;
    }
    return ret;
}

int fn_803494F0(u32 pid, int type, u32 cancelPid) {
    const char* msgs = lbl_8048AA60;
    DWC_Printf(0x80, msgs + 0x1D44, type - 0xD, pid);
    if (DWC_GetState() != 6) {
        DWC_Printf(0x80, msgs + 0x1D70);
        return 1;
    }
    switch (type) {
    case 0xD:
        if (lbl_805A0FA8[0]->unk1B4 != 7) {
            void** conn;
            lbl_805A0FA8[0]->unk1B4 = 7;
            lbl_805A0FA8[0]->unk224 = cancelPid;
            conn = DWCi_GetGT2ConnectionByProfileID(cancelPid, lbl_805A0FA8[0]->unkD + 1);
            if (conn != 0) {
                lbl_805A0FA8[0]->unk1BC = 2;
                gt2CloseConnectionHard(*conn);
                lbl_805A0FA8[0]->unk1BC = 0;
            } else {
                DWCMatchControl* c = lbl_805A0FA8[0];
                int count = c->unkD + 1;
                if (c != 0) {
                    u8* cursor = (u8*)c;
                    int j;
                    for (j = 0; j < count; cursor += 4, j++) {
                        if ((int)cancelPid == ((DWCMatchControl*)cursor)->profiles[0]) {
                            fn_80341E20(j, count);
                            break;
                        }
                    }
                }
            }
        }
        if (!DWCi_SendCancelMatchSynCommand(msgs, 1, pid)) {
            return 0;
        }
        break;
    case 0xE:
        if (lbl_805A0FA8[0]->unk1B4 == 0x13) {
            s64 now = OSGetTime();
            DWCMatchControl* c = lbl_805A0FA8[0];
            u64 rtt = (u64)(now - c->unk200) / (u64)(*(u32*)0x800000F8 / 4 / 1000);
            u64 half = rtt >> 1;
            u8 aid;
            if (half > 300) {
                u64 adj = half - 300;
                if (adj > (u64)(s64)(int)c->unk1C4) {
                    c->unk1C4 = adj;
                }
            }
            {
                DWCMatchControl* c2 = lbl_805A0FA8[0];
                int count = c2->unkD;
                u8* pidCursor = (u8*)c2 + 4;
                int j;
                aid = 0xFF;
                for (j = 1; j <= count; pidCursor += 4, j++) {
                    if ((int)pid == ((DWCMatchControl*)pidCursor)->profiles[0]) {
                        aid = c2->aidList[j];
                        break;
                    }
                }
                if (aid != 0xFF) {
                    c2->unk1EC |= (1 << aid);
                }
            }
            if ((lbl_805A0FA8[0]->unk308 & ~(1 << lbl_805A0FA8[0]->aidList[0])) == lbl_805A0FA8[0]->unk1EC) {
                DWCMatchControl* cLoop;
                int i;
                for (i = 1; i <= (cLoop = lbl_805A0FA8[0])->unkD; i++) {
                    if (!DWCi_SendCancelMatchSynCommand(msgs, 2, cLoop->profiles[i])) {
                        return 0;
                    }
                }
                lbl_805A0FA8[0]->unk1B4 = 0x14;
                DWC_Printf(0x80, msgs + 0xC20, lbl_805A0FA8[0]->unk1C4);
            }
        } else {
            if (!DWCi_SendCancelMatchSynCommand(msgs, 2, pid)) {
                return 0;
            }
        }
        break;
    case 0xF:
        if (lbl_805A0FA8[0]->unk1B4 == 7) {
            fn_80348ACC(2);
        }
        break;
    }
    return 1;
}

void ServerBrowserClear(void* sb);
int ServerBrowserLimitUpdateA(void* sb, int async, int disconnectOnComplete, u8* basicFields, int numBasicFields, const char* serverFilter, int maxServers);

static inline int DWCi_CalcLatencyLevelFromMs(s32 ms, int overrideLv) {
    int lv;
    if (overrideLv != -1) {
        return overrideLv;
    }
    lv = (ms + 4999) / 5000;
    if (lv > 10) {
        lv = 10;
    }
    return lv;
}

int fn_80342BA8(int pid) {
    const char* msgs = lbl_8048AA60;
    char filter[0x100];
    u8 keys[0xD0];
    int keyCount = 11;
    int async = 1;
    int err;
    keys[0] = 8;
    keys[1] = 0xA;
    keys[2] = 0x32;
    keys[3] = 0x33;
    keys[4] = 0x34;
    keys[5] = 0x35;
    keys[6] = 0x36;
    keys[7] = 0x37;
    keys[8] = 0x38;
    keys[9] = 0x39;
    keys[10] = 0x3A;
    if (lbl_805A0FA8[0]->unk16 == 0 || lbl_805A0FA8[0]->unk16 == 1) {
        DWCMatchEntry* entry = lbl_805338B0;
        u8* pKey = &keys[11];
        int i;
        for (i = 0; i < 0x9A; i++, entry++) {
            if (entry->unk0 != 0) {
                *pKey++ = entry->unk0;
                keyCount++;
            }
        }
    }
    switch (lbl_805A0FA8[0]->unk1B4) {
    case 2: {
        u32 targetPid = (u32)lbl_805A0FA8[0]->unk220;
        if (targetPid == 0) {
            DWCMatchControl* c = lbl_805A0FA8[0];
            u8 numEntry = c->unk18;
            snprintf(filter, 0x100, msgs + 0x108, msgs + 0x160, 0xB, "dwc_pid", c->unk218, numEntry, numEntry, msgs + 0x16C, c->unk16, msgs + 0x178, "dwc_pid");
            if (lbl_805A0F94 == 1) {
                s32 ms = (s32)(lbl_805A0FA8[0]->unk4C0 / (s64)(*(u32*)0x800000F8 / 4 / 1000));
                int overrideLv = lbl_8059F0B0;
                int minLv;
                int maxLv;
                if (DWCi_CalcLatencyLevelFromMs(ms, overrideLv) == 0) {
                    minLv = 0;
                    maxLv = 0;
                } else {
                    minLv = DWCi_CalcLatencyLevelFromMs(ms, overrideLv) - 1;
                    maxLv = DWCi_CalcLatencyLevelFromMs(ms, overrideLv) + 1;
                    if (minLv < 1) {
                        minLv = 1;
                    }
                    if (maxLv > 10) {
                        maxLv = 10;
                    }
                }
                snprintf(filter, 0x100, msgs + 0xD6C, filter, minLv, msgs + 0xD18, msgs + 0xD18, maxLv);
            }
            if (lbl_805A0FA8[0]->unk16 == 0) {
                int val = 0;
                if (lbl_805A0FA8[0]->unk4B0 != 0) {
                    val = 2;
                }
                snprintf(filter, 0x100, msgs + 0xD8C, filter, msgs + 0xD24, val);
                lbl_805A0FA8[0]->unk4B0 ^= 1;
            }
            snprintf(filter, 0x100, msgs + 0xD8C, filter, msgs + 0xD34, 0);
            if (lbl_805A0F90 != 0) {
                snprintf(filter, 0x100, msgs + 0xDA0, filter, lbl_805A0F90);
            }
        } else {
            pid = targetPid;
            goto pid_filter;
        }
        break;
    }
    case 3:
    case 4:
    pid_filter:
        snprintf(filter, 0x100, "%s = %u", "dwc_pid", pid);
        lbl_805A0FA8[0]->unk21C = pid;
        break;
    case 9:
    case 10:
    case 11:
        snprintf(filter, 0x100, "%s = %u", "dwc_pid", pid);
        async = 0;
        break;
    case 0xC:
        snprintf(filter, 0x100, msgs + 0xDAC, msgs + 0xD0C, lbl_805A0FA8[0]->unk4A0, msgs + 0xD24, 2, "dwc_pid", lbl_805A0FA8[0]->unk218);
        break;
    case 0xD:
        snprintf(filter, 0x100, "%s = %u", "dwc_pid", pid);
        break;
    default:
        DWC_Printf(2, msgs + 0xDD4);
        break;
    }
    DWC_Printf(0x40, msgs + 0xE00, filter);
    ServerBrowserClear(lbl_805A0FA8[0]->unkF8);
    err = ServerBrowserLimitUpdateA(lbl_805A0FA8[0]->unkF8, async, 0, keys, keyCount, filter, 6);
    if (err != 0 && err == 2) {
        DWC_Printf(0x400, "Retry\n");
    }
    if (err == 0) {
        lbl_805A0FA8[0]->unk188 = OSGetTime() + (s64)(*(u32*)0x800000F8 / 4 / 1000) * 30000;
        DWC_Printf(4, msgs + 0xE1C, msgs + 0xD58);
    }
    return err;
}

int fn_80341868(int error, int errorCode) {
    const char* msgs = lbl_8048AA60;
    if (DWC_GetState() != 5) {
        return 0;
    }
    if (lbl_805A0FA8[0]->unk15 == 2) {
        DWC_Printf(4, msgs + 0xC68);
        return 1;
    }
    if (error != 0) {
        DWCi_StopMatchingInlined(error, errorCode - 80000);
        return 1;
    }
    lbl_805A0FA8[0]->aidList[0] = 0;
    if (lbl_805A0FA8[0]->unk1BD == 1 || (lbl_805A0FA8[0]->unk1BC >= 1 && lbl_805A0FA8[0]->unk1BC <= 2)) {
        return 1;
    }
    if (lbl_805A0FA8[0]->unk1B0 != 0) {
        NNCancel(lbl_805A0FA8[0]->unk1B0);
        lbl_805A0FA8[0]->unk1B0 = 0;
    }
    if (lbl_805A0FA8[0]->unkD != 0) {
        if (lbl_805A0FA8[0]->unk1BC == 0) {
            lbl_805A0FA8[0]->unk1BC = 3;
            gt2CloseAllConnectionsHard(*lbl_805A0FA8[0]->gt2Holder);
        }
    } else {
        if (lbl_805A0FA8[0]->unk15 == 3) {
            DWCi_StopMatchingInlined(6, -80430);
        } else if (lbl_805A0FA8[0]->unk220 != 0) {
            DWC_Printf(0x40, msgs + 0xC8C);
            DWCi_ResumeMatching();
        } else if (lbl_805A0FA8[0]->unk1B4 == 1) {
            DWC_Printf(0x40, msgs + 0xCA0);
            lbl_805A0FA8[0]->unk1B4 = 0x17;
            lbl_805A0FA8[0]->unk208 = OSGetTime();
        } else {
            DWC_Printf(0x40, msgs + 0xCB8);
            fn_80348ACC(1);
        }
    }
    return 1;
}

void fn_8033CF1C(u8 numEntry, const char* addFilter, int matchedCb, int matchedParam, void* a4, void* a5, u32 a6, u32 a7, void* a8, const void* a9, void* a10) {
    const char* msgs = lbl_8048AA60;
    if (lbl_805A0F90 != 0) {
        DWC_Free(4, lbl_805A0F90, 0);
        lbl_805A0F90 = 0;
    }
    if (addFilter != 0) {
        char buf[0x100];
        int len = snprintf(buf, 0x100, msgs + 0x108, msgs + 0x160, 0xB, "dwc_pid", -1, 0x20, 0x20, msgs + 0x16C, 3, msgs + 0x178, "dwc_pid");
        int allocSize = (0x100 - len) - strlen(" and ()");
        lbl_805A0F90 = DWC_Alloc(4, allocSize);
        if (lbl_805A0F90 == 0) {
            DWCi_StopMatchingInlined(9, -80001);
            return;
        }
        DWCi_Np_CPUCopyFast(lbl_805A0F90, addFilter, allocSize);
        ((char*)lbl_805A0F90)[allocSize - 1] = 0;
    }
    fn_8034296C(0, numEntry, matchedCb, matchedParam);
    lbl_805A0FA8[0]->unk488 = a4;
    lbl_805A0FA8[0]->unk48C = a5;
    lbl_805A0FA8[0]->unk490 = a6;
    lbl_805A0FA8[0]->unk494 = a7;
    lbl_805A0FA8[0]->unk4C8 = a8;
    if (a9 != 0) {
        memcpy(lbl_805A0FA8[0]->unk4CC, a9, 4);
    } else {
        memset(lbl_805A0FA8[0]->unk4CC, 0, 4);
    }
    lbl_805A0FA8[0]->unk554 = a10;
    {
        DWCMatchControl* control = lbl_805A0FA8[0];
        if (control->unkF8 == 0) {
            u32 gamename = control->unk30C;
            control->unkF8 = ServerBrowserNewA(gamename, gamename, control->unk310, 0, 0x14, 1, 0, (void*)fn_8034AA48, 0);
        }
    }
    if (lbl_805A0FA8[0]->unkF8 == 0) {
        if (fn_80349F28(5) != 0) {
            return;
        }
    }
    DWCi_GPSetServerStatus(3);
    {
        DWCMatchControl* c = lbl_805A0FA8[0];
        if (c->unk30 != 0 && c->unk2E != 0) {
            if (!DWCi_StartMatching()) {
                return;
            }
        } else {
            c->unk1B4 = 0xE;
            lbl_805A0FA8[0]->unk210 = OSGetTime();
        }
    }
    if (lbl_805A0FA8[0]->qr2Obj == 0) {
        DWCi_QR2Startup(lbl_805A0FA8[0]->unk218);
    }
}

void DWCi_DoCancelMatching(void) {
    DWC_Printf(4, "CANCEL! state %d, numHost nn=%d gt2=%d.\n", lbl_805A0FA8[0]->unk1B4, lbl_805A0FA8[0]->_14, lbl_805A0FA8[0]->unkD);
    lbl_805A0FA8[0]->unk224 = 0;
    lbl_805A0FA8[0]->unk1BD = 1;
    if (lbl_805A0FA8[0]->unk15 == 3) {
        lbl_805A0FA8[0]->unk1BD = 1;
        if (lbl_805A0FA8[0]->unkD != 0) {
            gt2CloseAllConnectionsHard(*lbl_805A0FA8[0]->gt2Holder);
        } else {
            int res = fn_80343500(5, lbl_805A0FA8[0]->profiles[0], lbl_805A0FA8[0]->unk38[0], lbl_805A0FA8[0]->unkB8[0], 0, 0);
            lbl_805A0FA8[0]->unk21C = 0;
            if (lbl_805A0FA8[0]->unk16 == 0) {
                res = fn_80349F28(res);
            } else {
                res = fn_80349C88(res);
            }
            if (res != 0) {
                return;
            }
        }
        if (fn_80349C88(DWCi_GPSetServerStatus(1)) != 0) {
            return;
        }
        DWCi_CloseMatching();
        {
            DWCMatchControl* control = lbl_805A0FA8[0];
            u32 pid = control->unk224;
            int isServer = (pid != 0) ? 1 : (control->unk15 == 2);
            int isSelf = (pid == 0);
            ((void (*)(int, int, int, int, int, int))control->unk480)(
                0, 1, isSelf, isServer,
                DWCi_GetFriendListIndex(control->unk224), control->unk484);
        }
        lbl_805A0FA8[0]->unk1BD = 0;
        return;
    }
    {
        int state = lbl_805A0FA8[0]->unk1B4;
        if (state == 3 || state == 4 || state == 5 || state == 6 || state == 0x10 || state == 0x11) {
            if (lbl_805A0FA8[0]->unk21C != 0) {
                int res = fn_80343500(5, lbl_805A0FA8[0]->profiles[0], lbl_805A0FA8[0]->unk38[0], lbl_805A0FA8[0]->unkB8[0], 0, 0);
                lbl_805A0FA8[0]->unk21C = 0;
                if (lbl_805A0FA8[0]->unk16 == 0) {
                    res = fn_80349F28(res);
                } else {
                    res = fn_80349C88(res);
                }
                if (res != 0) {
                    return;
                }
            }
            {
                u32 data = 0;
                if (lbl_805A0FA8[0]->unk1A != 0 && lbl_805A0FA8[0]->unk34 != 0 && (int)lbl_805A0FA8[0]->unk34 != (int)lbl_805A0FA8[0]->unk218) {
                    int idx = lbl_805A0FA8[0]->unkD + 1;
                    int res = fn_80343500(0xC, lbl_805A0FA8[0]->unk34, lbl_805A0FA8[0]->unk38[idx], lbl_805A0FA8[0]->unkB8[idx], &data, 1);
                    lbl_805A0FA8[0]->unk1A = 0;
                    lbl_805A0FA8[0]->unk34 = 0;
                    if (lbl_805A0FA8[0]->unk16 == 0) {
                        res = fn_80349F28(res);
                    } else {
                        res = fn_80349C88(res);
                    }
                    if (res != 0) {
                        return;
                    }
                }
            }
        }
    }
    if (lbl_805A0FA8[0]->unk1B0 != 0) {
        NNCancel(lbl_805A0FA8[0]->unk1B0);
        lbl_805A0FA8[0]->unk1B0 = 0;
    }
    if (lbl_805A0FA8[0]->unkD != 0) {
        lbl_805A0FA8[0]->unk1BD = 1;
        gt2CloseAllConnectionsHard(*lbl_805A0FA8[0]->gt2Holder);
    }
    if (fn_80349C88(DWCi_GPSetServerStatus(1)) != 0) {
        return;
    }
    DWCi_CloseMatching();
    {
        DWCMatchControl* control = lbl_805A0FA8[0];
        u32 pid = control->unk224;
        int isServer = (pid != 0) ? 1 : (control->unk15 == 2);
        int isSelf = (pid == 0);
        ((void (*)(int, int, int, int, int, int))control->unk480)(
            0, 1, isSelf, isServer,
            DWCi_GetFriendListIndex(control->unk224), control->unk484);
    }
    lbl_805A0FA8[0]->unk1BD = 0;
}

int gt2Connect(void* socket, void* conn, const char* remoteAddr, const char* msg, int msgLen, int timeout, u32 callbacks, int blocking);
int DWCi_GT2GetConnectionListIdx(void);
void** DWCi_GetGT2ConnectionByIdx(int idx);
u8* DWCi_GetConnectionInfoByIdx(int idx);
void gt2SetConnectionData(void* conn, void* data);
void DWCi_ResetLastSomeDataRecvTimeByAid(u8 aid);

void fn_80340A48(void* conn, int result, const char* msg, int msgLen) {
    const char* msgs = lbl_8048AA60;
    if (lbl_805A0FA8[0] == 0 || (lbl_805A0FA8[0]->unk1B4 != 6 && lbl_805A0FA8[0]->unk1B4 != 0x11)) {
        DWC_Printf(0x80, msgs + 0xA58);
        return;
    }
    if (result != 0) {
        if (msg == 0) {
            msg = "";
        }
        DWC_Printf(0x80, msgs + 0xA80, result, msg);
        if (result == 5) {
            return;
        }
        if (result == 6) {
            lbl_805A0FA8[0]->_0C++;
            if (lbl_805A0FA8[0]->_0C > 5) {
                DWCMatchControl* c;
                int level;
                u32 pid;
                DWC_Printf(0x80, msgs + 0xA9C);
                lbl_805A0FA8[0]->_0C = 0;
                c = lbl_805A0FA8[0];
                level = 2;
                pid = c->profiles[c->_14];
                if ((c->unk16 == 0 || c->unk16 == 1) && c->unkF == 0) {
                    level = 1;
                }
                fn_80347390(pid, level);
            } else {
                char initMsg[24];
                DWCMatchControl* c;
                int err;
                DWC_Printf(0x80, msgs + 0xAB8);
                if (lbl_805A0FA8[0]->unk1B4 == 0x11) {
                    int llv = DWCi_GetLatencyLevel();
                    snprintf(initMsg, 0x18, msgs + 0xAD0, lbl_805A0FA8[0]->unk218, llv, lbl_805A0FA8[0]->unkE + 1, lbl_805A0FA8[0]->unk558);
                    DWC_Printf(0x200004, msgs + 0xAE0, initMsg);
                } else {
                    DWC_Printf(0x200004, msgs + 0xAF8);
                    snprintf(initMsg, 0x18, "%u", lbl_805A0FA8[0]->unk218);
                }
                c = lbl_805A0FA8[0];
                err = gt2Connect(*c->gt2Holder, 0, gt2AddressToString(c->unk228[c->_14], c->unk2A8[c->_14], 0), initMsg, -1, 5000, c->unk8, 0);
                if (err == 1) {
                    fn_8034A790(err);
                } else if (err != 0) {
                    DWCMatchControl* c2 = lbl_805A0FA8[0];
                    int level = 2;
                    u32 pid = c2->profiles[c2->_14];
                    if ((c2->unk16 == 0 || c2->unk16 == 1) && c2->unkF == 0) {
                        level = 1;
                    }
                    if (fn_80347390(pid, level) != 0) {
                        return;
                    }
                }
            }
        } else if (strcmp(msg, msgs + 0x990) == 0) {
            DWCMatchControl* c;
            DWC_Printf(0x200004, msgs + 0xB10);
            lbl_805A0FA8[0]->_0C = 0;
            c = lbl_805A0FA8[0];
            if ((c->unk16 == 0 || c->unk16 == 1) && c->unkF == 0) {
                u8 savedIdx;
                DWC_Printf(0x200004, msgs + 0xB20);
                if (lbl_805A0FA8[0]->unk16 == 1) {
                    savedIdx = lbl_805A0FA8[0]->unk1B9;
                }
                fn_80347390(lbl_805A0FA8[0]->profiles[lbl_805A0FA8[0]->_14], 1);
                if (lbl_805A0FA8[0]->unk16 == 1) {
                    lbl_805A0FA8[0]->unk1B9 = savedIdx;
                }
            } else {
                DWCMatchControl* c2;
                int level;
                u32 pid;
                DWC_Printf(0x200004, msgs + 0xB48);
                c2 = lbl_805A0FA8[0];
                level = 2;
                pid = c2->profiles[c2->_14];
                if ((c2->unk16 == 0 || c2->unk16 == 1) && c2->unkF == 0) {
                    level = 1;
                }
                fn_80347390(pid, level);
            }
        } else {
            DWCMatchControl* c = lbl_805A0FA8[0];
            int level = 2;
            u32 pid = c->profiles[c->unkD + 1];
            if ((c->unk16 == 0 || c->unk16 == 1) && c->unkF == 0) {
                level = 1;
            }
            if (fn_80347390(pid, level) != 0) {
                return;
            }
        }
    } else {
        int idx;
        DWC_Printf(0x80, msgs + 0xB58);
        idx = DWCi_GT2GetConnectionListIdx();
        if (idx == -1) {
            DWC_Printf(8, msgs + 0xB68);
            DWCi_StopMatchingInlined(6, -87100);
        } else {
            void** pConn = DWCi_GetGT2ConnectionByIdx(idx);
            u8* info = DWCi_GetConnectionInfoByIdx(idx);
            *pConn = conn;
            lbl_805A0FA8[0]->unkD++;
            info[0] = idx;
            *(u16*)(info + 2) = 0;
            *(u32*)(info + 4) = 0;
            info[1] = lbl_805A0FA8[0]->aidList[lbl_805A0FA8[0]->unkD];
            gt2SetConnectionData(conn, info);
            DWCi_ResetLastSomeDataRecvTimeByAid(info[1]);
            if (lbl_805A0FA8[0]->unk1B4 == 0x11) {
                fn_80347990(0);
            } else {
                fn_80347990(1);
            }
        }
    }
}

static inline int DWCi_CheckSBServerKeys(const char* msgs, void* server) {
    if (SBServerGetIntValueA(server, msgs + 0x1EB8, -1) == -1) {
        return 0;
    }
    if (SBServerGetIntValueA(server, msgs + 0x1EC4, -1) == -1) {
        return 0;
    }
    if (SBServerGetIntValueA(server, msgs + 0x16C, -1) == -1) {
        return 0;
    }
    if (SBServerGetIntValueA(server, msgs + 0x178, -1) == -1 &&
        SBServerGetIntValueA(server, msgs + 0x178, 0) == 0) {
        return 0;
    }
    if (SBServerGetIntValueA(server, msgs + 0x160, -1) == -1) {
        return 0;
    }
    return SBServerGetIntValueA(server, "dwc_pid", 0) != 0;
}

void fn_8034AA48(void* sb, int reason, void* server, void* instance) {
    const char* msgs = lbl_8048AA60;
    DWC_Printf(0x40, msgs + 0x1E60, reason, lbl_805A0FA8[0]->unk1B4);
    lbl_805A0F98++;
    if ((u32)(reason - 4) <= 1) {
        lbl_805A0FA8[0]->unk188 = 0;
        DWC_Printf(4, msgs + 0xBC, msgs + 0x1E50);
    } else {
        lbl_805A0FA8[0]->unk188 = OSGetTime() + (s64)(*(u32*)0x800000F8 / 4 / 1000) * 30000;
        DWC_Printf(4, msgs + 0x1E88, msgs + 0x1E50);
    }
    switch (reason) {
    case 0:
        fn_8034B17C(server);
        break;
    case 4: {
        int i;
        lbl_805A0FA8[0]->unk188 = 0;
        for (i = 0; i < ServerBrowserCount(sb); i++) {
            server = ServerBrowserGetServer(sb, i);
            if (!DWCi_CheckSBServerKeys(msgs, server)) {
                ServerBrowserRemoveServer(sb, server);
                DWC_Printf(0x400, msgs + 0x1ED0, i);
                i--;
            }
        }
        switch (lbl_805A0FA8[0]->unk1B4) {
        case 2:
            fn_8034B494(1);
            fn_8034B67C();
            if (ServerBrowserCount(sb) != 0) {
                int pid = SBServerGetIntValueA(ServerBrowserGetServer(sb, 0), "dwc_pid", 0);
                DWCMatchControl* control;
                int res;
                lbl_805A0FA8[0]->unk15 = 3;
                lbl_805A0FA8[0]->profiles[0] = pid;
                control = lbl_805A0FA8[0];
                if (control->unkF8 == 0) {
                    u32 gamename = control->unk30C;
                    control->unkF8 = ServerBrowserNewA(gamename, gamename, control->unk310, 0, 0x14, 1, 0, (void*)fn_8034AA48, 0);
                }
                if (lbl_805A0FA8[0]->unkF8 == 0) {
                    if (fn_80349F28(5) != 0) {
                        break;
                    }
                }
                lbl_805A0FA8[0]->unk1B4 = 3;
                res = fn_80346B10(lbl_805A0FA8[0]->profiles[0], 0);
                if (lbl_805A0FA8[0]->unk16 == 0) {
                    fn_80349F28(res);
                } else {
                    fn_80349C88(res);
                }
            } else {
                lbl_805A0FA8[0]->unkFC = 2;
                lbl_805A0FA8[0]->unk100 = OSGetTime();
            }
            break;
        case 4: {
            u32 ip = lbl_805A0FA8[0]->unk1C8;
            DWC_Printf(4, msgs + 0x1EE8, ip >> 24, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF, lbl_805A0FA8[0]->unk1C6);
            while (ServerBrowserCount(sb) != 0) {
                void* srv = ServerBrowserGetServer(sb, 0);
                if (SBServerGetPublicInetAddress(srv) == lbl_805A0FA8[0]->unk1C8 &&
                    SBServerGetPublicQueryPort(srv) == lbl_805A0FA8[0]->unk1C6) {
                    break;
                }
                ServerBrowserRemoveServer(sb, srv);
            }
            if (ServerBrowserCount(sb) != 0) {
                int pid = SBServerGetIntValueA(ServerBrowserGetServer(sb, 0), "dwc_pid", 0);
                if (lbl_805A0FA8[0]->unk16 == 1 && pid == lbl_805A0FA8[0]->profiles[0]) {
                    if (fn_8034B494(0) != 0) {
                        if (lbl_805A0FA8[0]->unkD != 0) {
                            int res;
                            for (i = 1; i <= lbl_805A0FA8[0]->unkD; i++) {
                                res = fn_80343500(0xA, lbl_805A0FA8[0]->profiles[i], lbl_805A0FA8[0]->unk38[i], lbl_805A0FA8[0]->unkB8[i], &lbl_805A0FA8[0]->unk360, lbl_805A0FA8[0]->unk360 + 1);
                                if (res != 0) {
                                    goto err_handle;
                                }
                            }
                            lbl_805A0FA8[0]->unk1A = 0;
                            lbl_805A0FA8[0]->unk34 = 0;
                            lbl_805A0FA8[0]->unk1BC = 1;
                            gt2CloseAllConnectionsHard(*lbl_805A0FA8[0]->gt2Holder);
                            lbl_805A0FA8[0]->unk1BC = 0;
                            DWC_Printf(0x40, msgs + 0x1280, lbl_805A0FA8[0]->unk21C);
                            res = 0;
                        err_handle:
                            if (lbl_805A0FA8[0]->unk16 == 0) {
                                res = fn_80349F28(res);
                            } else {
                                res = fn_80349C88(res);
                            }
                            if (res != 0) {
                                break;
                            }
                        }
                    } else {
                        int res = fn_80343500(5, lbl_805A0FA8[0]->profiles[0], lbl_805A0FA8[0]->unk38[0], lbl_805A0FA8[0]->unkB8[0], 0, 0);
                        lbl_805A0FA8[0]->unk21C = 0;
                        if (lbl_805A0FA8[0]->unk16 == 0) {
                            res = fn_80349F28(res);
                        } else {
                            res = fn_80349C88(res);
                        }
                        if (res != 0) {
                            break;
                        }
                        lbl_805A0FA8[0]->unk1B4 = 3;
                        res = fn_80346D80(0, 0, lbl_805A0FA8[0]->profiles[0]);
                        if (lbl_805A0FA8[0]->unk16 == 0) {
                            fn_80349F28(res);
                        } else {
                            fn_80349C88(res);
                        }
                        break;
                    }
                }
                lbl_805A0FA8[0]->unk1B4 = 5;
                fn_8034A500(fn_80343118(0, 0, ServerBrowserGetServer(sb, 0)));
            } else {
                lbl_805A0FA8[0]->unkFC = 2;
                lbl_805A0FA8[0]->unk100 = OSGetTime();
            }
            break;
        }
        case 0xC: {
            int cnt = ServerBrowserCount(sb);
            if (cnt != 0) {
                DWCMatchControl* c;
                u32 pid;
                DWC_Printf(0x200400, msgs + 0x1F10, cnt);
                c = lbl_805A0FA8[0];
                pid = SBServerGetIntValueA(ServerBrowserGetServer(sb, 0), "dwc_pid", 0);
                fn_8033D7D4(pid, c->unk480, c->unk484, c->unk488, c->unk48C, c->unk4C8, c->unk4CC, c->unk554);
            } else {
                lbl_805A0FA8[0]->unkFC = 2;
                lbl_805A0FA8[0]->unk100 = OSGetTime();
                DWC_Printf(0x200400, msgs + 0x1F34);
            }
            break;
        }
        case 0xD:
            if (ServerBrowserCount(lbl_805A0FA8[0]->unkF8) != 0) {
                int res = fn_80346B10(lbl_805A0FA8[0]->profiles[0], 0);
                if (lbl_805A0FA8[0]->unk16 == 0) {
                    res = fn_80349F28(res);
                } else {
                    res = fn_80349C88(res);
                }
                if (res == 0) {
                    lbl_805A0FA8[0]->unk1B4 = 3;
                }
            } else {
                fn_80342BA8(lbl_805A0FA8[0]->profiles[0]);
            }
            break;
        }
        break;
    }
    }
    lbl_805A0F98--;
}

void gt2Reject(void* conn, const char* msg, int len);
int gt2Accept(void* conn, u32 callbacks);
u16 SONtoHs(u16 value);

void fn_80340034(void* socket, void* conn, u32 ip, u16 port, int latency, const char* msg, int msgLen) {
    const char* msgs = lbl_8048AA60;
    int pid = 0;
    int llv = -1;
    int numPlayers = -1;
    int threshold = -1;
    int idx;
    if (lbl_805A0FA8[0] != 0 && lbl_805A0FA8[0]->unk1B4 == 5) {
        DWC_Printf(0x80, msgs + 0x674);
        return;
    }
    if (lbl_805A0FA8[0] == 0 || lbl_805A0FA8[0]->unk1B4 != 6 || lbl_805A0FA8[0]->unk1BD != 0) {
        gt2Reject(conn, msgs + 0x6B0, -1);
        DWC_Printf(0x80, msgs + 0x6BC);
        return;
    }
    if (lbl_8059F0B4 != -1) {
        DWC_Printf(0x200000, msgs + 0x6E0, latency, lbl_8059F0B4);
        latency = lbl_8059F0B4;
    }
    DWC_Printf(0x200000, msgs + 0x710, latency, msg);
    if (msg != 0 && msgLen != 0) {
        char* endptr;
        pid = strtoul(msg, &endptr, 10);
        if (*endptr == ':') {
            const char* msgEnd = msg + msgLen - 1;
            endptr++;
            if (endptr < msgEnd) {
                llv = strtoul(endptr, &endptr, 10);
            } else {
                gt2Reject(conn, msgs + 0x748, -1);
                DWC_Printf(0x80, msgs + 0x75C);
                return;
            }
            endptr++;
            if (endptr < msgEnd && endptr[-1] == ':') {
                numPlayers = strtoul(endptr, &endptr, 10);
            } else {
                gt2Reject(conn, msgs + 0x748, -1);
                DWC_Printf(0x80, msgs + 0x78C);
                return;
            }
            endptr++;
            if (endptr < msgEnd && endptr[-1] == ':') {
                threshold = strtoul(endptr, &endptr, 10);
            } else {
                gt2Reject(conn, msgs + 0x748, -1);
                DWC_Printf(0x80, msgs + 0x7BC);
                return;
            }
        }
    }
    DWC_Printf(0x200000, msgs + 0x7EC, pid);
    if (llv != -1) {
        DWC_Printf(0x200000, msgs + 0x814, llv);
        DWC_Printf(0x200000, msgs + 0x83C, numPlayers);
        DWC_Printf(0x200000, msgs + 0x864, threshold);
        DWC_Printf(0x200000, msgs + 0x88C, lbl_805A0FA8[0]->unk558);
    } else {
        DWC_Printf(0x200000, msgs + 0x8B4);
    }
    idx = DWCi_GT2GetConnectionListIdx();
    if (idx == -1) {
        gt2Reject(conn, msgs + 0x8E4, -1);
        DWC_Printf(0x80, msgs + 0x8F0);
        DWCi_StopMatchingInlined(6, -87100);
        return;
    }
    {
        DWCMatchControl* c = lbl_805A0FA8[0];
        if (ip != c->unk228[c->unkD] || SONtoHs(port) != c->unk2A8[c->unkD]) {
            if (pid == lbl_805A0FA8[0]->profiles[lbl_805A0FA8[0]->unkD]) {
                DWCMatchControl* c2;
                DWC_Printf(8, msgs + 0x914);
                lbl_805A0FA8[0]->unk228[lbl_805A0FA8[0]->unkD] = ip;
                c2 = lbl_805A0FA8[0];
                c2->unk2A8[c2->unkD] = SONtoHs(port);
            } else {
                gt2Reject(conn, msgs + 0x940, -1);
                DWC_Printf(8, msgs + 0x958, gt2AddressToString(ip, port, 0));
                return;
            }
        }
    }
    if (llv != -1 && !fn_8033FE64(latency, llv, numPlayers, threshold)) {
        gt2Reject(conn, msgs + 0x990, -1);
        DWC_Printf(0x200000, msgs + 0x9A8, gt2AddressToString(ip, port, 0));
        if (lbl_805A0FA8[0]->unk4A5 == 2) {
            lbl_805A0FA8[0]->unk4A5 = 0;
            DWC_Printf(0x200000, "retry.\n");
        } else if (lbl_805A0FA8[0]->unk4A5 == 1) {
            DWC_Printf(0x200000, "stop.\n");
        }
        if (*(s64*)&lbl_805A0FA8[0]->unk1A0 != 0) {
            DWC_Printf(0x200000, msgs + 0x9EC);
            lbl_805A0FA8[0]->unk1A4 = 0;
            lbl_805A0FA8[0]->unk1A0 = 0;
            if (fn_80347604(lbl_805A0FA8[0]->profiles[0], 0x12, -80920) != 0) {
                return;
            }
        } else if (lbl_805A0FA8[0]->unk3E8 == 6) {
            DWC_Printf(0x200000, msgs + 0x9F8);
            lbl_805A0FA8[0]->unk3E8 = 0xFF;
            lbl_805A0FA8[0]->unk3E9 = 0;
            DWC_Printf(0x40, msgs + 0x274, 6);
            if (!fn_80347604(lbl_805A0FA8[0]->profiles[0], 0x12, -80920)) {
                return;
            }
        }
        return;
    }
    lbl_805A0FA8[0]->unk1A4 = 0;
    lbl_805A0FA8[0]->unk1A0 = 0;
    if (!gt2Accept(conn, lbl_805A0FA8[0]->unk8)) {
        DWC_Printf(8, msgs + 0xA08);
        DWCi_StopMatchingInlined(6, -80410);
        return;
    }
    DWC_Printf(0x80, msgs + 0xA2C, gt2AddressToString(ip, port, 0), latency);
    lbl_805A0FA8[0]->unk3E8 = 0xFF;
    lbl_805A0FA8[0]->unk3E9 = 0;
    if (lbl_805A0FA8[0]->unkD == 0) {
        int halfLat = latency >> 1;
        lbl_805A0FA8[0]->unk1C2 = (halfLat >= 0xFFFF) ? 0xFFFF : halfLat;
        if (lbl_805A0FA8[0]->unk55C == 0) {
            lbl_805A0FA8[0]->unk55C = latency;
        }
    }
    {
        void** pConn = DWCi_GetGT2ConnectionByIdx(idx);
        u8* info = DWCi_GetConnectionInfoByIdx(idx);
        *pConn = conn;
        lbl_805A0FA8[0]->unkD++;
        info[0] = idx;
        info[1] = lbl_805A0FA8[0]->aidList[lbl_805A0FA8[0]->unkD - 1];
        *(u16*)(info + 2) = 0;
        *(u32*)(info + 4) = 0;
        DWCi_ResetLastSomeDataRecvTimeByAid(info[1]);
        gt2SetConnectionData(conn, info);
    }
    fn_80347990(2);
}
