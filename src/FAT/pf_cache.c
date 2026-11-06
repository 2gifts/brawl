#include <types.h>

// HYPOTHESIS: minimal views of PF_VOLUME and its cache; offsets taken from the original code.
struct PF_CACHE_PAGE;

// HYPOTHESIS: PF_CACHE_PAGE layout, modeled on the VF library's cache page.
struct PF_CACHE_PAGE {
    u16 stat;
    u16 option;
    u8* buffer;
    u8* p_buf;
    u8* p_mod_sbuf;
    u8* p_mod_ebuf;
    u32 size;
    u32 sector;
    void* signature;
    struct PF_CACHE_PAGE* p_next;
    struct PF_CACHE_PAGE* p_prev;
};

struct PF_VOLUME {
    union {
        u8 _pad0[8];
        u16 bytes_per_sector;
    };
    u32 total_sectors;
    union {
        u8 _padC[0x1628 - 0xC];
        struct {
            u8 _pad_a[0x14];
            u8 log2_bytes_per_sector;
            u8 _pad_b[0x1624 - 0x21];
            u32 cache_mode;
        };
    };
    u16 num_fat_pages;
    u16 num_data_pages;
    struct PF_CACHE_PAGE* pages;
    struct PF_CACHE_PAGE* p_current_fat;
    struct PF_CACHE_PAGE* p_current_data;
    u8 (*buffers)[512];
    u32 fat_buff_size;
    u32 data_buff_size;
    // HYPOTHESIS: signature value stored into pages written with set_sig.
    void* signature;
};

s32 PFCACHE_DoWritePage(struct PF_VOLUME* p_vol, struct PF_CACHE_PAGE** pp_head, struct PF_CACHE_PAGE* p_page, u32 set_sig);
s32 PFCACHE_DoReadPage(struct PF_VOLUME* p_vol, struct PF_CACHE_PAGE** pp_head, u32 sector, struct PF_CACHE_PAGE** pp_page, u32 set_sig);
s32 PFCACHE_DoReadPageAndFlushIfNeeded(struct PF_VOLUME* p_vol, struct PF_CACHE_PAGE** pp_head, u32 sector, struct PF_CACHE_PAGE** pp_page, u32 set_sig);
s32 PFCACHE_DoReadNumSector(struct PF_VOLUME* p_vol, struct PF_CACHE_PAGE** pp_head, u8* p_buf, u32 sector, u32 num_sector, u32* p_num_success);
s32 PFCACHE_DoWriteSector(struct PF_VOLUME* p_vol, struct PF_CACHE_PAGE** pp_head, const u8* p_buf, u32 sector);
s32 PFCACHE_DoFlushCache(struct PF_VOLUME* p_vol, struct PF_CACHE_PAGE* p_head);
struct PF_CACHE_PAGE* PFCACHE_SearchForPage(struct PF_VOLUME* p_vol, struct PF_CACHE_PAGE* p_head, u32 sector);

void PFCACHE_SetCache(struct PF_VOLUME* p_vol, struct PF_CACHE_PAGE* p_cache_page, u8 (*p_cache_buf)[512], u16 num_fat_pages, u16 num_data_pages) {
    p_vol->pages = p_cache_page;
    p_vol->buffers = p_cache_buf;
    p_vol->num_fat_pages = num_fat_pages;
    p_vol->num_data_pages = num_data_pages;
}

void PFCACHE_SetFATBufferSize(struct PF_VOLUME* p_vol, u32 size) {
    if (size != 0) {
        p_vol->fat_buff_size = size;
    }
}

void PFCACHE_SetDataBufferSize(struct PF_VOLUME* p_vol, u32 size) {
    if (size != 0) {
        p_vol->data_buff_size = size;
    }
}

struct PF_CACHE_PAGE* PFCACHE_SearchDataCache(struct PF_VOLUME* p_vol, u32 sector) {
    return PFCACHE_SearchForPage(p_vol, p_vol->p_current_data, sector);
}

s32 PFCACHE_WriteFATPage(struct PF_VOLUME* p_vol, struct PF_CACHE_PAGE* p_page) {
    s32 err = PFCACHE_DoWritePage(p_vol, &p_vol->p_current_fat, p_page, 0);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFCACHE_WriteDataPage(struct PF_VOLUME* p_vol, struct PF_CACHE_PAGE* p_page, u32 set_sig) {
    s32 err = PFCACHE_DoWritePage(p_vol, &p_vol->p_current_data, p_page, set_sig);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFCACHE_ReadFATPage(struct PF_VOLUME* p_vol, u32 sector, struct PF_CACHE_PAGE** pp_page) {
    s32 err = PFCACHE_DoReadPage(p_vol, &p_vol->p_current_fat, sector, pp_page, 0);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFCACHE_ReadDataPage(struct PF_VOLUME* p_vol, u32 sector, struct PF_CACHE_PAGE** pp_page, u32 set_sig) {
    s32 err = PFCACHE_DoReadPage(p_vol, &p_vol->p_current_data, sector, pp_page, set_sig);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFCACHE_ReadDataPageAndFlushIfNeeded(struct PF_VOLUME* p_vol, u32 sector, struct PF_CACHE_PAGE** pp_page, u32 set_sig) {
    s32 err = PFCACHE_DoReadPageAndFlushIfNeeded(p_vol, &p_vol->p_current_data, sector, pp_page, set_sig);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFCACHE_WriteFATSectorAndFreeIfNeeded(struct PF_VOLUME* p_vol, const u8* p_buf, u32 sector) {
    s32 err;

    if (sector >= p_vol->total_sectors) {
        return 0x10;
    }

    err = PFCACHE_DoWriteSector(p_vol, &p_vol->p_current_fat, p_buf, sector);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFCACHE_FlushAllCaches(struct PF_VOLUME* p_vol) {
    s32 err;

    err = PFCACHE_DoFlushCache(p_vol, p_vol->p_current_fat);
    if (err != 0) {
        return err;
    }
    err = PFCACHE_DoFlushCache(p_vol, p_vol->p_current_data);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFCACHE_ReadDataNumSector(struct PF_VOLUME* p_vol, u8* p_buf, u32 sector, u32 num_sector, u32* p_num_success) {
    s32 err;

    if (sector >= p_vol->total_sectors) {
        return 0x10;
    }

    err = PFCACHE_DoReadNumSector(p_vol, &p_vol->p_current_data, p_buf, sector, num_sector, p_num_success);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFDRV_lwrite(struct PF_VOLUME* p_vol, const u8* buf, u32 sector, u32 num_sectors, u32* p_num_success);
s32 PFCACHE_InitPageList(struct PF_VOLUME* p_vol, struct PF_CACHE_PAGE** pp_head, struct PF_CACHE_PAGE* pages, u8 (*buffers)[512], u16 num, u32 size, u32 is_fat);
s32 PFCACHE_DoAllocatePage(struct PF_VOLUME* p_vol, struct PF_CACHE_PAGE** pp_head, u32 sector, struct PF_CACHE_PAGE** pp_page, u32* p_is_hit);
s32 PFCACHE_DoWriteNumSectorAndFreeIfNeeded(struct PF_VOLUME* p_vol, struct PF_CACHE_PAGE** pp_head, const u8* p_buf, u32 sector, u32 num_sector, u32* p_num_success);
s32 PFCACHE_FlushPageIfNeeded(struct PF_VOLUME* p_vol, struct PF_CACHE_PAGE* p_page);
void* VFipf_memset(void* dst, s32 c, u32 length);
void* VFipf_memcpy(void* dst, const void* src, u32 length);

struct PF_CACHE_PAGE* PFCACHE_SearchForPage(struct PF_VOLUME* p_vol, struct PF_CACHE_PAGE* p_head, u32 sector) {
    struct PF_CACHE_PAGE* p_page;

    if (sector == 0xFFFFFFFF) {
        return 0;
    }
    if (!(p_head->stat & 1)) {
        return 0;
    }
    if (p_head->sector <= sector && p_head->sector + p_head->size - 1 >= sector) {
        p_head->p_buf = p_head->buffer + ((sector - p_head->sector) << p_vol->log2_bytes_per_sector);
        return p_head;
    }
    for (p_page = p_head->p_next; p_page != p_head; p_page = p_page->p_next) {
        if (!(p_page->stat & 1)) {
            return 0;
        }
        if (p_page->sector <= sector && p_page->sector + p_page->size - 1 >= sector) {
            p_page->p_buf = p_page->buffer + ((sector - p_page->sector) << p_vol->log2_bytes_per_sector);
            return p_page;
        }
    }
    return 0;
}

u32 PFCACHE_SearchForFreePage(struct PF_CACHE_PAGE* p_head, struct PF_CACHE_PAGE** pp_page) {
    struct PF_CACHE_PAGE* p_page = p_head->p_prev;

    while (p_page != p_head) {
        if (!(p_page->stat & 1)) {
            *pp_page = p_page;
            return 1;
        }
        if (p_page->sector != 0xFFFFFFFF) {
            *pp_page = p_page;
            return 0;
        }
        p_page = p_page->p_prev;
    }
    if (!(p_page->stat & 1)) {
        *pp_page = p_page;
        return 1;
    }
    if (p_page->sector != 0xFFFFFFFF) {
        *pp_page = p_page;
        return 0;
    }
    *pp_page = 0;
    return 0;
}

s32 PFCACHE_FlushPageIfNeeded(struct PF_VOLUME* p_vol, struct PF_CACHE_PAGE* p_page) {
    u32 num_success;
    u8* p_mod_sbuf;
    u32 num_sector;
    u32 sector;
    s32 err;

    if (p_page != 0 && (p_page->stat & 2)) {
        if (p_page->sector != 0xFFFFFFFF) {
            p_mod_sbuf = p_page->p_mod_sbuf;
            if (p_mod_sbuf == 0) {
                sector = p_page->sector;
                num_sector = p_page->size;
                err = PFDRV_lwrite(p_vol, p_page->buffer, sector, num_sector, &num_success);
            } else {
                sector = p_page->sector + ((u32)(p_mod_sbuf - p_page->buffer) >> p_vol->log2_bytes_per_sector);
                num_sector = ((u32)p_page->p_mod_ebuf >> p_vol->log2_bytes_per_sector) - ((u32)p_mod_sbuf >> p_vol->log2_bytes_per_sector) + 1;
                err = PFDRV_lwrite(p_vol, p_mod_sbuf, sector, num_sector, &num_success);
            }
            if (err != 0) {
                return err;
            }
            if (num_success != num_sector) {
                return 0x11;
            }
            p_page->p_mod_sbuf = 0;
            p_page->p_mod_ebuf = 0;
            p_page->stat &= ~2;
        }
    }
    return 0;
}

s32 PFCACHE_DoFlushCache(struct PF_VOLUME* p_vol, struct PF_CACHE_PAGE* p_head) {
    struct PF_CACHE_PAGE* p_page;
    s32 err;
    s32 ret;

    p_page = p_head;
    err = 0;

    if (!(p_head->stat & 1)) {
        return 0;
    }
    if ((p_vol->cache_mode & 1) || ((p_vol->cache_mode & 4) && (p_head->stat & 4))) {
        return 0;
    }

    do {
        if (!(p_page->stat & 1)) {
            break;
        }
        ret = PFCACHE_FlushPageIfNeeded(p_vol, p_page);
        if (ret != 0 && err == 0) {
            err = ret;
        }
        p_page = p_page->p_next;
    } while (p_page != p_head);

    return err;
}

s32 PFCACHE_InitCaches(struct PF_VOLUME* p_vol) {
    s32 err;

    p_vol->cache_mode = ((p_vol->cache_mode & ~1) | 2) & ~4;
    VFipf_memset(p_vol->buffers, 0, (p_vol->num_fat_pages + p_vol->num_data_pages) << 9);
    err = PFCACHE_InitPageList(p_vol, &p_vol->p_current_fat, p_vol->pages, p_vol->buffers, p_vol->num_fat_pages, p_vol->fat_buff_size, 1);
    if (err != 0) {
        return err;
    }
    err = PFCACHE_InitPageList(p_vol, &p_vol->p_current_data, p_vol->pages + p_vol->num_fat_pages, p_vol->buffers + p_vol->num_fat_pages, p_vol->num_data_pages, p_vol->data_buff_size, 0);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFCACHE_AllocateFATPage(struct PF_VOLUME* p_vol, u32 sector, struct PF_CACHE_PAGE** pp_page, u32* p_is_hit) {
    s32 err;

    if (p_vol->p_current_fat == 0) {
        p_vol->pages->buffer = (u8*)p_vol->buffers;
        p_vol->pages->p_buf = (u8*)p_vol->buffers;
        *pp_page = p_vol->pages;
        return 0;
    }
    err = PFCACHE_DoAllocatePage(p_vol, &p_vol->p_current_fat, sector, pp_page, p_is_hit);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFCACHE_AllocateDataPage(struct PF_VOLUME* p_vol, u32 sector, struct PF_CACHE_PAGE** pp_page, u32* p_is_hit) {
    s32 err;

    if (p_vol->p_current_data == 0) {
        p_vol->pages->buffer = (u8*)p_vol->buffers;
        p_vol->pages->p_buf = (u8*)p_vol->buffers;
        *pp_page = p_vol->pages;
        return 0;
    }
    err = PFCACHE_DoAllocatePage(p_vol, &p_vol->p_current_data, sector, pp_page, p_is_hit);
    if (err != 0) {
        return err;
    }
    return 0;
}

void PFCACHE_FreeFATPage(struct PF_VOLUME* p_vol, struct PF_CACHE_PAGE* p_page) {
    struct PF_CACHE_PAGE* p_head;
    u16 stat;

    if (p_vol->p_current_fat == 0) {
        return;
    }
    stat = p_page->stat;
    p_page->p_mod_sbuf = 0;
    p_page->stat = stat & 0xFFFC;
    p_page->p_mod_ebuf = 0;
    p_page->sector = 0xFFFFFFFF;
    p_page->signature = 0;
    p_head = p_vol->p_current_fat;
    if (p_page == p_head) {
        p_vol->p_current_fat = p_page->p_next;
        return;
    }
    if (p_page == p_head->p_prev) {
        return;
    }
    p_page->p_prev->p_next = p_page->p_next;
    p_page->p_next->p_prev = p_page->p_prev;
    p_head = p_vol->p_current_fat;
    p_page->p_next = p_head;
    p_page->p_prev = p_vol->p_current_fat->p_prev;
    p_head->p_prev = p_page;
    p_page->p_prev->p_next = p_page;
}

void PFCACHE_FreeDataPage(struct PF_VOLUME* p_vol, struct PF_CACHE_PAGE* p_page) {
    struct PF_CACHE_PAGE* p_head;
    u16 stat;

    if (p_vol->p_current_data == 0) {
        return;
    }
    stat = p_page->stat;
    p_page->p_mod_sbuf = 0;
    p_page->stat = stat & 0xFFFC;
    p_page->p_mod_ebuf = 0;
    p_page->sector = 0xFFFFFFFF;
    p_page->signature = 0;
    p_head = p_vol->p_current_data;
    if (p_page == p_head) {
        p_vol->p_current_data = p_page->p_next;
        return;
    }
    if (p_page == p_head->p_prev) {
        return;
    }
    p_page->p_prev->p_next = p_page->p_next;
    p_page->p_next->p_prev = p_page->p_prev;
    p_head = p_vol->p_current_data;
    p_page->p_next = p_head;
    p_page->p_prev = p_vol->p_current_data->p_prev;
    p_head->p_prev = p_page;
    p_page->p_prev->p_next = p_page;
}

s32 PFCACHE_WriteDataNumSectorAndFreeIfNeeded(struct PF_VOLUME* p_vol, const u8* p_buf, u32 sector, u32 num_sector, u32* p_num_success) {
    s32 err;

    if (sector >= p_vol->total_sectors) {
        return 0x10;
    }
    err = PFCACHE_DoWriteNumSectorAndFreeIfNeeded(p_vol, &p_vol->p_current_data, p_buf, sector, num_sector, p_num_success);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFCACHE_DoWritePage(struct PF_VOLUME* p_vol, struct PF_CACHE_PAGE** pp_head, struct PF_CACHE_PAGE* p_page, u32 set_sig) {
    struct PF_CACHE_PAGE* p_head;
    s32 err;

    p_head = *pp_head;
    if (p_page != p_head) {
        if (p_page == p_head->p_prev) {
            *pp_head = p_page;
        } else {
            p_page->p_prev->p_next = p_page->p_next;
            p_page->p_next->p_prev = p_page->p_prev;
            p_head = *pp_head;
            p_page->p_next = p_head;
            p_page->p_prev = (*pp_head)->p_prev;
            p_head->p_prev = p_page;
            p_page->p_prev->p_next = p_page;
            *pp_head = p_page;
        }
    }
    p_page->stat |= 2;
    if (p_page->p_mod_sbuf == 0) {
        p_page->p_mod_sbuf = p_page->p_buf;
        p_page->p_mod_ebuf = p_page->p_buf;
    } else if (p_page->p_buf < p_page->p_mod_sbuf) {
        p_page->p_mod_sbuf = p_page->p_buf;
    } else if (p_page->p_buf > p_page->p_mod_ebuf) {
        p_page->p_mod_ebuf = p_page->p_buf;
    }
    if (set_sig != 0) {
        p_page->signature = p_vol->signature;
    } else {
        p_page->signature = 0;
    }
    if ((p_vol->cache_mode & 1) || ((p_vol->cache_mode & 4) && (p_page->stat & 4))) {
        err = PFCACHE_FlushPageIfNeeded(p_vol, p_page);
        if (err != 0) {
            return err;
        }
    }
    return 0;
}

s32 PFCACHE_DoWriteSector(struct PF_VOLUME* p_vol, struct PF_CACHE_PAGE** pp_head, const u8* p_buf, u32 sector) {
    struct PF_CACHE_PAGE* p_page;
    u32 num_success;
    s32 err;

    p_page = PFCACHE_SearchForPage(p_vol, *pp_head, sector);
    if (p_page != 0) {
        VFipf_memcpy(p_page->p_buf, p_buf, p_vol->bytes_per_sector);
        p_page->stat |= 2;
        if (p_page->p_mod_sbuf == 0) {
            p_page->p_mod_sbuf = p_page->p_buf;
            p_page->p_mod_ebuf = p_page->p_buf;
        } else if (p_page->p_buf < p_page->p_mod_sbuf) {
            p_page->p_mod_sbuf = p_page->p_buf;
        } else if (p_page->p_buf > p_page->p_mod_ebuf) {
            p_page->p_mod_ebuf = p_page->p_buf;
        }
        err = PFCACHE_FlushPageIfNeeded(p_vol, p_page);
        if (err != 0) {
            return err;
        }
    } else {
        err = PFDRV_lwrite(p_vol, p_buf, sector, 1, &num_success);
        if (err != 0) {
            return err;
        }
        if (num_success != 1) {
            return 0x11;
        }
    }
    return 0;
}
