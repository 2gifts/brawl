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

BOOL __OSGetNandbootInfoHeader(OSNandbootInfoHeader* header) {
    NANDFileInfo file;
    u8 args[0x100] __attribute__((aligned(32)));
    OSNandbootInfoHeader buf __attribute__((aligned(32)));
    u32 *ptr, i, j, sum;

    memset(header, 0, sizeof(OSNandbootInfoHeader));

    if (NANDPrivateOpen("/shared2/sys/NANDBOOTINFO", &file, 1) != 0) {
        return FALSE;
    }

    if (NANDRead(&file, &buf, sizeof(OSNandbootInfoHeader)) != sizeof(OSNandbootInfoHeader)) {
        NANDClose(&file);
        return FALSE;
    }

    ptr = &buf.ArgOffset;
    sum = 0;
    for (i = 0; i < (sizeof(OSNandbootInfoHeader) - 4) / 4; i++) {
        sum += *ptr;
        ptr++;
    }

    for (i = 0; i < 0x1000 / sizeof(args); i++) {
        if (NANDRead(&file, args, sizeof(args)) != (s32)sizeof(args)) {
            NANDClose(&file);
            return FALSE;
        }

        ptr = (u32*)args;
        for (j = 0; j < sizeof(args) / 4; j++) {
            sum += *ptr;
            ptr++;
        }
    }

    if (NANDClose(&file) != 0) {
        return FALSE;
    }

    if (buf.CheckSum == sum) {
        memcpy(header, &buf, sizeof(OSNandbootInfoHeader));
        return TRUE;
    }

    return FALSE;
}

// TODO: clean up - 0x80003186 is the OS globals "nandboot info valid" byte; give it a named macro (OS_NANDBOOT_INFO_VALID)
u64 OSGetLastTitleId(void) {
    OSNandbootInfoHeader header;
    u64 titleId = 0;

    if (*(volatile u8*)0x80003186 != 0 && __OSGetNandbootInfoHeader(&header)) {
        titleId = header.LastTitleId;
    }

    return titleId;
}
