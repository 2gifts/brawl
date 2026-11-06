#include <types.h>
#include <FAT/pf2.h>

// Function bodies below are derived from https://github.com/jj-link/brawl.

extern void fn_803EBBE0(s32 *, s32 *);

// HYPOTHESIS: same layout as PF_DIR_ENT in the VF copy of PrFile2.
struct PF2_DIR_ENT {
    u16 long_name[261];
    u8 num_entry_LFNs;
    u8 ordinal;
    u8 check_sum;
    u8 align_pad[1];
    s8 short_name[13];
    u8 small_letter_flag;
    u8 attr;
    u8 create_time_ms;
    u16 create_time;
    u16 create_date;
    u16 access_date;
    u16 modify_time;
    u16 modify_date;
    u32 file_size;
    PF2_VOLUME* p_vol;
    u32 path_len;
    u32 start_cluster;
    u32 entry_sector;
    u16 entry_offset;
};

// HYPOTHESIS: same layout as PF_ENT_ITER in the VF copy of PrFile2.
struct PF2_ENT_ITER {
    u32 index;
    PF_VOLUME* p_vol;
    PF_FFD ffd;
    u32 file_sector_index;
    u32 sector;
    u16 offset;
    u16 offset_mask;
    u8 buf[32];
    u8 log2_entries_per_sector;
};

u8 PFENT_getcurrentDateTimeForEnt(s16 *arg0, s16 *arg1) {
    s32 date[3];
    s32 time[4];

    fn_803EBBE0(date, time);
    *arg0 = (s16)((date[2] & 0x1F) | ((date[1] << 5) & 0x1E0) | (((date[0] - 0x7BC) << 9) & 0xFE00));
    *arg1 = (s16)((((u32)time[2] >> 1) & 0x1F) | ((time[1] << 5) & 0x7E0) | ((time[0] << 11) & 0xF800));
    return (u8)time[3];
}


int PFENT_ITER_GetEntryOfPath(PF2_ENT_ITER* iter, PF2_DIR_ENT* entry, PF2_VOLUME* vol, PF2_STR* str, u32 no_look_last_token) {
    return fn_803DE800(iter, entry, vol, str, 0, no_look_last_token);
}

int PFENT_findEntry(PF_FFD* p_ffd, PF2_DIR_ENT* p_ent, u32 index, PF2_STR* pattern, u8 attr_required, u8 attr_unwanted) {
    u32 lpos;
    u32 ppos;

    return fn_803DD390(p_ffd, p_ent, index, pattern, attr_required, attr_unwanted, &lpos, &ppos);
}

int PFENT_allocateEntry(PF2_DIR_ENT* p_ent, u8 num_entries, PF_FFD* p_ffd, u32* p_next_chain, PF2_STR* p_filename) {
    u32 pos;

    return fn_803DD674(p_ent, num_entries, p_ffd, p_next_chain, p_filename, &pos);
}

int PFENT_ITER_GetEntryOfPattern(PF2_ENT_ITER* iter, PF2_DIR_ENT* ent, PF2_VOLUME* vol, PF2_STR* path) {
    return fn_803DE800(iter, ent, vol, path, 1, 1);
}

u8 PFENT_CalcCheckSum(PF2_DIR_ENT* p_ent) {
    u8 buf[11];
    u8 sum;
    u16 i;

    PFPATH_putShortName(buf, p_ent->short_name, 0);
    sum = 0;
    for (i = 0; i < 11; i++) {
        sum = (u8)(((sum & 1) ? 0x80 : 0) + (sum >> 1) + buf[i]);
    }
    return sum;
}

void PFENT_LoadShortNameFromBuf(PF2_DIR_ENT* p_ent, const u8* buf) {
    PFPATH_getShortName(p_ent->short_name, buf, buf[0xb]);
    if (p_ent->short_name[0] == 5) {
        p_ent->short_name[0] = -0x1B;
    }
}

s32 PFENT_GetRootDir(PF2_VOLUME* p_vol, PF2_DIR_ENT* p_ent) {
    p_ent->p_vol = p_vol;
    p_ent->long_name[0] = '\\';
    p_ent->long_name[1] = 0;
    p_ent->num_entry_LFNs = 0;
    p_ent->ordinal = 0;
    p_ent->check_sum = 0;
    p_ent->short_name[0] = '\\';
    p_ent->short_name[1] = 0;
    p_ent->small_letter_flag = 0;
    p_ent->attr = 0x10;
    p_ent->create_time_ms = 0;
    p_ent->create_time = 0;
    p_ent->create_date = 0;
    p_ent->access_date = 0;
    p_ent->modify_time = 0;
    p_ent->modify_date = 0;
    p_ent->file_size = 0;
    p_ent->path_len = 3;
    p_ent->start_cluster = 1;
    p_ent->entry_sector = 0xFFFFFFFF;
    p_ent->entry_offset = 0;
    return 0;
}

s32 PFENT_ITER_IteratorInitialize(PF2_ENT_ITER* p_iter, u32 index_start_from) {
    u32 num_success;
    s32 err;
    u32 index;
    s32 status;
    s32 read_err;
    u8 log2;

    p_iter->p_vol = p_iter->ffd.p_vol;
    index = p_iter->index;
    log2 = p_iter->p_vol->bpb.log2_bytes_per_sector - 5;
    p_iter->index = index_start_from;
    p_iter->log2_entries_per_sector = log2;
    p_iter->offset_mask = (1 << p_iter->log2_entries_per_sector) - 1;
    p_iter->file_sector_index = 0xFFFFFFFF;

    err = PFENT_RecalcEntryIterator(p_iter, 0);
    if (err != 0) {
        p_iter->index = index;
        PFENT_RecalcEntryIterator(p_iter, 0);
    } else {
        if (p_iter->sector == 0xFFFFFFFF) {
            status = 16;
        } else {
            read_err = PFSEC_ReadData(p_iter->p_vol, p_iter->buf, p_iter->sector, p_iter->offset, 32, &num_success, 0);
            status = 17;
            if (num_success == 32)
                status = read_err;
        }
        err = status != 0 ? status : 0;
    }

    return err;
}

// HYPOTHESIS: layout of the PrFile2 volume fields this unit touches.
struct PF2_VOLUME {
    PF_BPB bpb;
};

// HYPOTHESIS: the PrFile2 volume set; only the critical section is known here.
extern struct PF2_VOLUME_SET {
    u8 _0[0x64];
    PF_CRITICAL critical;
} lbl_805644C0;

u8 lbl_80492FD8[12] = "NO NAME    ";

s32 PFENT_MakeRootDir(PF2_VOLUME* p_vol) {
    PF_CACHE_PAGE* p_page;
    u32 written;
    u32 unused;
    u32 sector;
    s32 err;

    switch (p_vol->bpb.fat_type) {
    case 2:
        PFFAT_MakeRootDir(p_vol);
        break;
    case 0:
    case 1:
        err = PFCACHE_AllocateDataPage(p_vol, 0xFFFFFFFF, &p_page, &unused);
        if (err != 0) {
            return err;
        }
        VFipf_memset(p_page->p_buf, 0, p_vol->bpb.bytes_per_sector);
        for (sector = p_vol->bpb.first_root_dir_sector; sector < p_vol->bpb.first_data_sector; sector++) {
            err = PFSEC_WriteData(p_vol, p_page->p_buf, sector, 0, p_vol->bpb.bytes_per_sector, &written, 0);
            if (err != 0) {
                PFCACHE_FreeDataPage(p_vol, p_page);
                return err;
            }
            if (written != p_vol->bpb.bytes_per_sector) {
                PFCACHE_FreeDataPage(p_vol, p_page);
                return 17;
            }
        }
        PFCACHE_FreeDataPage(p_vol, p_page);
        break;
    default:
        return 7;
    }
    PF_enterCritical(&lbl_805644C0.critical);
    PFDRV_LoadVolumeLabelFromBuf(p_vol, lbl_80492FD8);
    PF_exitCritical(&lbl_805644C0.critical);
    return 0;
}

s32 PFENT_ITER_FindEntry(PF2_ENT_ITER* p_iter, PF2_DIR_ENT* p_ent, PF_STR* p_path, u32 wildcard, u32 no_look_last_token, u32* p_pos) {
    s32 err;

    *p_pos = 0;
    if (wildcard & no_look_last_token) {
        return 10;
    }
    if (PFSTR_StrNCmp(p_path, (const s8*)":", 1, 1, 1) == 0) {
        PFSTR_MoveStrPos(p_path, 2);
    }
    if (PFSTR_StrNCmp(p_path, (const s8*)"\\", 1, 0, 1) == 0 || PFSTR_StrNCmp(p_path, (const s8*)"/", 1, 0, 1) == 0) {
        err = PFENT_GetRootDir((PF2_VOLUME*)p_iter->p_vol, p_ent);
        if (err != 0) {
            return err;
        }
        if (PFSTR_StrNumChar(p_path, 1) == 1 && PFSTR_StrNCmp(p_path, (const s8*)"", 2, 0, 1) == 0) {
            return 0;
        }
    } else {
        err = PFVOL_GetCurrentDir((PF2_VOLUME*)p_iter->p_vol, p_ent);
        if (err != 0) {
            return err;
        }
    }
    err = PFENT_ITER_DoFindEntry(p_iter, p_ent, p_path, wildcard, no_look_last_token, p_pos);
    return err != 0 ? err : 0;
}
