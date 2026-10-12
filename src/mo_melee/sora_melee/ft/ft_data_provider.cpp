#include <types.h>
#include <ft/ft_manager.h>
#include <ut/ut_list.h>

#include <ft/ft_manager.h>
#include <so/so_archive_db.h>

extern "C" {
    s32 fn_27_70004(void* arg0);
    void fn_27_143EFC(ftDataProvider* arg0, utListNode* arg1);

    u32 fn_80046998();
    // HYPOTHESIS: takes the archive manager returned by getManager(0)
    u32 fn_800469AC(utArchiveManager* arg0);
} // extern "C"

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
