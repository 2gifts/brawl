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

// TODO: clean up, give variables real names
/* jj_803E2468 */
struct Ctx {
    unsigned short unk0;
    unsigned char pad002[0x0A];
    unsigned int unkC;
    unsigned char pad010[0x08];
    unsigned short unk18;
    unsigned char pad01A[0x06];
    unsigned char unk20;
    unsigned char pad021;
    unsigned char unk22;
    unsigned char pad023[0x05];
    unsigned int unk28;
    unsigned char pad02C[0x1F84 - 0x2C];
    int unk1F84;
    unsigned char pad1F88[0x1FC0 - 0x1F88];
    int (*unk1FC0)(int);
};

inline int inline_fn(struct Ctx *arg0) {
    return arg0->unk0 - 1;
}

extern int fn_803E8DC0(void *, unsigned short *, unsigned int, unsigned int, unsigned short);

// TODO: clean up, give variables real names
int fn_803E2468(struct Ctx *arg0, int arg1, int *arg2) {
    unsigned short temp_r28;
    unsigned short var_r27;
    int var_r26;
    unsigned int var_r25;
    unsigned char buf[4];
    unsigned short temp_r9;
    unsigned short temp_r0;
    int temp_r3;
    int temp_r3_2;
    int (*temp_r12)(int);
    unsigned int temp_r0_3;
    unsigned short sp8;

    temp_r0 = (unsigned short)(arg1 + (arg1 >> 1));
    temp_r9 = arg0->unk18;
    var_r25 = 1;
    var_r27 = (unsigned short)(arg0->unk28 + ((int)temp_r0 >> arg0->unk20));
    temp_r28 = (unsigned short)(temp_r0 & inline_fn(arg0));
    if (temp_r9 & 0x80) {
        var_r25 = temp_r9 & 7;
    }

    do {
        if ((int)temp_r28 < (int)inline_fn(arg0)) {
            var_r26 = fn_803E8DC0(arg0, (unsigned short *)buf, (unsigned short)var_r27, temp_r28, 2);
        } else {
            temp_r3 = fn_803E8DC0(arg0, (unsigned short *)buf, (unsigned short)var_r27, temp_r28, 1);
            var_r26 = temp_r3;
            if (temp_r3 == 0) {
                var_r26 = fn_803E8DC0(arg0, (unsigned short *)(buf + 1),
                                      (unsigned short)var_r27 + 1, 0, 1);
            }
        }
        if (var_r26 == 0x1000) {
            temp_r12 = arg0->unk1FC0;
            if (temp_r12 != 0) {
                temp_r3_2 = temp_r12(arg0->unk1F84);
                if (temp_r3_2 == 0) {
                    continue;
                }
                if (temp_r3_2 == 1) {
                    if (arg0->unk22 >= 2 && var_r25 < arg0->unk22) {
                        var_r25 += 1;
                        var_r27 += (unsigned short)arg0->unkC;
                        continue;
                    }
                }
            }
        }
        if (var_r26 != 0) {
            *arg2 = -1;
            return var_r26;
        }
    } while (var_r26 != 0);

    sp8 = *(unsigned short *)buf;
    temp_r0_3 = ((sp8 >> 8) & 0xFF) | (((unsigned int)sp8 << 8) & 0xFF00);
    if (arg1 & 1) {
        *arg2 = (temp_r0_3 >> 4) & 0xFFF;
    } else {
        *arg2 = temp_r0_3 & 0xFFF;
    }
    return 0;
}

// TODO: clean up, give variables real names
/* jj_803E119C */
typedef struct Info { char pad0[0x10]; u32 value; char pad14[8]; u32 count; char pad20[0x14]; u32 limit; } Info;
typedef struct Record { int key; u32 value; int owner; } Record;
typedef struct Context { int owner; char pad4[4]; int *state; char padc[0x10]; int flag; char pad20[0x10]; Record *record; Info *info; } Context;
extern int fn_803E0A88(Context *, int, int, int, u32 *, u32 *);
// TODO: clean up, give variables real names
int fn_803E119C(Context *ctx, int key, int arg2, u32 *out) {
    u32 value, other;
    Info *info = ctx->info;
    u32 count = info->count;
    Record *record = ctx->record;
    int result;
    if ((u32)*ctx->state == 1) {
        if (count <= 1) { *out = -1; return 0; }
        *ctx->state = info->value;
    }
    *out = -1;
    result = fn_803E0A88(ctx, key, 2, arg2, &other, &value);
    switch (result) {
    case 0:
        if (other >= 2 && other < ctx->info->limit + 2) *out = other;
        result = 0;
        break;
    default:
        break;
    }
    if (result != 0) return result;
    if (*out == 0xFFFFFFFF) return 0;
    record->owner = ctx->owner;
    record->key = key;
    record->value = *out;
    return 0;
}

