#include <types.h>
#include <FAT/pf2.h>

// Function bodies below are derived from https://github.com/jj-link/brawl.

u32 PFFAT_GetSectorSpecified(PF_FFD* p_ffd, u32 index, u32 may_allocate, u32* p_cluster) {
    u32 err = fn_803E12B4(p_ffd, index, may_allocate ? 1 : 0, 0, p_cluster);

    if (err != 0) {
        return err;
    }
    return 0;
}

int PFFAT_GetSectorAllocated(PF_FFD* p_ffd, u32 index, u32 size, u32* p_cluster) {
    int err = fn_803E12B4(p_ffd, index, 2, size, p_cluster);

    if (err != 0) {
        return err;
    }
    return 0;
}

void PFFAT_InitHint(PF_FAT_HINT* p_hint) {
    p_hint->chain_index = 0;
    p_hint->cluster = 0;
    p_hint->file_version = 0;
}

int PFFAT_ResetFFD(PF_FFD* p_ffd, u32* p_start_cluster) {
    p_ffd->p_start_cluster = p_start_cluster;
    p_ffd->file_version = 1;
    if (p_ffd->p_hint != NULL) {
        p_ffd->p_hint->file_version = 0;
    }
    p_ffd->last_access_cluster.cluster = 0;
    p_ffd->last_access_cluster.chain_index = 0;
    p_ffd->last_cluster.num_last_cluster = 0;
    p_ffd->last_cluster.max_chain_index = 0;
    p_ffd->cluster_link.buffer = NULL;
    return 0;
}

// PFFAT_InitFFD; the FFD layout is the VF one.
int PFFAT_InitFFD(PF_FFD* p_ffd, PF_FAT_HINT* p_hint, PF_VOLUME* p_vol, u32* p_start_cluster) {
    p_ffd->p_hint = p_hint;
    p_ffd->p_vol = p_vol;
    p_ffd->p_start_cluster = p_start_cluster;
    p_ffd->file_version = 1;
    if (p_hint != NULL) {
        p_hint->file_version = 0;
    }
    p_ffd->last_access_cluster.cluster = 0;
    p_ffd->last_access_cluster.chain_index = 0;
    p_ffd->last_cluster.num_last_cluster = 0;
    p_ffd->last_cluster.max_chain_index = 0;
    p_ffd->cluster_link.buffer = NULL;
    return 0;
}

int PFFAT_FinalizeFFD(PF_FFD* p_ffd) {
    p_ffd->cluster_link.buffer = NULL;
    return 0;
}
