#include <types.h>

// Function bodies below are taken from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
#pragma auto_inline off

extern void fn_803EBBE0(s32 *, s32 *);

u8 PFENT_getcurrentDateTimeForEnt(s16 *arg0, s16 *arg1) {
    s32 date[3];
    s32 time[4];

    fn_803EBBE0(date, time);
    *arg0 = (s16)((date[2] & 0x1F) | ((date[1] << 5) & 0x1E0) | (((date[0] - 0x7BC) << 9) & 0xFE00));
    *arg1 = (s16)((((u32)time[2] >> 1) & 0x1F) | ((time[1] << 5) & 0x7E0) | ((time[0] << 11) & 0xF800));
    return (u8)time[3];
}

int fn_803DE800(void*, void*, void*, void*, int, int);

int PFENT_ITER_GetEntryOfPath(void* iter, void* entry, void* vol, void* str, int unk) {
    return fn_803DE800(iter, entry, vol, str, 0, unk);
}
