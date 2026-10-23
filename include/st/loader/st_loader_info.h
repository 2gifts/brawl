#pragma once

#include <StaticAssert.h>
#include <st/loader/st_loader.h>
#include <types.h>

class stLoaderInfo : public stLoader {
public:
    // The resource set the HUD currently has loaded (0x48-0x50) and the one requested for the next load (0x54-0x5C).
    // HYPOTHESIS: the field types follow the request setters below.
    int m_resourceType;          // 0x48
    bool m_isOnline;             // 0x4C
    u32 m_resourceSub;           // 0x50
    int m_reqResourceType;       // 0x54
    bool m_reqIsOnline;          // 0x58
    u32 m_reqResourceSub;        // 0x5C
    // HYPOTHESIS: the Subspace common stage loader that stLoaderInfoAdventure asks for the radar resources.
    stLoader* m_commonLoader;    // 0x60

    stLoaderInfo();

    virtual void processBegin();
    virtual ~stLoaderInfo();
    virtual void entryEntity();
    virtual void removeEntity();

    // HYPOTHESIS: parameter types from the callers' register use; the name keeps the original spelling (map).
    void setResouceType(int type, u32 sub);
    void setOnline(bool isOnline);
};
static_assert(sizeof(stLoaderInfo) == 0x64, "Class is wrong size!");
