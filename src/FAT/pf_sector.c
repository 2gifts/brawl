#include <types.h>

// TODO: clean up, give variables real names
/* jj_803E8DC0 */
typedef struct {
    u16 limit;
    u8 pad0[6];
    u32 count;
    u8 pad1[0x1f8c - 0x0c];
    u16 flags;
} Obj;

typedef struct {
    u8 pad0[8];
    u32 base;
} Sp;

extern s32 PFDRV_IsInserted(void *a);
extern s32 PFCACHE_ReadFATPage(void *a, u32 b, void **c);
extern void VFipf_memcpy(void *a, void *b, s32 c);

s32 PFSEC_ReadFAT(Obj *self, void *buf, u32 idx, u32 offset, s32 len) {
    void *sp8;
    s32 r;

    if (!(self->flags & 1) || PFDRV_IsInserted(self) == 0) {
        return 9;
    }
    if (idx >= self->count || offset >= self->limit || (s32)(offset + len) > (s32)self->limit) {
        return 10;
    }
    r = PFCACHE_ReadFATPage(self, idx, &sp8);
    if (r != 0) {
        return r;
    }
    VFipf_memcpy(buf, (void*)(((Sp *)sp8)->base + offset), len);
    return 0;
}

