#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqNetFriendMelee : public gfSequence {
public:
    s32 unk8;
    union {
        u8 unkC[0x8];
        struct {
            u8 _padC[0x5];
            u8 m_isNetOnline; // HYPOTHESIS: set from the application flag at 0x110
            u8 _pad12[0x2];
        };
    };

    static sqNetFriendMelee* create();
    sqNetFriendMelee(const char* name) : gfSequence(name), unk8(0) { }
    virtual void start();
    virtual void setNext();
    virtual void exit();
};
static_assert(sizeof(sqNetFriendMelee) == 0x14, "Class is wrong size!");
