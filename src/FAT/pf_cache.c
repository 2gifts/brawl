#include <types.h>

// HYPOTHESIS: minimal views of PF_VOLUME and its cache; offsets taken from the original code.
struct PF_CACHE_PAGE;

struct PF_VOLUME {
    u8 _pad0[8];
    u32 total_sectors;
    u8 _padC[0x1628 - 0xC];
    u16 num_fat_pages;
    u16 num_data_pages;
    struct PF_CACHE_PAGE* pages;
    struct PF_CACHE_PAGE* p_current_fat;
    struct PF_CACHE_PAGE* p_current_data;
    u8 (*buffers)[512];
    u32 fat_buff_size;
    u32 data_buff_size;
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
