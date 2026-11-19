#pragma once
// Local BrawlHeaders shadow: adds the stLoader constructor (sora_melee 0x23AAC4) that the info loaders call.
// The rest of the class is unchanged.

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <gm/gm_global.h>
#include <types.h>

class stLoader : public gfTask {
public:
    gmGlobalModeMelee* m_globalModeMelee;
    u8 m_state;
    char _0x45[3];

    stLoader(const char* taskName);

    virtual void processBegin();
    virtual ~stLoader();
    virtual void entryEntity();
    virtual void removeEntity();
    virtual bool isLoad();
    virtual bool isEmpty();
    // TODO: clean up - placeholder names for the non-virtual helpers (the map only names isEntity).
    bool isFinish();            // sora_melee 0x23ABC4
    bool isEntity();            // sora_melee 0x23AC10: the entity is entered
    void setLockRequested();    // sora_melee 0x23AC3C
    bool requestOnce();         // sora_melee 0x23AC48
    void setGlobalModeMelee(gmGlobalModeMelee* globalModeMelee); // sora_melee 0x23AC6C
};
static_assert(sizeof(stLoader) == 0x48, "Class is wrong size!");
