#include <types.h>
#include <FAT/pf2.h>

// HYPOTHESIS: layout of the PrFile2 volume fields these functions touch.
struct PF2_VOLUME {
    u8 unk0[0x1648];
    char label[0xC];
    u8 unk1654[0x1F8C - 0x1654];
    u16 flags;
};

u32 PFDRV_IsInserted(PF2_VOLUME* p_vol) {
    return (p_vol->flags >> 2) & 1;
}

u32 PFDRV_IsMountRequested(PF2_VOLUME* p_vol) {
    return (p_vol->flags >> 4) & 1;
}

u32 PFDRV_IsUnmountRequested(PF2_VOLUME* p_vol) {
    return (p_vol->flags >> 5) & 1;
}

void PFDRV_ClearMountRequested(PF2_VOLUME* p_vol) {
    p_vol->flags &= ~0x10;
}

void PFDRV_ClearUnmountRequested(PF2_VOLUME* p_vol) {
    p_vol->flags &= ~0x20;
}

void PFDRV_LoadVolumeLabelFromBuf(PF2_VOLUME* p_vol, u8* buf) {
    VFipf_memcpy(p_vol->label, buf, 11);
    p_vol->label[11] = 0;
}

// Function bodies below are derived from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own.

int pdm_open_disk(PF2_VOLUME* p_vol, u32* p_result) {
    int err;

    *p_result = 0;
    err = fn_803D79B0(p_vol);
    if (err != 0) {
        return err;
    }
    return 0;
}

int pdm_open_partition(PF2_VOLUME* p_vol, const u8* param, u32* p_result) {
    int err;

    *p_result = 0;
    err = fn_803D8878(p_vol, param);
    if (err != 0) {
        return err;
    }
    return 0;
}
