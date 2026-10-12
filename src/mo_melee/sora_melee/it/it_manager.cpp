#include <types.h>
#include <it/it_manager.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

void fn_27_2A7970(itManager* self, BaseItem* item, s32 unk1, s32 unk2);


void fn_803F86E4(void* unk1, void* unk2, ...);
void fn_803FC984();
extern u8 __files[];
extern u8 lbl_27_data_7908C[];

} // extern "C"

u32 itManager::getItemNum(itKind kind, int variation, int taskId, int genId) {
    u32 count = 0;
    for (s32 i = 0; i < m_itemArrayList.size(); i++) {
        // MATCH-ONLY: the original calls the const at() overload on the member each time
        BaseItem* item = static_cast<const soArrayList<BaseItem*, 64>&>(m_itemArrayList).at(i);
        if (item != NULL && (((s32) kind == (s32) item->m_kind) || ((s32) kind == -1)) &&
            ((variation == 0) || (variation == (s32) item->m_variation)) &&
            (((u32) (taskId + 0x10000) == 0xFFFF) || ((u32) taskId == (u32) item->m_createOwnerTaskId)) &&
            (((u32) (genId + 0x10000) == 0xFFFF) || ((u32) genId == (u32) item->m_genId))) {
            count++;
        }
    }
    return count;
}


void itManager::removeItem1(int createOwnerTaskId) {
    if ((u32)createOwnerTaskId != -1U) {
        for (s32 i = 0; i < m_itemArrayList.size(); i++) {
            BaseItem*& item = m_itemArrayList.at(i);
            if (item != NULL && (u32)createOwnerTaskId == (u32)item->m_createOwnerTaskId) {
                m_itemArrayList.erase(i);
                fn_27_2A7970(this, item, 0, 1);
                i--;
            }
        }
    }
}


BaseItem* itManager::createItem(itKind kind, u32 variation, int createOwnerTaskId, soResourceModule* resourceModule, u8 groupNo, int brresId, int texResIndex, int texResId) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    return createBaseItem(&pos, &pos, 1.0f, kind, variation, createOwnerTaskId, -1, resourceModule, groupNo, brresId, texResIndex, texResId, 20);
}


BaseItem* itManager::getItemFromInstanceId(int instanceId) {
    for (s32 i = 0; i < m_itemArrayList.size(); i++) {
        BaseItem* item = m_itemArrayList.at(i);
        if (item != NULL && (u32)instanceId == (u32)item->m_instanceId) {
            return item;
        }
    }
    return NULL;
}


bool itManager::isExclusiveSpecialItem(int unk1, bool unk2) {
    if ((u32)unk1 >= NUM_IT_KINDS) {
        fn_803F86E4(__files + 0xA0, lbl_27_data_7908C);
        fn_803FC984();
    }
    return (((u32*)&m_itemSwitch)[(u32)unk1 >> 5] & (1 << (unk1 & 31))) != 0;
}
