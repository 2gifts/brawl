#include <types.h>
#include <FAT/pf2.h>
#include <revolution/VF/pf_critical.h>

// HYPOTHESIS: layout of the second PrFile2 volume; the first fields are shared with
// the VF copy (PF_VOLUME) shifted by 0x3C, the rest is only known where used.
struct PF2_VOLUME {
    u8 _0[0xE3C];
    PF_SDD sdds[3];
    PF_DIR udds[3];
    s32 num_opened_files;
    s32 num_opened_directories;
    union {
        u8 _1624[0x1F80 - 0x1624];
        struct {
            u8 _pad1624[0x20];
            s32 unk1644;
        };
    };
    s32 last_error;
    u8 _1F84[0xC];
    PF_CRITICAL critical;
    u8 _1FA0[0x28];
};

// HYPOTHESIS: the PrFile2 volume set; only last_error is known.
extern struct PF2_VOLUME_SET {
    u8 _0[0x40];
    s32 last_error;
} lbl_805644C0;

void PFDIR_FinalizeAllDirs(PF2_VOLUME* p_vol) {
    u32 i;
    u32 zero;
    PF_SDD* p_sdd;

    zero = 0;
    i = 0;
    do {
        p_sdd = &p_vol->sdds[(u16)i];
        p_sdd->stat = zero;
        PFFAT_FinalizeFFD(&p_sdd->ffd);
        i++;
    } while (i < 3);

    p_vol->udds[0].stat &= ~1;
    p_vol->udds[1].stat &= ~1;
    p_vol->udds[2].stat &= ~1;
    p_vol->num_opened_directories = 0;
}

int PFDIR_fsnext(PF2_DTA* dta) {
    PF2_VOLUME* p_vol;
    s32 err;

    p_vol = dta->p_vol;
    PF_enterCritical(&p_vol->critical);
    err = PFVOL_CheckForRead(p_vol);
    if (err != 0) {
        PF_exitCritical(&p_vol->critical);
        lbl_805644C0.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    err = PFDIR_p_fsnext(dta);
    if (err != 0) {
        lbl_805644C0.last_error = err;
        p_vol->last_error = err;
    }
    PF_exitCritical(&p_vol->critical);
    return err;
}

int PFDIR_fsfirst(PF_STR* p_pattern, u8 attr, PF2_DTA* dta) {
    PF2_VOLUME* p_vol;
    s32 err;

    p_vol = PFPATH_GetVolumeFromPath(p_pattern);
    PF_enterCritical(&p_vol->critical);
    err = PFVOL_CheckForRead(p_vol);
    if (err != 0) {
        PF_exitCritical(&p_vol->critical);
        lbl_805644C0.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    err = PFDIR_p_fsfirst(p_vol, p_pattern, attr, dta);
    if (err != 0) {
        dta->p_vol = 0;
        lbl_805644C0.last_error = err;
        p_vol->last_error = err;
    }
    PF_exitCritical(&p_vol->critical);
    return err;
}

int PFDIR_fstat(PF_STR* p_path, void* p_stat) {
    PF2_VOLUME* p_vol;
    s32 err;

    p_vol = PFPATH_GetVolumeFromPath(p_path);
    PF_enterCritical(&p_vol->critical);
    err = PFVOL_CheckForRead(p_vol);
    if (err != 0) {
        PF_exitCritical(&p_vol->critical);
        lbl_805644C0.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    err = PFDIR_p_fstat(p_vol, p_path, p_stat);
    if (err != 0) {
        lbl_805644C0.last_error = err;
        p_vol->last_error = err;
    }
    PF_exitCritical(&p_vol->critical);
    return err;
}

int PFDIR_opendir(PF_STR* p_path, PF2_DIR** pp_dir) {
    PF2_VOLUME* p_vol;
    s32 err;

    p_vol = PFPATH_GetVolumeFromPath(p_path);
    PF_enterCritical(&p_vol->critical);
    err = PFVOL_CheckForRead(p_vol);
    if (err != 0) {
        PF_exitCritical(&p_vol->critical);
        lbl_805644C0.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    err = PFDIR_p_opendir(p_vol, p_path, pp_dir);
    if (err != 0) {
        lbl_805644C0.last_error = err;
        p_vol->last_error = err;
    } else {
        p_vol->num_opened_directories++;
    }
    PF_exitCritical(&p_vol->critical);
    return err;
}

void PFCLUSTER_UpdateLastAccessCluster(PF_FILE* p_file, u32 sector) {
    PF_BPB* bpb;
    u32 index;

    if (p_file->cursor.position == 0) {
        p_file->p_sfd->ffd.last_access_cluster.cluster = 0;
        p_file->p_sfd->ffd.last_access_cluster.chain_index = 0;
        return;
    }
    if (p_file == 0) {
        bpb = 0;
    } else {
        bpb = (PF_BPB*)p_file->p_sfd->dir_entry.p_vol;
    }
    if ((p_file->cursor.position & (bpb->bytes_per_sector - 1)) == 0) {
        index = p_file->cursor.file_sector_index;
        if ((index & (bpb->sectors_per_cluster - 1)) == 0) {
            if (index == 0) {
                return;
            }
            index--;
            p_file->p_sfd->ffd.last_access_cluster.chain_index = index >> bpb->log2_sectors_per_cluster;
            p_file->p_sfd->ffd.last_access_cluster.cluster = ((sector - 1 - bpb->first_data_sector) >> bpb->log2_sectors_per_cluster) + 2;
            return;
        }
    }
    p_file->p_sfd->ffd.last_access_cluster.chain_index = p_file->cursor.file_sector_index >> bpb->log2_sectors_per_cluster;
    p_file->p_sfd->ffd.last_access_cluster.cluster = ((sector - bpb->first_data_sector) >> bpb->log2_sectors_per_cluster) + 2;
}

// HYPOTHESIS: a PrFile2 directory handle; same layout as PF_DIR.
struct PF2_DIR {
    u32 stat;
    PF_SDD* p_sdd;
};

int PFDIR_closedir(PF2_DIR* dir) {
    PF2_VOLUME* p_vol;
    s32 err;
    PF_SDD* p_sdd;

    if (dir == 0) {
        p_vol = 0;
    } else {
        p_vol = (PF2_VOLUME*)dir->p_sdd->dir_entry.p_vol;
    }
    PF_enterCritical(&p_vol->critical);
    err = PFVOL_CheckForRead(p_vol);
    if (err != 0) {
        PF_exitCritical(&p_vol->critical);
        lbl_805644C0.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    p_sdd = dir->p_sdd;
    if ((p_sdd->stat & 1) == 0 || (p_sdd->stat & 2) == 0) {
        PF_exitCritical(&p_vol->critical);
        lbl_805644C0.last_error = 10;
        p_vol->last_error = 10;
        return 10;
    }
    p_sdd->num_handlers--;
    if (p_sdd->num_handlers == 0) {
        dir->p_sdd->stat &= ~1;
    }
    dir->stat &= ~1;
    p_vol->num_opened_directories--;
    PF_exitCritical(&p_vol->critical);
    return 0;
}

int PFDIR_mkdir(PF_STR* p_path) {
    PF2_VOLUME* p_vol;
    s32 err;

    p_vol = PFPATH_GetVolumeFromPath(p_path);
    PF_enterCritical(&p_vol->critical);
    err = PFVOL_CheckForWrite(p_vol);
    if (err != 0) {
        PF_exitCritical(&p_vol->critical);
        lbl_805644C0.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    p_vol->unk1644 = 0;
    err = PFDIR_p_mkdir(p_vol, p_path, 0, 0);
    if (err != 0) {
        lbl_805644C0.last_error = err;
        p_vol->last_error = err;
    } else {
        err = PFCACHE_FlushFATCache(p_vol);
        if (err != 0) {
            lbl_805644C0.last_error = err;
            p_vol->last_error = err;
        } else {
            err = PFCACHE_FlushDataCacheSpecific(p_vol, 0);
            if (err != 0) {
                lbl_805644C0.last_error = err;
                p_vol->last_error = err;
            }
        }
    }
    p_vol->unk1644 = 0;
    PF_exitCritical(&p_vol->critical);
    return err;
}
