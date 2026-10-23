#pragma once

#include <StaticAssert.h>
#include <st/loader/st_loader_info.h>
#include <types.h>

class gfArchive;
class scResourceLoader;

class stLoaderInfoAdventure : public stLoaderInfo {
public:
    gfArchive* m_archive;              // 0x64
    scResourceLoader* m_loader;        // 0x68

    stLoaderInfoAdventure();

    virtual void processBegin();
    virtual ~stLoaderInfoAdventure();
    virtual void entryEntity();
    virtual void removeEntity();
};
static_assert(sizeof(stLoaderInfoAdventure) == 0x6C, "Class is wrong size!");
