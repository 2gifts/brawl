#include <types.h>
#include <FAT/pf2.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_805639C0[0xa30] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_805643F0[0xd0] __attribute__((aligned(8))) = { 0 };

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

typedef struct PF2_PDM_PARTITION {
    u32 status;
    struct PF2_PDM_DISK* p_disk;
    u32 unk8;
    u32 unkC;
    u32 unk10;
    s32 driver_last_error;
    u32 unk18;
    u32 unk1C;
    PF2_PDM_CALLBACK callback;
} PF2_PDM_PARTITION;

typedef struct PF2_PDM_DISK {
    u32 status;
    u32 unk4[5];
    PDM_DISK_INFO media_attribute;
    struct PF2_PDM_PARTITION* p_cur_part;
} PF2_PDM_DISK;

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
