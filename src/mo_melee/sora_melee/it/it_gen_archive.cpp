#include <types.h>
#include <it/it_gen_archive.h>
#include <so/so_archive_db.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

extern "C" u32 fn_80045DC0(utArchiveManager* manager, u32 index, u32 unk);

bool itGenArchive::isFailed()
{
    u32 archiveId = m_archiveId;
    if (archiveId == 0xFFFF) {
        return true;
    }
    return fn_80045DC0(soArchiveDb::getManager(2), archiveId, 0) == 2;
}
