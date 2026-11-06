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
