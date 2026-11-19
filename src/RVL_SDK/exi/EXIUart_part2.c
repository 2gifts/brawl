#include "revolution.h"

// TODO: clean up, give variables real names
/* jj_801E5DBC */
extern s32 EXIImm(s32, void *, s32, s32, s32);
extern s32 EXISync(s32);
extern s32 EXISelect(s32, s32, s32);
extern s32 EXIDeselect(s32);
extern s32 EXILock(s32, s32, s32);
extern s32 EXIUnlock(s32);

s32 EXIWriteReg(s32 arg0, s32 arg1, s32 arg2, u8 *arg3, s32 arg4) {
    u32 spC;
    s32 sp8;
    s32 err;
    u32 v;

    sp8 = arg2;
    switch (arg4) {
    case 1:
        spC = *arg3 << 24;
        break;
    case 2:
        v = *(u16 *)arg3;
        spC = (v << 24) | ((v << 8) & 0xFF0000);
        break;
    default:
        v = *(u32 *)arg3;
        spC = ((v << 24) | ((v << 8) & 0xFF0000)) | ((v >> 24) | ((v >> 8) & 0xFF00));
        break;
    }

    err = EXILock(arg0, arg1, 0) == 0;
    if (err) {
        return 0;
    }
    err |= EXISelect(arg0, arg1, 4) == 0;
    if (err) {
        EXIUnlock(arg0);
        return 0;
    }
    err |= EXIImm(arg0, &sp8, 4, 1, 0) == 0;
    err |= EXISync(arg0) == 0;
    err |= EXIImm(arg0, &spC, 4, 1, 0) == 0;
    err |= EXISync(arg0) == 0;
    err |= EXIDeselect(arg0) == 0;
    err |= EXIUnlock(arg0) == 0;
    return !err;
}

