#include <ft/ft_common_data_accesser.h>
#include <ft/ft_info.h>
#include <ft/ft_manager.h>
#include <gf/gf_heap_manager.h>
#include <it/it_manager.h>
#include <snd/snd_system.h>
#include <so/so_archive_db.h>
#include <types.h>
#include <ut/ut_list.h>
#include <gf/gf_module.h>

extern "C" {
    s32 fn_27_70004(void* arg0);
    void fn_27_143EFC(ftDataProvider* arg0, utListNode* arg1);

    // soArchiveDb::release (the map name); em_manager.cpp calls it by this stand-in name.
    void fn_27_6FDE0(u32 index);
    extern const int lbl_27_rodata_1C60[];
    extern const int lbl_27_rodata_1D3C[];
} // extern "C"

extern int g_ftItemEnabled;

struct ftDataNode : public utListNode {
    char unk8[8];
    u8 unk10;
    char unk11[0xB];
    char unk1C[4];
};

struct ftDataProviderFields {
    char unk0[4];
    ftDataNode unk4;
    char unk24[0x1064];
    utList unk1088;
    utList unk1094;
};

ftDataProvider::ftDataTaskResource::~ftDataTaskResource() {
}

ftDataProvider::ftDataTask::~ftDataTask() {
}

utRef::~utRef() {
}

ftDataProvider::~ftDataProvider() {
    if (removeResourceTask(&m_commonTask, true) == true) {
        if (m_commonSoundGroup >= 0) {
            g_sndSystem->freeGroup(m_commonSoundGroup, 0);
            m_commonSoundGroup = -1;
        }
    }
    ftCommonDataAccesser::reset();
    fn_27_6FDE0(0);
    g_ftDataProvider = NULL;
}

ftDataProvider::ftDataTaskResource* ftDataProvider::searchTask(ftKind kind, int p2, int p3, int p4, u32 p5) {
    ftDataTaskResource* task = searchList(&m_activeTaskList, kind, p2, p3, p4, p5);
    if (task == NULL) {
        task = searchList(&m_compTaskList, kind, p2, p3, p4, p5);
    }
    return task;
}

int ftInfo::getSoundGroupId(int kind) {
    return lbl_27_rodata_1C60[kind];
}

int ftInfo::getKirbySoundGroupId(int kind) {
    return lbl_27_rodata_1D3C[kind];
}

void ftDataProvider::reqItem(ftKind kind) {
    if (ftCommonDataAccesser::isValidDat(kind) == true) {
        ftData* data = g_ftCommonDataAccesser.getData(kind);
        if (data->unk60 != NULL) {
            for (int i = 0; i < data->unk60->m_count; i++) {
                if (g_ftItemEnabled != 0) {
                    if (!itManager::getInstance()->isCompItemKindArchive(data->unk60->m_items[i], 0, true)) {
                        itManager::getInstance()->preloadItemKindArchive(data->unk60->m_items[i], 0, itArchive::Temp, true);
                    }
                }
            }
        }
    }
}

void ftDataProvider::process() {
    ftDataProviderFields* self = (ftDataProviderFields*)this;
    ftDataNode* head = &self->unk4;

    if (head->unk10 == 3 && fn_27_70004(&head->unk1C) != 0) {
        fn_27_143EFC(this, head);
        head->unk10 = 4;
    }
    if (head->unk10 == 4) {
        ftDataNode* node = (ftDataNode*)self->unk1088.getNode(0U);
        while (node != NULL) {
            head = (ftDataNode*)node->m_next;
            if (fn_27_70004(&node->unk1C) != 0) {
                fn_27_143EFC(this, node);
                node->unk10 = 4;
            }
            if (node->unk10 == 4) {
                self->unk1088.remove(node);
                self->unk1094.addTail(node);
            }
            node = head;
        }
    }
}


bool ftDataProvider::isReady() {
    utArchiveManager* arcManager = soArchiveDb::getManager(0);
    if (arcManager->isExistRemoveTask() == 1U || arcManager->isExistCancelTask() == 1U) {
        return false;
    }
    return true;
}

void* ftDataProvider::getBinData(int p2, int p4, ftKind kind, int fileIndex, int p5, int* size) {
    ftDataTaskResource* task = searchList(&m_compTaskList, kind, p2, 0xFF, p4, 0xFF);
    if (task != NULL) {
        return task->m_archiveItem.getData(Data_Type_Misc, fileIndex, size, 0, p5);
    }
    return NULL;
}

bool ftDataProvider::isExistEffect(u32 resourceId, u32 p2) {
    if (resourceId != 0xFFFF) {
        ftDataTaskResource* task = searchResource(&m_compTaskList, resourceId);
        if (task != NULL) {
            if (p2 == 1 && task->m_ref.m_count > 1) {
                return false;
            }
            return task->m_archiveItem.isExistEffect();
        }
    }
    return false;
}

bool ftDataProvider::removeResourceTask(ftDataTask* task, bool cancel) {
    if (task->tryRemove(cancel) == true) {
        m_activeTaskList.removeExist(task);
        m_compTaskList.removeExist(task);
        m_freeTaskList.addTail(task);
        return true;
    }
    return false;
}

void ftDataProvider::ftDataTask::remove(bool) {
    m_ref.m_count = 0;
    m_state = 0;
}

ftDataProvider::ftDataTaskResource::ftDataTaskResource() : m_archiveItem(0) {
    unk24 = 0xFF;
    unk28 = NULL;
    unk2C = -1;
}

void ftDataProvider::ftDataTaskResource::remove(bool cancel) {
    ftDataTask::remove(cancel);
    if (unk28 != NULL) {
        gfHeapManager::free(unk28);
        unk28 = NULL;
    }
    unk2C = -1;
    unk30 = 0;
    m_archiveItem.unload(cancel);
    m_archiveItem.init();
}

void ftDataProvider::ftDataTaskResource::init(const char* path, long kind, u8 p3, u8 p4, u8 p5, u8 p6, bool p7, srHeapType heap, u8 state, long p10, bool p11) {
    m_kind = kind;
    unk30 = p3;
    unk31 = p5;
    unk32 = p6;
    m_state = state;
    unk24 = heap;
    unk33_7 = p7;
    unk33_0 = p4;
    unk2C = p10;
    if (path != NULL) {
        size_t length = strlen(path) + 1;
        unk28 = (char*)gfHeapManager::alloc(heap, length);
        strcpy(unk28, path);
        int offset = 0;
        switch (p4) {
        case 6:
        case 7:
            switch (kind) {
            case 3:
            case 5:
            case 11:
            case 13:
            case 14:
            case 24:
            case 29:
            case 30:
            case 31:
            case 48:
                offset = 1;
                break;
            }
            break;
        }
        m_archiveItem.reqLoad(path, heap, p7, offset, p11, 0x1F, true);
    }
    m_ref.m_count++;
}

void ftDataProvider::removeMask(ftKind kind, int p2, int p3, u32 mask, u32 p5, bool cancel) {
    if (mask & 0x1) {
        remove(kind, p2, p3, 0, p5, cancel);
    }
    if (mask & 0x2) {
        remove(kind, p2, p3, 1, p5, cancel);
    }
    if (mask & 0x4) {
        remove(kind, p2, p3, 2, p5, cancel);
    }
    if (mask & 0x8) {
        remove(kind, p2, p3, 3, p5, cancel);
    }
    if (mask & 0x10) {
        remove(kind, p2, p3, 4, p5, cancel);
    }
    if (mask & 0x20) {
        remove(kind, p2, p3, 5, p5, cancel);
    }
    if (mask & 0x40) {
        remove(kind, p2, p3, 6, p5, cancel);
    }
    if (mask & 0x80) {
        remove(kind, p2, p3, 7, p5, cancel);
    }
    if (mask & 0x100) {
        remove(kind, p2, p3, 8, p5, cancel);
    }
    if (mask & 0x200) {
        remove(kind, p2, p3, 9, p5, cancel);
    }
    if (mask & 0x400) {
        remove(kind, p2, p3, 10, p5, cancel);
    }
    if (mask & 0x800) {
        remove(kind, p2, p3, 11, p5, cancel);
    }
    if (mask & 0x1000) {
        remove(kind, p2, p3, 12, p5, cancel);
    }
    if (mask & 0x2000) {
        remove(kind, p2, p3, 13, p5, cancel);
    }
    if (mask & 0x4000) {
        remove(kind, p2, p3, 14, p5, cancel);
    }
    if (mask & 0x8000) {
        remove(kind, p2, p3, 15, p5, cancel);
    }
}

bool ftDataProvider::isFinishMask(ftKind kind, int p2, int p3, u32 mask, u32 p5, u32 checkSound) {
    if (mask & 0x1) {
        if (!isFinish(kind, p2, p3, 0, p5, checkSound)) {
            return false;
        }
    }
    if (mask & 0x2) {
        if (!isFinish(kind, p2, p3, 1, p5, checkSound)) {
            return false;
        }
    }
    if (mask & 0x4) {
        if (!isFinish(kind, p2, p3, 2, p5, checkSound)) {
            return false;
        }
    }
    if (mask & 0x8) {
        if (!isFinish(kind, p2, p3, 3, p5, checkSound)) {
            return false;
        }
    }
    if (mask & 0x10) {
        if (!isFinish(kind, p2, p3, 4, p5, checkSound)) {
            return false;
        }
    }
    if (mask & 0x20) {
        if (!isFinish(kind, p2, p3, 5, p5, checkSound)) {
            return false;
        }
    }
    if (mask & 0x40) {
        if (!isFinish(kind, p2, p3, 6, p5, checkSound)) {
            return false;
        }
    }
    if (mask & 0x80) {
        if (!isFinish(kind, p2, p3, 7, p5, checkSound)) {
            return false;
        }
    }
    if (mask & 0x100) {
        if (!isFinish(kind, p2, p3, 8, p5, checkSound)) {
            return false;
        }
    }
    if (mask & 0x200) {
        if (!isFinish(kind, p2, p3, 9, p5, checkSound)) {
            return false;
        }
    }
    if (mask & 0x400) {
        if (!isFinish(kind, p2, p3, 10, p5, checkSound)) {
            return false;
        }
    }
    if (mask & 0x800) {
        if (!isFinish(kind, p2, p3, 11, p5, checkSound)) {
            return false;
        }
    }
    if (mask & 0x1000) {
        if (!isFinish(kind, p2, p3, 12, p5, checkSound)) {
            return false;
        }
    }
    if (mask & 0x2000) {
        if (!isFinish(kind, p2, p3, 13, p5, checkSound)) {
            return false;
        }
    }
    if (mask & 0x4000) {
        if (!isFinish(kind, p2, p3, 14, p5, checkSound)) {
            return false;
        }
    }
    if (mask & 0x8000) {
        if (!isFinish(kind, p2, p3, 15, p5, checkSound)) {
            return false;
        }
    }
    return true;
}

u32 ftDataProvider::reqMask(ftKind kind, int p2, int p3, u32 mask, u32 p5, srHeapType heap, bool p7, bool p8) {
    u32 requested = 0;
    if (mask & 0x1) {
        if (req(kind, p2, p3, 0, p5, heap, p7, p8) == true) {
            requested |= 0x1;
        }
    }
    if (mask & 0x2) {
        if (req(kind, p2, p3, 1, p5, heap, p7, p8) == true) {
            requested |= 0x2;
        }
    }
    if (mask & 0x4) {
        if (req(kind, p2, p3, 2, p5, heap, p7, p8) == true) {
            requested |= 0x4;
        }
    }
    if (mask & 0x8) {
        if (req(kind, p2, p3, 3, p5, heap, p7, p8) == true) {
            requested |= 0x8;
        }
    }
    if (mask & 0x10) {
        if (req(kind, p2, p3, 4, p5, heap, p7, p8) == true) {
            requested |= 0x10;
        }
    }
    if (mask & 0x20) {
        if (req(kind, p2, p3, 5, p5, heap, p7, p8) == true) {
            requested |= 0x20;
        }
    }
    if (mask & 0x40) {
        if (req(kind, p2, p3, 6, p5, heap, p7, p8) == true) {
            requested |= 0x40;
        }
    }
    if (mask & 0x80) {
        if (req(kind, p2, p3, 7, p5, heap, p7, p8) == true) {
            requested |= 0x80;
        }
    }
    if (mask & 0x100) {
        if (req(kind, p2, p3, 8, p5, heap, p7, p8) == true) {
            requested |= 0x100;
        }
    }
    if (mask & 0x200) {
        if (req(kind, p2, p3, 9, p5, heap, p7, p8) == true) {
            requested |= 0x200;
        }
    }
    if (mask & 0x400) {
        if (req(kind, p2, p3, 10, p5, heap, p7, p8) == true) {
            requested |= 0x400;
        }
    }
    if (mask & 0x800) {
        if (req(kind, p2, p3, 11, p5, heap, p7, p8) == true) {
            requested |= 0x800;
        }
    }
    if (mask & 0x1000) {
        if (req(kind, p2, p3, 12, p5, heap, p7, p8) == true) {
            requested |= 0x1000;
        }
    }
    if (mask & 0x2000) {
        if (req(kind, p2, p3, 13, p5, heap, p7, p8) == true) {
            requested |= 0x2000;
        }
    }
    if (mask & 0x4000) {
        if (req(kind, p2, p3, 14, p5, heap, p7, p8) == true) {
            requested |= 0x4000;
        }
    }
    if (mask & 0x8000) {
        if (req(kind, p2, p3, 15, p5, heap, p7, p8) == true) {
            requested |= 0x8000;
        }
    }
    return requested;
}

extern "C" const char* lbl_27_rodata_1320[0x53];

bool ftDataProvider::isFinish(ftKind kind, int p2, int p3, int resourceType, u32 p5, u32 checkSound) {
    switch (resourceType) {
    case 0: {
        const char* moduleName = lbl_27_rodata_1320[kind];
        if (moduleName != NULL && !g_gfModuleManager->isLoaded(moduleName)) {
            return false;
        }
        return true;
    }
    case 1: {
        if (searchList(&m_compTaskList, kind, 0xFF, 0xFF, resourceType, p5) == NULL) {
            return false;
        }
        ftData* data = g_ftCommonDataAccesser.getData(kind);
        if (data->unk60 != NULL) {
            for (int itemIndex = 0; itemIndex < data->unk60->m_count; itemIndex++) {
                if (g_ftItemEnabled != 0) {
                    if (!itManager::getInstance()->isCompItemKindArchive(data->unk60->m_items[itemIndex], 0, true)) {
                        itManager::getInstance()->preloadItemKindArchive(data->unk60->m_items[itemIndex], 0, itArchive::Temp, true);
                        return false;
                    }
                }
            }
        }
        return true;
    }
    case 3:
        if (checkSound == 1) {
            return g_sndSystem->isFinishLoad1(lbl_27_rodata_1C60[kind]);
        }
        return true;
    case 2:
        if (checkSound == 1) {
            return g_sndSystem->isFinishLoad1(lbl_27_rodata_1C60[0x1C]);
        }
        return true;
    case 9:
        if (checkSound == 1) {
            return g_sndSystem->isFinishLoad1(lbl_27_rodata_1D3C[kind]);
        }
        return true;
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
        return searchList(&m_compTaskList, kind, p2, p3, resourceType, p5) != NULL;
    }
    return false;
}
