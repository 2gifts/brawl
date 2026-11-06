#include <types.h>
#include <FAT/pf2.h>

// HYPOTHESIS: the PrFile2 (second build) layouts; only the fields these functions touch are known.
typedef struct PF2_SFD PF2_SFD;
struct PF2_SFD {
    u32 stat;
    union {
        PF_FFD ffd;
        struct {
            u8 _ffd_pad[0x34];
            PF2_VOLUME* ffd_p_vol;  // HYPOTHESIS: PF_FFD::p_vol seen as the PF2 volume
        };
    };
    union {
        u8 unk3C[0x228];
        struct {
            u8 _entry_pad[0x222];
            u16 last_access_date;  // HYPOTHESIS: 0x25E
            u16 time;              // HYPOTHESIS: 0x260
            u16 date;              // HYPOTHESIS: 0x262
        };
    };
    u32 file_size;       // 0x264
    PF2_VOLUME* p_vol;   // 0x268
    u8 unk26C[0x10];
    PF_LOCK lock;        // 0x27C
    u8 unk28C[0x10];
};

struct PF2_FILE {
    u32 stat;
    s32 open_mode;
    PF2_SFD* p_sfd;
    PF_FAT_HINT hint;
    s32 last_error;
    PF_CURSOR cursor;
    u16 lock_count;
};

struct PF2_VOLUME {
    PF_BPB bpb;
    u32 num_free_clusters;
    u32 last_free_cluster;
    PF2_SFD sfds[5];
    PF2_FILE ufds[5];
    u8 unkE3C[0x7E0];
    s32 num_opened_files;  // 0x161C
    u8 unk1620[0x24];
    s32 unk1644;
    u8 unk1648[0x938];
    s32 last_error;        // 0x1F80
    u8 unk1F84[0xC];
    PF_CRITICAL critical;  // 0x1F90
    u8 unk1FA0[0x28];
};

// HYPOTHESIS: the PrFile2 volume set; last_error is at 0x40.
typedef struct PF2_VOLUME_SET {
    u8 unk0[0x40];
    s32 last_error;
} PF2_VOLUME_SET;
extern PF2_VOLUME_SET lbl_805644C0;

void PFFILE_Cursor_MoveToEnd(PF2_FILE* p_file) {
    PF2_VOLUME* p_vol;

    p_file->cursor.position = p_file->p_sfd->file_size;
    if (p_file == NULL) {
        p_vol = NULL;
    } else {
        p_vol = p_file->p_sfd->p_vol;
    }
    p_file->cursor.file_sector_index = p_file->cursor.position >> p_vol->bpb.log2_bytes_per_sector;
    p_file->cursor.offset_in_sector = p_file->cursor.position & (p_vol->bpb.bytes_per_sector - 1);
}

void PFFILE_FinalizeAllFiles(PF2_VOLUME* p_vol) {
    u16 i;
    PF2_SFD* p_sfd;

    for (i = 0; i < 5; i++) {
        p_sfd = &p_vol->sfds[i];
        p_sfd->stat = 0;
        PFFAT_FinalizeFFD(&p_sfd->ffd);
    }
    p_vol->ufds[0].stat &= ~1;
    p_vol->ufds[1].stat &= ~1;
    p_vol->ufds[2].stat &= ~1;
    p_vol->ufds[3].stat &= ~1;
    p_vol->ufds[4].stat &= ~1;
    p_vol->num_opened_files = 0;
}

int PFFILE_p_fread(PF2_VOLUME* p_vol, u8* p_buf, u32 size, u32 count, PF2_FILE* p_file, u32* p_count_read) {
    u32 size_read;
    int err;

    size_read = 0;
    *p_count_read = 0;
    if ((p_file->open_mode & 8) == 0 && ((p_file->open_mode & 1) != 0 || (p_file->open_mode & 4) != 0)) {
        return 10;
    }
    if ((p_file->p_sfd->lock.mode & 3) != 0 && p_file->lock_count == 0) {
        return 25;
    }
    if (p_file->cursor.position >= p_file->p_sfd->file_size) {
        return 28;
    }
    err = PFFILE_Cursor_Read(p_file, p_buf, size * count, &size_read);
    *p_count_read = size_read / size;
    if (err != 0) {
        return err;
    }
    return 0;
}

int PFFILE_remove(PF_STR* p_path_str) {
    PF2_VOLUME* p_vol;
    int err;

    p_vol = PFPATH_GetVolumeFromPath(p_path_str);
    PF_enterCritical(&p_vol->critical);
    err = PFVOL_CheckForWrite(p_vol);
    if (err != 0) {
        PF_exitCritical(&p_vol->critical);
        lbl_805644C0.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    p_vol->unk1644 = 0;
    err = PFFILE_p_remove(p_vol, p_path_str);
    if (err != 0) {
        PF_exitCritical(&p_vol->critical);
        lbl_805644C0.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    err = PFCACHE_FlushFATCache(p_vol);
    if (err != 0) {
        lbl_805644C0.last_error = err;
        p_vol->last_error = err;
    }
    PF_exitCritical(&p_vol->critical);
    return err;
}

int PFFILE_fopen(PF_STR* p_path_str, s32 mode, PF2_FILE** pp_file) {
    PF2_VOLUME* p_vol;
    int err;

    *pp_file = NULL;
    p_vol = PFPATH_GetVolumeFromPath(p_path_str);
    PF_enterCritical(&p_vol->critical);
    err = PFVOL_CheckForWrite(p_vol);
    if (err != 0 && (err != 11 || mode != 2)) {
        PF_exitCritical(&p_vol->critical);
        lbl_805644C0.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    err = PFFILE_p_fopen(p_vol, p_path_str, mode, pp_file);
    if (err != 0) {
        lbl_805644C0.last_error = err;
        p_vol->last_error = err;
    } else {
        p_vol->num_opened_files++;
    }
    PF_exitCritical(&p_vol->critical);
    return err;
}

void PFFILE_Cursor_MoveToClusterEnd(PF2_FILE* p_file, u32 size) {
    PF2_VOLUME* p_vol;
    u32 num_alloc_clusters;

    if (p_file == NULL) {
        p_vol = NULL;
    } else {
        p_vol = p_file->p_sfd->p_vol;
    }
    PFFAT_CountAllocatedClusters(&p_file->p_sfd->ffd, size, &num_alloc_clusters);
    if (num_alloc_clusters > (0xFFFFFFFF >> (p_vol->bpb.log2_bytes_per_sector + p_vol->bpb.log2_sectors_per_cluster))) {
        p_file->cursor.position = 0xFFFFFFFF;
        if (p_file == NULL) {
            p_vol = NULL;
        } else {
            p_vol = p_file->p_sfd->p_vol;
        }
        p_file->cursor.file_sector_index = p_file->cursor.position >> p_vol->bpb.log2_bytes_per_sector;
        p_file->cursor.offset_in_sector = p_file->cursor.position & (p_vol->bpb.bytes_per_sector - 1);
    } else {
        p_file->cursor.position = num_alloc_clusters << (p_vol->bpb.log2_bytes_per_sector + p_vol->bpb.log2_sectors_per_cluster);
        if (p_file == NULL) {
            p_vol = NULL;
        } else {
            p_vol = p_file->p_sfd->p_vol;
        }
        p_file->cursor.file_sector_index = p_file->cursor.position >> p_vol->bpb.log2_bytes_per_sector;
        p_file->cursor.offset_in_sector = p_file->cursor.position & (p_vol->bpb.bytes_per_sector - 1);
    }
}

u32 PFFILE_Cursor_AdvanceToRead(PF2_FILE* p_file, u32 n, u32 sector) {
    u32 new_sector;
    u32 sector_offset;
    u32 ok;
    PF2_VOLUME* p_vol;
    PF2_VOLUME* p_vol_end;

    ok = 1;
    p_vol = (p_file == NULL) ? NULL : p_file->p_sfd->p_vol;
    sector_offset = p_file->cursor.offset_in_sector + n;
    new_sector = sector + (sector_offset >> p_vol->bpb.log2_bytes_per_sector);
    if (p_file->cursor.position + n < p_file->p_sfd->file_size) {
        if (((p_vol->bpb.bytes_per_sector - 1) & sector_offset) != 0) {
            p_file->cursor.sector = new_sector;
        } else {
            p_file->cursor.sector = 0xFFFFFFFF;
        }
        p_file->cursor.position = p_file->cursor.position + n;
    } else if (p_file->p_sfd->file_size == 0) {
        p_file->cursor.sector = 0xFFFFFFFF;
        p_file->cursor.position = 0;
        if (n != 0) {
            ok = 0;
        }
    } else {
        ok = 0;
        p_file->cursor.sector = 0xFFFFFFFF;
        p_file->cursor.position = p_file->p_sfd->file_size;
    }
    if (p_file == NULL) {
        p_vol_end = NULL;
    } else {
        p_vol_end = p_file->p_sfd->p_vol;
    }
    p_file->cursor.file_sector_index = p_file->cursor.position >> p_vol_end->bpb.log2_bytes_per_sector;
    p_file->cursor.offset_in_sector = p_file->cursor.position & (p_vol_end->bpb.bytes_per_sector - 1);
    PFCLUSTER_UpdateLastAccessCluster(p_file, new_sector);
    return ok;
}

int fn_803E5A50(void* buf, u32 size, u32 count, PF2_FILE* file, u32* count_read) {
    int err;
    PF2_VOLUME* p_vol;
    u32 size_read;

    *count_read = 0;
    p_vol = (file == NULL) ? NULL : file->p_sfd->p_vol;
    PF_enterCritical(&p_vol->critical);
    err = PFVOL_CheckForRead(p_vol);
    if (err != 0) {
        PF_exitCritical(&p_vol->critical);
        lbl_805644C0.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    if ((file->p_sfd->stat & 1) == 0 || (file->p_sfd->stat & 2) == 0) {
        PF_exitCritical(&p_vol->critical);
        lbl_805644C0.last_error = 0x26;
        p_vol->last_error = 0x26;
        return 0x26;
    }
    p_vol->unk1644 = (s32)file;
    err = PFFILE_p_fread(p_vol, (u8*)buf, size, count, file, &size_read);
    *count_read = size_read;
    p_vol->unk1644 = 0;
    if (err != 0) {
        lbl_805644C0.last_error = err;
        file->p_sfd->ffd_p_vol->last_error = err;
        file->last_error = err;
    }
    PF_exitCritical(&p_vol->critical);
    return err;
}

int fn_803E5B8C(void* buf, u32 size, u32 count, PF2_FILE* file, u32* count_written) {
    int err;
    PF2_VOLUME* p_vol;
    u32 size_written;

    *count_written = 0;
    p_vol = (file == NULL) ? NULL : file->p_sfd->p_vol;
    PF_enterCritical(&p_vol->critical);
    err = PFVOL_CheckForWrite(p_vol);
    if (err != 0) {
        PF_exitCritical(&p_vol->critical);
        lbl_805644C0.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    if ((file->p_sfd->stat & 1) == 0 || (file->p_sfd->stat & 2) == 0) {
        PF_exitCritical(&p_vol->critical);
        lbl_805644C0.last_error = 0x26;
        p_vol->last_error = 0x26;
        return 0x26;
    }
    p_vol->unk1644 = (s32)file;
    err = PFFILE_p_fwrite(p_vol, (u8*)buf, size, count, file, &size_written);
    *count_written = size_written;
    p_vol->unk1644 = 0;
    if (err != 0) {
        lbl_805644C0.last_error = err;
        file->p_sfd->ffd_p_vol->last_error = err;
        file->last_error = err;
    }
    if (size_written != 0) {
        PFENT_getcurrentDateTimeForEnt(&file->p_sfd->date, &file->p_sfd->time);
        file->p_sfd->last_access_date = file->p_sfd->date;
        file->p_sfd->stat |= 4;
    }
    PF_exitCritical(&p_vol->critical);
    return err;
}
