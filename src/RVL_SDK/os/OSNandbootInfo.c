#include "revolution/os/OSNandbootinfo.h"
#include "revolution/nand.h"

static u32 CheckSum(OSNandbootInfo* info) {
    u32 *ptr, i, sum;
    
    ptr = (u32*)&(info->ArgOffset);
    sum = 0;
    for (i = 0; i < (sizeof(OSNandbootInfo) - 4) / 4; i++)
    {
        sum = sum + *ptr;
        ptr++;
    }
    
    return sum;
}

BOOL __OSCreateNandbootInfo(void) {
    NANDStatus status;
    s32 result;

    result = NANDPrivateGetStatus("/shared2/sys/NANDBOOTINFO", &status);
    if (result == 0 && status.permission == 0x3F) {
        return TRUE;
    }
    else if (result == 0 && status.permission != 0x3F) {
        result = NANDPrivateDelete("/shared2/sys/NANDBOOTINFO");

        if (result != 0) {
            return FALSE;
        }
    }
    else if (result == -12) {
        
    }
    else {
        return FALSE;
    }

    result = NANDPrivateCreate("/shared2/sys/NANDBOOTINFO", 63, 0);

    if (result != 0) {
        return FALSE;
    }

    return TRUE;
}

BOOL __OSWriteNandbootInfo(OSNandbootInfo *info) {
    NANDFileInfo fileInfo;
    s32 result;
    info->CheckSum = CheckSum(info);
    result = NANDPrivateOpen("/shared2/sys/NANDBOOTINFO", &fileInfo, 2);
    
    if (result == 0) {
        result = NANDWrite(&fileInfo, info, sizeof(OSNandbootInfo));

        if (result != sizeof(OSNandbootInfo)) {
            NANDClose(&fileInfo);
            return FALSE;
        }

        result = NANDClose(&fileInfo);
        if (result != 0) {
            return FALSE;
        }
    }
    else {
        return FALSE;
    }

    return TRUE;
}

// TODO: clean up, give variables real names
/* jj_801E3844 */
s32 __OSGetNandbootInfoHeader(void* out);

typedef struct {
    u8 pad[0x18];
    s32 hi;
    s32 lo;
} Out;

s64 fn_801E3844(void) {
    Out sp8;
    s32 var_r31 = 0;
    s32 var_r30 = 0;
    if (*(u8*)0x80003186 != 0 && __OSGetNandbootInfoHeader(&sp8) != 0) {
        var_r31 = sp8.hi;
        var_r30 = sp8.lo;
    }
    return ((s64)var_r31 << 32) | (u32)var_r30;
}

