#include <types.h>
#include <FAT/pf2.h>

// HYPOTHESIS: layout of the PrFile2 volume fields these functions touch.
struct PF2_VOLUME {
    u8 unk0[0x1648];
    char label[0xC];
    u8 unk1654[0x1F84 - 0x1654];
    s32 last_error;
    u8 unk1F88[4];
    u16 flags;
    u8 unk1F8E[0x1FBC - 0x1F8E];
    void* p_part;
    u8 unk1FC0[8];
};

// HYPOTHESIS: the PrFile2 volume set; 26 volumes follow a 0xA0 byte header.
extern struct PF2_VOLUME_SET {
    u8 unk0[0x44];
    s32 last_error;
    u8 unk48[0xA0 - 0x48];
    PF2_VOLUME volumes[26];
} lbl_805644C0;

// HYPOTHESIS: the BIOS parameter block fields of a PrFile2 volume.
typedef struct PF2_BPB {
    u16 bytes_per_sector;
    u16 reserved_sectors;
    u16 root_entries;
    u8 sectors_per_cluster;
    u8 num_fats;
    u32 total_sectors;
    u32 fat_size;
    u8 unk10[0xC];
    u32 fat_type;
    u8 sector_shift;
    u8 cluster_shift;
    u16 unk22;
    u16 root_dir_sectors;
    u8 unk26[6];
    u32 root_start;
    u32 data_start;
    u32 num_clusters;
} PF2_BPB;

// HYPOTHESIS: the media insert and eject callback pair.
struct PDM_CALLBACKS {
    void (*insert)(u32);
    void (*eject)(u32);
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

int PFDRV_unmount(PF2_VOLUME* p_vol, u32 mode) {
    int err;

    err = pdm_part_unmount(p_vol->p_part, mode == 1);
    if (err != 0) {
        if (err == 0x15) {
            s32 code = pdm_part_get_driver_error_code(p_vol->p_part);
            lbl_805644C0.last_error = code;
            p_vol->last_error = code;
            return 0x1000;
        }
        return -1;
    }
    return 0;
}

void PFDRV_MediaEject(u32 part) {
    int i;

    for (i = 0; i < 26; i++) {
        PF2_VOLUME* p_vol = &lbl_805644C0.volumes[i];
        if ((u32)p_vol->p_part == part) {
            if (p_vol->flags & 8) {
                p_vol->flags |= 0x20;
            }
            p_vol->flags &= ~0xC;
        }
    }
}

int PFDRV_init(PF2_VOLUME* p_vol) {
    PDM_CALLBACKS callbacks;
    int err;

    callbacks.insert = PFDRV_MediaInsert;
    callbacks.eject = PFDRV_MediaEject;
    if (pdm_part_register_callback(p_vol->p_part, &callbacks) != 0) {
        return -1;
    }
    err = pdm_part_init(p_vol->p_part);
    if (err != 0) {
        pdm_part_unregister_callback(p_vol->p_part);
        if (err == 0x15) {
            s32 code = pdm_part_get_driver_error_code(p_vol->p_part);
            lbl_805644C0.last_error = code;
            p_vol->last_error = code;
            return 0x1000;
        }
        return -1;
    }
    return 0;
}

void pdm_bpb_CalcBPBFields(PF2_BPB* p_bpb) {
    u16 bps = p_bpb->bytes_per_sector;
    u16 spc;
    s32 root_sectors;
    u32 data_start;
    u32 clusters;

    for (p_bpb->sector_shift = 0; (bps >>= 1) != 0; p_bpb->sector_shift++) {
    }
    spc = p_bpb->sectors_per_cluster;
    for (p_bpb->cluster_shift = 0; (spc >>= 1) != 0; p_bpb->cluster_shift++) {
    }
    root_sectors = (p_bpb->bytes_per_sector + (p_bpb->root_entries << 5) - 1) >> p_bpb->sector_shift;
    p_bpb->root_dir_sectors = root_sectors;
    data_start = p_bpb->num_fats * p_bpb->fat_size + ((u16)root_sectors + p_bpb->reserved_sectors);
    p_bpb->data_start = data_start;
    clusters = (p_bpb->total_sectors - data_start) >> p_bpb->cluster_shift;
    p_bpb->num_clusters = clusters;
    if (clusters < 0xFF5) {
        p_bpb->fat_type = 0;
        p_bpb->root_start = p_bpb->reserved_sectors + p_bpb->num_fats * p_bpb->fat_size;
    } else if (clusters < 0xFFF5) {
        p_bpb->fat_type = 1;
        p_bpb->root_start = p_bpb->reserved_sectors + p_bpb->num_fats * p_bpb->fat_size;
    } else {
        p_bpb->fat_type = 2;
        p_bpb->root_start = 0;
    }
}

int pdm_bpb_InitVolumeWithFSINFOSector(PF2_BPB* p_bpb, u8* buf, u32* p_free_count, u32* p_next_free) {
    u32* words = (u32*)buf;
    u32 lead_sig;
    u32 struct_sig;
    u32 trail_sig;
    u32 free_count;
    u32 next_free;
    u32 raw;

    *p_free_count = 0xFFFFFFFF;
    *p_next_free = 0xFFFFFFFF;
    raw = words[0];
    lead_sig = (raw >> 24) | ((raw >> 8) & 0xFF00) | ((raw << 8) & 0xFF0000) | (raw << 24);
    if (lead_sig == 0x41615252) {
        raw = words[0x79];
        struct_sig = (raw >> 24) | ((raw >> 8) & 0xFF00) | ((raw << 8) & 0xFF0000) | (raw << 24);
        if (struct_sig == 0x61417272) {
            raw = words[0x7F];
            trail_sig = (raw >> 24) | ((raw >> 8) & 0xFF00) | ((raw << 8) & 0xFF0000) | (raw << 24);
            if (trail_sig == 0xAA550000) {
                raw = words[0x7A];
                free_count = (raw >> 24) | ((raw >> 8) & 0xFF00) | ((raw << 8) & 0xFF0000) | (raw << 24);
                raw = words[0x7B];
                next_free = (raw >> 24) | ((raw >> 8) & 0xFF00) | ((raw << 8) & 0xFF0000) | (raw << 24);
                if (free_count >= 2 && free_count < p_bpb->num_clusters + 2) {
                    *p_free_count = free_count;
                }
                if (next_free >= 2 && next_free < p_bpb->num_clusters + 2) {
                    *p_next_free = next_free;
                }
            }
        }
    }
    return 0;
}
