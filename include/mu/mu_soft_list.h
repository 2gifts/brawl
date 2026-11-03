#pragma once

#include <StaticAssert.h>
#include <types.h>

// One title of the Masterpieces list: a record of the list in the relocated file (the public symbol muSoftDataList).
// HYPOTHESIS: the names of the fields follow how muSoftListDataManager reads them; the others are not read there.
struct muSoftListData {
    int m_id;               // 0x00 1 ... ; a negative id ends the list
    int m_category;         // 0x04 the page of the list the title is on
    u32 m_unk08;
    u32 m_unk0C;
    u32 m_unk10;
    int m_subTitleLines;    // 0x14 2: the subtitle takes two lines
    u32 m_unk18;
};
static_assert(sizeof(muSoftListData) == 0x1C, "Class is wrong size!");

// The titles of the Masterpieces list that can be played, sorted by page, and which of them are new. The flags come from
// the save data (gmGlobalRecord::m_softFlags) and the new flags go back to it when the manager is destroyed.
class muSoftListDataManager {
public:
    enum {
        SoftMax = 0x214,        // the flags of the save data have room for this many titles
        CategoryCount = 11,
        CategorySoftMax = 100,
    };

    muSoftListData* m_list;                                         // 0x0
    int m_count;                                                    // 0x4 the number of titles in the list
    int m_categoryStart[CategoryCount];                             // 0x8 the first title of each page in the list
    int m_categoryCount[CategoryCount];                             // 0x34 the titles of each page that can be played
    int m_categoryLines[CategoryCount];                             // 0x60 the lines they take
    muSoftListData* m_categorySofts[CategoryCount][CategorySoftMax]; // 0x8C the titles of each page that can be played
    u8 m_unlocked[SoftMax];                                         // 0x11BC by id - 1
    u8 m_new[SoftMax];                                              // 0x13D0 by id - 1

    muSoftListDataManager();
    ~muSoftListDataManager();
    void setData(u8* file, u32 size);
    muSoftListData* getSoftListData(int category, int index);
    bool isSubTitleTwoLine(int category, int index);
    u8 isNew(int category, int index);
    void resetNewFlag(int category, int index);
};
static_assert(sizeof(muSoftListDataManager) == 0x15E4, "Class is wrong size!");
