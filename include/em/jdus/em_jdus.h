#pragma once

// SHADOW of BrawlHeaders em/jdus/em_jdus.h (-I include comes first).

#include <StaticAssert.h>
#include <em/enemy.h>
#include <types.h>

class emJdus : public Enemy {
    u8 unk5310[0xb14];
public:
    emJdus(u32 p1, emCreate* create);
    virtual ~emJdus();
    // TODO: emJdus virtual functions
};
static_assert(sizeof(emJdus) == 0x5e24, "Class is wrong size!");
