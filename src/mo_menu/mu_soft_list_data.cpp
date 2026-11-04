#include <types.h>

#include <gm/gm_global.h>
#include <string.h>
#include <ut/ut_relocate.h>

#include <mu/mu_soft_list.h>

// The data manager of the Masterpieces list is a file of its own: the list task calls each of its methods with `bl`,
// the .data of this file (the name of the relocated list and the names of the consoles) comes before the string pool
// of the task, and the task reads the names of the consoles by their own label.
// HYPOTHESIS: the name of the file.

// The flag n of the save data (two for each title, see gmGlobalRecord::m_softFlags).
// BYTES: written as a ternary; with `!= 0` the loop of setData is unrolled eight times instead of the original's two.
static inline bool isSoftFlag(u32* flags, int n) {
    return (flags[n / 32] & (1 << (n % 32))) ? true : false;
}

static inline void clearSoftFlag(u32* flags, int n) {
    flags[n / 32] &= ~(1 << (n % 32));
}

muSoftListDataManager::muSoftListDataManager() {
    m_list = NULL;
}

// The titles that are no longer new are written back to the save data.
muSoftListDataManager::~muSoftListDataManager() {
    int bit = 1;
    u32* flags = g_GameGlobal->m_record->m_softFlags;
    for (int i = 0; i < SoftMax; i++, bit += 2) {
        if (m_new[i] == false) {
            clearSoftFlag(flags, bit);
        }
    }
}

// Reads the flags of the save data and the list of the file, and sorts the titles that can be played by page.
void muSoftListDataManager::setData(u8* file, u32 size) {
    m_count = 0;
    memset(m_categoryStart, 0, sizeof(m_categoryStart));
    memset(m_categoryCount, 0, sizeof(m_categoryCount));
    memset(m_categoryLines, 0, sizeof(m_categoryLines));
    memset(m_categorySofts, 0, sizeof(m_categorySofts));
    memset(m_unlocked, 0, sizeof(m_unlocked));
    memset(m_new, 0, sizeof(m_new));
    u32* flags = g_GameGlobal->m_record->m_softFlags;
    for (int i = 0; i < SoftMax; i++) {
        m_unlocked[i] = isSoftFlag(flags, i * 2);
        m_new[i] = isSoftFlag(flags, i * 2 + 1);
    }

    utRelocate relocate(file, size);
    m_list = static_cast<muSoftListData*>(relocate.getPublicAddress("muSoftDataList"));
    int category = m_list[0].m_category;
    int n = 0;
    for (int i = 0; i < SoftMax; i++) {
        if (m_list[i].m_id < 0) {
            m_count = i;
            break;
        }
        if (category != m_list[i].m_category) {
            category = m_list[i].m_category;
            m_categoryStart[category] = i;
            n = 0;
        }
        if (m_unlocked[m_list[i].m_id - 1] == true) {
            m_categorySofts[category][n++] = &m_list[i];
            m_categoryCount[category]++;
            m_categoryLines[category]++;
            if (m_list[i].m_subTitleLines == 2) {
                m_categoryLines[category]++;
            }
        }
    }
}

muSoftListData* muSoftListDataManager::getSoftListData(int category, int index) {
    return m_categorySofts[category][index];
}

bool muSoftListDataManager::isSubTitleTwoLine(int category, int index) {
    return m_categorySofts[category][index]->m_subTitleLines == 2;
}

u8 muSoftListDataManager::isNew(int category, int index) {
    return m_new[m_categorySofts[category][index]->m_id - 1];
}

void muSoftListDataManager::resetNewFlag(int category, int index) {
    m_new[m_categorySofts[category][index]->m_id - 1] = false;
}

// The names of the consoles, by language (0 Japanese, 1 the others) and category. The US list has no Famicom Disk
// System, so category 2 has no name and is skipped (see muSoftListTask::processAnim). The Japanese names are UTF-8 bytes (this file
// is compiled as Shift JIS).
const char* muSoftListDataManager::sCategoryNames[2 * CategoryCount] = {
    // Japanese
    "\xE3\x82\xB2\xE3\x83\xBC\xE3\x83\xA0\xEF\xBC\x86\xE3\x82\xA6\xE3\x82\xAA\xE3\x83\x83\xE3\x83\x81", // ゲーム＆ウオッチ
    "\xE3\x83\x95\xE3\x82\xA1\xE3\x83\x9F\xE3\x83\xAA\xE3\x83\xBC\xE3\x82\xB3\xE3\x83\xB3\xE3\x83\x94\xE3\x83\xA5\xE3\x83\xBC\xE3\x82\xBF", // ファミリーコンピュータ
    "\xE3\x83\x87\xE3\x82\xA3\xE3\x82\xB9\xE3\x82\xAF\xE3\x82\xB7\xE3\x82\xB9\xE3\x83\x86\xE3\x83\xA0", // ディスクシステム
    "\xE3\x82\xB2\xE3\x83\xBC\xE3\x83\xA0\xE3\x83\x9C\xE3\x83\xBC\xE3\x82\xA4", // ゲームボーイ
    "\xE3\x82\xB9\xE3\x83\xBC\xE3\x83\x91\xE3\x83\xBC\xE3\x83\x95\xE3\x82\xA1\xE3\x83\x9F\xE3\x82\xB3\xE3\x83\xB3", // スーパーファミコン
    "\xE3\x83\x90\xE3\x83\xBC\xE3\x83\x81\xE3\x83\xA3\xE3\x83\xAB\xE3\x83\x9C\xE3\x83\xBC\xE3\x82\xA4", // バーチャルボーイ
    "\xE3\x83\x8B\xE3\x83\xB3\xE3\x83\x86\xE3\x83\xB3\xE3\x83\x89\xE3\x82\xA6" "64", // ニンテンドウ64
    "\xE3\x82\xB2\xE3\x83\xBC\xE3\x83\xA0\xE3\x83\x9C\xE3\x83\xBC\xE3\x82\xA4\xE3\x82\xA2\xE3\x83\x89\xE3\x83\x90\xE3\x83\xB3\xE3\x82\xB9", // ゲームボーイアドバンス
    "\xE3\x82\xB2\xE3\x83\xBC\xE3\x83\xA0\xE3\x82\xAD\xE3\x83\xA5\xE3\x83\xBC\xE3\x83\x96", // ゲームキューブ
    "\xE3\x83\x8B\xE3\x83\xB3\xE3\x83\x86\xE3\x83\xB3\xE3\x83\x89\xE3\x83\xBC" "DS", // ニンテンドーDS
    "Wii",
    // the other languages
    "GAME & WATCH", "Nintendo Entertainment System", "", "GAME BOY", "Super Nintendo Entertainment System", "VIRTUAL BOY",
    "Nintendo 64", "GAME BOY ADVANCE", "Nintendo GameCube", "Nintendo DS", "Wii",
};
