#include <types.h>
#include <FAT/fat_vf.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_80563798[0x1a8] __attribute__((aligned(8))) = { 0 };

// Function bodies below are derived from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
#pragma auto_inline off

void fn_803C9168(int, int, int, int, int);

void fn_803C9ECC(int a, int b, int c) {
    fn_803C9168(a, b, c, 3, -10);
}

s32 nanddrv_format(PDM_DISK* p_disk, const u8* param) {
    return p_disk ? 0 : -20;
}

s32 nanddrv_pread(PDM_DISK* p_disk, u8* p_buf, u32 block, u32 num_blocks, u32* p_num_success) {
    *p_num_success = 0;
    if (p_disk == NULL || p_buf == NULL || num_blocks == 0 || p_num_success == NULL) {
        return -20;
    }
    return fn_803CACCC(num_blocks, p_buf, block, 0x200, p_num_success, p_disk);
}

s32 nanddrv_BuildUpFSInfoSector(u8* buf) {
    return dCommon_MakeFsInfoSec(buf, 0x200) == 0 ? 0 : -0x15;
}

s32 nanddrv_init(PDM_DISK* p_disk) {
    if (p_disk == NULL) {
        return -0x14;
    }
    dCommon_setFatTypeToDisk(p_disk, 1);
    VFipdm_disk_notify_media_insert(p_disk);
    return 0;
}

s32 fn_803CAA94(PDM_DISK* p_disk, const u8* p_buf, u32 block, u32 num_blocks, u32* p_num_success) {
    *p_num_success = 0;
    if (p_disk == NULL || p_buf == NULL || num_blocks == 0 || p_num_success == NULL) {
        return -20;
    }
    return nanddrv_physical_write(num_blocks, p_buf, block, 0x200, p_num_success, p_disk);
}

s32 fn_803CABD4(PDM_DISK* p_disk) {
    if (p_disk == NULL) {
        return -0x14;
    }
    dCommon_setFatTypeToDisk(p_disk, 1);
    return 0;
}

s32 fn_803CAC0C(PDM_DISK* p_disk, PDM_DISK_INFO* p_disk_info) {
    u32 total;

    if (p_disk == NULL || p_disk_info == NULL) {
        return -0x14;
    }
    total = (dCommon_getFileSizeFromDisk(p_disk) - 0x1F) >> 9;
    p_disk_info->total_sectors = total;
    p_disk_info->cylinders = total / 255 / 63 >> 9;
    p_disk_info->heads = 0xFF;
    p_disk_info->sectors_per_track = 0x3F;
    p_disk_info->bytes_per_sector = 0x200;
    p_disk_info->media_attr = 0;
    p_disk_info->format_param = NULL;
    return 0;
}
