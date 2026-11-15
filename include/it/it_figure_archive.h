#pragma once

#include <StaticAssert.h>
#include <types.h>

namespace nw4r {
namespace g3d {
class ResFileData;
}
}

// One trophy (figure) archive: loaded from its path, or taken from the stage for the enemy figures.
class itFigureArchive {
public:
    u32 m_kind;
    u32 m_resourceId;
    u8 unk8;
    u32 unkC;

    itFigureArchive(u32 kind, const char* path);
    virtual ~itFigureArchive();
    bool isComp();
    bool isRemovable();
    nw4r::g3d::ResFileData* getTexFile(u32 index);
};
static_assert(sizeof(itFigureArchive) == 0x14, "Class is wrong size!");
