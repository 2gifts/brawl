#pragma once

// SHADOW of BrawlHeaders em/robobeam/em_robobeam.h (-I include comes first).

#include <StaticAssert.h>
#include <em/enemy.h>
#include <types.h>

class emRobobeam : public Enemy {
    u8 unk5310[0xe7c];
public:
    emRobobeam(u32 p1, emCreate* create);
    virtual ~emRobobeam();
    // TODO: emRobobeam virtual functions
};
static_assert(sizeof(emRobobeam) == 0x618c, "Class is wrong size!");
