#include <types.h>
#include <FAT/pf2.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_805639C0[0xa30] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_805643F0[0xd0] __attribute__((aligned(8))) = { 0 };

// This unit's functions call each other out of line, as other FAT units do.
#pragma auto_inline off

// Function bodies below are derived from https://github.com/jj-link/brawl.

extern void fn_803D8E00(void *self, int mode);

void fn_803D80B0(int *arg0) {
    *arg0 &= ~4;
    fn_803D8E00(arg0, 0);
}

extern void fn_803D8E00(void* self, int mode);

void fn_803D80C4(u32* self) {
    *self |= 0x4;
    fn_803D8E00(self, 1);
}

int fn_803D8988(const unsigned int *p)
{
    return (p[0] >> 2) & 1;
}

// The disk-geometry converters of this build, as in VFipdm_disk_convert_*; the
// disk argument is unused.
s32 fn_803D781C(void* p_disk, u32 sector, u32 num_sector, u16 bps, u32* p_block, u32* p_num_block) {
    u32 ratio;
    s32 err = 0;

    *p_block = sector;
    *p_num_block = num_sector;
    if (bps > 0x200) {
        ratio = (bps >> 9) & 0x7F;
        if (ratio == 2) *p_block *= 2;
        else if (ratio == 4) *p_block *= 4;
        else if (ratio == 8) *p_block *= 8;
        if (ratio == 2) *p_num_block *= 2;
        else if (ratio == 4) *p_num_block *= 4;
        else if (ratio == 8) *p_num_block *= 8;
    } else if (bps < 0x200) {
        err = 4;
    }
    return err;
}

s32 fn_803D78CC(void* p_disk, u32 block, u32 num_block, u16 bps, u32* p_sector, u32* p_num_sector) {
    u32 ratio;
    s32 err = 0;

    *p_sector = block;
    *p_num_sector = num_block;
    if (bps > 0x200) {
        ratio = (bps >> 9) & 0x7F;
        if (ratio == 2) *p_sector >>= 1;
        else if (ratio == 4) *p_sector >>= 2;
        else if (ratio == 8) *p_sector >>= 3;

        if (ratio == 2) *p_num_sector >>= 1;
        else if (ratio == 4) *p_num_sector >>= 2;
        else if (ratio == 8) *p_num_sector >>= 3;
    } else if (bps < 0x200) {
        err = 4;
    }
    return err;
}

// HYPOTHESIS: layout of the disk and partition handles of this build, from the offsets the code uses.
typedef struct PF2_PDM_CALLBACK {
    u32 func;
    u32 arg;
} PF2_PDM_CALLBACK;

typedef struct PF2_PDM_DISK PF2_PDM_DISK;

// HYPOTHESIS: driver function table of a disk, offsets from the indirect calls.
typedef struct PF2_PDM_DRIVER {
    s32 (*init)(PF2_PDM_DISK* p_disk);
    u32 unk4;
    u32 unk8;
    s32 (*finalize)(PF2_PDM_DISK* p_disk);
    s32 (*format)(PF2_PDM_DISK* p_disk, const u8* param);
} PF2_PDM_DRIVER;

typedef struct PF2_PDM_PARTITION {
    u32 status;
    struct PF2_PDM_DISK* p_disk;
    u32 unk8;
    u32 start_sector;
    u32 end_sector;
    s32 driver_last_error;
    u32 unk18;
    u32 unk1C;
    PF2_PDM_CALLBACK callback;
} PF2_PDM_PARTITION;

struct PF2_PDM_DISK {
    u32 status;
    PF2_PDM_DRIVER* p_driver;
    u32 unk8;
    s16 init_count;
    s16 mount_count;
    u32 unk10;
    s32 semaphore;
    PDM_DISK_INFO media_attribute;
    struct PF2_PDM_PARTITION* p_cur_part;
};

void pdm_part_set_driver_error_code(PF2_PDM_PARTITION* p_part, s32 error_code) {
    p_part->driver_last_error = error_code;
}

s32 pdm_part_get_driver_error_code(PF2_PDM_PARTITION* p_part) {
    return p_part->driver_last_error;
}

s32 pdm_disk_set_disk(PF2_PDM_DISK* p_disk, PF2_PDM_PARTITION* p_part) {
    p_disk->p_cur_part = p_part;
    return 0;
}

s32 pdm_part_register_callback(PF2_PDM_PARTITION* p_part, PF2_PDM_CALLBACK* p_callback) {
    p_part->callback.func = p_callback->func;
    p_part->callback.arg = p_callback->arg;
    return 0;
}

s32 pdm_part_unregister_callback(PF2_PDM_PARTITION* p_part) {
    p_part->callback.func = 0;
    p_part->callback.arg = 0;
    return 0;
}

s32 pdm_disk_get_media_bps(PF2_PDM_DISK* p_disk, u16* p_bps) {
    *p_bps = p_disk->media_attribute.bytes_per_sector;
    return 0;
}

s32 pdm_disk_get_media_attribute(PF2_PDM_DISK* p_disk, PDM_DISK_INFO* p_attribute) {
    *p_attribute = p_disk->media_attribute;
    return 0;
}

s32 pdm_disk_init_disk_manager(void) {
    VFipf_memset(lbl_805639C0, 0, 0xa2c);
    return 0;
}

s32 pdm_part_is_boot_sector(const u8* buf) {
    s32 is_boot = 1;
    u16 bps;

    if (!((buf[0] == 0xEB && buf[2] == 0x90) || buf[0] == 0xE9)) {
        is_boot = 0;
    }
    if (!(buf[0x1FE] == 0x55 && buf[0x1FF] == 0xAA)) {
        is_boot = 0;
    }
    if (((u32)(buf + 0xB)) & 1) {
        bps = buf[0xB] | (buf[0xC] << 8);
    } else {
        u16 raw = *(const u16*)(buf + 0xC);
        bps = ((raw >> 8) & 0xFF) | ((raw << 8) & 0xFF00);
    }
    if (!(bps == 0x200 || bps == 0x400 || bps == 0x800 || bps == 0x1000)) {
        is_boot = 0;
    }
    if (!(buf[0xD] == 1 || buf[0xD] == 2 || buf[0xD] == 4 || buf[0xD] == 8 || buf[0xD] == 0x10 || buf[0xD] == 0x20 ||
          buf[0xD] == 0x40 || buf[0xD] == 0x80)) {
        is_boot = 0;
    }
    if (!(buf[0x15] == 0xF0 || buf[0x15] == 0xF8 || buf[0x15] == 0xF9 || buf[0x15] == 0xFA || buf[0x15] == 0xFB ||
          buf[0x15] == 0xFC || buf[0x15] == 0xFD || buf[0x15] == 0xFE || buf[0x15] == 0xFF)) {
        is_boot = 0;
    }
    return is_boot;
}

s32 pdm_disk_mount(PF2_PDM_DISK* p_disk);
s32 pdm_part_get_start_sector(PF2_PDM_PARTITION* p_part);
s32 pdm_disk_physical_read(PF2_PDM_DISK* p_disk, u8* buf, u32 psector, u32 num_sector, u16 bps, u32* p_num_success);

s32 pdm_disk_format(PF2_PDM_DISK* p_disk, const u8* param) {
    s32 driver_err;

    if (fn_803EC3D4(p_disk->semaphore) != 0) {
        return 0x14;
    }
    driver_err = p_disk->p_driver->format(p_disk, param);
    if (fn_803EC418(p_disk->semaphore) != 0) {
        return 0x14;
    }
    if (driver_err != 0) {
        pdm_part_set_driver_error_code(p_disk->p_cur_part, driver_err);
        return 0x15;
    }
    return 0;
}

s32 pdm_part_format(PF2_PDM_PARTITION* p_part, const u8* param) {
    s32 err;

    err = pdm_disk_set_disk(p_part->p_disk, p_part);
    if (err != 0) {
        return err;
    }
    err = pdm_disk_format(p_part->p_disk, param);
    return err != 0 ? err : 0;
}

s32 pdm_disk_init(PF2_PDM_DISK* p_disk) {
    s32 driver_err;

    if (p_disk->init_count == 0) {
        if (fn_803EC3D4(p_disk->semaphore) != 0) {
            return 0x14;
        }
        driver_err = p_disk->p_driver->init(p_disk);
        if (fn_803EC418(p_disk->semaphore) != 0) {
            return 0x14;
        }
        if (driver_err != 0) {
            pdm_part_set_driver_error_code(p_disk->p_cur_part, driver_err);
            return 0x15;
        }
    }
    p_disk->init_count++;
    return 0;
}

s32 pdm_disk_unmount(PF2_PDM_DISK* p_disk, u32 mode) {
    s32 err = 0;

    if (p_disk->mount_count == 1) {
        if (fn_803EC3D4(p_disk->semaphore) != 0) {
            return 0x14;
        }
        err = p_disk->p_driver->finalize(p_disk);
        if (fn_803EC418(p_disk->semaphore) != 0) {
            return 0x14;
        }
        if (err != 0) {
            pdm_part_set_driver_error_code(p_disk->p_cur_part, err);
            err = 0x15;
        }
    } else if (p_disk->mount_count == 0) {
        return 0xe;
    }
    if (err == 0 || mode == 1) {
        p_disk->mount_count--;
    }
    return err;
}

s32 pdm_part_init(PF2_PDM_PARTITION* p_part) {
    s32 err;

    if (!(p_part->status & 4)) {
        err = pdm_disk_set_disk(p_part->p_disk, p_part);
        if (err != 0) {
            return err;
        }
        err = pdm_disk_init(p_part->p_disk);
        if (err != 0) {
            return err;
        }
    } else {
        return 0xb;
    }
    p_part->status |= 4;
    return 0;
}

s32 pdm_part_mount(PF2_PDM_PARTITION* p_part) {
    s32 err;

    if (!(p_part->status & 8)) {
        err = pdm_disk_set_disk(p_part->p_disk, p_part);
        if (err != 0) {
            return err;
        }
        err = pdm_disk_mount(p_part->p_disk);
        if (err != 0) {
            return err;
        }
        err = pdm_part_get_start_sector(p_part);
        if (err != 0) {
            pdm_disk_unmount(p_part->p_disk, 0);
            return err;
        }
    } else {
        return 0xc;
    }
    p_part->status = (p_part->status & ~2) | 8;
    return 0;
}

s32 pdm_part_unmount(PF2_PDM_PARTITION* p_part, u32 mode) {
    s32 err = 0;
    s32 result;

    if (p_part->status & 8) {
        result = pdm_disk_set_disk(p_part->p_disk, p_part);
        if (result != 0) {
            return result;
        }
        result = pdm_disk_unmount(p_part->p_disk, mode);
        if (result != 0) {
            err = result;
        }
    } else {
        err = 0xe;
    }
    if (err == 0 || mode == 1) {
        p_part->status &= ~2;
        p_part->status &= ~8;
    }
    return err;
}

u32 pdm_part_chg_ltop(PF2_PDM_PARTITION* p_part, u32 lsector, u16 lbps);

s32 pdm_part_logical_read(PF2_PDM_PARTITION* p_part, u8* buf, u32 lsector, u32 num_sector, u16 bps, u32* p_num_success) {
    s32 err;
    u32 end_sector;
    u32 total;

    if ((p_part->status & 8) != 8 || (p_part->status & 2) == 2) {
        return 0xf;
    }
    err = pdm_disk_set_disk(p_part->p_disk, p_part);
    if (err != 0) {
        return err;
    }
    end_sector = p_part->end_sector;
    if (end_sector <= lsector) {
        return 0x12;
    }
    total = lsector + num_sector;
    if (end_sector < total) {
        num_sector -= total - end_sector;
    }
    err = pdm_disk_physical_read(p_part->p_disk, buf, pdm_part_chg_ltop(p_part, lsector, bps), num_sector, bps, p_num_success);
    if (err != 0) {
        return err;
    }
    return 0;
}
