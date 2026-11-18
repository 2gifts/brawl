#include <types.h>
#include <revolution/NAND/nand.h>

// HYPOTHESIS: entry of the NAND file table (23 entries of 0xF0 bytes) relating a disk to its NAND file.
typedef struct FaNandDskFile {
    void* disk;
    char path[0x54];
    NANDFileInfo info;
    u32 flags;
    u8 _pad[0xF0 - 0xE8];
} FaNandDskFile;

// HYPOTHESIS: nanddrv function table in .rodata.
extern u8 lbl_8041F398[0x20];

typedef struct FaDiskTbl {
    const void* p_func;
    u32 ui_ext;
} FaDiskTbl;

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_80598098[0x40] __attribute__((aligned(8))) = { 0 };
FaNandDskFile lbl_805980D8[0x17] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A1278[0x4] __attribute__((aligned(4))) = { 0 };
s32 lbl_805A127C = 0;
s32 lbl_805A1280 = 0;
unsigned char lbl_805A1284[0x1c] __attribute__((aligned(4))) = { 0 };

// pfk_get_semaphore (the map name); pf2.h calls it by this stand-in name.
extern s32 fn_803EC3D4(s32 resource);
// pfk_release_semaphore (the map name); pf2.h calls it by this stand-in name.
extern s32 fn_803EC418(s32 resource);
s32 fa_nanddrv_CreateNANDFileForDsk(FaNandDskFile* file);
s32 fa_nanddrv_physical_read(u32 num_blocks, u8* buf, u32 block, u32 bps, u32* p_num_success, void* p_disk);
s32 fa_nanddrv_physical_write(u32 num_blocks, const u8* buf, u32 block, u32 bps, u32* p_num_success, void* p_disk);

s32 fa_nanddrv_init_drv_tbl(FaDiskTbl* p_disk_tbl, u32 ui_ext) {
    p_disk_tbl->ui_ext = ui_ext;
    p_disk_tbl->p_func = lbl_8041F398;
    return 0;
}

s32 fa_nanddrv_pread(u32* p_disk, u8* p_buf, u32 block, u32 num_blocks, u32* p_num_success) {
    s32 result;
    fn_803EC3D4(lbl_805A1280);
    *p_num_success = 0;
    if (p_disk == NULL) {
        fn_803EC418(lbl_805A1280);
        return -0x14;
    }
    if (p_buf == NULL) {
        fn_803EC418(lbl_805A1280);
        return -0x14;
    }
    result = fa_nanddrv_physical_read(num_blocks, p_buf, block, 0x200, p_num_success, p_disk);
    if (result != 0) {
        fn_803EC418(lbl_805A1280);
        return result;
    }
    fn_803EC418(lbl_805A1280);
    return 0;
}

s32 fa_nanddrv_pwrite(u32* p_disk, const u8* p_buf, u32 block, u32 num_blocks, u32* p_num_success) {
    s32 result;
    fn_803EC3D4(lbl_805A1280);
    *p_num_success = 0;
    if (p_disk == NULL) {
        fn_803EC418(lbl_805A1280);
        return -0x14;
    }
    if (p_buf == NULL) {
        fn_803EC418(lbl_805A1280);
        return -0x14;
    }
    result = fa_nanddrv_physical_write(num_blocks, p_buf, block, 0x200, p_num_success, p_disk);
    if (result != 0) {
        fn_803EC418(lbl_805A1280);
        return result;
    }
    fn_803EC418(lbl_805A1280);
    return 0;
}

s32 fa_nanddrv_RelateNANDFileToDsk(void* p_disk, FaNandDskFile* file, int create) {
    s32 result = NANDChangeDir((const char*)lbl_80598098);
    if (result != 0) {
        return result;
    }
    if (file->flags & 4) {
        result = 0;
    } else {
        result = NANDOpen(file->path, &file->info, 3);
    }
    if (create != 0) {
        result = -0xc;
    }
    switch (result) {
    case -0xc:
        result = fa_nanddrv_CreateNANDFileForDsk(file);
        if (result == 0) {
            file->disk = p_disk;
        }
        break;
    case 0:
        file->disk = p_disk;
        result = 0;
        break;
    }
    return result;
}

void* VFipf_memset(void* dst, int ch, size_t n);

s32 fa_nanddrv_BuildUpFSInfoSector(u8* buf) {
    VFipf_memset(buf, 0, 0x200);
    if ((u32)buf & 3) {
        buf[0] = 0x52;
        buf[0x1] = 0x52;
        buf[0x2] = 0x61;
        buf[0x3] = 0x41;
    } else {
        *(u32*)buf = 0x52526141;
    }
    if ((u32)(buf + 0x1e4) & 3) {
        buf[0x1e4] = 0x72;
        buf[0x1e5] = 0x72;
        buf[0x1e6] = 0x41;
        buf[0x1e7] = 0x61;
    } else {
        *(u32*)(buf + 0x1e4) = 0x72724161;
    }
    if ((u32)(buf + 0x1e8) & 3) {
        buf[0x1e8] = 0xff;
        buf[0x1e9] = 0xff;
        buf[0x1ea] = 0xff;
        buf[0x1eb] = 0xff;
    } else {
        *(u32*)(buf + 0x1e8) = 0xffffffff;
    }
    if ((u32)(buf + 0x1ec) & 3) {
        buf[0x1ec] = 0xff;
        buf[0x1ed] = 0xff;
        buf[0x1ee] = 0xff;
        buf[0x1ef] = 0xff;
    } else {
        *(u32*)(buf + 0x1ec) = 0xffffffff;
    }
    if ((u32)(buf + 0x1fc) & 3) {
        buf[0x1fc] = 0x0;
        buf[0x1fd] = 0x0;
        buf[0x1fe] = 0x55;
        buf[0x1ff] = 0xaa;
    } else {
        *(u32*)(buf + 0x1fc) = 0x55aa;
    }
    return 0;
}

// TODO: clean up, give variables real names
/* jj_803EC6E8 */
extern s32 fn_8020A710(u8 *, s32, s32);
extern s32 fn_8020AF84(u8 *, s32, s32);
extern void fn_8020B9BC(u8 *, u8 *, s32);
extern s32 fn_8020D250(u8 *);
extern void *fn_803D634C(void *, int, u32);
extern s32 fn_803D6480(const u8 *, const u8 *);

extern u8 lbl_80598098[0x40];
extern u8 lbl_8059FF18[2];
extern u8 lbl_8059FF1C[3];

s32 fn_803EC6E8(u8 *arg0)
{
    u8 sp8[0x10];
    u16 n;
    u8 *p;
    u16 total;
    u8 *dst;
    s32 r;
    s32 t;

    n = 0;
    total = 0;
    if (*(u32 *)(arg0 + 0xE4) & 4) {
        return 0;
    }
    p = arg0 + 4;
    fn_803D634C(sp8, 0, 0x10);
    dst = sp8;
    for (;;) {
        switch ((s8)*p) {
        case 47:
            if (n == 0) {
                if (total == 0) {
                    r = fn_8020D250(lbl_8059FF18);
                    if (r != 0) {
                        return r;
                    }
                }
                p++;
                goto copy;
            }
            if ((s8)p[1] == 0) {
                if (n > 0xC) {
                    return -0x14;
                }
                r = fn_8020A710(sp8, 0x30, 0);
                if (r != 0 && r != -6) {
                    goto done;
                }
                fn_8020B9BC(sp8, arg0 + 0x58, 3);
                return fn_8020D250(lbl_80598098);
            }
            if (n > 0xB) {
                return -0x14;
            }
            if (fn_803D6480(sp8, lbl_8059FF1C) != 0) {
                r = fn_8020AF84(sp8, 0x30, 0);
            } else {
                r = 0;
            }
            if (r == 0 || r == -6 || r == -1) {
                t = fn_8020D250(sp8);
                if (t != 0) {
                    return t;
                }
            } else {
                return r;
            }
            p++;
            fn_803D634C(sp8, 0, 0x10);
            dst = sp8;
            n = 0;
            goto copy;
        case 0:
            if (n > 0xC) {
                return -0x14;
            }
            r = fn_8020A710(sp8, 0x30, 0);
            if (r != 0 && r != -6) {
                goto done;
            }
            fn_8020B9BC(sp8, arg0 + 0x58, 3);
            return fn_8020D250(lbl_80598098);
        default:
        copy:
            *dst = *p;
            n++;
            total++;
            p++;
            dst++;
            break;
        }
    }
done:
    return r;
}

