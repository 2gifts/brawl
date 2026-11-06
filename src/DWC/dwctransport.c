#include <types.h>

// HYPOTHESIS: Brawl's DWC transport connection layout, stride 0x48, sendState at 0x1C.
typedef struct DWCTransportConnectionView
{
    u8* sendBuffer;
    u8* recvBuffer;
    int recvBufferSize;
    int sendingSize;
    int recvingSize;
    int requestSendSize;
    int requestRecvSize;
    u8 sendState;
    u8 recvState;
    u8 lastRecvState;
    u8 pads[3];
    u16 lastRecvType;
    u8 unk24[0x1C];
    s64 lastRecvTick;
} DWCTransportConnectionView;

// HYPOTHESIS: Brawl's DWC transport info, 32 connections followed by callbacks.
typedef struct DWCTransportInfoView
{
    DWCTransportConnectionView connections[32];
    void* sendCallback;
    void* recvCallback;
    void* recvTimeoutCallback;
    void (*pingCallback)(int latency, u8 aid);
    u16 sendSplitMax;
} DWCTransportInfoView;

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
DWCTransportInfoView* lbl_805A0FB0[2] __attribute__((aligned(4))) = { 0 };

extern void* memset(void*, int, unsigned long);
extern s64 OSGetTime(void);
extern u8 DWCi_GetConnectionAID(void* connection);
extern void DWC_Printf(unsigned long level, const char* format, ...);

int DWC_SetUserRecvCallback(void* callback)
{
    if (lbl_805A0FB0[0] == 0)
    {
        return 0;
    }
    lbl_805A0FB0[0]->recvCallback = callback;
    return 1;
}

void DWCi_InitTransport(DWCTransportInfoView* info)
{
    lbl_805A0FB0[0] = info;
    memset(info, 0, sizeof(DWCTransportInfoView));
    lbl_805A0FB0[0]->sendSplitMax = 0x5B9;
}

void DWCi_ResetLastSomeDataRecvTimeByAid(u8 aid)
{
    lbl_805A0FB0[0]->connections[aid].lastRecvTick = OSGetTime();
}

void DWCi_PingCallback(void* connection, int latency)
{
    if (lbl_805A0FB0[0]->pingCallback != 0)
    {
        lbl_805A0FB0[0]->pingCallback(latency, DWCi_GetConnectionAID(connection));
    }
}

int DWC_SetRecvBuffer(u8 aid, u8* recvBuffer, int size)
{
    DWCTransportConnectionView* connection = &lbl_805A0FB0[0]->connections[aid];
    if ((int)connection->recvState == 2)
    {
        DWC_Printf(0x10000, "+++ Cannot set recv buffer\n");
        return 0;
    }
    connection->recvBuffer = recvBuffer;
    connection->recvBufferSize = size;
    connection->recvState = 1;
    connection->recvingSize = 0;
    connection->requestRecvSize = 0;
    return 1;
}
