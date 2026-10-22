#pragma once

#include <StaticAssert.h>
#include <gm/gm_global_record.h>
#include <st/loader/st_loader_sand_bag.h>
#include <types.h>

class stLoaderLocalAdventure : public stLoaderSandBag {
public:
    // HYPOTHESIS: a word copied from the adventure save data (offset 0x620) when the local load starts.
    int unk6C;
    gmAdvData* m_advData;

    stLoaderLocalAdventure();

    virtual void processBegin();
    virtual ~stLoaderLocalAdventure();
    virtual void entryEntity();
    virtual void removeEntity();
    virtual bool isEntry();
    virtual void lockEntity();
    virtual void unlockEntity();
    virtual bool isRemove();
};
static_assert(sizeof(stLoaderLocalAdventure) == 0x74, "Class is wrong size!");
