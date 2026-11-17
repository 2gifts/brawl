#include <types.h>
#include <it/it_gen_archive.h>
#include <so/so_archive_db.h>

bool itGenArchive::isComp() {
    u32 archiveId = m_archiveId;

    if (archiveId == 0xFFFF) {
        return false;
    }
    return soArchiveDb::getManager(2)->getLoadResult(archiveId, nullptr) != 0;
}

bool itGenArchive::isFailed()
{
    u32 archiveId = m_archiveId;
    if (archiveId == 0xFFFF) {
        return true;
    }
    return soArchiveDb::getManager(2)->getLoadResult(archiveId, nullptr) == 2;
}

bool itGenArchive::isRemovable() {
    u32 archiveId = m_archiveId;

    if (archiveId == 0xFFFF) {
        return true;
    }
    return soArchiveDb::getManager(2)->getLoadResult(archiveId, nullptr) != 0;
}

// HYPOTHESIS: the "genParamSet" table of the archive: one list of parameter records for each of the five levels.
struct itGenParamSet {
    struct Level {
        itGenParam* m_params;
        u32 m_count;
    } m_levels[5];
};

itGenParam* itGenArchive::getGenParam(int id, int level) {
    u32 archiveId = m_archiveId;
    if (archiveId == 0xFFFF) {
        return NULL;
    }
    itGenParamSet* set = static_cast<itGenParamSet*>(soArchiveDb::getManager(2)->getPublicAddress(archiveId, "genParamSet", 0, 0, -1));
    if (set == NULL) {
        return NULL;
    }
    if (level < 0 || level >= 5) {
        return NULL;
    }
    itGenParamSet::Level& entry = set->m_levels[level];
    for (u32 i = 0; i < entry.m_count; i++) {
        if ((u32)entry.m_params[i].m_id == (u32)id) {
            return &entry.m_params[i];
        }
    }
    return NULL;
}

itGenParam* itGenArchive::getGenParamFromIndex(u32 index, u32 level) {
    u32 archiveId = m_archiveId;
    if (archiveId == 0xFFFF) {
        return NULL;
    }
    itGenParamSet* set = static_cast<itGenParamSet*>(soArchiveDb::getManager(2)->getPublicAddress(archiveId, "genParamSet", 0, 0, -1));
    if (set == NULL) {
        return NULL;
    }
    if ((int)level < 0 || (int)level >= 5) {
        return NULL;
    }
    itGenParamSet::Level& entry = set->m_levels[level];
    if (index >= entry.m_count) {
        return NULL;
    }
    return &entry.m_params[index];
}

u32 itGenArchive::getGenParamNum(u32 level) {
    u32 archiveId = m_archiveId;
    if (archiveId == 0xFFFF) {
        return 0;
    }
    itGenParamSet* set = static_cast<itGenParamSet*>(soArchiveDb::getManager(2)->getPublicAddress(archiveId, "genParamSet", 0, 0, -1));
    if (set == NULL) {
        return 0;
    }
    if ((int)level < 0 || (int)level >= 5) {
        return 0;
    }
    itGenParamSet::Level& entry = set->m_levels[level];
    return entry.m_count;
}
