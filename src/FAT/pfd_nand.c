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
