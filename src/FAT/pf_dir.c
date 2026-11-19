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

// TODO: clean up, give variables real names
/* jj_803DADEC */
extern s32 fn_803E2414(void *, void *, s32, void *);

typedef struct Fn803DADECCopy {
    u32 words[0x90];
} Fn803DADECCopy;

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void *fn_803DADEC(void *arg0, void *arg1) {
    u8 *walk = (u8 *)arg0;
    u8 *candidate = (u8 *)arg0 + 0xE3C;
    u8 *freeEntry = 0;
    int i;

    for (i = 0; i < 3; i++) {
        if (!(*(u32 *)(walk + 0xE3C) & 1) ||
            (((*(u32 *)(walk + 0xE3C) & 1) &&
              ((*(u32 *)(walk + 0xE3C) == 0) & 2)))) {
            if (freeEntry == 0)
                freeEntry = candidate;
        } else if (*(u32 *)((u8 *)arg1 + 0x22C) == *(u32 *)(walk + 0x10A8) &&
                   *(u32 *)((u8 *)arg1 + 0x238) == *(u32 *)(walk + 0x10B4) &&
                   *(u16 *)((u8 *)arg1 + 0x23C) == *(u16 *)(walk + 0x10B8)) {
            return (u8 *)arg0 + i * 0x280 + 0xE3C;
        }
        walk += 0x280;
        candidate += 0x280;
    }

    if (freeEntry == 0)
        return 0;

    *(u32 *)freeEntry = 3;
    *(u16 *)(freeEntry + 4) = 0;
    *(Fn803DADECCopy *)(freeEntry + 0x40) = *(Fn803DADECCopy *)arg1;
    fn_803E2414(freeEntry + 8, 0, *(s32 *)((u8 *)arg1 + 0x22C), freeEntry + 0x274);
    return freeEntry;
}

/* jj_803DABBC */
typedef struct Disk { char pad[0x20]; u8 shift; char pad21[0x17]; u32 size; char pad3c[0x1f70]; u16 flags; } Disk;
typedef struct Inner { u32 flags; char pad04[0x30]; void *ptr; char pad38[0x22c]; u32 offset; Disk *disk; } Inner;
typedef struct A { u32 a, b, c; } A;
typedef struct B { u32 a, b, c, d; } B;
typedef struct Obj { u32 flags, pad; Inner *inner; A x; u32 pad18; B y; } Obj;

extern void fn_803E46AC(Obj *, u32);
extern int fn_803E4668(Obj *);
extern int fn_803E1458(void *, u32, u32, int *);
extern int fn_803E1494(void *, u32, u32, int *, u32 *);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
int fn_803DABBC(Obj *obj, u32 len, u32 *out) {
    int spC;
    u32 count;
    int result;
    Disk *disk;
    B y;
    A x;
    *out = 0;
    if (!(obj->inner->flags & 1) || !(obj->inner->flags & 2)) return 0x26;
    disk = obj == 0 ? 0 : obj->inner->disk;
    y = obj->y;
    x = obj->x;
    obj->inner->ptr = &obj->x;
    spC = -1;
    if (disk->flags & 4) {
        if (disk->size + 0x10000 != 0xffff && disk->size == 0) return 6;
    }
    fn_803E46AC(obj, obj->inner->offset + len);
    if (obj->y.a + 0x10000 == 0xffff) { *out = 0; return 0x25; }
    result = fn_803E1458((char *)obj->inner + 4, obj->y.c, len, &spC);
    if (result) {
        obj->y = y;
        obj->x = x;
        return result;
    }
    count = 0;
    result = fn_803E1494((char *)obj->inner + 4, obj->y.c, len, &spC, &count);
    obj->y = y;
    obj->x = x;
    if (result) return result;
    if ((u32)(spC + 0x10000) == 0xffffU) {
        fn_803E4668(obj);
        return 0x1C;
    }
    count <<= disk->shift;
    *out = len < count ? len : count;
    return 0;
}

