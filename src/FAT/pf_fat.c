#include <types.h>
#include <FAT/pf2.h>

// HYPOTHESIS: layout of the second PrFile2 volume as far as the FAT code uses it; the BPB leads
// it as in the VF copy, and the driver callback with its argument sit near the end.
struct PF2_VOLUME {
    PF_BPB bpb;
    u8 unk38[0x1F84 - 0x38];
    void* p_callback_arg;
    u8 unk1F88[0x1FC0 - 0x1F88];
    int (*p_callback)(void*);
};

// HYPOTHESIS: per-FAT-type constants in this unit's .rodata, 0x14 bytes each; only the
// end-of-chain marker at 0x8 is known.
typedef struct PF_FAT_TYPE_INFO {
    u32 unk0;
    u32 unk4;
    u32 end_of_chain;
    u32 unkC;
    u32 unk10;
} PF_FAT_TYPE_INFO;
extern const PF_FAT_TYPE_INFO lbl_8041B0E0[3];

// Function bodies below are derived from https://github.com/jj-link/brawl.

u32 PFFAT_GetSectorSpecified(PF_FFD* p_ffd, u32 index, u32 may_allocate, u32* p_cluster) {
    u32 err = fn_803E12B4(p_ffd, index, may_allocate ? 1 : 0, 0, p_cluster);

    if (err != 0) {
        return err;
    }
    return 0;
}

int PFFAT_GetSectorAllocated(PF_FFD* p_ffd, u32 index, u32 size, u32* p_cluster) {
    int err = fn_803E12B4(p_ffd, index, 2, size, p_cluster);

    if (err != 0) {
        return err;
    }
    return 0;
}

void PFFAT_InitHint(PF_FAT_HINT* p_hint) {
    p_hint->chain_index = 0;
    p_hint->cluster = 0;
    p_hint->file_version = 0;
}

int PFFAT_ResetFFD(PF_FFD* p_ffd, u32* p_start_cluster) {
    p_ffd->p_start_cluster = p_start_cluster;
    p_ffd->file_version = 1;
    if (p_ffd->p_hint != NULL) {
        p_ffd->p_hint->file_version = 0;
    }
    p_ffd->last_access_cluster.cluster = 0;
    p_ffd->last_access_cluster.chain_index = 0;
    p_ffd->last_cluster.num_last_cluster = 0;
    p_ffd->last_cluster.max_chain_index = 0;
    p_ffd->cluster_link.buffer = NULL;
    return 0;
}

// PFFAT_InitFFD; the FFD layout is the VF one.
int PFFAT_InitFFD(PF_FFD* p_ffd, PF_FAT_HINT* p_hint, PF_VOLUME* p_vol, u32* p_start_cluster) {
    p_ffd->p_hint = p_hint;
    p_ffd->p_vol = p_vol;
    p_ffd->p_start_cluster = p_start_cluster;
    p_ffd->file_version = 1;
    if (p_hint != NULL) {
        p_hint->file_version = 0;
    }
    p_ffd->last_access_cluster.cluster = 0;
    p_ffd->last_access_cluster.chain_index = 0;
    p_ffd->last_cluster.num_last_cluster = 0;
    p_ffd->last_cluster.max_chain_index = 0;
    p_ffd->cluster_link.buffer = NULL;
    return 0;
}

int PFFAT_FinalizeFFD(PF_FFD* p_ffd) {
    p_ffd->cluster_link.buffer = NULL;
    return 0;
}

s32 PFFAT16_WriteFATEntryWithBuf(PF2_VOLUME* p_vol, u32 cluster, u32 value, PF_CACHE_PAGE* p_page) {
    u32 offset = cluster * 2;
    u16 sector = p_vol->bpb.active_FAT_sector + (offset >> p_vol->bpb.log2_bytes_per_sector);
    u16 pos = offset & (p_vol->bpb.bytes_per_sector - 1);
    u32 i;
    s32 err = 0;
    s32 first_err = 0;

    if (p_page->sector != sector) {
        for (i = 0; i < p_vol->bpb.num_active_FATs; i++) {
            s32 werr = PFSEC_WriteFAT(p_vol, p_page->p_buf, p_page->sector + i * p_vol->bpb.sectors_per_FAT, 0, p_vol->bpb.bytes_per_sector);
            if (werr != 0 && first_err == 0) {
                first_err = werr;
            }
        }
        p_page->sector = sector;
        err = PFSEC_ReadFAT(p_vol, p_page->p_buf, sector, 0, p_vol->bpb.bytes_per_sector);
    }
    p_page->p_buf[pos] = value;
    p_page->p_buf[pos + 1] = value >> 8;
    if (err != 0) {
        first_err = err;
    }
    return first_err;
}

s32 PFFAT32_WriteFATEntry(PF2_VOLUME* p_vol, u32 cluster, u32 value) {
    u32 offset = cluster * 4;
    u32 sector = (u16)(p_vol->bpb.active_FAT_sector + (offset >> p_vol->bpb.log2_bytes_per_sector));
    u16 pos = offset & (p_vol->bpb.bytes_per_sector - 1);
    u32 buf;
    u16 i;
    s32 first_err;
    s32 err;

    err = PFSEC_ReadFAT(p_vol, (u8*)&buf, sector, pos, 4);
    if (err != 0) {
        return err;
    }
    first_err = 0;
    i = 0;
    buf = (buf >> 24) | ((buf >> 8) & 0xFF00) | ((buf & 0xFF00) << 8) | (buf << 24);
    buf &= 0xF0000000;
    buf |= value & 0x0FFFFFFF;
    buf = (buf >> 24) | ((buf >> 8) & 0xFF00) | ((buf & 0xFF00) << 8) | (buf << 24);
    for (; i < p_vol->bpb.num_active_FATs; i++) {
        err = PFSEC_WriteFAT(p_vol, (u8*)&buf, sector, pos, 4);
        if (err != 0 && first_err == 0) {
            first_err = err;
        }
        sector += p_vol->bpb.sectors_per_FAT;
    }
    return first_err;
}

s32 PFFAT32_WriteFATEntryWithBuf(PF2_VOLUME* p_vol, u32 cluster, u32 value, PF_CACHE_PAGE* p_page) {
    u32 offset = cluster * 4;
    u32 sector = (u16)(p_vol->bpb.active_FAT_sector + (offset >> p_vol->bpb.log2_bytes_per_sector));
    u16 pos = offset & (p_vol->bpb.bytes_per_sector - 1);
    s32 err = 0;
    s32 first_err = 0;
    u16 i;
    u32 entry;

    if (p_page->sector != sector) {
        for (i = 0; i < p_vol->bpb.num_active_FATs; i++) {
            s32 werr = PFSEC_WriteFAT(p_vol, p_page->p_buf, p_page->sector + i * p_vol->bpb.sectors_per_FAT, 0, p_vol->bpb.bytes_per_sector);
            if (werr != 0 && first_err == 0) {
                first_err = werr;
            }
        }
        err = PFSEC_ReadFAT(p_vol, p_page->p_buf, sector, 0, p_vol->bpb.bytes_per_sector);
        if (err != 0) {
            return err;
        }
        p_page->sector = sector;
    }
    entry = *(u32*)(p_page->p_buf + pos);
    entry = (entry >> 24) | ((entry >> 8) & 0xFF00) | ((entry & 0xFF00) << 8) | (entry << 24);
    entry = (value & 0x0FFFFFFF) | (entry & 0xF0000000);
    *(u32*)(p_page->p_buf + pos) = (entry >> 24) | ((entry >> 8) & 0xFF00) | ((entry & 0xFF00) << 8) | (entry << 24);
    if (err != 0) {
        first_err = err;
    }
    return first_err;
}

s32 PFFAT16_ReadFATEntry(PF2_VOLUME* p_vol, u32 cluster, u32* p_value) {
    u32 offset = cluster * 2;
    u32 sector = (u16)(p_vol->bpb.active_FAT_sector + (offset >> p_vol->bpb.log2_bytes_per_sector));
    u16 pos = offset & (p_vol->bpb.bytes_per_sector - 1);
    u16 buf;
    u16 flags = p_vol->bpb.ext_flags;
    s32 err;
    u32 fat_index = 1;
    int action;

    if (flags & 0x80) {
        fat_index = flags & 7;
    }
    do {
        err = PFSEC_ReadFAT(p_vol, (u8*)&buf, sector, pos, 2);
        if (err == 0x1000 && p_vol->p_callback != NULL) {
            action = p_vol->p_callback(p_vol->p_callback_arg);
            if (action == 0) {
                continue;
            }
            if (action == 1 && p_vol->bpb.num_active_FATs >= 2 && fat_index < p_vol->bpb.num_active_FATs) {
                fat_index++;
                sector += p_vol->bpb.sectors_per_FAT;
                continue;
            }
        }
        if (err != 0) {
            *p_value = -1;
            return err;
        }
    } while (err != 0);
    *p_value = ((buf >> 8) & 0xFF) | ((buf << 8) & 0xFF00);
    return 0;
}

s32 PFFAT32_ReadFATEntry(PF2_VOLUME* p_vol, u32 cluster, u32* p_value) {
    u32 offset = cluster * 4;
    u32 sector = (u16)(p_vol->bpb.active_FAT_sector + (offset >> p_vol->bpb.log2_bytes_per_sector));
    u16 pos = offset & (p_vol->bpb.bytes_per_sector - 1);
    u32 buf;
    s32 err;
    u32 fat_index = 1;
    int action;

    if (p_vol->bpb.ext_flags & 0x80) {
        fat_index = p_vol->bpb.ext_flags & 7;
    }
    do {
        err = PFSEC_ReadFAT(p_vol, (u8*)&buf, sector, pos, 4);
        if (err == 0x1000 && p_vol->p_callback != NULL) {
            action = p_vol->p_callback(p_vol->p_callback_arg);
            if (action == 0) {
                continue;
            }
            if (action == 1 && p_vol->bpb.num_active_FATs >= 2 && fat_index < p_vol->bpb.num_active_FATs) {
                fat_index++;
                sector += p_vol->bpb.sectors_per_FAT;
                continue;
            }
        }
        if (err != 0) {
            *p_value = -1;
            return err;
        }
    } while (err != 0);
    *p_value = ((buf >> 24) | ((buf >> 8) & 0xFF00) | ((buf & 0xFF00) << 8) | (buf << 24)) & 0x0FFFFFFF;
    return 0;
}

s32 PFFAT_GetClusterContinuousSectorInChain(PF_FFD* p_ffd, u32 initial_cluster, u32 chain_index, u32 size, u32* p_num_sector) {
    PF_VOLUME* p_vol = p_ffd->p_vol;
    u32 next_cluster = -1;
    u32 chk_clstlnk = 1;
    u32 cluster = initial_cluster;
    s32 err;

    while (next_cluster != 0) {
        err = PFFAT_ReadCluster(p_ffd, cluster, chain_index, &next_cluster, &chk_clstlnk);
        if (err != 0) {
            return err;
        }
        if (cluster + 1 != next_cluster) {
            if (next_cluster == lbl_8041B0E0[p_vol->bpb.fat_type].end_of_chain) {
                p_ffd->last_cluster.num_last_cluster = cluster;
                p_ffd->last_cluster.max_chain_index = chain_index - 1;
            }
            break;
        }
        p_ffd->p_hint->chain_index++;
        p_ffd->p_hint->cluster = next_cluster;
        *p_num_sector += p_vol->bpb.sectors_per_cluster;
        if ((*p_num_sector << p_vol->bpb.log2_bytes_per_sector) >= size) {
            break;
        }
        cluster = next_cluster;
        chain_index++;
    }
    return 0;
}

s32 PFFAT_getContinuousSector(PF_FFD* p_ffd, u32 file_sector_index, u32 size, u32* p_sector, u32* p_num_sector) {
    PF_VOLUME* p_vol = p_ffd->p_vol;
    u32 cluster = -1;
    u32 sector;
    s32 err;

    sector = *p_sector;
    if (sector != -1) {
        cluster = ((sector - p_vol->bpb.first_data_sector) >> p_vol->bpb.log2_sectors_per_cluster) + 2;
    } else {
        err = PFFAT_GetSectorSpecified(p_ffd, file_sector_index, 0, p_sector);
        if (err != 0) {
            return err;
        }
        sector = *p_sector;
        if (sector != -1) {
            cluster = ((sector - p_vol->bpb.first_data_sector) >> p_vol->bpb.log2_sectors_per_cluster) + 2;
        }
    }

    if (sector != -1) {
        *p_num_sector = (p_vol->bpb.first_data_sector + ((cluster - 1) << p_vol->bpb.log2_sectors_per_cluster)) - sector;
    } else {
        *p_num_sector = 0;
    }

    if (*p_sector != -1 && (*p_num_sector << p_vol->bpb.log2_bytes_per_sector) < size) {
        err = PFFAT_GetClusterContinuousSectorInChain(p_ffd, cluster, (file_sector_index >> p_vol->bpb.log2_sectors_per_cluster) + 1, size, p_num_sector);
        if (err != 0) {
            return err;
        }
    }
    return 0;
}

s32 PFFAT_ReadFATSector(PF2_VOLUME* p_vol, PF_CACHE_PAGE* p_page, u32 cluster) {
    u32 offset;
    s32 err;
    u32 fat_index;
    u16 flags;

    switch (p_vol->bpb.fat_type) {
    case FAT_12:
        offset = (u16)(cluster + (cluster >> 1));
        break;
    case FAT_16:
        offset = cluster * 2;
        break;
    case FAT_32:
        offset = cluster * 4;
        break;
    default:
        return 0xF;
    }
    p_page->sector = (u16)(p_vol->bpb.active_FAT_sector + (offset >> p_vol->bpb.log2_bytes_per_sector));
    fat_index = 1;
    flags = p_vol->bpb.ext_flags;
    if (flags & 0x80) {
        fat_index = flags & 7;
    }
    do {
        err = PFSEC_ReadFAT(p_vol, p_page->p_buf, p_page->sector, 0, p_vol->bpb.bytes_per_sector);
        if (err == 0x1000 && p_vol->p_callback != NULL) {
            int action = p_vol->p_callback(p_vol->p_callback_arg);
            if (action == 0) {
                continue;
            }
            if (action == 1 && p_vol->bpb.num_active_FATs >= 2 && fat_index < p_vol->bpb.num_active_FATs) {
                fat_index++;
                p_page->sector += p_vol->bpb.sectors_per_FAT;
                continue;
            }
        }
        if (err != 0) {
            return err;
        }
    } while (err != 0);
    return err;
}

s32 PFFAT_ResetCluster(PF_VOLUME* p_vol, u32 cluster) {
    PF_CACHE_PAGE* p_page;
    u32 is_hit;
    u32 success_size;
    u32 sector;
    u32 first_sector;
    u32 i;
    s32 err;

    err = PFCACHE_AllocateDataPage(p_vol, -1, &p_page, &is_hit);
    if (err != 0) {
        return err;
    }
    first_sector = p_vol->bpb.first_data_sector + ((cluster - 2) << p_vol->bpb.log2_sectors_per_cluster);
    for (i = 0; i < p_vol->bpb.sectors_per_cluster; i++) {
        sector = first_sector + i;
        if (PFSEC_ReadData(p_vol, p_page->p_buf, sector, 0, p_vol->bpb.bytes_per_sector, &success_size, 1) != 0) {
            break;
        }
        if (success_size != p_vol->bpb.bytes_per_sector) {
            break;
        }
        VFipf_memset(p_page->p_buf, 0, p_vol->bpb.bytes_per_sector);
        if (PFSEC_WriteData(p_vol, p_page->p_buf, sector, 0, p_vol->bpb.bytes_per_sector, &success_size, 1) != 0) {
            break;
        }
        if (success_size != p_vol->bpb.bytes_per_sector) {
            break;
        }
    }
    PFCACHE_FreeDataPage(p_vol, p_page);
    return 0;
}
