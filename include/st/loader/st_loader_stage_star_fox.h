#pragma once

#include <StaticAssert.h>
#include <st/loader/st_loader_stage.h>
#include <types.h>

// The stage loader of the stages made of several parts loaded one after another (Corneria/Lylat parts, the Castle
// Siege parts): the parts are shuffled into a table and loaded in that order (sora_melee st_loader_stage_star_fox.o).
class stLoaderStageStarFox : public stLoaderStage {
public:
    u8 m_partIndex;      // 0xCC, the part buildFileName names
    u8 m_loadedStageKind; // 0xCD
    u8 m_isLocked : 1;   // 0xCE, 0x80: HYPOTHESIS: processBegin does not start loading while it is set
    u8 m_flags : 7;
    u8 m_tableIndex;     // 0xCF, the next entry of m_table
    u8 m_tableNum;       // 0xD0
    u8 m_table[4];       // 0xD1, the part order
    char _D5[3];

    stLoaderStageStarFox();

    virtual void processBegin();
    virtual ~stLoaderStageStarFox();
    virtual bool isLoad();
    virtual void buildFileName();

    void setStage(Stage* stage);
    void shuffuleTable(int num);
};
static_assert(sizeof(stLoaderStageStarFox) == 0xD8, "Class is wrong size!");
