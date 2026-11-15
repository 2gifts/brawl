#include <it/it_figure_archive.h>
#include <gf/gf_archive.h>
#include <gf/gf_memory_pool.h>
#include <it/it_manager.h>
#include <so/so_archive_db.h>
#include <st/stage.h>
#include <ut/ut_archive_manager.h>
#include <types.h>

extern "C" {
// utArchiveManager::reqLoad
u32 fn_800453AC(utArchiveManager* manager, const char* path, Heaps::HeapType heap, u32 mode, int offset, int size, u32 id,
                utArchiveEventObserver* observer, bool async);
// HYPOTHESIS: utArchiveManager's registration of an archive; returns its resource id.
u32 fn_800455E0(utArchiveManager* manager, gfArchive* archive);
// utArchiveManager::unload
void fn_80045E64(utArchiveManager* manager, u32 resourceId);
}


// HYPOTHESIS: the item manager mode word (3 loads every figure into the fixed heap).
static inline int getItemManagerMode() {
    return *reinterpret_cast<int*>(reinterpret_cast<u8*>(itManager::getInstance()) + 0x50);
}

itFigureArchive::itFigureArchive(u32 kind, const char* path) {
    m_kind = kind;
    m_resourceId = 0xFFFF;
    unk8 = 0;
    unkC = 0;
    Heaps::HeapType heap = static_cast<Heaps::HeapType>(0x24);
    if (getItemManagerMode() == 3 || kind == 0xFFFFFFF0) {
        if (g_HeapInfos[0x26].m_name != NULL) {
            heap = static_cast<Heaps::HeapType>(0x26);
        }
    } else if (kind == 0xFFFFFFF1) {
        if (g_Stage != NULL) {
            gfArchive* archive = NULL;
            g_Stage->getItemPacEnemyFigure(&archive);
            if (archive != NULL) {
                m_resourceId = fn_800455E0(soArchiveDb::getManager(2), archive);
            }
        }
    } else if (g_HeapInfos[0x24].m_name != NULL) {
        heap = static_cast<Heaps::HeapType>(0x24);
    } else if (g_HeapInfos[0x8].m_name != NULL) {
        heap = static_cast<Heaps::HeapType>(0x8);
    }
    if (m_resourceId == 0xFFFF) {
        m_resourceId = fn_800453AC(soArchiveDb::getManager(2), path, heap, 0, 0, 0, 0xFF, &lbl_805A00C0, true);
    }
}

itFigureArchive::~itFigureArchive() {
    fn_80045E64(soArchiveDb::getManager(2), m_resourceId);
}

bool itFigureArchive::isComp() {
    return soArchiveDb::getManager(2)->getLoadResult(m_resourceId, NULL) != 0;
}

bool itFigureArchive::isRemovable() {
    return soArchiveDb::getManager(2)->getLoadResult(m_resourceId, NULL) != 0;
}

nw4r::g3d::ResFileData* itFigureArchive::getTexFile(u32 index) {
    u32 resourceId = m_resourceId;
    if (resourceId == 0xFFFF) {
        return NULL;
    }
    if (soArchiveDb::getManager(2)->getLoadResult(resourceId, NULL) != 1) {
        return NULL;
    }
    return soArchiveDb::getManager(2)->getResFileFromId(resourceId, Data_Type_Tex, index, 0, -1);
}
