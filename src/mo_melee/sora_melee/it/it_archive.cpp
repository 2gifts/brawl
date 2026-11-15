#include <it/it_archive.h>
#include <it/it_gen_archive.h>
#include <it/it_manager.h>
#include <so/so_archive_db.h>
#include <types.h>
#include <snd/snd_system.h>
#include <ut/ut_archive_manager.h>

// HYPOTHESIS: utArchiveManager's release of one resource (main, unnamed).
extern "C" void fn_80045E64(utArchiveManager* manager, u32 resourceId);

// HYPOTHESIS: two unused generator sheets the unit keeps (constructed by the static initialiser).
static itGenSheetKind g_itArchiveSheet0(Stages::srStageKind(0xff), 0);
static itGenSheetKind g_itArchiveSheet1(Stages::srStageKind(0xff), 1);

itArchive::~itArchive() {
    fn_80045E64(soArchiveDb::getManager(2), m_paramId);
    if ((u32)m_brresId != 0xFFFF) {
        fn_80045E64(soArchiveDb::getManager(2), m_brresId);
    }
    fn_80045E64(soArchiveDb::getManager(2), m_commonParamId);
    if ((u32)m_soundGroup != 0xFFFFFFFF && m_32 >= 0) {
        g_sndSystem->freeGroup(m_32, 0);
    }
}

// An item archive: the item's parameter archive (m_paramId), its model archive (m_brresId, 0xFFFF for none) and the
// shared parameter archive (m_commonParamId), loaded through the item archive manager; m_40 counts frames to wait.

void itArchive::procWait() {
    if (m_40 < 0) {
        return;
    }
    if (!soArchiveDb::getManager(2)->getLoadResult(m_paramId, NULL)) {
        return;
    }
    if ((u32)m_brresId != 0xFFFF && !soArchiveDb::getManager(2)->getLoadResult(m_brresId, NULL)) {
        return;
    }
    if (!soArchiveDb::getManager(2)->getLoadResult(m_commonParamId, NULL)) {
        return;
    }
    m_40--;
}

bool itArchive::isComp() {
    if (m_40 < 0) {
        return true;
    }
    if (m_40 > 0) {
        return false;
    }
    if (!soArchiveDb::getManager(2)->getLoadResult(m_paramId, NULL)) {
        return false;
    }
    u32 brresId = m_brresId;
    if (brresId != 0xFFFF && !soArchiveDb::getManager(2)->getLoadResult(brresId, NULL)) {
        return false;
    }
    if (!soArchiveDb::getManager(2)->getLoadResult(m_commonParamId, NULL)) {
        return false;
    }
    return true;
}

bool itArchive::isRemovable() {
    if (!soArchiveDb::getManager(2)->getLoadResult(m_paramId, NULL)) {
        return false;
    }
    u32 brresId = m_brresId;
    if (brresId != 0xFFFF && !soArchiveDb::getManager(2)->getLoadResult(brresId, NULL)) {
        return false;
    }
    if (!soArchiveDb::getManager(2)->getLoadResult(m_commonParamId, NULL)) {
        return false;
    }
    brresId = m_brresId;
    if (brresId != 0xFFFF && soArchiveDb::getManager(2)->isExistEffect(brresId)) {
        return false;
    }
    // HYPOTHESIS: the item manager's flag that archives may be released now.
    if (!*(reinterpret_cast<u8*>(itManager::getInstance()) + 0x6D4)) {
        return false;
    }
    return true;
}

int itArchive::getSoundGroup() {
    return m_soundGroup;
}

// HYPOTHESIS: the shared parameter table of the items (one entry per item kind) and the two per-kind blocks it points to.
struct itAllParamTable {
    u32 unk0;
    u8** m_entries;
};

void* itArchive::getAllParam(int index, u32 archiveId) {
    itAllParamTable* table = static_cast<itAllParamTable*>(
        soArchiveDb::getManager(2)->getPublicAddress(m_commonParamId, "allParam", 0, 0, -1));
    u8* entry = table->m_entries[index];
    *reinterpret_cast<void**>(entry + 0xE8) = soArchiveDb::getManager(2)->getPublicAddress(m_paramId, "animParam", 0, 0, archiveId);
    *reinterpret_cast<void**>(entry + 0xEC) = soArchiveDb::getManager(2)->getPublicAddress(m_paramId, "subParam", 0, 0, archiveId);
    return table;
}

void* itArchive::getPublicData(const char* name) {
    int archiveId = itManager::getInstance()->getItemKindArchiveId(m_itKind);
    return soArchiveDb::getManager(2)->getPublicAddress(m_paramId, name, 0, 0, archiveId);
}

nw4r::g3d::ResFileData* itArchive::getResFileFromId(ARCNodeType type, u32 fileIndex, u32 groupId) {
    u32 brresId = m_brresId;
    if (brresId != 0xFFFF) {
        return soArchiveDb::getManager(2)->getResFileFromId(brresId, type, fileIndex, groupId, -1);
    }
    return NULL;
}

void* itArchive::getData(int fileKind, ARCNodeType type, int fileIndex, int groupId) {
    u32 resourceId;
    if (fileKind == 0) {
        resourceId = m_brresId;
    } else if (fileKind == 1) {
        resourceId = m_paramId;
    } else {
        return NULL;
    }
    int size = 0;
    return soArchiveDb::getManager(2)->getData(resourceId, type, fileIndex, &size, groupId, -1);
}
