#pragma once

// SHADOW of BrawlHeaders em/teckin/em_teckin.h (-I include comes first).

#include <StaticAssert.h>
#include <em/enemy.h>
#include <types.h>

class emTeckin : public Enemy {
    u8 unk5310[0xa24];
public:
    emTeckin(u32 p1, emCreate* create);
    virtual ~emTeckin();
    // TODO: emTeckin virtual functions
};
static_assert(sizeof(emTeckin) == 0x5d34, "Class is wrong size!");

