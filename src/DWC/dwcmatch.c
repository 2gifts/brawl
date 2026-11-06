#include <types.h>

void* memset(void*, int, u32);
void DWC_Printf(u32 level, const char* format, ...);

// HYPOTHESIS: layout of the match control block, only the fields these functions touch.
typedef struct DWCMatchControl {
    void* gpConnection;
    void** gt2Holder; // HYPOTHESIS: points at a struct whose first word is the GT2 socket
    u8 _08[5];
    u8 unkD;
    u8 unkE;
    u8 unkF;
    void* qr2Obj;
    u8 _14;
    u8 unk15;
    u8 unk16;
    u8 unk17;
    u8 unk18;
    u8 unk19;
    u8 unk1A;
    u8 _1B;
    u32 unk1C;
    u32 unk20;
    u32 unk24;
    int unk28;
    u8 unk2C;
    u8 _2D;
    u16 unk2E;
    u32 unk30;
    u32 unk34;
    u8 _38[0xC0];
    void* unkF8;
    u8 _FC[0xC];
    int unk108;
    u8 _10C[0x84];
    u8 unk190;
    u8 unk191;
    u8 _192[2];
    int unk194;
    int unk198;
    int unk19C;
    u8 _1A0[0x14];
    int unk1B4;
    u8 _1B8[2];
    u8 unk1BA;
    u8 _1BB;
    u8 unk1BC;
    u8 unk1BD;
    u8 _1BE[4];
    u16 unk1C2;
    u8 _1C4[0x34];
    s64 unk1F8;
    u8 _200[0x18];
    u32 unk218;
    int unk21C;
    void* unk220;
    u8 _224[0xC4];
    u8 aidList[1];
    u8 _2E9[0x1F];
    u32 unk308;
    u32 unk30C;
    u32 unk310;
    u8 _314[0xD4];
    u8 unk3E8;
    u8 _3E9[0x97];
    int unk480;
    int unk484;
    u8 _488[0x1D];
    u8 unk4A5;
    u8 _4A6[0xA];
    u8 unk4B0;
    u8 _4B1[0xF];
    s64 unk4C0;
    u8 _4C8[0x90];
    u32 unk558;
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
    u8 _1[3];
    void* unk4;
    int unk8;
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

void DWCi_SendSBMsgCommand(u8 command, u32 ip, u16 port, u32* data, int count) {
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
            return;
        }
    }
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
            result = DWCi_HandleGPError(result);
        }
        if (result != 0) {
            return 0;
        }
        return 1;
    } else {
        if (gpIsBuddy(lbl_805A0FA8[0]->gpConnection, lbl_805A0FA8[0]->unk108)) {
            lbl_805A0FA8[0]->unk1B4 = 3;
            result = DWCi_SendResvCommand((void*)lbl_805A0FA8[0]->unk108, 0);
            if (lbl_805A0FA8[0]->unk16 == 0) {
                result = DWCi_HandleSBError(result);
            } else {
                result = DWCi_HandleGPError(result);
            }
            if (result != 0) {
                return 0;
            }
        } else {
            lbl_805A0FA8[0]->unk1B4 = 0xD;
            DWCi_SBUpdateAsync(lbl_805A0FA8[0]->unk108);
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

void fn_8034C4DC(void) {
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

void fn_8034C4E0(void) {
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

int fn_8033CAC0(int a, int b) {
    if (b < a) {
        return 0;
    }
    lbl_8059F0B8 = a;
    lbl_8059F0BC = b;
    return 1;
}

void fn_8033CAE0(int* a, int* b) {
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
void DWCi_SetGPStatus(int status, const char* statusString, const char* locationString);

void DWCi_GPSetServerStatus(int status) {
    char value[12];
    char buffer[0x2F];
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
    DWCi_SetGPStatus(status, buffer, 0);
}


void fn_8034DE58(void);

void fn_8033CB04(void) {
    fn_8034DE58();
}

void fn_8034C6E0(int cookie) {
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
