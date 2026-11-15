#pragma once

#include <StaticAssert.h>
#include <gm/gm_global.h>
#include <st/loader/st_loader_stage.h>
#include <types.h>

// The stage loader of the Subspace Emissary: the file name comes from the four bytes of the adventure stage id
// (sora_melee st_loader_stage_adventure.o).
class stLoaderStageAdventure : public stLoaderStage {
public:
    u32 m_advStageId;    // 0xCC, the id the loaded file was built from
    gmAdvData* m_advData; // 0xD0

    stLoaderStageAdventure();

    virtual ~stLoaderStageAdventure();
    virtual bool isRemove();
    virtual void buildFileName();
};
static_assert(sizeof(stLoaderStageAdventure) == 0xD4, "Class is wrong size!");
