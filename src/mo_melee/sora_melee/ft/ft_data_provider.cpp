#include <ft/ft_common_data_accesser.h>
#include <ft/ft_info.h>
#include <ft/ft_manager.h>
#include <it/it_manager.h>
#include <snd/snd_system.h>
#include <so/so_archive_db.h>
#include <types.h>
#include <ut/ut_list.h>

extern "C" {
    s32 fn_27_70004(void* arg0);
    void fn_27_143EFC(ftDataProvider* arg0, utListNode* arg1);

    u32 fn_80046998();
    // HYPOTHESIS: takes the archive manager returned by getManager(0)
    u32 fn_800469AC(utArchiveManager* arg0);
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
    if (fn_80046998() == 1U || fn_800469AC(arcManager) == 1U) {
        return false;
    }
    return true;
}
