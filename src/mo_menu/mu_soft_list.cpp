#include <types.h>

#include <gm/gm_global.h>
#include <string.h>
#include <ut/ut_relocate.h>

#include <mu/mu_soft_list.h>

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
