#include <RVLFaceLib/RVLFaceLibInternal.h>
#include <revolution/MEM.h>
#include <string.h>

// TODO: clean up, give variables real names
extern unsigned char lbl_80492F50[0xD];

unsigned char* fn_803CD478(void) {
    return lbl_80492F50;
}

// Neighbouring RFL_System code in the original; the state lives in .sbss of this unit.
RFLiManager* sRFLiManager;
RFLErrcode sRFLLastErrCode;
u8 sRFLiFileBrokenType;
s32 sRFLLastReason;

#define RFLiGetLastReason_() (RFLAvailable() ? RFLiGetManager()->lastReason : NAND_RESULT_OK)

BOOL RFLAvailable(void) {
    return sRFLiManager != NULL;
}

static void* allocal_(u32 size, s32 align) {
    return MEMAllocFromExpHeapEx(RFLiGetManager()->tmpHeap, size, align);
}

void* RFLiAlloc32(u32 size) {
    return allocal_(size, 32);
}

void RFLiFree(void* block) {
    MEMFreeToExpHeap(RFLiGetManager()->tmpHeap, block);
}

RFLiDBManager* RFLiGetDBManager(void) {
    return !RFLAvailable() ? NULL : &RFLiGetManager()->dbMgr;
}

RFLiHDBManager* RFLiGetHDBManager(void) {
    return !RFLAvailable() ? NULL : &RFLiGetManager()->hdbMgr;
}

RFLiLoader* RFLiGetLoader(void) {
    return !RFLAvailable() ? NULL : &RFLiGetManager()->loader;
}

BOOL RFLiGetWorking(void) {
    return !RFLAvailable() ? NULL : RFLiGetManager()->working;
}

void RFLiSetWorking(BOOL working) {
    RFLiGetManager()->working = working;
}

RFLiManager* RFLiGetManager(void) {
    return sRFLiManager;
}

RFLErrcode RFLGetAsyncStatus(void) {
    if (!RFLAvailable()) {
        return sRFLLastErrCode;
    }

    if (RFLiIsWorking()) {
        return RFLErrcode_Busy;
    }

    if (RFLiCriticalError()) {
        return RFLErrcode_Fatal;
    }

    return RFLiGetManager()->lastErrCode;
}

RFLiAccessInfo* RFLiGetAccInfo(RFLiFileType type) {
    return !RFLAvailable() ? NULL : &RFLiGetManager()->info[type];
}

RFLiCtrlBufManager* RFLiGetCtrlBufManager(void) {
    return !RFLAvailable() ? NULL : &RFLiGetManager()->ctrlMgr;
}

s32 RFLiGetLastReason(void) {
    return !RFLAvailable() ? NAND_RESULT_OK : RFLiGetManager()->lastReason;
}

void RFLiSetFileBroken(RFLiFileBrokenType type) {
    if (RFLAvailable()) {
        RFLiGetManager()->brokenType |= (1 << type) & 0xFF;
    }
}

BOOL RFLiNotFoundError(void) {
    u8* broken = &sRFLiFileBrokenType;

    if (RFLAvailable()) {
        broken = &RFLiGetManager()->brokenType;
    }

    return *broken >> RFLiFileBrokenType_DBNotFound & 1;
}

BOOL RFLiNeedRepairError(void) {
    u8* broken = &sRFLiFileBrokenType;

    if (RFLAvailable()) {
        broken = &RFLiGetManager()->brokenType;
    }

    return *broken >> RFLiFileBrokenType_DBBroken & 1;
}

BOOL RFLiCriticalError(void) {
    u8* broken = &sRFLiFileBrokenType;

    if (RFLAvailable()) {
        broken = &RFLiGetManager()->brokenType;
    }

    return *broken & 1 << RFLiFileBrokenType_ResBroken || *broken & 1 << RFLiFileBrokenType_Corrupt;
}
