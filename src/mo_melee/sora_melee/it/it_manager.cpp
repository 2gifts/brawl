#include <types.h>
#include <it/it_manager.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

void fn_27_2A7970(itManager* self, BaseItem* item, s32 unk1, s32 unk2);

} // extern "C"

void itManager::removeItem(BaseItem* item) {
    if (item != NULL) {
        for (s32 i = 0; i < m_itemArrayList.size(); i++) {
            if (m_itemArrayList.at(i) == item) {
                m_itemArrayList.erase(i);
                fn_27_2A7970(this, item, 0, 1);
                return;
            }
        }
    }
}
