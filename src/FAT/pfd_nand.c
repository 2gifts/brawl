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

// TODO: clean up, give variables real names
/* jj_803ECB5C */
extern void* fn_803D634C(void* dest, int value, u32 size);

#define SWAP16(v) ((u16)((((v) & 0xFF) << 8) | (((v) >> 8) & 0xFF)))
#define SWAP32(v) ((((v) << 24) | (((v) << 8) & 0xFF0000)) | (((v) >> 24) | (((v) >> 8) & 0xFF00)))

s32 fn_803ECB5C(u8* vol, u8* buf, u32* outType, s32 sectors)
{
    u32 nsec;
    u32 spf;
    s32 type;
    u32 tmp;

    if (sectors != 0) {
        nsec = sectors;
    } else {
        nsec = *(u32*)(vol + 0x54) >> 9;
    }

    if (nsec < 0xFF5) {
        type = 0;
        spf = ((((nsec << 2) - nsec) >> 1) + 0x1FF) >> 9;
    } else if (nsec < 0xFFF5) {
        type = 1;
        spf = ((nsec << 1) + 0x1FF) >> 9;
    } else {
        type = 2;
        spf = ((nsec << 2) + 0x1FF) >> 9;
    }

    fn_803D634C(buf, 0, 0x200);

    buf[0] = 0xEB;
    buf[2] = 0x90;
    buf[0x1FE] = 0x55;
    buf[0x1FF] = 0xAA;

    if ((u32)(buf + 0x18) & 1) {
        buf[0x18] = 0x3F;
        buf[0x19] = 0;
    } else {
        *(u16*)(buf + 0x18) = 0x3F00;
    }
    if ((u32)(buf + 0x1A) & 1) {
        buf[0x1A] = 0xFF;
        buf[0x1B] = 0;
    } else {
        *(u16*)(buf + 0x1A) = 0xFF00;
    }
    if ((u32)(buf + 0xB) & 1) {
        buf[0xB] = 0;
        buf[0xC] = 2;
    } else {
        *(u16*)(buf + 0xC) = 2;
    }
    buf[0xD] = 1;
    if ((u32)(buf + 0xE) & 1) {
        buf[0xE] = 1;
        buf[0xF] = 0;
    } else {
        *(u16*)(buf + 0xE) = 0x100;
    }
    tmp = (u32)(buf + 0x11) & 1;
    buf[0x10] = 2;
    if (tmp != 0) {
        buf[0x11] = 0x80;
        buf[0x12] = 0;
    } else {
        *(u16*)(buf + 0x12) = 0x8000;
    }
    buf[0x15] = 0xF0;

    if (nsec < 0x10000) {
        if ((u32)(buf + 0x13) & 1) {
            buf[0x13] = nsec;
            buf[0x14] = nsec >> 8;
        } else {
            *(u16*)(buf + 0x14) = SWAP16(nsec);
        }
        if ((u32)(buf + 0x20) & 3) {
            buf[0x20] = 0;
            buf[0x21] = 0;
            buf[0x22] = 0;
            buf[0x23] = 0;
        } else {
            *(u32*)(buf + 0x20) = 0;
        }
    } else {
        if ((u32)(buf + 0x13) & 1) {
            buf[0x13] = 0;
            buf[0x14] = 0;
        } else {
            *(u16*)(buf + 0x14) = 0;
        }
        if ((u32)(buf + 0x20) & 3) {
            buf[0x20] = nsec;
            buf[0x21] = nsec >> 8;
            buf[0x22] = nsec >> 16;
            buf[0x23] = nsec >> 24;
        } else {
            *(u32*)(buf + 0x20) = SWAP32(nsec);
        }
    }

    if (type == 2) {
        if (tmp != 0) {
            buf[0x11] = 0;
            buf[0x12] = 0;
        } else {
            *(u16*)(buf + 0x12) = 0;
        }
        if ((u32)(buf + 0x24) & 3) {
            buf[0x24] = spf;
            buf[0x25] = spf >> 8;
            buf[0x26] = spf >> 16;
            buf[0x27] = spf >> 24;
        } else {
            *(u32*)(buf + 0x24) = SWAP32(spf);
        }
        if ((u32)(buf + 0x28) & 1) {
            buf[0x28] = 0;
            buf[0x29] = 0;
        } else {
            *(u16*)(buf + 0x28) = 0;
        }
        if ((u32)(buf + 0x2A) & 1) {
            buf[0x2A] = 0;
            buf[0x2B] = 0;
        } else {
            *(u16*)(buf + 0x2A) = 0;
        }
        if ((u32)(buf + 0x2C) & 3) {
            buf[0x2C] = 2;
            buf[0x2D] = 0;
            buf[0x2E] = 0;
            buf[0x2F] = 0;
        } else {
            *(u32*)(buf + 0x2C) = 0x02000000;
        }
        if ((u32)(buf + 0x30) & 1) {
            buf[0x30] = 1;
            buf[0x31] = 0;
        } else {
            *(u16*)(buf + 0x30) = 0x100;
        }
        buf[0x41] = 0;
        buf[0x42] = 0x29;
        if ((u32)(buf + 0x43) & 3) {
            buf[0x43] = 0x34;
            buf[0x44] = 0x12;
            buf[0x45] = 0;
            buf[0x46] = 0;
        } else {
            *(u32*)(buf + 0x4C) = 0x34120000;
        }
    } else {
        if (tmp != 0) {
            buf[0x11] = 0x80;
            buf[0x12] = 0;
        } else {
            *(u16*)(buf + 0x12) = 0x8000;
        }
        if ((u32)(buf + 0x16) & 1) {
            buf[0x16] = spf;
            buf[0x17] = spf >> 8;
        } else {
            *(u16*)(buf + 0x16) = SWAP16(spf);
        }
        buf[0x25] = 0;
        buf[0x26] = 0x29;
        if ((u32)(buf + 0x27) & 3) {
            buf[0x27] = 0x34;
            buf[0x28] = 0x12;
            buf[0x29] = 0;
            buf[0x2A] = 0;
        } else {
            *(u32*)(buf + 0x30) = 0x34120000;
        }
    }

    *outType = type;
    return 0;
}

